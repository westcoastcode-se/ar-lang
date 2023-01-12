#include "SourceCodeParser.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeClassDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParserState::ParserState(Compiler* c, Token* t, SourceCode* sc, SyntaxTreeNodePackage* root)
	: compiler(c), token(t), sourceCode(sc), parentNode(root),
	package(root), function(nullptr), clazz(nullptr) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodePackage* package)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(package), package(package), function(state->function), clazz(state->clazz) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodeFuncDef* function)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(function), package(state->package), function(function), clazz(state->clazz) {
}

ParserState::ParserState(const ParserState* state, SyntaxTreeNodeClassDef* clazz)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(clazz), package(state->package), function(state->function), clazz(clazz) {
}
