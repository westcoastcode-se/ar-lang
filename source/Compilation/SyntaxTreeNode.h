#pragma once

#include "../Common.h"
#include "../Array.h"
#include "SourceCodeView.h"
#include "SourceCodeParser.h"
#include "ParseError.h"
#include "CompileError.h"
#include "../Primitive.h"
#include "../Builder/Linker.h"

namespace WestCoastCode::Compilation
{
	struct ID
	{
		// The ID
		const I32 value;

		ID();

		ID(const ID& rhs) : value(rhs.value) {}
	};

	static inline std::ostream& operator<< (std::ostream& out, ID const& t)
	{
		return out << "ID(" << t.value << ")";
	}

	class ISyntaxTree;
	class ISyntaxTreeNode;
	class ISyntaxTreeNodeOp;
	class ISyntaxTreeNodeType;
	class ISyntaxTreeNodePackage;
	class ISyntaxTreeNodeFuncArg;
	class ISyntaxTreeNodeFuncDef;
	class ISyntaxTreeNodeTypeRef;
	class ISyntaxTreeNodeFuncBody;

	// Flags used to help configure the search algorithm when using the Query functionality
	enum class QuerySearchFlag : int
	{
		// Searching backwards from the current node. Useful for searching for variables
		// declared before the current node in a function
		Backwards = 1 << 0,

		// Include imports when searching for nodes
		TraverseImports = 1 << 1,

		// Traverse child-nodes
		TraverseChildren = 1 << 2,

		// Traverse upwards to parent etc.
		TraverseParent = 1 << 3
	};
	typedef int QuerySearchFlags;

	// Flags used to help configure the search algorithm when using the Visit functionality
	enum class VisitFlag : int
	{
		// Include more children
		IncludeChildren = 1 << 0
	};
	typedef int VisitFlags;

	enum class VisitorResult
	{
		// Stop querying for any more nodes
		Stop,

		// Continue querying for more nodes
		Continue,

		// Continue query but also exclude children
		ContinueExcludeChildren,
	};

	enum class VisitResult
	{
		// Stop querying for any more nodes
		Stop,

		// Continue querying for more nodes
		Continue,
	};

	// Visitor
	class ISyntaxTreeNodeVisitor
	{
	public:
		virtual ~ISyntaxTreeNodeVisitor() {}

		// Visit the supplied node. Return true if we want to continue search for more nodes
		virtual VisitorResult Visit(ISyntaxTreeNode* node) = 0;
	};

	// Optimizer. It is assumed that the optimization is done bottom-up
	class ISyntaxTreeNodeOptimizer
	{
	public:
		virtual ~ISyntaxTreeNodeOptimizer() {}

		// Optimize the supplied node and return a list of nodes that should replace the supplied node
		virtual Vector<ISyntaxTreeNodeOp*> Optimize(ISyntaxTreeNodeOp* node) = 0;
	};

	class IStringify
	{
	public:
		virtual ~IStringify() {}

		// Stringify this syntax tree node
		virtual void ToString(StringStream& s, int indent) const = 0;

		// Helper function for indentations
		static String Indent(int indent) {
			String s;
			for (int i = 0; i < indent; ++i)
				s += "  ";
			return s;
		}
	};

	class ISyntaxTreeNode : public IStringify
	{
	public:
		virtual ~ISyntaxTreeNode() {}

		// Get the unique id for this node
		virtual const ID& GetID() const = 0;

		// Get the tree that this node is part of
		virtual ISyntaxTree* GetSyntaxTree() const = 0;

		// Get the root node
		virtual ISyntaxTreeNode* GetRootNode() = 0;

		// Get the parent node
		virtual ISyntaxTreeNode* GetParent() const = 0;

		// Set the parent of this node
		virtual void SetParent(ISyntaxTreeNode* parent) = 0;

		// Get all root nodes in the syntax tree
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const = 0;

		// Get all sibling nodes before this node
		virtual Vector<ISyntaxTreeNode*> GetSiblingsBefore() const {
			return Default::GetSiblingsBefore(this);
		}

		// Visit all children in the entire tree
		virtual VisitResult Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags) {
			return Default::Visit(this, visitor, flags);
		}

		// Query for nodes in an upwards/revsersed manner, from this node's point of view
		virtual VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags) {
			return Default::Query(this, visitor, flags);
		}

		// Get the source code which this node is created from
		virtual const SourceCodeView* GetSourceCode() const = 0;

		// Resolve references
		virtual void ResolveReferences() {
			Default::ResolveReferences(this);
		}

		// Link the nodes
		virtual void Compile(Builder::Linker* linker) {
			Default::Compile(this, linker);
		}

		// Optimize this tree node's children using the supplied optimizer
		virtual void Optimize(ISyntaxTreeNodeOptimizer* optimizer) {
			Default::Optimize(this, optimizer);
		}

	public:
		// Default implementations
		struct Default
		{
			static Vector<ISyntaxTreeNode*> GetSiblingsBefore(const ISyntaxTreeNode* node);
			static VisitResult Visit(ISyntaxTreeNode* node, ISyntaxTreeNodeVisitor* visitor, VisitFlags flags);
			static VisitResult Query(ISyntaxTreeNode* node, ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags);
			static void ResolveReferences(ISyntaxTreeNode* node);
			static void Compile(ISyntaxTreeNode* node, Builder::Linker* linker);
			static void Optimize(ISyntaxTreeNode* node, ISyntaxTreeNodeOptimizer* optimizer);
		};
	};

	class ISyntaxTreeNodeImport : public ISyntaxTreeNode
	{

	};

	class INamedSyntaxTreeNode : public ISyntaxTreeNode
	{
	public:
		// Get the name of the node
		virtual ReadOnlyString GetName() const = 0;
	};

	// Represents a package
	class ISyntaxTreeNodePackage : public INamedSyntaxTreeNode
	{
	};

	// A specific type
	class ISyntaxTreeNodeType : public INamedSyntaxTreeNode
	{
	public:
		// Check to see if the supplied type is compatible
		virtual bool IsCompatibleWith(ISyntaxTreeNodeType* otherType) const { return true; }
	};

	// Represents a type that contains multiple types. For example:
	// (int32, bool) is considered a multitype of one int32 and one bool.
	class ISyntaxTreeNodeMultiType : public ISyntaxTreeNodeType
	{
	public:
		// All types that this node referrs to
		virtual ReadOnlyArray<ISyntaxTreeNodeType*> GetTypes() const = 0;
	};

	// A primitive. These are normally created by the compiler outside the source code
	// parser and put into the root package
	class ISyntaxTreeNodePrimitive : public ISyntaxTreeNodeType
	{
	public:
		// The memory size of this primitive
		virtual I32 GetSize() const = 0;

		// Get the interpreter primitive type
		virtual PrimitiveType GetPrimitiveType() const = 0;
	};

	// A reference to another syntax tree node
	class ISyntaxTreeNodeRef : public INamedSyntaxTreeNode
	{
	public:
		// Various query types that can be used when searching for specific definitions
		enum class DefinitionQueryType : I32
		{
			Package = 1 << 0,
			Class = 1 << 1,
			Func = 1 << 2,
			Arg = 1 << 3,
			Local = 1 << 4,
			Global = 1 << 5,
			Member = 1 << 6,
			Primitive = 1 << 7,
		};
		typedef int DefinitionQueryTypes;

		// Represents all types
		static constexpr DefinitionQueryTypes Type = (I32)DefinitionQueryType::Class | 
			(I32)DefinitionQueryType::Primitive;

		// Represents all nodes
		static constexpr DefinitionQueryTypes All = INT32_MAX;

		// Get all definitions that this reference referres to. This
		// is normally resolved during the Resolve phase but can, in specific cases,
		// be resolved when the tree is being parsed (for example, if it points to a primitive)
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const = 0;

		// Get which types this reference is searching for
		virtual DefinitionQueryTypes GetQueryTypes() const = 0;
	};

	class ISyntaxTreeNodeFuncDef : public INamedSyntaxTreeNode
	{
	public:
		// Get the package this function is part of
		virtual ISyntaxTreeNodePackage* GetPackage() = 0;

		// Get all arguments 
		virtual ReadOnlyArray<ISyntaxTreeNodeFuncArg*> GetArguments() const = 0;

		// Get return type
		virtual ISyntaxTreeNodeType* GetReturnType() const = 0;

		// Is this function a void function
		virtual bool IsVoidReturn() const = 0;

		// Get the function body
		virtual ISyntaxTreeNodeFuncBody* GetBody() const = 0;
	};

	class ISyntaxTreeNodeFuncArg : public INamedSyntaxTreeNode
	{
	public:
		// Get the type which the argument variable is of
		virtual ISyntaxTreeNodeTypeRef* GetVariableType() const = 0;
	};

	// A reference to a type
	class ISyntaxTreeNodeTypeRef : public ISyntaxTreeNodeType
	{
	public:
		// All types that this reference resolved into. The item at the top of the vector
		// is the one closest to the reference
		virtual ReadOnlyArray<ISyntaxTreeNodeType*> GetDefinitions() const = 0;
	};

	//
	class ISyntaxTreeNodeFuncLocal : public INamedSyntaxTreeNode
	{
	public:
	};

	// A scope
	class ISyntaxTreeNodeFuncScope : public INamedSyntaxTreeNode
	{
	public:
		// Get all locals part of this scope
		virtual ReadOnlyArray<ISyntaxTreeNodeFuncLocal*> GetLocals() const = 0;
	};

	// Base class for operations that's executed in the function body
	class ISyntaxTreeNodeOp : public ISyntaxTreeNode
	{
	public:
		// The normal compile functionality is no longer allowed. Implement the other Compile
		// method instead.
		void Compile(Builder::Linker* linker) final {}

		// This normal optimize functionality is no longer allowed. 
		virtual void Optimize(ISyntaxTreeNodeOptimizer* optimizer) final {}

		// Compile this operation
		virtual void Compile(Builder::Linker* linker, Builder::Instructions& instructions) = 0;

		// Optimize this operator
		virtual Vector<ISyntaxTreeNodeOp*> OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer) = 0;

		// Get the function this operation is part of (if any)
		virtual ISyntaxTreeNodeFuncDef* GetFunction() = 0;

		// Get the package this operation is part of
		virtual ISyntaxTreeNodePackage* GetPackage() = 0;

		// Get the type which this operator results into onto the stack
		virtual ISyntaxTreeNodeType* GetStackType() = 0;
	};

	// Type-cast the result of any child operations into a new type
	class ISyntaxTreeNodeOpTypeCast : public ISyntaxTreeNodeOp
	{
	public:
		// Cast from the supplied type
		virtual ISyntaxTreeNodeType* FromType() = 0;
	};

	// A node containing the function body logic
	class ISyntaxTreeNodeFuncBody : public ISyntaxTreeNode
	{
	public:
		// Get the body content
		virtual ReadOnlyString GetText() const = 0;

		// Get the definition used when executing this body
		virtual ISyntaxTreeNodeFuncDef* GetFunction() const = 0;
	};

	class ISyntaxTreeNodeOpBinop : public ISyntaxTreeNodeOp
	{
	public:
		enum Op {
			Plus,
			Minus,
			Mult,
			Div,
			Equals,
			NotEquals,
			LessThen,
			LessThenEquals,
			GreaterThen,
			GreaterThenEquals,
			BitAnd,
			BitOr,
			BitXor,
			Unknown
		};

		static Op FromTokenType(TokenType type)
		{
			switch (type)
			{
			case TokenType::OpPlus:
				return Plus;
			case TokenType::OpMinus:
				return Minus;
			case TokenType::OpMult:
				return Mult;
			case TokenType::OpDiv:
				return Div;
			case TokenType::TestEquals:
				return Equals;
			case TokenType::TestNotEquals:
				return NotEquals;
			case TokenType::TestLt:
				return LessThen;
			case TokenType::TestLte:
				return	LessThenEquals;
			case TokenType::TestGt:
				return GreaterThen;
			case TokenType::TestGte:
				return GreaterThenEquals;
			case TokenType::BitAnd:
				return BitAnd;
			case TokenType::BitOr:
				return BitOr;
			case TokenType::BitXor:
				return BitXor;
			default:
				return Unknown;
			}
		}

		// Get the node on the left-hand side
		virtual ISyntaxTreeNodeOp* GetLeft() const = 0;

		// Get the node on the right-hand side
		virtual ISyntaxTreeNodeOp* GetRight() const = 0;

		// Get the operator
		virtual Op GetOperator() const = 0;
	};

	class ISyntaxTreeNodeOpUnaryop : public ISyntaxTreeNodeOp
	{
	public:
		enum Op {
			Minus,
			Plus,
			BitNot,
			Unknown
		};

		static Op FromTokenType(TokenType type)
		{
			switch (type)
			{
			case TokenType::OpPlus:
				return Plus;
			case TokenType::OpMinus:
				return Minus;
			case TokenType::BitNot:
				return BitNot;
			default:
				return Unknown;
			}
		}

		// Get the node on the right-hand side
		virtual ISyntaxTreeNodeOp* GetRight() const = 0;

		// Get the operator
		virtual Op GetOperator() const = 0;
	};

	class ISyntaxTreeNodeScope : public ISyntaxTreeNodeOp
	{
	};

	class ISyntaxTreeNodeOpReturn : public ISyntaxTreeNodeOp
	{
	};

	class ISyntaxTreeNodeConstant : public ISyntaxTreeNodeOp
	{
	public:
		// Get the constant value
		virtual const PrimitiveValue& GetValue() const = 0;

	};
}

