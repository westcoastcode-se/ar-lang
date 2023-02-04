#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNode* RecursiveDetector::Find(const SyntaxTreeNode* node) const
{
	// Search upwards in the syntax tree to see if we found the supplied tree node. If so, then
	// return the first child item that caused the recusion in the first place.
	//
	// Minimal recursion is: A -> B -> A

	// No parent? Then no recursion is possible
	if (parent == nullptr)
		return NULL;

	const RecursiveDetector* prev = this;
	RecursiveDetector* rt = parent;
	while (rt != nullptr) {
		if (rt->node == node) {
			return prev->node;
		}
		prev = rt;
		rt = rt->parent;
	}
	return nullptr;
}

void RecursiveDetector::RaiseErrorIfRecursion(const SyntaxTreeNode* node) const
{
	if (Find(node)) {
		throw CompileErrorRecursionDetected(node);
	}
}

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

void SyntaxTreeNode::DestroyChild(SyntaxTreeNode* child)
{
	auto idx = _children.FindIndex(child);
	if (idx != -1) {
		_children.RemoveAt(idx);
		delete child;
	}
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

	// Query backwards. Do not query siblings if parent is a package and we are allowed to query it
	if (BIT_ISSET(flags, QuerySearchFlag::Backwards))
	{
		bool querySiblings = true;
		if (BIT_ISSET(flags, QuerySearchFlag::TraverseParent)) {
			if (dynamic_cast<SyntaxTreeNodePackage*>(GetParent())) {
				querySiblings = false;
			}
		}

		if (querySiblings) {
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
	}

	if (BIT_ISSET(flags, QuerySearchFlag::TraverseParent))
	{
		auto parent = _parent;
		if (parent)
		{
			//flags &= ~(QuerySearchFlags)QuerySearchFlag::TraverseChildren;
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

bool SyntaxTreeNode::Resolve(RecursiveDetector* detector)
{
	detector->RaiseErrorIfRecursion(this);
	RecursiveDetector childDetector(detector, this);
	for (auto c : _children)
		c->Resolve(&childDetector);
	return true;
}

void SyntaxTreeNode::ReplaceChild(I32 index, SyntaxTreeNode* node)
{
	delete _children[index];
	_children[index] = node;
	OnChildAdded(node);
	node->SetParent(this);
}

void SyntaxTreeNode::ReplaceChild(SyntaxTreeNode* old, SyntaxTreeNode* node)
{
	auto idx = _children.FindIndex(old);
	if (idx == -1)
		return;
	_children[idx] = node;
	delete old;
	OnChildAdded(node);
	node->SetParent(this);
}

I32 SyntaxTreeNode::ReplaceChildren(I32 index, ReadOnlyArray<SyntaxTreeNode*> nodes)
{
	index += _children.InsertAt(nodes, index);
	delete _children.RemoveAt(index);
	return index;
}
