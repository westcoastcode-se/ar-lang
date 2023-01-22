#pragma once

#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents the root package.
	///
	/// Everything in the root package is automatically imported when creating a new package
	class SyntaxTreeNodeRoot : public SyntaxTreeNodePackage
	{
	public:
		SyntaxTreeNodeRoot();

		/// @brief Get the primitive with the supplied name
		/// @param name The name of the primitive
		/// @return The primitive - if found
		SyntaxTreeNodePrimitive* FindPrimitive(ReadOnlyString name);

#pragma region SyntaxTreeNodePackage
		ISyntaxTree* GetSyntaxTree() const final { return _syntaxTree; }
		void Compile(Builder::Linker* linker) final;
#pragma endregion

	private:
		friend class SyntaxTree;

		/// @brief Create a new syntax tree root
		static SyntaxTreeNodeRoot* Create();

		/// @brief Add the supplied primitive
		/// @param primitive
		void AddPrimitive(SyntaxTreeNodePrimitive* primitive);

		/// @param syntaxTree 
		void SetSyntaxTree(ISyntaxTree* syntaxTree) { _syntaxTree = syntaxTree; }

	private:
		/// @brief All primitives part of the root node
		Map<ReadOnlyString, SyntaxTreeNodePrimitive*> _primitives;
		ISyntaxTree* _syntaxTree;
	};
}
