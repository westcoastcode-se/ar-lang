#pragma once

#include "../SyntaxTreeNodeOp.h"
#include "../Functions/SyntaxTreeNodeFuncRef.h"

namespace WestCoastCode::Compilation
{
	class ARLANG_API SyntaxTreeNodeOpCallFunc : public SyntaxTreeNodeOp
	{
	public:
		SyntaxTreeNodeOpCallFunc(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body);

		/// @return The function to be called by this operation
		SyntaxTreeNodeFuncRef* GetFunction();

#pragma region SyntaxTreeNodeOp
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
		SyntaxTreeNodeType* GetType() final;
		bool Resolve(RecursiveDetector* detector) final;
#pragma endregion

		/// @brief Set which function to call
		/// @param func 
		void SetFunction(SyntaxTreeNodeFuncRef* func);

	private:
		SyntaxTreeNodeFuncRef* _function;
	};
}
