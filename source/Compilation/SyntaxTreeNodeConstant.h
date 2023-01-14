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
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final {
			return ReadOnlyArray<ISyntaxTreeNode*>();
		}
		virtual VisitResult Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) override;
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		const Interpreter::PrimitiveValue& GetValue() const final { return _value; }
	
	public:
		// Parse the supplied state and return a constant
		static SyntaxTreeNodeConstant* Parse(const ParserState* state);

	private:
		ISyntaxTreeNodeFuncDef* const _function;
		ISyntaxTreeNode* _parent;
		const SourceCodeView _sourceCode;
		const Interpreter::PrimitiveValue _value;
	};
}
