#pragma once

#include "SyntaxTree.h"
#include "../Interpreter/Primitive.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	class SyntaxTreeNodePrimitive : public ISyntaxTreeNodePrimitive
	{
	public:
		SyntaxTreeNodePrimitive(SyntaxTreeNodePackage* package, 
			size_t size, Interpreter::PrimitiveType primitiveType, ReadOnlyString name)
			: _package(package), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(nullptr),
			_unrefInto(nullptr) {}

		SyntaxTreeNodePrimitive(SyntaxTreeNodePackage* package,
			size_t size, Interpreter::PrimitiveType primitiveType, ReadOnlyString name,
			SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto)
			: _package(package), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(inheritsFrom),
			_unrefInto(unrefInto) {}

		// Inherited via ISyntaxTreeNodePrimitive
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetParent() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final;
		const SourceCodeView* GetSourceCode() const final { return nullptr; }
		void ToString(StringStream& s, int indent) const final;
		size_t GetSize() const final { return _stackSize; }
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const override;
		virtual bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		virtual bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;

	private:
		SyntaxTreeNodePackage* const _package;
		const size_t _stackSize;
		const Interpreter::PrimitiveType _primitiveType;
		const ReadOnlyString _name;
		SyntaxTreeNodePrimitive* const _inheritsFrom;
		SyntaxTreeNodePrimitive* const _unrefInto;



	};
}
