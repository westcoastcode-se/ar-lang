#pragma once

#include "../Common.h"
#include "../Array.h"
#include "SourceCodeView.h"
#include "SourceCodeParser.h"
#include "ParseError.h"
#include "CompileError.h"
#include "../Interpreter/Primitive.h"
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
	class ISyntaxTreeNodePackage;
	class ISyntaxTreeNodeFuncArg;
	class ISyntaxTreeNodeFuncRet;
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

		// Get the stack type used if this node results into a push onto the stack
		virtual ISyntaxTreeNode* GetStackType() {
			return Default::GetStackType(this);
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
			static ISyntaxTreeNode* GetStackType(ISyntaxTreeNode* node);
		};
	};

	class ISyntaxTreeNodeImport : public ISyntaxTreeNode
	{

	};

	class ISyntaxTreeNodeOpBinop : public ISyntaxTreeNode
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
		virtual ISyntaxTreeNode* GetLeft() const = 0;

		// Get the node on the right-hand side
		virtual ISyntaxTreeNode* GetRight() const = 0;

		// Get the operator
		virtual Op GetOperator() const = 0;

		// Get the function this operation is part of (if any)
		virtual ISyntaxTreeNodeFuncDef* GetFunction() const = 0;

		// Get the package this operation is part of
		virtual ISyntaxTreeNodePackage* GetPackage() const = 0;
	};

	class ISyntaxTreeNodeOpUnaryop : public ISyntaxTreeNode
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
		virtual ISyntaxTreeNode* GetRight() const = 0;

		// Get the operator
		virtual Op GetOperator() const = 0;

		// Get the function this operation is part of (if any)
		virtual ISyntaxTreeNodeFuncDef* GetFunction() const = 0;

		// Get the package this operation is part of
		virtual ISyntaxTreeNodePackage* GetPackage() const = 0;
	};

	class INamedSyntaxTreeNode : public ISyntaxTreeNode
	{
	public:
		// Get the name of the node
		virtual ReadOnlyString GetName() const = 0;
	};

	class ISyntaxTreeNodePackage : public INamedSyntaxTreeNode
	{
	};

	class ISyntaxTreeNodePrimitive : public INamedSyntaxTreeNode
	{
	public:
		// The memory size of this primtive
		virtual I32 GetSize() const = 0;
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
		// Get all arguments 
		virtual ReadOnlyArray<ISyntaxTreeNodeFuncArg*> GetArguments() const = 0;

		// Get all return types
		virtual ReadOnlyArray<ISyntaxTreeNodeFuncRet*> GetReturns() const = 0;

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

	class ISyntaxTreeNodeFuncRet : public ISyntaxTreeNode
	{
	public:
		// Get the return type
		virtual ISyntaxTreeNodeTypeRef* GetReturnType() const = 0;
	};

	// A reference to a type
	class ISyntaxTreeNodeTypeRef : public INamedSyntaxTreeNode
	{
	public:
		// All types that this reference resolved into. The item at the top of the vector
		// is the one closest to the reference
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const = 0;
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
		virtual ReadOnlyArray<ISyntaxTreeNodeFuncScope*> GetLocals() const = 0;
	};

	// A node containing the function body logic
	class ISyntaxTreeNodeFuncBody : public ISyntaxTreeNode
	{
	public:
		// Get the body content
		virtual ReadOnlyString GetText() const = 0;

		// Get the function body
		virtual ISyntaxTreeNodeFuncDef* GetFunction() const = 0;
	};

	class ISyntaxTreeNodeScope : public ISyntaxTreeNode
	{
	};

	class ISyntaxTreeNodeOpReturn : public ISyntaxTreeNode
	{
	};

	class ISyntaxTreeNodeConstant : public ISyntaxTreeNode
	{
	public:
		// Get the constant value
		virtual const Interpreter::PrimitiveValue& GetValue() const = 0;
	};
}

