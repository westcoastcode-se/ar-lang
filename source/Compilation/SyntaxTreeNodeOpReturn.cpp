#include "SyntaxTreeNodeOpReturn.h"
#include "SyntaxTreeNodeFuncBody.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpReturn::~SyntaxTreeNodeOpReturn()
{
	for (auto n : _children)
		delete n;
}

void SyntaxTreeNodeOpReturn::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "OpReturn()" << std::endl;
	for (auto i : _children)
		i->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeOpReturn::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpReturn::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpReturn::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeOpReturn::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	for (auto c : _children)
		c->Compile(linker, instructions);
	instructions.Ret();
}

ISyntaxTreeNodeType* SyntaxTreeNodeOpReturn::GetStackType()
{
	if (_children.Size() == 1) {
		return _children[0]->GetStackType();
	}
	throw CompileErrorNotImplemented(this, "MultiReturn");
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpReturn::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
{
	for (int i = 0; i < _children.Size(); ++i) {
		auto optimized = _children[i]->OptimizeOp(optimizer);
		if (!optimized.IsEmpty()) {
			if (optimized.Size() == 1) {
				delete _children[i];
				_children[i] = static_cast<ISyntaxTreeNodeOp*>(optimized[0]);
			}
			else {
				i += _children.InsertAt(optimized, i);
				delete _children.RemoveAt(i);
				i -= 1;
			}

		}
	}
	return optimizer->Optimize(this);
}

void SyntaxTreeNodeOpReturn::AddOp(ISyntaxTreeNodeOp* node)
{
	_children.Add(node);
	node->SetParent(this);
}
