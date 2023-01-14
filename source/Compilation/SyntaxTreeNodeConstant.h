#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeConstant : public ISyntaxTreeNodeConstant
	{
	public:
		SyntaxTreeNodeConstant(ISyntaxTreeNodeFuncDef* func, SourceCodeView sourceCode, 
			const Interpreter::PrimitiveValue& value)
			: _function(func), _parent(nullptr), _sourceCode(sourceCode), _value(value) {

		}

		// Inherited via ISyntaxTreeNode
		const ID& GetID() const final { return _id; }
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final {
			return ReadOnlyArray<ISyntaxTreeNode*>();
		}
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		const Interpreter::PrimitiveValue& GetValue() const final { return _value; }
		void Compile(Builder::Linker* linker, Builder::Instructions& instructions) final;
		ISyntaxTreeNodeFuncDef* GetFunction() final { return _function; }
		ISyntaxTreeNodePackage* GetPackage() final { return _function->GetPackage(); }
	
	public:
		// Parse the supplied state and return a constant
		static SyntaxTreeNodeConstant* Parse(const ParserState* state);

	private:
		const ID _id;
		ISyntaxTreeNodeFuncDef* const _function;
		ISyntaxTreeNode* _parent;
		const SourceCodeView _sourceCode;
		const Interpreter::PrimitiveValue _value;
	};
}
