#pragma once

#include "../SyntaxTreeNodeType.h"
#include "../SyntaxTreeNodeRef.h"
#include "../SyntaxTreeNodeVar.h"

namespace WestCoastCode::Compilation
{
	/// @brief A type that referrs to an unknown variable. This node is a companion node
	///        for the SyntaxTreeNodeVarRef node
	class ARLANG_API SyntaxTreeNodeVarTypeRef : public SyntaxTreeNodeType
	{
	public:
		SyntaxTreeNodeVarTypeRef(SourceCodeView sourceCode, SyntaxTreeNodeRef* ref);

#pragma region SyntaxTreeNodeType
		ReadOnlyString GetName() const final { return _reference->GetName(); }
		void ToString(StringStream& s, int indent) const final;
		bool Resolve(RecursiveDetector* detector) final;
		SyntaxTreeNodeType* GetType();
#pragma endregion

	private:
		/// @brief The reference
		SyntaxTreeNodeRef* const _reference;
		/// @brief Definitions that's resolved by this type-ref
		Vector<SyntaxTreeNodeVar*> _definitions;
	};
}
