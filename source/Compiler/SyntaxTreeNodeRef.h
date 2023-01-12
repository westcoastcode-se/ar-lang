#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeRefSection;

	// A reference node
	class SyntaxTreeNodeRef : public ISyntaxTreeNodeRef
	{
	public:
		SyntaxTreeNodeRef(SourceCodeView sourceCode, DefinitionQueryType queryType)
			:_parent(nullptr), _sourceCode(sourceCode), _queryTypes((DefinitionQueryTypes)queryType) {}

		SyntaxTreeNodeRef(SourceCodeView sourceCode, DefinitionQueryTypes queryTypes)
			:_parent(nullptr), _sourceCode(sourceCode), _queryTypes(queryTypes) {}

		~SyntaxTreeNodeRef() final;

		// Inherited via ISyntaxTreeNodeRef
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		const Vector<ISyntaxTreeNode*>& GetDefinitions() const final { return _definitions; }
		ISyntaxTree* GetSyntaxTree() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const final;
		bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTreeNode* GetRootNode() final;
		ReadOnlyString GetName() const final { return _name; }
		DefinitionQueryTypes GetQueryTypes() const final { return _queryTypes; }

	public:
		// Add a new section node
		void AddNode(SyntaxTreeNodeRefSection* section);

		// Parse
		static SyntaxTreeNodeRef* Parse(ParserState* state, DefinitionQueryType queryType,
			DefinitionQueryTypes sectionTypes);

		// Parse
		static SyntaxTreeNodeRef* Parse(ParserState* state);

	private:
		ISyntaxTreeNode* _parent;
		List<ISyntaxTreeNode*> _children;
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
			DefinitionQueryType queryType)
			:_parent(nullptr), _sourceCode(sourceCode), _name(name), _queryTypes((DefinitionQueryTypes)queryType) {}

		SyntaxTreeNodeRefSection(SourceCodeView sourceCode, ReadOnlyString name, 
			DefinitionQueryTypes queryTypes)
			:_parent(nullptr), _sourceCode(sourceCode), _name(name), _queryTypes(queryTypes) {}

		~SyntaxTreeNodeRefSection() final;

		// Inherited via ISyntaxTreeNodeRef
		const Vector<ISyntaxTreeNode*>& GetDefinitions() const final { return _definitions; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		void ToString(StringStream& s, int indent) const final;
		virtual ISyntaxTree* GetSyntaxTree() const final;
		virtual ISyntaxTreeNode* GetRootNode() final;
		void SetParent(ISyntaxTreeNode* parent) final;
		bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const final;
		bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		ReadOnlyString GetName() const final { return _name; }
		DefinitionQueryTypes GetQueryTypes() const final { return _queryTypes; }
	
	public:
		// Set the query types
		void SetQueryTypes(DefinitionQueryTypes queryTypes) { _queryTypes = queryTypes; }

		// Add a new section node
		void AddNode(SyntaxTreeNodeRefSection* section);

		// Parse
		static SyntaxTreeNodeRefSection* Parse(ParserState* state,
			DefinitionQueryTypes queryTypes);

	private:
		ISyntaxTreeNode* _parent;
		List<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;
		DefinitionQueryTypes _queryTypes;
		Vector<ISyntaxTreeNode*> _definitions;

	};
}
