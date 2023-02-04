#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeRefSection;

	/// @brief A node that referres to another node. Useful during the compilation phase when we refer to nodes
	///        that aren't parsed yet, for example calling a function or refering to a type
	class SyntaxTreeNodeRef : public SyntaxTreeNode
	{
	public:
		/// @brief Various query types that can be used when searching for specific definitions
		enum DefinitionQueryType : I32
		{
			Package = 1 << 0,
			Class = 1 << 1,
			Func = 1 << 2,
			Arg = 1 << 3,
			Local = 1 << 4,
			Global = 1 << 5,
			Member = 1 << 6,
			Primitive = 1 << 7,
		};
		typedef int DefinitionQueryTypes;

		/// @brief Represents all types
		static constexpr DefinitionQueryTypes Type = Class | Primitive;

		/// @brief Represents all variables that we can read- and write from
		static constexpr DefinitionQueryTypes Variables = Arg | Local | Global | Func;

		/// @brief Represents every node
		static constexpr DefinitionQueryTypes All = INT32_MAX;

		SyntaxTreeNodeRef(SourceCodeView view, DefinitionQueryTypes queryType)
			:SyntaxTreeNode(view), _queryTypes(queryType) {}

		SyntaxTreeNodeRef(SourceCodeView view, DefinitionQueryTypes queryType, ReadOnlyString name)
			:SyntaxTreeNode(view), _queryTypes(queryType), _name(name) {}

		/// @return All definitions found during the resolve phase that matches this reference
		inline ReadOnlyArray<SyntaxTreeNode*> GetDefinitions() const { return _definitions; }

		/// @return The name of the node we are referring to
		inline ReadOnlyString GetName() const { return _name; }

		/// @return What types are we searching for
		DefinitionQueryTypes GetQueryTypes() const { return _queryTypes; }

#pragma region SyntaxTreeNode
		void ToString(StringStream& s, int indent) const final;
		bool Resolve(RecursiveDetector* detector) final;
#pragma endregion

	public:
		/// @brief Parse the source code and convert the content into a reference
		/// @param state 
		/// @param queryType The type of nodes we are looking for
		/// @param sectionTypes Where the node is alllowed to exist
		/// @return 
		static SyntaxTreeNodeRef* Parse(const ParserState* state, DefinitionQueryTypes queryType = All,
			DefinitionQueryTypes sectionTypes = All);

	private:
		/// @brief Resolve definitions for this references based on the supplied parent
		/// @param parent The parent node
		void ResolveFromParent(SyntaxTreeNode* parent);

	private:
		DefinitionQueryTypes _queryTypes;
		ReadOnlyString _name;
		Vector<SyntaxTreeNode*> _definitions;
	};
}
