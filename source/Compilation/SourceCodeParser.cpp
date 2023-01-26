#include "SourceCodeParser.h"
#include "SyntaxTreeNodePackage.h"
#include "Functions/SyntaxTreeNodeFuncDef.h"
#include "Functions/SyntaxTreeNodeFuncDefBody.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParserState::ParserState(Compiler* c, Token* t, SourceCode* sc, SyntaxTreeNodePackage* root)
	: compiler(c), token(t), sourceCode(sc), parentNode(root),
	package(root), function(nullptr), functionBody(nullptr) {
}

ParserState::ParserState(const ParserState* state, Token* t)
	: compiler(state->compiler), token(t), sourceCode(state->sourceCode),
	parentNode(state->parentNode), package(state->package), function(state->function), functionBody(state->functionBody) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodePackage* package)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(package), package(package), function(state->function), functionBody(state->functionBody) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodeFuncDef* function)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(function), package(state->package), function(function), functionBody(state->functionBody) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodeFuncDefBody* body)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(body), package(state->package), function(state->function), functionBody(body) {
}
