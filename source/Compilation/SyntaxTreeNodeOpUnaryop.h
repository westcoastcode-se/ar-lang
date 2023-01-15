#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// Unary operator node
	class SyntaxTreeNodeOpUnaryop : public ISyntaxTreeNodeOpUnaryop
	{
	public:
		SyntaxTreeNodeOpUnaryop(SourceCodeView sourceCode,
			ISyntaxTreeNodeFuncDef* function,
			ISyntaxTreeNodeOp* right, Op op)
			: _parent(nullptr), _children(right), _sourceCode(sourceCode), _op(op), _function(function) {}

		~SyntaxTreeNodeOpUnaryop() final;

		// Stringify the operator
		static const ReadOnlyString ToString(const Op op) {
			ENUM_STRING_START(op);
			ENUM_STRING(Plus);
			ENUM_STRING(Minus);
			ENUM_STRING(BitNot);
			ENUM_STRING_END();
		}

		// Inherited via ISyntaxTreeNodeOpUnaryop
		const ID& GetID() const final { return _id; }
		ISyntaxTreeNodeOp* GetRight() const final { return _children[0]; }
		Op GetOperator() const final { return _op; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		void ToString(StringStream& s, int indent) const final;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		ISyntaxTreeNodeFuncDef* GetFunction() final { return _function; }
		ISyntaxTreeNodePackage* GetPackage() final { return _function->GetPackage(); }
		void Compile(Builder::Linker* linker, Builder::Instructions& instructions) final;
		ISyntaxTreeNodeType* GetStackType() final;
		Vector<ISyntaxTreeNodeOp*> OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer) final;

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		Array<ISyntaxTreeNodeOp*, 1> _children;
		SourceCodeView _sourceCode;
		Op _op;
		ISyntaxTreeNodeFuncDef* _function;

	};
}
