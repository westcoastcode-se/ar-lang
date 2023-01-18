#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodePrimitive.h"
#include "Compiler.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

namespace
{
    void Convert(PrimitiveValue& value,
        PrimitiveType from, PrimitiveType to) {
        value.type = to;
        if (from == PrimitiveType::I64) {
            switch (to)
            {
            case PrimitiveType::F32:
                value.f32 = (F32)value.i64;
                return;
            case PrimitiveType::F64:
                value.f64 = (F64)value.i64;
                return;
            }
        }
        else if (from == PrimitiveType::U64) {
            switch (to)
            {
            case PrimitiveType::F32:
                value.f32 = (F32)value.u64;
                return;
            case PrimitiveType::F64:
                value.f64 = (F64)value.u64;
                return;
            }
        }
        else if (from == PrimitiveType::I32) {
            switch (to)
            {
            case PrimitiveType::I64:
                value.i64 = (I64)value.i32;
                return;
            case PrimitiveType::F32:
                value.f32 = (F32)value.i32;
                return;
            case PrimitiveType::F64:
                value.f64 = (F64)value.i32;
                return;
            }
        }
        else if (from == PrimitiveType::U32) {
            switch (to)
            {
            case PrimitiveType::F32:
                value.f32 = (F32)value.u32;
                return;
            case PrimitiveType::F64:
                value.f64 = (F64)value.u32;
                return;
            }
        }
        else if (from == PrimitiveType::F64) {
            switch (to)
            {
            case PrimitiveType::F32:
                value.f32 = (F32)value.f64;
                return;
            }
        }
    }
}

void SyntaxTreeNodeConstant::ToString(StringStream& s, int indent) const
{
    s << _id << Indent(indent);
    s << "Constant(value=";
    switch (_value.type)
    {
    case PrimitiveType::I16:
        s << _value.i16;
        break;
    case PrimitiveType::I32:
        s << _value.i32;
        break;
    case PrimitiveType::U32:
        s << _value.u32;
        break;
    case PrimitiveType::I64:
        s << _value.i64;
        break;
    case PrimitiveType::U64:
        s << _value.u64;
        break;
    case PrimitiveType::F32:
        s << _value.f32;
        break;
    case PrimitiveType::F64:
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
    // Figure out the new constant value by compile-time evaluating the expression
    // into a new type
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
    PrimitiveValue newValue = _value;
    Convert(newValue, static_cast<SyntaxTreeNodePrimitive*>(_stackType)->GetPrimitiveType(), 
        primitive->GetPrimitiveType());
    return ARLANG_NEW SyntaxTreeNodeConstant(_function, _sourceCode, newValue, primitive);
}

SyntaxTreeNodeConstant* SyntaxTreeNodeConstant::Parse(const ParserState* state)
{
    Token* const t = state->token;

    ISyntaxTreeNodePrimitive* stackType = nullptr;
    PrimitiveValue value;
    switch (t->GetType())
    {
    case TokenType::Bool:
        value.bool4 = t->ToBool();
        value.type = PrimitiveType::Bool;
        stackType = state->compiler->FindPrimitive(ReadOnlyString("bool"));
        break;
    case TokenType::Int:
        if (t->GetModifier() == TokenModifier::HintUnsigned)
        {
            value.u64 = t->ToU64();
            if (value.u64 > (U64)UINT32_MAX)
            {
                value.type = PrimitiveType::U64;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("uint64"));
            }
            else {
                value.u32 = (U32)value.u64;
                value.type = PrimitiveType::U32;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("uint32"));
            }
        }
        else
        {
            value.i64 = t->ToI64();
            if (value.i64 > (I64)INT32_MAX) {
                value.type = PrimitiveType::I64;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("int64"));
            }
            else {
                value.i32 = (I32)value.i64;
                value.type = PrimitiveType::I32;
                stackType = state->compiler->FindPrimitive(ReadOnlyString("int32"));
            }
        }
        break;
    case TokenType::Decimal:
        if (t->GetModifier() == TokenModifier::HintFloat)
        {
            value.f32 = t->ToF32();
            value.type = PrimitiveType::F32;
            stackType = state->compiler->FindPrimitive(ReadOnlyString("float32"));
        }
        else
        {
            value.f64 = t->ToF64();
            value.type = PrimitiveType::F64;
            stackType = state->compiler->FindPrimitive(ReadOnlyString("float64"));
        }

        break;
    default:
        throw ParseErrorExpectedConstant(state);
    }

    t->Next();
    return ARLANG_NEW SyntaxTreeNodeConstant(state->function, SourceCodeView(state->sourceCode, t),
        value, stackType);
}
