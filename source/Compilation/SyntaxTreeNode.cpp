#include "SyntaxTreeNode.h"
#include <atomic>

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

namespace
{
	std::atomic<I32> nextId(0);
}

ID::ID()
	: value(++nextId)
{
}

Vector<ISyntaxTreeNode*> ISyntaxTreeNode::Default::GetSiblingsBefore(const ISyntaxTreeNode* node)
{
	auto parent = node->GetParent();
	if (parent == nullptr)
		return Vector<ISyntaxTreeNode*>();

	auto children = parent->GetChildren();
	Vector<ISyntaxTreeNode*> siblings;
	for (auto child : children) {
		if (child == node)
			break;
		siblings.Add(child);
	}
	return siblings;
}

VisitResult ISyntaxTreeNode::Default::Visit(ISyntaxTreeNode* node, ISyntaxTreeNodeVisitor* visitor, VisitFlags flags)
{
	// Visit this object
	auto result = visitor->Visit(node);
	if (result == VisitorResult::Stop)
		return VisitResult::Stop;

	// Continue search among children if allowed
	if (result != VisitorResult::ContinueExcludeChildren &&
		BIT_ISSET(flags, VisitFlag::IncludeChildren))
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

VisitResult ISyntaxTreeNode::Default::Query(ISyntaxTreeNode* node, ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	// Visit this object
	auto result = visitor->Visit(node);
	if (result == VisitorResult::Stop)
		return VisitResult::Stop;

	if (BIT_ISSET(flags, QuerySearchFlag::Backwards)) {
		// Visit siblings
		auto siblings = node->GetSiblingsBefore();
		for (auto sibling : siblings) {
			switch (sibling->Query(visitor, 0))
			{
			case VisitResult::Stop:
				return VisitResult::Stop;
			default:
				break;
			}
		}
	}

	if (BIT_ISSET(flags, QuerySearchFlag::TraverseParent)) {
		auto parent = node->GetParent();
		if (parent) {
			flags &= ~(QuerySearchFlags)QuerySearchFlag::TraverseChildren;
			switch (parent->Query(visitor, flags))
			{
			case VisitResult::Stop:
				return VisitResult::Stop;
			default:
				break;
			}
		}
	}

	if (BIT_ISSET(flags, QuerySearchFlag::TraverseChildren)) {
		auto children = node->GetChildren();
		for (auto c : children) {
			switch (c->Query(visitor, 0))
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

void ISyntaxTreeNode::Default::ResolveReferences(ISyntaxTreeNode* node)
{
	for (auto child : node->GetChildren())
		child->ResolveReferences();
}

void ISyntaxTreeNode::Default::Compile(ISyntaxTreeNode* node, Builder::Linker* linker)
{
	for (auto child : node->GetChildren())
		child->Compile(linker);
}

void ISyntaxTreeNode::Default::Optimize(ISyntaxTreeNode* node, ISyntaxTreeNodeOptimizer* optimizer)
{
	for (auto child : node->GetChildren())
		child->Optimize(optimizer);
}
