#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeRefSection;

	// A reference node
	class SyntaxTreeNodeRef : public ISyntaxTreeNodeRef
	{
	public:
		SyntaxTreeNodeRef(SourceCodeView sourceCode, DefinitionQueryTypes queryType)
			:_parent(nullptr), _sourceCode(sourceCode), _queryTypes(queryType) {}

		~SyntaxTreeNodeRef() final;

		// Inherited via ISyntaxTreeNodeRef
		const ID& GetID() const final { return _id; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const final { return _definitions; }
		ISyntaxTree* GetSyntaxTree() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTreeNode* GetRootNode() final;
		ReadOnlyString GetName() const final { return _name; }
		DefinitionQueryTypes GetQueryTypes() const final { return _queryTypes; }
		void ResolveReferences() final;

	public:
		// Add a new section node
		void SetSectionNode(SyntaxTreeNodeRefSection* section);

		// Parse
		static SyntaxTreeNodeRef* Parse(const ParserState* state, DefinitionQueryTypes queryType,
			DefinitionQueryTypes sectionTypes);

		// Parse
		static SyntaxTreeNodeRef* Parse(const ParserState* state);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;
		DefinitionQueryTypes _queryTypes;
		Vector<ISyntaxTreeNode*> _definitions;
	};

	// A reference section node. Must be a child of the SyntaxTreeNodeRef type
	class SyntaxTreeNodeRefSection : public ISyntaxTreeNodeRef
	{
	public:
		SyntaxTreeNodeRefSection(SourceCodeView sourceCode, ReadOnlyString name, 
			DefinitionQueryTypes queryTypes)
			:_parent(nullptr), _sourceCode(sourceCode), _name(name), _queryTypes(queryTypes) {}

		~SyntaxTreeNodeRefSection() final;

		// Inherited via ISyntaxTreeNodeRef
		const ID& GetID() const final { return _id; }
		ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const final { return _definitions; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		void ToString(StringStream& s, int indent) const final;
		virtual ISyntaxTree* GetSyntaxTree() const final;
		virtual ISyntaxTreeNode* GetRootNode() final;
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyString GetName() const final { return _name; }
		DefinitionQueryTypes GetQueryTypes() const final { return _queryTypes; }
	
	public:
		// Set the query types
		void SetQueryTypes(DefinitionQueryTypes queryTypes) { _queryTypes = queryTypes; }

		// Add a new section node
		void SetSubSection(SyntaxTreeNodeRefSection* section);

		// Resolve this section from the supplied parent's point of view
		void ResolveFromParent(ISyntaxTreeNode* parent);

		// Parse
		static SyntaxTreeNodeRefSection* Parse(const ParserState* state,
			DefinitionQueryTypes queryTypes);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;
		DefinitionQueryTypes _queryTypes;
		Vector<ISyntaxTreeNode*> _definitions;

	};
}
