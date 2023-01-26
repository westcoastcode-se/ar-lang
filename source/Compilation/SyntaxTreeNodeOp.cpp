#include "SyntaxTreeNodeOp.h"
#include "Functions/SyntaxTreeNodeFuncDefBody.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOp::SyntaxTreeNodeOp(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body)
	: SyntaxTreeNode(view), _body(body)
{
}

SyntaxTreeNodeFuncDef* SyntaxTreeNodeOp::GetFunction() const
{
	return _body->GetFunction();
}

void SyntaxTreeNodeOp::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
	for (auto child : GetChildren())
		static_cast<SyntaxTreeNodeOp*>(child)->Compile(linker, target);
}

Vector<SyntaxTreeNodeOp*> SyntaxTreeNodeOp::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
{
	auto children = GetChildren();
	for (auto i = 0; i < children.Size(); ++i) {
		auto op = dynamic_cast<SyntaxTreeNodeOp*>(children[i]);
		if (op != nullptr) {
			auto optimized = op->OptimizeOp(optimizer);
			if (optimized.IsEmpty())
				continue;
			if (optimized.Size() == 1)
				ReplaceChild(i, optimized[0]);
			else {
				i = ReplaceChildren(i, optimized);
				i--; // Because of the ++i in the for loop
				// Fetch the children again because the underlying memory might've changed
				children = GetChildren();
			}
		}
		else {
			children[i]->Optimize(optimizer);
		}
	}

	if (optimizer->Accept(this))
		return optimizer->Optimize(this);
	else
		return Vector<SyntaxTreeNodeOp*>();
}
