#pragma once

#include "SyntaxTreeNode.h"
#include "SourceCodeParser.h"
#include "SyntaxTreeNodeOp.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;

	/// @brief The body of a function. All children in the body must be an operation node type
	class ARLANG_API SyntaxTreeNodeFuncBody : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeFuncBody(SourceCodeView view, SyntaxTreeNodeFuncDef* func);

		/// @return The function this body is part of
		inline SyntaxTreeNodeFuncDef* GetFunction() const { return _function; }

#pragma region SyntaxTreeNode
		void ToString(StringStream& s, int indent) const final;
		void OnChildAdded(SyntaxTreeNode* child) final;
		void Compile(Builder::Linker* linker) final;
		void Optimize(ISyntaxTreeNodeOptimizer* optimizer) final;
#pragma endregion

	public:
		typedef SyntaxTreeNodeOp* (*ParseFn)(ParserState*);

		// Parse a function body using the supplied state
		static SyntaxTreeNodeFuncBody* Parse(ParserState* state);

	private:
		//
		static SyntaxTreeNodeOp* ParseBody(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseReturn(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseCompare(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseExpr(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseTerm(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseFactor(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseAtom(ParserState* state);

		//
		static SyntaxTreeNodeOp* ParseUnaryop(ParserState* state,
			TokenType tokenType,
			ParseFn rightFunc);

		//
		static SyntaxTreeNodeOp* ParseBinop(ParserState* state,
			const Vector<TokenType>& types,
			ParseFn leftFunc, ParseFn rightFunc);

		// Does the supplied vector contain the supplied type
		static bool Contains(const Vector<TokenType>& tokens, TokenType type);

	private:
		SyntaxTreeNodeFuncDef* const _function;
		ReadOnlyString _text;
	};
}
