#include "SyntaxTreeNode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

VisitResult ISyntaxTreeNode::Default::Visit(ISyntaxTreeNode* node, ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags)
{
	// Visit this object
	auto result = visitor->Visit(node);
	if (result == VisitorResult::Stop)
		return VisitResult::Stop;

	// Continue search among children if allowed
	if (result != VisitorResult::ContinueExcludeChildren &&
		(flags & (I32)VisitFlag::IncludeChildren))
	{
		for (auto child : node->GetChildren())
		{
			switch (child->Visit(visitor, flags))
			{
			case VisitResult::Stop:
				return VisitResult::Stop;
			default:
				break;
			}
		}
	}
	return VisitResult::Continue;
}
