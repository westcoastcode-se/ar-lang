#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

void SyntaxTreeNodeConstant::ToString(StringStream& s, int indent) const
{
    s << _id << Indent(indent);
    s << "Constant(value=";
    switch (_value.type)
    {
    case Interpreter::PrimitiveType::I32:
        s << _value.i32;
        break;
    case Interpreter::PrimitiveType::U32:
        s << _value.u32;
        break;
    case Interpreter::PrimitiveType::I64:
        s << _value.i64;
        break;
    case Interpreter::PrimitiveType::U64:
        s << _value.u64;
        break;
    case Interpreter::PrimitiveType::F32:
        s << _value.f32;
        break;
    case Interpreter::PrimitiveType::F64:
        s << _value.f64;
        break;
    default:
        s << "?";
        break;
    }
    s << ")" << std::endl;
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

void SyntaxTreeNodeConstant::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
    //instructions.Ldc(stackType, _value);
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
