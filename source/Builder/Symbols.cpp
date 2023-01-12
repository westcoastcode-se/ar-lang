#include "Symbols.h"
#include "../Interpreter/Bytecode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;

Package::~Package()
{
	for (auto& t : _types)
		delete t;

	for (auto& g : _globals)
		delete g;

	for (auto& f : _functions)
		delete f;
}

Function* Package::Add(Function* function)
{
	_functions.push_back(function);
	function->SetParent(this);
	return function;
}

Type* Package::Add(Type* type)
{
	_types.push_back(type);
	type->SetParent(this);
	return type;
}

Global* Package::Add(Global* global)
{
	_globals.push_back(global);
	global->SetParent(this);
	return global;
}

I32 Package::ResolveHeaderMemory(I32 offset)
{
	if (_parent != nullptr) {
		_signature = _parent->GetSignature();
		if (!_signature.empty())
			_signature += String(".");
		_signature += _name;
	}
	_signature += _name;

	offset = offset + sizeof(Interpreter::ProcessPackageHeader) + (I32)_signature.size();
	for (auto& t : _types)
		offset = static_cast<Type*>(t)->ResolveHeaderMemory(offset);
	for (auto& g : _globals)
		offset = static_cast<Global*>(g)->ResolveHeaderMemory(offset);
	for (auto& f : _functions)
		offset = static_cast<Function*>(f)->ResolveHeaderMemory(offset);
	return offset;
}

I32 Package::ResolveGlobalVariables(I32 offset)
{
	for (auto& g : _globals)
		offset = static_cast<Global*>(g)->ResolveGlobalVariables(offset);
	return offset;
}

I32 Package::ResolveFunctionBody(I32 offset)
{
	for (auto& f : _functions)
		offset = static_cast<Function*>(f)->ResolveFunctionBody(offset);
	return offset;
}

void Package::SerializeHeader(MemoryStream& stream)
{
	auto header = stream.Reserve<Interpreter::ProcessPackageHeader>();
	header->signatureLength = (I32)_signature.size();
	header->signatureOffset = stream.GetOffset();
	header->typesCount = (I32)_types.size();
	header->globalsCount = (I32)_globals.size();
	header->functionsCount = (I32)_functions.size();
	stream.Write(_signature);

	header->typesOffset = stream.GetOffset();
	for (auto& t : _types)
		t->SerializeHeader(stream);

	header->globalsOffset = stream.GetOffset();
	for (auto& g : _globals)
		g->SerializeHeader(stream);

	header->functionsOffset = stream.GetOffset();
	for (auto& f : _functions)
		f->SerializeHeader(stream);

	header->offset = stream.GetOffset();
}

void Package::Serialize(MemoryStream& stream)
{
	for (auto& t : _types)
		t->Serialize(stream);

	for (auto& g : _globals)
		g->Serialize(stream);

	for (auto& f : _functions)
		f->Serialize(stream);
}

void Package::SerializeReadOnly(MemoryStream& stream)
{
	for (auto& f : _functions)
		f->SerializeReadOnly(stream);
}

Global::~Global()
{
}

void Global::SerializeHeader(MemoryStream& stream)
{
	auto header = stream.Reserve<Interpreter::ProcessGlobalHeader>();
	header->signatureLength = (I32)_signature.size();
	header->signatureOffset = stream.GetOffset();
	header->ptr = _offset;
	header->expectedSize = _type->GetStackSize();
	header->typeOffset = _type->GetOffset();
	stream.Write(_signature);
	header->offset = stream.GetOffset();
}

void Global::Serialize(MemoryStream& stream)
{
	Bytes ptr = stream.Reserve(_type->GetStackSize());
	//memset(ptr, init_value, _type->GetSize());
}

I32 Global::ResolveHeaderMemory(I32 offset)
{
	if (_parent != nullptr) {
		_signature = _parent->GetSignature();
		if (!_signature.empty())
			_signature += String(".");
		_signature += _name;
	}

	return offset + sizeof(Interpreter::ProcessGlobalHeader) + (I32)_signature.size();
}

I32 Global::ResolveGlobalVariables(I32 offset)
{
	_offset = offset;
	return offset + _type->GetStackSize();
}

I32 Type::ResolveHeaderMemory(I32 offset)
{
	if (_parent != nullptr) {
		_signature = _parent->GetSignature();
		if (!_signature.empty())
			_signature += String(".");

	}
	_signature += _name;

	return offset + sizeof(Interpreter::ProcessTypeHeader) + (I32)_signature.size();
}

void Type::SerializeHeader(MemoryStream& stream)
{
	auto header = stream.Reserve<Interpreter::ProcessTypeHeader>();
	header->signatureLength = (I32)_signature.size();
	header->signatureOffset = stream.GetOffset();
	header->expectedStackSize = _stackSize;
	stream.Write(_signature);
	header->offset = stream.GetOffset();
}

void Function::AddArgument(Type* t)
{
	_arguments.push_back(t);
}

void Function::AddReturn(Type* t)
{
	_returns.push_back(t);
}

I32 Function::ResolveHeaderMemory(I32 offset)
{
	if (_parent != nullptr) {
		_signature = _parent->GetSignature();
		_signature += String("#");
		_signature += _name;
		_signature += "(";
		for (int i = 0; i < _arguments.size(); ++i) {
			if (i > 0)
				_signature += ",";
			_signature += _arguments[i]->GetSignature();
		}
		_signature += ")";
	}

	return offset + sizeof(Interpreter::ProcessFunctionHeader) + (I32)_signature.size();
}

I32 Function::ResolveFunctionBody(I32 offset)
{
	_entrypointOffset = offset;
	return _entrypointOffset + _instructions.GetSize() + sizeof(Interpreter::InstrEoe);
}

void Function::SerializeHeader(MemoryStream& stream)
{
	auto header = stream.Reserve<Interpreter::ProcessFunctionHeader>();
	header->signatureLength = (I32)_signature.size();
	header->signatureOffset = stream.GetOffset();
	header->expectedStackSize = GetArgumentsSize();
	header->entryOffset = _entrypointOffset;
	stream.Write(_signature);
	header->offset = stream.GetOffset();
}

void Function::Serialize(MemoryStream& stream)
{
}

void Function::SerializeReadOnly(MemoryStream& stream)
{
	_instructions.Serialize(stream);
	stream.Eoe();
}

I32 Function::GetArgumentsSize() const
{
	I32 size = 0;
	for (auto& t : _arguments)
		size += t->GetStackSize();
	return size;
}

I32 Function::GetReturnsSize() const
{
	I32 size = 0;
	for (auto& t : _returns)
		size += t->GetStackSize();
	return size;
}

Instructions::~Instructions()
{
	for (auto& i : _instructions)
		delete i;
	for (auto& m : _markers)
		delete m;
}

Instr* Instructions::Add(Instr* instr)
{
	_instructions.push_back(instr);
	instr->SetOffset(_stackSize);
	_stackSize += instr->GetSize();
	return instr;
}

void Instructions::End()
{
}

void Instructions::Serialize(MemoryStream& stream)
{
	for (auto& i : _instructions)
		i->Write(stream);
}

InstrAdd* Instructions::Add(Type* type)
{
	return static_cast<InstrAdd*>(Add(new InstrAdd(type)));
}

InstrSub* Instructions::Sub(Type* type)
{
	return static_cast<InstrSub*>(Add(new InstrSub(type)));
}

InstrMult* Instructions::Mult(Type* type)
{
	return static_cast<InstrMult*>(Add(new InstrMult(type)));
}

InstrDiv* Instructions::Div(Type* type)
{
	return static_cast<InstrDiv*>(Add(new InstrDiv(type)));
}

InstrLdc* Instructions::Ldc(Type* stackType, Interpreter::PrimitiveValue value)
{
	return static_cast<InstrLdc*>(Add(new InstrLdc(stackType, value)));
}

InstrRet* Instructions::Ret()
{
	return static_cast<InstrRet*>(Add(new InstrRet(_function)));
}

InstrMarker* Instructions::Marker()
{
	auto marker = new InstrMarker();
	_markers.push_back(marker);
	return marker;
}