#include "Process.h"
#include "Bytecode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

namespace
{
	ReadOnlyString NameFromPackageSignature(ReadOnlyString signature)
	{
		if (signature.empty())
			return signature;

		const Char* bytes = signature.data();
		const Char* end = bytes + signature.size();

		const char* const start = bytes;
		const char* s = end - 1;
		for (; s != start; --s) {
			if (*s == '.') {
				s++;
				break;
			}
		}
		return ReadOnlyString(s, end);
	}

	ReadOnlyString NameFromFunctionSignature(ReadOnlyString signature)
	{
		if (signature.empty())
			return signature;

		const Char* bytes = signature.data();
		const Char* end = bytes + signature.size();

		const char* const start = bytes;
		const char* s = end - 1;
		for (; s != start; --s) {
			if (*s == '#') {
				s++;
				break;
			}
		}
		return ReadOnlyString(s, end);
	}
}

Package::~Package() {
	for (auto f : _functions)
		delete f;
	for (auto g : _globals)
		delete g;
	for (auto t : _types)
		delete t;
}

const Function* Package::FindFunction(ReadOnlyString name) const {
	for (auto& f : _functions) {
		if (f->GetName() == name)
			return f;
	}
	return nullptr;
}

Process::Process()
	: _bytecode(nullptr), _bytecodeDelete(false), _currentThread(nullptr)
{
}

Process::~Process()
{
	if (_bytecode != nullptr && _bytecodeDelete)
	{
		free(_bytecode);
		_bytecode = nullptr;
	}

	for (auto p : _packages)
		delete p;
}

void Process::Load(Bytes bytecode, bool deleteOnExit)
{
	if (bytecode == nullptr)
		throw ProcessErrorFailedToLoad();

	_bytecode = bytecode;
	_bytecodeDelete = deleteOnExit;

	auto header = (const ProcessHeader*)bytecode;
	if (header->header[0] != 'A' || header->header[1] != 'R' || header->header[2] != 'L' || header->header[3] != '0')
		throw ProcessErrorFailedToLoad();
	if (header->version != ARLANG_INTERPRETER_VERSION)
		throw ProcessErrorIncompatibleVersion(header->version);

	// First package information is expected to be after the header
	auto ptr = bytecode + header->packageOffset;

	// Parse each package
	for (int i = 0; i < header->packagesCount; ++i) {
		auto packageHeader = (const ProcessPackageHeader*)ptr;
		const ReadOnlyString packageSignature(bytecode + packageHeader->signatureOffset, packageHeader->signatureLength);
		const ReadOnlyString packageName = NameFromPackageSignature(packageSignature);
		auto package = new Package(packageName, packageSignature, (I32)_packages.Size());
		_packages.Add(package);
		_namedPackages[package->GetSignature()] = package;

		auto typeHeader = (const ProcessTypeHeader*)(bytecode + packageHeader->typesOffset);
		for (int j = 0; j < packageHeader->typesCount; ++j)
		{
			const ReadOnlyString signature(bytecode + typeHeader->signatureOffset, typeHeader->signatureLength);
			const ReadOnlyString name = NameFromPackageSignature(signature);
			auto type = new Type(name, signature, (I32)_types.Size());
			_types.Add(type);
			_namedTypes[signature] = type;
			
			typeHeader = (const ProcessTypeHeader*)(bytecode + typeHeader->offset);
		}

		auto globalHeader = (const ProcessGlobalHeader*)(bytecode + packageHeader->globalsOffset);
		for (int j = 0; j < packageHeader->globalsCount; ++j)
		{
			const ReadOnlyString signature(bytecode + typeHeader->signatureOffset, typeHeader->signatureLength);
			const ReadOnlyString name = NameFromPackageSignature(signature);
			auto type = new Global(name, signature, (I32)_globals.Size(), bytecode + globalHeader->ptr);
			_globals.Add(type);
			_namedGlobals[signature] = type;

			globalHeader = (const ProcessGlobalHeader*)(bytecode + globalHeader->offset);
		}

		auto functionHeader = (const ProcessFunctionHeader*)(bytecode + packageHeader->functionsOffset);
		for (int j = 0; j < packageHeader->functionsCount; ++j)
		{
			const ReadOnlyString signature(bytecode + functionHeader->signatureOffset, functionHeader->signatureLength);
			const ReadOnlyString name = NameFromFunctionSignature(signature);
			auto function = new Function(name, signature, (I32)_functions.Size(), bytecode + functionHeader->entryOffset,
				functionHeader->expectedStackSize);
			_functions.Add(function);
			_namedFunctions[signature] = function;
			package->AddFunction(function);

			functionHeader = (const ProcessFunctionHeader*)(bytecode + functionHeader->offset);
		}

		ptr = bytecode + packageHeader->offset;
	}

	// Figure out the range
	_bytecodeExecutionStart = _bytecode + header->codeOffset;
	_bytecodeExecutionEnd = _bytecodeExecutionStart + header->codeSize;
}

Thread* Process::NewThread() {
	if (_pausedThreads.empty()) {
		auto t = new Thread(this);
		return t;
	}
	auto t = _pausedThreads.top();
	_pausedThreads.pop();
	return t;
}

void Process::Exec(Thread* thread, const Function* entrypoint) {
	if (_currentThread != nullptr) {
		throw ProcessErrorUnsupported("no support for multiple threads yet");
	}

	_currentThread = thread;
	try {
		_currentThread->ExecEntrypoint(entrypoint);
		_currentThread = nullptr;
	}
	catch (const std::exception&) {
		_currentThread = nullptr;
		throw;
	}
}

const Package* Process::FindPackage(ReadOnlyString signature) const {
	const auto it = _namedPackages.find(signature);
	if (it == _namedPackages.end())
		return nullptr;
	return it->second;
}

const Function* Process::FindFunction(ReadOnlyString signature) const {
	const auto it = _namedFunctions.find(signature);
	if (it == _namedFunctions.end())
		return nullptr;
	return it->second;
}

const Global* Process::FindGlobal(ReadOnlyString signature) const {
	const auto it = _namedGlobals.find(signature);
	if (it == _namedGlobals.end())
		return nullptr;
	return it->second;
}
