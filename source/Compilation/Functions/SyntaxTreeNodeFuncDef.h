#pragma once

#include "../SyntaxTreeNodeFunc.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDefBody;

	/// @brief A normal function definition
	class ARLANG_API SyntaxTreeNodeFuncDef : public SyntaxTreeNodeFunc
	{
	public:
		SyntaxTreeNodeFuncDef(SourceCodeView view, ReadOnlyString name);

		/// @return The body that implements this function
		inline SyntaxTreeNodeFuncDefBody* GetBody() const { return _body; }

		/// @return The symbol
		Builder::Function* GetSymbol();

		/// @brief Set the body that implements this function
		/// @param body 
		void SetBody(SyntaxTreeNodeFuncDefBody* body);

#pragma region SyntaxTreeNodeFunc
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker) final;
#pragma endregion

		/// @brief Parse source code into a function definition node. Will throw ParseError if parsing of the 
		///	       source code failed
		/// @param state 
		/// @return 
		static SyntaxTreeNodeFuncDef* Parse(ParserState* state);

	private:
		SyntaxTreeNodeFuncDefBody* _body;
		Builder::Function* _symbol;
	};
}
