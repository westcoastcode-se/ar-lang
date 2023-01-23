#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	/// @brief A type definition. It might be a struct, a primitive. It might also be a reference to
	///        the real SyntaxTreeNodeType
	class SyntaxTreeNodeType : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeType(SourceCodeView view)
			: SyntaxTreeNode(view) {}

		/// @return The name of the type
		virtual ReadOnlyString GetName() const { return ReadOnlyString(); }

		/// @brief Fetches the most likely type definition
		/// @return The type definition
		virtual SyntaxTreeNodeType* GetType() { return this; }

		/// @brief Fetches all potential type definitions
		/// @return A list of all types
		virtual ReadOnlyArray<SyntaxTreeNodeType*> GetTypes() { return Vector<SyntaxTreeNodeType*>(this); }

		/// @brief Check if the supplied definition is compatible enough for the compiler
		///        to perform compile-time casting on it
		/// @param def 
		/// @return 
		/// 
		virtual bool IsCompatibleWith(SyntaxTreeNodeType* def) { return false; }
	};
}
