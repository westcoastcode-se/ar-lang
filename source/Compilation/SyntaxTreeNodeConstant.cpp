#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeFuncDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

void SyntaxTreeNodeConstant::ToString(StringStream& s, int indent) const
{
    s << Indent(indent);
    s << "Constant()" << std::endl;
}

ISyntaxTree* WestCoastCode::Compilation::SyntaxTreeNodeConstant::GetSyntaxTree() const
{
    return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* WestCoastCode::Compilation::SyntaxTreeNodeConstant::GetRootNode()
{
    if (_parent)
        return _parent->GetRootNode();
    return this;
}

void SyntaxTreeNodeConstant::SetParent(ISyntaxTreeNode* parent)
{
    _parent = parent;
}

VisitResult WestCoastCode::Compilation::SyntaxTreeNodeConstant::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags)
{
    return VisitResult();
}

SyntaxTreeNodeConstant* SyntaxTreeNodeConstant::Parse(const ParserState* state)
{
    Token* const t = state->token;

    Interpreter::PrimitiveValue value;
    switch (t->GetType())
    {
    case TokenType::Int:
        if (t->GetModifier() == TokenModifier::Negative) {
            value.i64 = t->ToI64();
            value.type = Interpreter::PrimitiveType::I32;
        }
        else {
            value.u64 = t->ToU64();
            value.type = Interpreter::PrimitiveType::I32;
        }
        break;
    case TokenType::Decimal:
        value.f64 = t->ToF64();
        value.type = Interpreter::PrimitiveType::F64;
        break;
    default:
        throw ParseErrorExpectedConstant(state);
    }

    t->Next();
    return new SyntaxTreeNodeConstant(state->function, SourceCodeView(state->sourceCode, t),
        value);
}
