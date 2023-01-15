#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeOpReturn.h"
#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeScope.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncBody::SyntaxTreeNodeFuncBody(SourceCodeView sourceCode)
	: _function(nullptr), _parent(nullptr), _sourceCode(sourceCode)
{
}

SyntaxTreeNodeFuncBody::~SyntaxTreeNodeFuncBody()
{
	for (auto c : _children)
		delete c;
}

void SyntaxTreeNodeFuncBody::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "FuncBody(definition=" << _function->GetID() << ")" << std::endl;
	for (auto c : _children)
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
	_parent = parent;
}

void SyntaxTreeNodeFuncBody::Compile(Builder::Linker* linker)
{
	auto funcSymbol = static_cast<SyntaxTreeNodeFuncDef*>(_function)->GetSymbol();
	Builder::Instructions& instructions = funcSymbol->Begin();
	for (auto child : _children)
		child->Compile(linker, instructions);
	instructions.End();
}

void SyntaxTreeNodeFuncBody::AddOp(ISyntaxTreeNodeOp* node)
{
	_children.Add(node);
	node->SetParent(this);
}

void SyntaxTreeNodeFuncBody::SetFunction(ISyntaxTreeNodeFuncDef* funcdef)
{
	assert(_function == nullptr &&
		"a function body cannot have two definitions");
	_function = funcdef;
}

SyntaxTreeNodeFuncBody* SyntaxTreeNodeFuncBody::Parse(ParserState* state)
{
	Token* const t = state->token;

	// Expect a function body
	if (t->GetType() != TokenType::BracketLeft)
		throw ParseErrorSyntaxError(state, "expected '{'");
	t->Next();

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();

	auto body = new SyntaxTreeNodeFuncBody(SourceCodeView(state->sourceCode, t));
	auto mem = MemoryGuard(body);
	auto scope = new SyntaxTreeNodeScope(SourceCodeView(state->sourceCode, t), state->function);
	body->AddOp(scope);

	// Parse each body statement
	while (t->Next() != TokenType::BracketRight) {
		auto node = ParseBody(state);
		scope->AddOp(node);
	}

	body->_text = ReadOnlyString(first.data(), t->GetString().data());
	return mem.Done();
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseBody(ParserState* state)
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

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseReturn(ParserState* state)
{
	Token* const t = state->token;
	t->Next();

	auto op = new SyntaxTreeNodeOpReturn(SourceCodeView(state->sourceCode, t), state->function);
	auto mem = MemoryGuard(op);

	const SyntaxTreeNodeFuncDef* const function = state->function;
	const auto& returns = function->GetReturnType();
	
	if (dynamic_cast<ISyntaxTreeNodeMultiType*>(returns))
	{
		auto multiType = static_cast<ISyntaxTreeNodeMultiType*>(returns);
		auto subtypes = multiType->GetTypes();

		for (int i = 0; i < subtypes.Size(); ++i) {
			ISyntaxTreeNodeOp* const node = ParseCompare(state);
			auto nodeType = node->GetStackType();
			if (!nodeType->IsCompatibleWith(subtypes[i])) {
				throw ParseErrorIncompatibleTypes(state, nodeType, subtypes[i]);
			}
			op->AddOp(node);

			if (i < subtypes.Size() - 1) {
				if (t->GetType() != TokenType::Comma)
					throw ParseErrorSyntaxError(state, "expected ,");
				t->Next();
			}
		}
	}
	else
	{
		ISyntaxTreeNodeOp* const node = ParseCompare(state);
		auto nodeType = node->GetStackType();
		if (!nodeType->IsCompatibleWith(returns)) {
			throw ParseErrorIncompatibleTypes(state, nodeType, returns);
		}
		op->AddOp(node);
	}
	return mem.Done();
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseCompare(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() == TokenType::Not) {
		return ParseUnaryop(state, t->GetType(), ParseCompare);
	}
	else {
		static const Vector<TokenType> types(
			TokenType::TestEquals, TokenType::TestNotEquals,
			TokenType::TestLt, TokenType::TestLte, TokenType::TestGt, TokenType::TestGte
		);
		return ParseBinop(state, types, ParseExpr, ParseExpr);
	}
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseExpr(ParserState* state)
{
	static const Vector<TokenType> types(
			   TokenType::OpPlus, TokenType::OpMinus
	);
	return ParseBinop(state, types, ParseTerm, ParseTerm);
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseTerm(ParserState* state)
{
	static const Vector<TokenType> types(
			   TokenType::OpMult, TokenType::OpDiv
	);
	return ParseBinop(state, types, ParseFactor, ParseFactor);
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseFactor(ParserState* state)
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

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseAtom(ParserState* state)
{
	Token* const t = state->token;
	switch (t->GetType())
	{
	case TokenType::Int:
		return SyntaxTreeNodeConstant::Parse(state);
	}
	

	return nullptr;
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseUnaryop(ParserState* state, TokenType tokenType,
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

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseBinop(ParserState* state, const Vector<TokenType>& types,
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
		const auto size = types.Size();
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

		if (size == types.Size())
			break;
	}
	return guard.Done();
}

bool SyntaxTreeNodeFuncBody::Contains(const Vector<TokenType>& tokens, TokenType type)
{
	const auto size = tokens.Size();
	for (auto i = 0; i < size; ++i) {
		if (tokens[i] == type)
			return true;
	}

	return false;
}
