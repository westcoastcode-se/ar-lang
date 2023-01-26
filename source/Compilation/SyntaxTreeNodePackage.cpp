#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeImport.h"
#include "Functions/SyntaxTreeNodeFuncDef.h"
#include "Functions/SyntaxTreeNodeFuncDefBody.h"
#include "Types/SyntaxTreeNodePrimitive.h"
#include "Compiler.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

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
		for (auto c : GetChildren())
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
	s << GetID() << Indent(indent);
	s << "Package(name=" << _name << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodePackage::OnAddedToParent(SyntaxTreeNode* parent)
{
	assert(dynamic_cast<SyntaxTreeNodePackage*>(parent) != nullptr &&
		"incompatible parent");
}

void SyntaxTreeNodePackage::OnChildAdded(SyntaxTreeNode* parent)
{
	assert((
		dynamic_cast<SyntaxTreeNodePackage*>(parent) != nullptr ||
		dynamic_cast<SyntaxTreeNodeFunc*>(parent) != nullptr ||
		dynamic_cast<SyntaxTreeNodePrimitive*>(parent) != nullptr ||
		dynamic_cast<SyntaxTreeNodeImport*>(parent) != nullptr ||
		dynamic_cast<SyntaxTreeNodeFuncDefBody*>(parent) != nullptr
		) && "incompatible child node");
}

void SyntaxTreeNodePackage::Compile(Builder::Linker* linker)
{
	if (_symbol == nullptr)
		_symbol = linker->AddPackage(new Builder::Package(_name));
	SyntaxTreeNode::Compile(linker);
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

		VisitorResult Visit(SyntaxTreeNode* node) {
			package = dynamic_cast<SyntaxTreeNodePackage*>(node);
			if (package && package->_name == name) {
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
		package = ARLANG_NEW SyntaxTreeNodePackage(SourceCodeView(state->sourceCode, t), t->GetString());
		package->AddChild(ARLANG_NEW SyntaxTreeNodeImport(SourceCodeView(state->sourceCode, t),
			dynamic_cast<SyntaxTreeNodePackage*>(state->parentNode)));
		state->package->AddChild(package);
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
			package->AddChild(funcdef);

			// Parse the function body
			auto childState2 = ParserState(&childState, funcdef);
			auto funcbody = SyntaxTreeNodeFuncDefBody::Parse(&childState2);
			funcdef->SetBody(funcbody);
			package->AddChild(funcbody);
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
