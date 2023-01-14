#pragma once

#include "SyntaxTreeNode.h"
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncBody : public ISyntaxTreeNodeFuncBody
	{
	public:
		SyntaxTreeNodeFuncBody(SourceCodeView sourceCode);

		~SyntaxTreeNodeFuncBody() final;

		// Inherited via ISyntaxTreeNodeFuncBody
		const ID& GetID() const final { return _id; }
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyString GetText() const final { return _text; }
		ISyntaxTreeNodeFuncDef* GetFunction() const final { return _function; }
		void Compile(Builder::Linker* linker) final;

	public:
		typedef ISyntaxTreeNodeOp* (*ParseFn)(ParserState*);

		// Add a child node
		void AddOp(ISyntaxTreeNodeOp* node);

		// Set the definition associated with this function body
		void SetFunction(ISyntaxTreeNodeFuncDef* funcdef);

		// Parse a function body using the supplied state
		static SyntaxTreeNodeFuncBody* Parse(ParserState* state);

	private:
		//
		static ISyntaxTreeNodeOp* ParseBody(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseReturn(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseCompare(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseExpr(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseTerm(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseFactor(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseAtom(ParserState* state);

		//
		static ISyntaxTreeNodeOp* ParseUnaryop(ParserState* state,
			TokenType tokenType,
			ParseFn rightFunc);

		//
		static ISyntaxTreeNodeOp* ParseBinop(ParserState* state,
			const Vector<TokenType>& types,
			ParseFn leftFunc, ParseFn rightFunc);

		// Does the supplied vector contain the supplied type
		static bool Contains(const Vector<TokenType>& tokens, TokenType type);

	private:
		const ID _id;
		ISyntaxTreeNodeFuncDef* _function;
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		ReadOnlyString _text;
		Vector<ISyntaxTreeNodeOp*> _children;
	};
}
