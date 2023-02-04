#pragma once

#include "SyntaxTreeNodeVarDef.h"
#include "../Types/SyntaxTreeNodeTypeRef.h"

namespace WestCoastCode::Compilation
{
	/// @brief A reference to a variable
	class ARLANG_API SyntaxTreeNodeVarRef : public SyntaxTreeNodeVar
	{
	public:
		SyntaxTreeNodeVarRef(SourceCodeView sourceCode, ReadOnlyString name);

		/// @return All definitions resolved by this reference
		inline ReadOnlyArray<SyntaxTreeNodeVarDef*> GetDefinitions() { return _definitions; }

#pragma region SyntaxTreeNodeVar
		void ToString(StringStream& s, int indent) const final;
		bool Resolve(RecursiveDetector* detector) final;
		inline SyntaxTreeNodeType* GetType() final { return _type; }
#pragma endregion

		/// @brief Parse the source code and extract a variable reference 
		/// @param state 
		/// @return 
		static SyntaxTreeNodeVarRef* Parse(const ParserState* state);

	private:
		/// @brief Definitions that's resolved
		Vector<SyntaxTreeNodeVarDef*> _definitions;
		/// @brief The resolved type
		SyntaxTreeNodeType* _type;
	};
}
