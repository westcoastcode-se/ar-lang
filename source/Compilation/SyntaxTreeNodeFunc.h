#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeTypeDef;
	class SyntaxTreeNodePackage;
	class SyntaxTreeNodeFuncBody;
	class SyntaxTreeNodeFuncArg;

	/// @brief A function node
	class ARLANG_API SyntaxTreeNodeFunc : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeFunc(SourceCodeView view, ReadOnlyString name);

		/// @return The name of this function
		inline ReadOnlyString GetName() const { return _name; }

		/// @brief Get all arguments that this function requires
		inline ReadOnlyArray<SyntaxTreeNodeFuncArg*> GetArguments() const { return _arguments; }

		/// @return The type returned after calling the function
		inline SyntaxTreeNodeTypeDef* GetReturnType() const { return _returnType; }

		/// @return true if this function doesn't return anything
		inline bool IsVoidReturn() const { return _returnType == nullptr; }

		/// @return The body that implements this function
		inline SyntaxTreeNodeFuncBody* GetBody() const { return _body; }

		/// @return The closest package this function is part of
		SyntaxTreeNodePackage* GetPackage();

		/// @return The symbol
		inline Builder::Function* GetSymbol() const { return _symbol; }

#pragma region SyntaxTreeNode
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker) final;
#pragma endregion

	private:
		friend class SyntaxTreeNodePackage;

		/// @brief Set the body that implements this function
		/// @param body 
		void SetBody(SyntaxTreeNodeFuncBody* body);

		/// @brief Add an argument
		/// @param arg 
		void AddArgument(SyntaxTreeNodeFuncArg* arg);

		/// @brief Set the return type
		void SetReturnType(SyntaxTreeNodeTypeDef* returnType);

		/// @brief Parse source code into a function definition node. Will throw ParseError if parsing of the 
		///	       source code failed
		/// @param state 
		/// @return 
		static SyntaxTreeNodeFunc* Parse(ParserState* state);

	private:
		ReadOnlyString _name;
		Vector<SyntaxTreeNodeFuncArg*> _arguments;
		SyntaxTreeNodeTypeDef* _returnType;
		SyntaxTreeNodeFuncBody* _body;
		Builder::Function* _symbol;
	};
}
