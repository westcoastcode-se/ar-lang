#include "SourceCodeParser.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeClassDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParserState::ParserState(ParserState* state, SyntaxTreeNodePackage* package)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(package), package(package), function(state->function), clazz(state->clazz) {
}

ParserState::ParserState(ParserState* state, SyntaxTreeNodeFuncDef* function)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(function), package(state->package), function(function), clazz(state->clazz) {
}

ParserState::ParserState(ParserState* state, SyntaxTreeNodeClassDef* clazz)
	: compiler(state->compiler), token(state->token), sourceCode(state->sourceCode),
	parentNode(clazz), package(state->package), function(state->function), clazz(clazz) {
}
