#pragma once

#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeType.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;

	/// @brief An argument for a function
	class SyntaxTreeNodeFuncArg : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeFuncArg(SourceCodeView view, ReadOnlyString name,
			SyntaxTreeNodeType* type);

		/// @return The name of this argument
		inline ReadOnlyString GetName() const { return _name; }

		/// @return A reference for the type of this argument
		inline SyntaxTreeNodeType* GetType() const { return _type; }

#pragma region SyntaxTreeNode
		void ToString(StringStream& s, int indent) const final;
#pragma endregion

		/// @brief Parse the source code and convert it into a argument
		/// @param state 
		/// @return 
		static SyntaxTreeNodeFuncArg* Parse(ParserState* state);

	private:
		ReadOnlyString _name;
		SyntaxTreeNodeType* const _type;
	};
}
