#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeOpReturn.h"
#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodeFuncDef.h"
using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncBody::SyntaxTreeNodeFuncBody(SourceCodeView sourceCode)
	: _function(nullptr), _parent(nullptr), _sourceCode(sourceCode)
{
}

SyntaxTreeNodeFuncBody::~SyntaxTreeNodeFuncBody()
{
	for (auto&& c : _children)
		delete c;
}

void SyntaxTreeNodeFuncBody::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "FuncBody()" << std::endl;
	for (auto&& c : _children)
		c->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeFuncBody::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeFuncBody::SetParent(ISyntaxTreeNode* parent)
{
	_function = dynamic_cast<ISyntaxTreeNodeFuncDef*>(parent);
	assert(_function != nullptr &&
		"Expected the parent to be a function definition");
	_parent = parent;
}

bool SyntaxTreeNodeFuncBody::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeFuncBody::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeFuncBody::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

void SyntaxTreeNodeFuncBody::AddNode(ISyntaxTreeNode* node)
{
	_children.push_back(node);
	node->SetParent(this);
}

SyntaxTreeNodeFuncBody* SyntaxTreeNodeFuncBody::Parse(ParserState* state)
{
	Token* const t = state->token;

	// Expect a function body
	if (t->Next() != TokenType::BracketLeft)
		throw ParseErrorSyntaxError(state, "expected '{'");
	t->Next();

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();

	auto body = new SyntaxTreeNodeFuncBody(SourceCodeView(state->sourceCode, t));
	auto mem = MemoryGuard(body);

	// Parse each body statement
	while (t->Next() != TokenType::BracketLeft)
		body->AddNode(ParseBody(state));


	body->_text = ReadOnlyString(first.data(), t->GetString().data());
	return mem.Done();
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseBody(ParserState* state)
{
	Token* const t = state->token;
	if (t->IsKeyword())
	{
		switch (t->GetType())
		{
		case TokenType::Return:
			return ParseReturn(state);
		default:
			throw ParseErrorNotImplemented(state);
		}
	}

	return ParseCompare(state);
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseReturn(ParserState* state)
{
	Token* const t = state->token;
	t->Next();

	auto op = new SyntaxTreeNodeOpReturn(SourceCodeView(state->sourceCode, t), state->function);
	auto mem = MemoryGuard(op);

	const SyntaxTreeNodeFuncDef* const function = state->function;
	const auto& returns = function->GetReturns();
	auto numReturnsLeft = returns.size();
	while (numReturnsLeft > 0) {
		if (numReturnsLeft > 0) {
			if (t->GetType() != TokenType::Comma)
				throw ParseErrorSyntaxError(state, "expected ,");
			t->Next();
		}
		ISyntaxTreeNode* const node = ParseCompare(state);
		op->AddNode(node);
	}
	return mem.Done();
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseCompare(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() == TokenType::Not) {
		return ParseUnaryop(state, t->GetType(), ParseCompare);
	}
	else {
		static const Vector<TokenType> types{
			TokenType::TestEquals, TokenType::TestNotEquals,
			TokenType::TestLt, TokenType::TestLte, TokenType::TestGt, TokenType::TestGte
		};
		return ParseBinop(state, types, ParseExpr, ParseExpr);
	}
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseExpr(ParserState* state)
{
	static const Vector<TokenType> types{
			   TokenType::OpPlus, TokenType::OpMinus
	};
	return ParseBinop(state, types, ParseTerm, ParseTerm);
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseTerm(ParserState* state)
{
	static const Vector<TokenType> types{
			   TokenType::OpMult, TokenType::OpDiv
	};
	return ParseBinop(state, types, ParseFactor, ParseFactor);
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseFactor(ParserState* state)
{
	Token* const t = state->token;
	switch (t->GetType())
	{
	case TokenType::OpMinus:
	case TokenType::OpPlus:
	case TokenType::BitNot:
		return ParseUnaryop(state, t->GetType(), ParseFactor);
	default:
		return ParseAtom(state);
	}
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseAtom(ParserState* state)
{
	return nullptr;
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseUnaryop(ParserState* state, TokenType tokenType,
	ParseFn rightFunc)
{
	Token* const t = state->token;
	auto right = rightFunc(state);
	auto guard = MemoryGuard(right);

	// Try to merge all children
	// right = arC_syntax_tree_merge_children(s, guard.Done());
	// guard = MemoryGuard(right);

	auto unaryop = new SyntaxTreeNodeOpUnaryop(SourceCodeView(state->sourceCode, t),
		state->function, guard.Done(), SyntaxTreeNodeOpUnaryop::FromTokenType(tokenType));
	guard = MemoryGuard(unaryop);
	// return arC_syntax_tree_merge_children(s, guard.Done());
	return unaryop;
}

ISyntaxTreeNode* SyntaxTreeNodeFuncBody::ParseBinop(ParserState* state, const Vector<TokenType>& types, 
	ParseFn leftFunc, ParseFn rightFunc)
{
	Token* const t = state->token;
	if (rightFunc == nullptr)
		rightFunc = leftFunc;

	auto left = leftFunc(state);
	auto guard = MemoryGuard(left);

	// Try to merge all children
	//left = arC_syntax_tree_merge_children(s, guard.Done());
	//guard = MemoryGuard(left);

	while (true) {
		const auto size = types.size();
		const auto tokenType = t->GetType();
		for (auto i = 0; i < size; ++i) {
			if (types[i] != tokenType)
				continue;
			t->Next();

			auto right = rightFunc(state);

			// Try to merge all children
			//right = arC_syntax_tree_merge_children(s, right);

			auto binop = new SyntaxTreeNodeOpBinop(SourceCodeView(state->sourceCode, t), state->function,
				guard.Done(), right, SyntaxTreeNodeOpBinop::FromTokenType(tokenType));
			left = binop;
			guard = MemoryGuard(left);

			// Try to merge all children
			//left = arC_syntax_tree_merge_children(s, guard.Done());
			//guard = MemoryGuard(left);
		}

		if (size == types.size())
			break;
	}
	return guard.Done();
}

bool SyntaxTreeNodeFuncBody::Contains(const Vector<TokenType>& tokens, TokenType type)
{
	const auto size = tokens.size();
	for (auto i = 0; i < size; ++i) {
		if (tokens[i] == type)
			return true;
	}

	return false;
}
