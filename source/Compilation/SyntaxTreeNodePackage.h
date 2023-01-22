#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents a package
	class SyntaxTreeNodePackage : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodePackage(const SourceCodeView sourceCode, ReadOnlyString name)
			: SyntaxTreeNode(sourceCode), _name(name), _symbol(nullptr) {}

		/// @return The name of the package
		inline ReadOnlyString GetName() { return _name; }


		/// @return The builder symbol for this package
		inline Builder::Package* GetSymbol() const { return _symbol; }

		/// @brief Parse source code into a package node. Will throw ParseError 
		///        if the source code is invalid
		/// @param state 
		/// @return 
		static SyntaxTreeNodePackage* Parse(const ParserState* state);

#pragma region SyntaxTreeNode
		void Compile(Builder::Linker* linker) override;
		VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags) final;
		void ToString(StringStream& s, int indent) const override;
		void OnAddedToParent(SyntaxTreeNode* parent) final;
		void OnChildAdded(SyntaxTreeNode* parent) final;
#pragma endregion

	private:
		ReadOnlyString _name;
		Builder::Package* _symbol;
		Vector<SyntaxTreeNodePackage*> _packages;
	};
}
