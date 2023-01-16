#pragma once

#include "SyntaxTreeNode.h"
#include "../Primitive.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	class SyntaxTreeNodePrimitive : public ISyntaxTreeNodePrimitive
	{
	public:
		SyntaxTreeNodePrimitive(SyntaxTreeNodePackage* package, 
			I32 size, PrimitiveType primitiveType, ReadOnlyString name)
			: _package(package), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(nullptr),
			_unrefInto(nullptr), _symbol(nullptr) {}

		SyntaxTreeNodePrimitive(SyntaxTreeNodePackage* package,
			I32 size, PrimitiveType primitiveType, ReadOnlyString name,
			SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto)
			: _package(package), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(inheritsFrom),
			_unrefInto(unrefInto), _symbol(nullptr) {}

		// Inherited via ISyntaxTreeNodePrimitive
		const ID& GetID() const final { return _id; }
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetParent() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final;
		const SourceCodeView* GetSourceCode() const final { return nullptr; }
		void ToString(StringStream& s, int indent) const final;
		I32 GetSize() const final { return _stackSize; }
		PrimitiveType GetPrimitiveType() const final { return _primitiveType; }
		virtual ISyntaxTreeNode* GetRootNode() override;
		void Compile(Builder::Linker* linker) final;

		// Get the builder symbol for this primitive
		Builder::Type* GetSymbol() const { return _symbol; }

	private:
		const ID _id;
		SyntaxTreeNodePackage* const _package;
		const I32 _stackSize;
		const PrimitiveType _primitiveType;
		const ReadOnlyString _name;
		SyntaxTreeNodePrimitive* const _inheritsFrom;
		SyntaxTreeNodePrimitive* const _unrefInto;

		Builder::Type* _symbol;

	};
}
