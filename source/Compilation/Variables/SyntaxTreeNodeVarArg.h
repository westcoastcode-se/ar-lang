#pragma once

#include "SyntaxTreeNodeVarDef.h"
#include "../Types/SyntaxTreeNodeTypeRef.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;

	/// @brief A argument variable
	class ARLANG_API SyntaxTreeNodeVarArg : public SyntaxTreeNodeVarDef
	{
	public:
		SyntaxTreeNodeVarArg(SourceCodeView sourceCode, ReadOnlyString name);

		/// @return The function this argument is definde 
		inline SyntaxTreeNodeFuncDef* GetFunction() const { return _function; }

#pragma region SyntaxTreeNodeVar
		void ToString(StringStream& s, int indent) const final;
		bool Resolve(RecursiveDetector* detector) final;
		SyntaxTreeNodeType* GetType() final { return _type; }
#pragma endregion

	private:
		/// @brief The function
		SyntaxTreeNodeFuncDef* _function;
		/// @brief The type this argument 
		SyntaxTreeNodeTypeRef* _type;
	};
}
