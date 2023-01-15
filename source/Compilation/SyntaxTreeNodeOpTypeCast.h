#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeOpTypeCast : public ISyntaxTreeNodeOpTypeCast
	{
	public:
		SyntaxTreeNodeOpTypeCast(SourceCodeView sourceCode,
			ISyntaxTreeNodeFuncDef* func)
			: _sourceCode(sourceCode), _function(func), _parent(nullptr), _children(nullptr, nullptr) {}

		~SyntaxTreeNodeOpTypeCast() final;

		// Inherited via ISyntaxTreeNodeOpTypeCast
		virtual void ToString(StringStream& s, int indent) const override;
		virtual const ID& GetID() const final { return _id; }
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual ISyntaxTreeNode* GetParent() const final { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		virtual const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		virtual void Compile(Builder::Linker* linker, Builder::Instructions& instructions) override;
		virtual ISyntaxTreeNodeFuncDef* GetFunction() final { return _function; }
		virtual ISyntaxTreeNodePackage* GetPackage() final { return _function->GetPackage(); }
		virtual ISyntaxTreeNodeType* GetStackType() final;
		virtual ISyntaxTreeNodeType* FromType() final;
		Vector<ISyntaxTreeNodeOp*> OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer) final;

		// Set the new type to cast to
		void SetNewType(ISyntaxTreeNodeType* newType);

		// Add operation that we want to cast the result
		void SetOp(ISyntaxTreeNodeOp* op);

	private:
		const ID _id;
		const SourceCodeView _sourceCode;
		ISyntaxTreeNodeFuncDef* const _function;
		ISyntaxTreeNode* _parent;
		Array<ISyntaxTreeNode*, 2> _children;

	public:
		// Optimizer that merges this type-cast with the underlying node
		class Optimize0_Merge : public ISyntaxTreeNodeOptimizer {
		public:
			Vector<ISyntaxTreeNodeOp*> Optimize(ISyntaxTreeNodeOp* node) final;
		};
	};
}
