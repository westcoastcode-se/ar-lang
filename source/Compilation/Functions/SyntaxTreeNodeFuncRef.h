#pragma once

#include "../SyntaxTreeNodeFunc.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents a reference to a function
	class ARLANG_API SyntaxTreeNodeFuncRef : public SyntaxTreeNodeFunc
	{
	public:
		SyntaxTreeNodeFuncRef(SourceCodeView sourceCode, ReadOnlyString name);

		/// @return All definitions resolved by this reference
		inline ReadOnlyArray<SyntaxTreeNodeFunc*> GetDefinitions() { return _definitions; }

		/// @brief Parse the source code and extract a function reference 
		/// @param state 
		/// @return 
		static SyntaxTreeNodeFuncRef* Parse(const ParserState* state);

#pragma region SyntaxTreeNodeFunc
		void ToString(StringStream& s, int indent) const final;
		bool Resolve(RecursiveDetector* detector) final;
#pragma endregion


	private:
		/// @brief Definitions that's resolved by this func-ref
		Vector<SyntaxTreeNodeFunc*> _definitions;
	};

}
