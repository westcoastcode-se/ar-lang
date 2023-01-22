#include "SyntaxTreeNode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNode::SyntaxTreeNode(SourceCodeView view)
	: _parent(nullptr), _children(), _sourceCode(view)
{
}

SyntaxTreeNode::~SyntaxTreeNode()
{
	SyntaxTreeNode::DestroyChildren();
}

ReadOnlyArray<SyntaxTreeNode*> SyntaxTreeNode::GetChildren() const
{ 
	return _children;
}

void SyntaxTreeNode::AddChild(SyntaxTreeNode* node)
{
	_children.Add(node);
	OnChildAdded(node);
	node->SetParent(this);
}

void SyntaxTreeNode::SetParent(SyntaxTreeNode* parent)
{
	_parent = parent;
	OnAddedToParent(parent);
}

void SyntaxTreeNode::Optimize(ISyntaxTreeNodeOptimizer* optimizer)
{
	for (auto n : _children)
		n->Optimize(optimizer);
}

void SyntaxTreeNode::DestroyChildren()
{
	for (auto n : _children)
		delete n;
	_children.Clear();
}

Vector<SyntaxTreeNode*> SyntaxTreeNode::GetSiblingsBefore() const
{
	if (_parent == nullptr)
		return Vector<SyntaxTreeNode*>();

	auto children = _parent->GetChildren();
	Vector<SyntaxTreeNode*> siblings;
	for (auto child : children) {
		if (child == this)
			break;
		siblings.Add(child);
	}
	return siblings;
}

VisitResult SyntaxTreeNode::Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags)
{
	// Visit this object
	auto result = visitor->Visit(this);
	if (result == VisitorResult::Stop)
		return VisitResult::Stop;

	// Continue search among children if allowed
	if (result != VisitorResult::ContinueExcludeChildren &&
		BIT_ISSET(flags, VisitFlag::IncludeChildren))
	{
		for (auto child : _children)
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

VisitResult SyntaxTreeNode::Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	// Visit this object
	auto result = visitor->Visit(this);
	if (result == VisitorResult::Stop)
		return VisitResult::Stop;

	if (BIT_ISSET(flags, QuerySearchFlag::Backwards))
	{
		// Visit siblings
		auto siblings = GetSiblingsBefore();
		for (auto sibling : siblings)
		{
			switch (sibling->Query(visitor, 0))
			{
			case VisitResult::Stop:
				return VisitResult::Stop;
			default:
				break;
			}
		}
	}

	if (BIT_ISSET(flags, QuerySearchFlag::TraverseParent))
	{
		auto parent = _parent;
		if (parent)
		{
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

	if (BIT_ISSET(flags, QuerySearchFlag::TraverseChildren))
	{
		for (auto c : _children)
		{
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

void SyntaxTreeNode::ToString(StringStream& s, int indent) const
{
	for (auto c : _children)
		c->ToString(s, indent + 1);
}

void SyntaxTreeNode::Compile(Builder::Linker* linker)
{
	for (auto c : _children)
		c->Compile(linker);
}

void SyntaxTreeNode::Resolve()
{
	for (auto c : _children)
		c->Resolve();
}

void SyntaxTreeNode::ReplaceChild(I32 index, SyntaxTreeNode* node)
{
	delete _children[index];
	_children[index] = node;
	OnChildAdded(node);
	node->SetParent(this);
}

I32 SyntaxTreeNode::ReplaceChildren(I32 index, ReadOnlyArray<SyntaxTreeNode*> nodes)
{
	index += _children.InsertAt(nodes, index);
	delete _children.RemoveAt(index);
	return index;
}
