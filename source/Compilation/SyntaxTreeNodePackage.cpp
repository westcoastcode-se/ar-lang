#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeImport.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "Compiler.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodePackage::~SyntaxTreeNodePackage()
{
	for (auto i : _children)
		delete i;
}

ISyntaxTreeNode* SyntaxTreeNodePackage::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodePackage::SetParent(ISyntaxTreeNode* parent)
{
	assert(dynamic_cast<SyntaxTreeNodePackage*>(parent) &&
		"A package can only have another package as a parent");
	_parent = dynamic_cast<SyntaxTreeNodePackage*>(parent);
}

VisitResult SyntaxTreeNodePackage::Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	// You don't search backwards from the package and beyond
	flags &= ~(I32)QuerySearchFlag::Backwards;

	// Visit this node
	switch (visitor->Visit(this))
	{
	case VisitorResult::Stop:
		return VisitResult::Stop;
	case VisitorResult::ContinueExcludeChildren:
		flags &= ~(I32)QuerySearchFlag::TraverseChildren;
		break;
	default:
		break;
	}

	// Search among children
	if (flags & (I32)QuerySearchFlag::TraverseChildren)
	{
		// Stop query more child nodes
		flags &= ~(I32)QuerySearchFlag::TraverseChildren;
		for (auto c : _children)
		{
			const auto result = c->Query(visitor, flags);
			if (result == VisitResult::Stop) {
				return VisitResult::Stop;
			}	
		}
	}

	return VisitResult::Continue;
}

void SyntaxTreeNodePackage::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Package(name=" << _name << ")" << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

void SyntaxTreeNodePackage::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}

SyntaxTreeNodePackage* SyntaxTreeNodePackage::Parse(const ParserState* state)
{
	Token* const t = state->token;	
	if (t->Next() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Helper class used for searching for packages
	class PackageVisitor : public ISyntaxTreeNodeVisitor
	{
	public:
		PackageVisitor(ReadOnlyString name) : name(name), package(nullptr) {}

		VisitorResult Visit(ISyntaxTreeNode* node) {
			package = dynamic_cast<SyntaxTreeNodePackage*>(node);
			if (package && package->GetName() == name) {
				return VisitorResult::Stop;
			}
			package = nullptr;
			return VisitorResult::Continue;
		}

		const ReadOnlyString name;
		SyntaxTreeNodePackage* package;
	} visitor(t->GetString());

	// Search for a package in the syntax tree. The package string is a little special because
	// we know that a package definition's parent will be part of the same string, so you can figure out the
	// full signature based on the parent's signatures
	state->parentNode->Query(&visitor, (I32)QuerySearchFlag::TraverseChildren);
	SyntaxTreeNodePackage* package = visitor.package;
	if (package == NULL) {
		package = new SyntaxTreeNodePackage(SourceCodeView(state->sourceCode, t), t->GetString());
		auto rootNode = state->parentNode->GetRootNode();
		package->AddNode(new SyntaxTreeNodeImport(SourceCodeView(state->sourceCode, t), 
			dynamic_cast<SyntaxTreeNodePackage*>(rootNode)));
		state->package->AddNode(package);
	}

	// Package name delimiter
	if (t->Next() == TokenType::Dot) {
		ParserState childState(state, package);
		package = Parse(&childState);
	}

	// Parse tokens that's allowed to be at the package level
	while (true) {
		switch (t->GetType()) {
		case TokenType::Func: {
			auto childState = ParserState(state, package);
			auto funcdef = SyntaxTreeNodeFuncDef::Parse(&childState);
			package->AddNode(funcdef);

			// Parse the function body
			auto childState2 = ParserState(&childState, funcdef);
			funcdef->SetBody(SyntaxTreeNodeFuncBody::Parse(&childState2));
			break;
		}
		case TokenType::Package:
			return package;
		case TokenType::Eof:
			return package;
		default:
			t->Next();
		}
	}

	return package;
}
