#pragma once

#include "../Common.h"
#include "../Array.h"
#include "SourceCodeView.h"
#include "SourceCodeParser.h"
#include "ParseError.h"

namespace WestCoastCode::Compilation
{
	class ISyntaxTree;
	class ISyntaxTreeNode;
	class ISyntaxTreeNodePackage;
	class ISyntaxTreeNodeFuncArg;
	class ISyntaxTreeNodeFuncRet;
	class ISyntaxTreeNodeFuncDef;
	class ISyntaxTreeNodeTypeRef;
	class ISyntaxTreeNodeFuncBody;

	// Various query types that can be used when searching for specific definitions
	enum class DefinitionQueryType : int
	{
		Package = 1 << 0,
		Class = 1 << 1,
		Func = 1 << 2,
		Arg = 1 << 3,
		Local = 1 << 4,
		Global = 1 << 5,
		Member = 1 << 6,
		Primitive = 1 << 7,
		Type = Class | Primitive,
		All = Package | Type | Func | Arg | Local | Global | Member,
	};
	typedef int DefinitionQueryTypes;

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

	// Visitor
	template<class BaseClass>
	class ISyntaxTreeNodeVisitor
	{
	public:
		typedef BaseClass Node;
		virtual ~ISyntaxTreeNodeVisitor() {}

		// Visit the supplied node. Return true if we want to continue search for more nodes
		virtual bool Visit(Node* node) = 0;
	};

	// Interface for the syntax tree
	class ISyntaxTree
	{
	public:
		virtual ~ISyntaxTree() {}

		// Stringify this syntax tree node
		virtual void ToString(StringStream& s) const = 0;

		// Visit all children in the entire tree and returns true if one or more
		// results are found
		virtual void Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const = 0;

		// Visit all children in the entire tree
		virtual void Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) = 0;

		// Get the root package where primitives and built-in functions are located
		virtual ISyntaxTreeNodePackage* GetRootNode() = 0;
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

		// Visit all children in the entire tree and returns true if one or more
		// results are found
		virtual bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const = 0;

		// Visit all children in the entire tree
		virtual bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) = 0;

		// Query for nodes in an upwards/revsersed manner, from this node's point of view
		virtual bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) = 0;

		// Get the source code which this node is created from
		virtual const SourceCodeView* GetSourceCode() const = 0;
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
		virtual size_t GetSize() const = 0;
	};

	// A reference to another syntax tree node
	class ISyntaxTreeNodeRef : public INamedSyntaxTreeNode
	{
	public:
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

	class ISyntaxTreeNodeOpReturn : public ISyntaxTreeNode
	{

	};
}
