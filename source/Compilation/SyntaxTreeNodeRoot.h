#pragma once

#include "SyntaxTreeNodePackage.h"
#include "Types/SyntaxTreeNodePrimitive.h"

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
		SyntaxTree* GetSyntaxTree() const final { return _syntaxTree; }
		void Compile(Builder::Linker* linker) final;
#pragma endregion

	private:
		friend class SyntaxTree;

		/// @brief Create a new syntax tree root
		static SyntaxTreeNodeRoot* Create();

		/// @brief Add the supplied primitive
		/// @param primitive
		void AddPrimitive(SyntaxTreeNodePrimitive* primitive);

		/// @brief Add a new primitive
		/// @param size 
		/// @param primitiveType 
		/// @param name 
		SyntaxTreeNodePrimitive* AddPrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name);

		/// @brief 
		/// @param size 
		/// @param primitiveType 
		/// @param name 
		/// @param inheritsFrom 
		/// @param unrefInto 
		/// @return 
		SyntaxTreeNodePrimitive* AddPrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name,
			SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto);

		/// @param syntaxTree 
		void SetSyntaxTree(SyntaxTree* syntaxTree) { _syntaxTree = syntaxTree; }

	private:
		/// @brief All primitives part of the root node
		Map<ReadOnlyString, SyntaxTreeNodePrimitive*> _primitives;
		/// @brief The syntax tree
		SyntaxTree* _syntaxTree;
	};
}
