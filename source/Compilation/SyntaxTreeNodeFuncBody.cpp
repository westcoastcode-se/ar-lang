#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeOpReturn.h"
#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeScope.h"
#include "SyntaxTreeNodeOpTypeCast.h"
#include "SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncBody::SyntaxTreeNodeFuncBody(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* func)
	: _function(func), _parent(nullptr), _sourceCode(sourceCode)
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

void SyntaxTreeNodeFuncBody::Optimize(ISyntaxTreeNodeOptimizer* optimizer)
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
}

void SyntaxTreeNodeFuncBody::AddOp(ISyntaxTreeNodeOp* node)
{
	_children.Add(node);
	node->SetParent(this);
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

	auto body = new SyntaxTreeNodeFuncBody(SourceCodeView(state->sourceCode, t), state->function);
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
	case TokenType::Identity:
		// Identity might be:
		// 1. Variable (local, argument, global, constant)
		// 2. Type (type, primitive, class, package)
		// 3. Function
		// 4. Method
		break;
	case TokenType::ParantLeft: {
		t->Next();

		// It might be a type-cast
		// It might also be a (local1 + 10), so the
		if (t->GetType() == TokenType::Identity) {
			Token peek(t);
			if (peek.Next() == TokenType::ParantRight) {
				auto newType = SyntaxTreeNodeTypeRef::Parse(state);
				t->Next();
				auto cast = new SyntaxTreeNodeOpTypeCast(SourceCodeView(state->sourceCode, t),
					state->function);
				cast->SetNewType(newType);
				auto guard = MemoryGuard(cast);
				cast->SetOp(ParseCompare(state));
				return guard.Done();
			}
		}

		auto node = ParseBody(state);
		if (t->GetType() != TokenType::ParantRight) {
			throw ParseErrorSyntaxError(state, "expected ')'");
		}
		t->Next();
		return node;
	}
	}
	

	return nullptr;
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseUnaryop(ParserState* state, TokenType tokenType,
	ParseFn rightFunc)
{
	Token* const t = state->token;
	auto right = rightFunc(state);
	auto guard = MemoryGuard(right);

	auto unaryop = new SyntaxTreeNodeOpUnaryop(SourceCodeView(state->sourceCode, t),
		state->function, guard.Done(), SyntaxTreeNodeOpUnaryop::FromTokenType(tokenType));
	guard = MemoryGuard(unaryop);

	return guard.Done();
}

ISyntaxTreeNodeOp* SyntaxTreeNodeFuncBody::ParseBinop(ParserState* state, const Vector<TokenType>& types,
	ParseFn leftFunc, ParseFn rightFunc)
{
	Token* const t = state->token;
	if (rightFunc == nullptr)
		rightFunc = leftFunc;

	auto left = leftFunc(state);
	auto guard = MemoryGuard(left);

	while (true) {
		const auto size = types.Size();
		const auto tokenType = t->GetType();
		for (auto i = 0; i < size; ++i) {
			if (types[i] != tokenType)
				continue;
			t->Next();

			auto right = rightFunc(state);
			left = new SyntaxTreeNodeOpBinop(SourceCodeView(state->sourceCode, t), state->function,
				guard.Done(), right, SyntaxTreeNodeOpBinop::FromTokenType(tokenType));
			guard = MemoryGuard(left);
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
