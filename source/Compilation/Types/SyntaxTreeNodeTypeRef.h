#pragma once

#include "../SyntaxTreeNodeType.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents reference to a type. Used primarily during compile time when we don't know which type
	///        the developer is referring to in the source code. This can only be resolved when all source code
	///        is parsed
	class SyntaxTreeNodeTypeRef : public SyntaxTreeNodeType
	{
	public:
		SyntaxTreeNodeTypeRef(SourceCodeView sourceCode, ReadOnlyString name);

		/// @brief Parse the source code and extract a type reference 
		/// @param state 
		/// @return 
		static SyntaxTreeNodeTypeRef* Parse(const ParserState* state);

#pragma region SyntaxTreeNodeType
		inline ReadOnlyString GetName() const final { return _name; }
		void ToString(StringStream& s, int indent) const final;
		SyntaxTreeNodeType* GetType() final;
		ReadOnlyArray<SyntaxTreeNodeType*> GetTypes() final { return _definitions; }
		bool Resolve(RecursiveDetector* detector) final;
		void OnChildAdded(SyntaxTreeNode* child) final;
#pragma endregion

	private:
		/// @brief The name of this reference
		const ReadOnlyString _name;
		/// @brief Definitions that's resolved by this type-ref
		Vector<SyntaxTreeNodeType*> _definitions;
	};
}
