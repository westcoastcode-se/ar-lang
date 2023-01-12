#pragma once

#include "SyntaxTree.h"
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncBody : public ISyntaxTreeNodeFuncBody
	{
	public:
		SyntaxTreeNodeFuncBody(SourceCodeView sourceCode);

		~SyntaxTreeNodeFuncBody() final;

		// Inherited via ISyntaxTreeNodeFuncBody
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		virtual bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const override;
		virtual bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		virtual bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyString GetText() const final { return _text; }
		ISyntaxTreeNodeFuncDef* GetFunction() const final { return _function; }
		const Vector<ISyntaxTreeNodeFuncLocal*>& GetLocals() const { return _locals; }

	public:
		typedef ISyntaxTreeNode* (*ParseFn)(ParserState*);

		// Add a child node
		void AddNode(ISyntaxTreeNode* node);

		// Parse a function body using the supplied state
		static SyntaxTreeNodeFuncBody* Parse(ParserState* state);

		//
		static ISyntaxTreeNode* ParseBody(ParserState* state);

		//
		static ISyntaxTreeNode* ParseReturn(ParserState* state);

		//
		static ISyntaxTreeNode* ParseCompare(ParserState* state);

		//
		static ISyntaxTreeNode* ParseExpr(ParserState* state);

		//
		static ISyntaxTreeNode* ParseTerm(ParserState* state);

		//
		static ISyntaxTreeNode* ParseFactor(ParserState* state);

		//
		static ISyntaxTreeNode* ParseAtom(ParserState* state);

		//
		static ISyntaxTreeNode* ParseUnaryop(ParserState* state, 
			TokenType tokenType,
			ParseFn rightFunc);

		//
		static ISyntaxTreeNode* ParseBinop(ParserState* state,
			const Vector<TokenType>& types,
			ParseFn leftFunc, ParseFn rightFunc);

		// Does the supplied vector contain the supplied type
		static bool Contains(const Vector<TokenType>& tokens, TokenType type);

	private:
		ISyntaxTreeNodeFuncDef* _function;
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		ReadOnlyString _text;
		List<ISyntaxTreeNode*> _children;
		Vector<ISyntaxTreeNodeFuncLocal*> _locals;
	};
}