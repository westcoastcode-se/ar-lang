#include "ID.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

namespace
{
	std::atomic<I32> nextId(0);
}

ID::ID()
	: value(++nextId)
{
}
