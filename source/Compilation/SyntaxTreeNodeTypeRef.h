#pragma once

#include "SyntaxTreeNodeTypeDef.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents reference to a type. Used primarily during compile time when we don't know which type
	///        the developer is referring to in the source code. This can only be resolved when all source code
	///        is parsed
	class SyntaxTreeNodeTypeRef : public SyntaxTreeNodeTypeDef
	{
	public:
		SyntaxTreeNodeTypeRef(SourceCodeView sourceCode);

		/// @brief Parse the source code and extract a type reference 
		/// @param state 
		/// @return 
		static SyntaxTreeNodeTypeRef* Parse(const ParserState* state);

#pragma region SyntaxTreeNodeTypeDef
		void ToString(StringStream& s, int indent) const final;
		SyntaxTreeNodeTypeDef* GetType() final;
		ReadOnlyArray<SyntaxTreeNodeTypeDef*> GetTypes() final { return _definitions; }
		void Resolve() final;
		void OnChildAdded(SyntaxTreeNode* child) final;
#pragma endregion

	private:
		/// @brief Definitions that's resolved by this type-ref
		Vector<SyntaxTreeNodeTypeDef*> _definitions;
	};
}
