#include "SyntaxTreeNodeFuncDefBody.h"
#include "../Operations/SyntaxTreeNodeOpReturn.h"
#include "../Operations/SyntaxTreeNodeOpBinop.h"
#include "../Operations/SyntaxTreeNodeOpUnaryop.h"
#include "../Operations/SyntaxTreeNodeConstant.h"
#include "../Operations/SyntaxTreeNodeOpScope.h"
#include "../Operations/SyntaxTreeNodeOpTypeCast.h"
#include "../Operations/SyntaxTreeNodeOpCallFunc.h"
#include "../Operations/SyntaxTreeNodeOpReadVar.h"
#include "../Types/SyntaxTreeNodeTypeRef.h"
#include "../Types/SyntaxTreeNodeMultiType.h"
#include "../Functions/SyntaxTreeNodeFuncRef.h"
#include "../Variables/SyntaxTreeNodeVarRef.h"
#include "SyntaxTreeNodeFuncDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

static const Vector<TokenType> PARSE_COMPARE_TOKENS(
	TokenType::TestEquals, TokenType::TestNotEquals,
	TokenType::TestLt, TokenType::TestLte, TokenType::TestGt, TokenType::TestGte
);

static const Vector<TokenType> PARSE_EXPR_TOKENS(
	TokenType::OpPlus, TokenType::OpMinus
);

static const Vector<TokenType> PARSE_TERM_TOKENS(
	TokenType::OpMult, TokenType::OpDiv
);

SyntaxTreeNodeFuncDefBody::SyntaxTreeNodeFuncDefBody(SourceCodeView view, SyntaxTreeNodeFuncDef* func)
	: SyntaxTreeNode(view), _function(func)
{
}

VisitResult SyntaxTreeNodeFuncDefBody::Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	// The function definition is part of the search query
	if (_function->Query(visitor, flags) == VisitResult::Stop) {
		return VisitResult::Stop;
	}

	return SyntaxTreeNode::Query(visitor, flags);
}

void SyntaxTreeNodeFuncDefBody::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "FuncDefBody(definition=" << _function->GetID() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodeFuncDefBody::OnChildAdded(SyntaxTreeNode* child)
{
	assert(dynamic_cast<SyntaxTreeNodeOp*>(child) != nullptr &&
		"children of SyntaxTreeNodeFuncDefBody must be an operation node");
}

void SyntaxTreeNodeFuncDefBody::Compile(Builder::Linker* linker)
{
	auto funcSymbol = _function->GetSymbol();
	Builder::Instructions& target = funcSymbol->Begin();
	for (auto child : GetChildren())
		static_cast<SyntaxTreeNodeOp*>(child)->Compile(linker, target);
	target.End();
}

void SyntaxTreeNodeFuncDefBody::Optimize(ISyntaxTreeNodeOptimizer* optimizer)
{
	auto children = GetChildren();
	for (auto i = 0; i < children.Size(); ++i) {
		auto optimized = static_cast<SyntaxTreeNodeOp*>(children[i])->OptimizeOp(optimizer);
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
}

SyntaxTreeNodeFuncDefBody* SyntaxTreeNodeFuncDefBody::Parse(ParserState* state)
{
	Token* const t = state->token;

	// Expect a function body
	if (t->GetType() != TokenType::BracketLeft)
		throw ParseErrorSyntaxError(state, "expected '{'");
	t->Next();

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();

	auto body = ARLANG_NEW SyntaxTreeNodeFuncDefBody(SourceCodeView(state->sourceCode, t), state->function);
	auto mem = MemoryGuard(body);
	auto scope = ARLANG_NEW SyntaxTreeNodeOpScope(SourceCodeView(state->sourceCode, t), state->functionBody);
	body->AddChild(scope);

	// Parse each body statement
	ParserState innerState(state, body);
	while (t->Next() != TokenType::BracketRight) {
		auto node = ParseBody(&innerState);
		scope->AddChild(node);
	}

	body->_text = ReadOnlyString(first.data(), t->GetString().data());
	return mem.Done();
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseBody(ParserState* state)
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

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseReturn(ParserState* state)
{
	Token* const t = state->token;
	t->Next();

	auto op = ARLANG_NEW SyntaxTreeNodeOpReturn(SourceCodeView(state->sourceCode, t), state->functionBody);
	auto mem = MemoryGuard(op);

	const SyntaxTreeNodeFunc* const function = state->function;
	const auto& returns = function->GetReturnType();
	
	if (dynamic_cast<SyntaxTreeNodeMultiType*>(returns))
	{
		auto multiType = static_cast<SyntaxTreeNodeMultiType*>(returns);
		auto subtypes = multiType->GetTypes();

		for (int i = 0; i < subtypes.Size(); ++i) {
			SyntaxTreeNodeOp* const node = ParseCompare(state);
			op->AddChild(node);

			if (i < subtypes.Size() - 1) {
				if (t->GetType() != TokenType::Comma)
					throw ParseErrorSyntaxError(state, "expected ,");
				t->Next();
			}
		}
	}
	else
	{
		SyntaxTreeNodeOp* const node = ParseCompare(state);
		op->AddChild(node);
	}
	return mem.Done();
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseCompare(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() == TokenType::Not) {
		return ParseUnaryop(state, t->GetType(), ParseFactor);
	}
	else {
		return ParseBinop(state, PARSE_COMPARE_TOKENS, ParseExpr, ParseExpr);
	}
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseExpr(ParserState* state)
{
	return ParseBinop(state, PARSE_EXPR_TOKENS, ParseTerm, ParseTerm);
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseTerm(ParserState* state)
{
	return ParseBinop(state, PARSE_TERM_TOKENS, ParseFactor, ParseFactor);
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseFactor(ParserState* state)
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

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseAtom(ParserState* state)
{
	Token* const t = state->token;
	switch (t->GetType())
	{
	case TokenType::Bool:
	case TokenType::Int:
	case TokenType::Decimal:
		return SyntaxTreeNodeConstant::Parse(state);
	case TokenType::Identity: {
		Token peek(t);
		
		// This can be either:
		// 1. function = identity(
		// 2. template function = identity<
		// 3. variable
		peek.Next();
		if (peek.GetType() == TokenType::ParantLeft) {
			auto funcRef = SyntaxTreeNodeFuncRef::Parse(state);
			if (t->GetType() != TokenType::ParantLeft)
				throw ParseErrorSyntaxError(state, "expected '(");
			auto funcGuard = MemoryGuard(funcRef);
			auto call = ARLANG_NEW SyntaxTreeNodeOpCallFunc(state->GetView(), state->functionBody);
			auto guard = MemoryGuard(call);

			// Extract each argument
			while (t->Next() != TokenType::ParantRight) {
				auto node = ParseCompare(state);
				call->AddChild(node);
				// Is there a comma? Then this means that one more parameter is supplied
				if (t->GetType() == TokenType::Comma) {
					t->Next();
				}
			}
			t->Next();
			call->SetFunction(funcGuard.Done());
			return guard.Done();
		}
		else if (peek.GetType() == TokenType::TestLt) {
			throw ParseErrorNotImplemented(state);
		}
		else if (peek.GetType() == TokenType::Assign) {
			throw ParseErrorNotImplemented(state);
		}
		else if (peek.GetType() == TokenType::DeclAndAssign) {
			throw ParseErrorNotImplemented(state);
		}
		else {
			auto varRef = SyntaxTreeNodeVarRef::Parse(state);
			auto guard = MemoryGuard(varRef);

			auto read = ARLANG_NEW SyntaxTreeNodeOpReadVar(state->GetView(), state->functionBody);
			read->AddChild(guard.Done());
			return read;
		}
		break;
	}
	case TokenType::ParantLeft: {
		t->Next();

		// Is the inner content not an identity, then handle it
		// as a normal ordering operator
		if (t->GetType() != TokenType::Identity) {
			auto node = ParseCompare(state);
			auto guard = MemoryGuard(node);
			if (t->GetType() != TokenType::ParantRight)
				throw ParseErrorSyntaxError(state, "expected ')'");
			t->Next();
			return guard.Done();
		}

		// Handle special case for if the first content of the parantheses is an identity.
		// For example:
		// 1. (Type) = casting
		// 2. (Variable) = variable
		// 3. (Variable + 10) = variable + 10
		// 4. (Type)-10 = casting -10
		// 5. (Variable)-10 = Variable - 10
		// 6. (*Type) = casting to pointer type
		// 7. (&Type) = reference-of type
		// 8. (&value) = pointer-to value

		// It might be a type-cast
		// It might also be a (local1 + 10), so the
		if (t->GetType() == TokenType::Identity) {
			Token peek(t);
			if (peek.Next() == TokenType::ParantRight) {
				auto newType = SyntaxTreeNodeTypeRef::Parse(state);
				auto guard = MemoryGuard(newType);
				t->Next();
				
				auto view = SourceCodeView(state->sourceCode, t);
				auto op = ParseCompare(state);
				auto guard2 = MemoryGuard(op);
				
				return SyntaxTreeNodeOpTypeCast::Cast(view, state->functionBody, guard.Done(), guard2.Done());
			}
		}
	}
	}
	
	throw ParseErrorSyntaxError(state, "Unknown op");
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseUnaryop(ParserState* state, TokenType tokenType,
	ParseFn rightFunc)
{
	Token* const t = state->token;
	t->Next();

	auto right = rightFunc(state);
	auto guard = MemoryGuard(right);

	auto unaryop = ARLANG_NEW SyntaxTreeNodeOpUnaryop(SourceCodeView(state->sourceCode, t),
		state->functionBody, SyntaxTreeNodeOpUnaryop::FromTokenType(tokenType));
	unaryop->AddChild(guard.Done());
	guard = MemoryGuard(unaryop);
	return guard.Done();
}

SyntaxTreeNodeOp* SyntaxTreeNodeFuncDefBody::ParseBinop(ParserState* state, const Vector<TokenType>& types,
	ParseFn leftFunc, ParseFn rightFunc)
{
	Token* const t = state->token;
	if (rightFunc == nullptr)
		rightFunc = leftFunc;

	auto left = leftFunc(state);
	auto guard = MemoryGuard(left);

	const auto size = types.Size();
	for (I32 i = 0; i < size; ++i) {
		const auto tokenType = t->GetType();
		if (types[i] != tokenType)
			continue;
		t->Next();

		auto right = rightFunc(state);
		left = ARLANG_NEW SyntaxTreeNodeOpBinop(SourceCodeView(state->sourceCode, t), 
			state->functionBody,
			SyntaxTreeNodeOpBinop::FromTokenType(tokenType));
		left->AddChild(guard.Done());
		left->AddChild(right);
		guard = MemoryGuard(left);
		
		// Retry parsing. -1 will become 0 after "++i" is done in the for loop
		i = -1; 
	}

	return guard.Done();
}

bool SyntaxTreeNodeFuncDefBody::Contains(const Vector<TokenType>& tokens, TokenType type)
{
	const auto size = tokens.Size();
	for (auto i = 0; i < size; ++i) {
		if (tokens[i] == type)
			return true;
	}

	return false;
}
