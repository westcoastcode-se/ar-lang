#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	/// @brief A type definition. It might be a struct, a primitive. It might also be a reference to
	///        the real SyntaxTreeNodeTypeDef
	class SyntaxTreeNodeTypeDef : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeTypeDef(SourceCodeView view)
			: SyntaxTreeNode(view) {}

		/// @return The name of this type
		inline ReadOnlyString GetName() const { return _name; }

		/// @brief Fetches the most likely type definition
		/// @return The type definition
		virtual SyntaxTreeNodeTypeDef* GetType() { return this; }

		/// @brief Fetches all potential type definitions
		/// @return A list of all types
		virtual ReadOnlyArray<SyntaxTreeNodeTypeDef*> GetTypes() { return Vector<SyntaxTreeNodeTypeDef*>(this); }

		/// @brief Check if the supplied definition is compatible
		/// @param def 
		/// @return 
		virtual bool IsCompatibleWith(SyntaxTreeNodeTypeDef* def) { return true; }

	protected:
		ReadOnlyString _name;
	};
}
