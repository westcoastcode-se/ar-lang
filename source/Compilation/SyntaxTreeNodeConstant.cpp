#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodePrimitive.h"
#include "Compiler.h"

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
    s << ",type=" << _stackType->GetName();
    s << ")" << std::endl;
}

ISyntaxTree* SyntaxTreeNodeConstant::GetSyntaxTree() const
{
    return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeConstant::GetRootNode()
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
    auto symbol = static_cast<SyntaxTreeNodePrimitive*>(_stackType)->GetSymbol();
    instructions.Ldc(symbol, _value);
}

SyntaxTreeNodeConstant* SyntaxTreeNodeConstant::Cast(ISyntaxTreeNodeType* newType)
{
    SyntaxTreeNodePrimitive* primitive = nullptr;
    auto ref = dynamic_cast<ISyntaxTreeNodeTypeRef*>(newType);
    if (ref != nullptr) {
        auto definitions = ref->GetDefinitions();
        if (definitions.IsEmpty())
            return nullptr;

        primitive = dynamic_cast<SyntaxTreeNodePrimitive*>(definitions[0]);
    }

    // Is the type a primitive type?
    if (primitive == nullptr) {
        
        auto primitive = dynamic_cast<SyntaxTreeNodePrimitive*>(newType);
        if (primitive == nullptr) {
            return nullptr;
        }
    }

    // Convert the constant value
    Interpreter::PrimitiveValue newValue = _value;
    const auto newPrimitiveType = primitive->GetPrimitiveType();
    //TODO: Convert value if different from current primitive
    newValue.type = newPrimitiveType;
    return new SyntaxTreeNodeConstant(_function, _sourceCode, newValue, primitive);
}

SyntaxTreeNodeConstant* SyntaxTreeNodeConstant::Parse(const ParserState* state)
{
    Token* const t = state->token;

    ISyntaxTreeNodePrimitive* stackType = nullptr;
    Interpreter::PrimitiveValue value;
    switch (t->GetType())
    {
    case TokenType::Bool:
        value.bool4 = t->ToBool();
        value.type = Interpreter::PrimitiveType::Bool;
        stackType = state->compiler->FindPrimitive(ReadOnlyString("bool"));
        break;
    case TokenType::Int:
        if (t->GetModifier() == TokenModifier::Negative) {
            value.i64 = t->ToI64();
            value.type = Interpreter::PrimitiveType::I32;
            stackType = state->compiler->FindPrimitive(ReadOnlyString("int32"));
        }
        else {
            U64 i = t->ToU64();
            if (i > (I64)INT32_MAX) {
                value.u64 = i;
                value.type = Interpreter::PrimitiveType::U32;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("uint32"));
            }
            else {
                value.i64 = (I64)i;
                value.type = Interpreter::PrimitiveType::I32;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("int32"));
            }
        }
        break;
    case TokenType::Decimal:
        value.f64 = t->ToF64();
        value.type = Interpreter::PrimitiveType::F64;
        stackType = state->compiler->FindPrimitive(ReadOnlyString("float64"));
        break;
    default:
        throw ParseErrorExpectedConstant(state);
    }

    t->Next();
    return new SyntaxTreeNodeConstant(state->function, SourceCodeView(state->sourceCode, t),
        value, stackType);
}
