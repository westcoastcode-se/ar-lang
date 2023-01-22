#pragma once

#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeRoot.h"

namespace WestCoastCode::Compilation
{
	/// @brief Tree structure for where the source code is converted and placed into
	class ARLANG_API SyntaxTree
	{
	public:
		SyntaxTree();

		~SyntaxTree();

		/// @brief Convert the syntax tree into a readable string
		/// @param s 
		void ToString(StringStream& s) const;

		/// @brief Visit all nodes in the syntax tree
		/// @param visitor 
		/// @param flags 
		void Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags);

		/// @brief Get the root node for the syntax tree
		/// @return The root node
		inline SyntaxTreeNodeRoot* GetRootNode() { return _root; }

		/// @brief Resolve all unresolved references
		void Resolve();

		/// @brief Compile the syntaxtree and link it together into bytecode
		/// @param linker 
		void Compile(Builder::Linker* linker);

		/// @brief Optimize the syntax tree using the supplied optimizer
		/// @param optimizer 
		void Optimize(ISyntaxTreeNodeOptimizer* optimizer);

	private:
		SyntaxTreeNodeRoot* const _root;
	};
}
