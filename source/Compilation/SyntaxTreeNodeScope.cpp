#include "SyntaxTreeNodeScope.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeScope::~SyntaxTreeNodeScope()
{
	for (auto n : _children)
		delete n;
}

void SyntaxTreeNodeScope::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Scope(locals=?)" << std::endl;
	for (auto i : _children)
		i->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeScope::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeScope::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeScope::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeScope::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	for (auto child : _children)
		child->Compile(linker, instructions);
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeScope::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
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

void SyntaxTreeNodeScope::AddOp(ISyntaxTreeNodeOp* node)
{
	_children.Add(node);
	node->SetParent(this);
}
