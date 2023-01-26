#pragma once

#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeType.h"

namespace WestCoastCode::Compilation
{
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
		inline SyntaxTreeNodeType* GetReturnType() const { return _returnType; }

		/// @return true if this function doesn't return anything
		inline bool IsVoidReturn() const { return _returnType == nullptr; }

		/// @brief Add an argument
		/// @param arg 
		void AddArgument(SyntaxTreeNodeFuncArg* arg);

		/// @brief Set the return type
		void SetReturnType(SyntaxTreeNodeType* returnType);

	private:
		ReadOnlyString _name;
		Vector<SyntaxTreeNodeFuncArg*> _arguments;
		SyntaxTreeNodeType* _returnType;
	};
}
