#pragma once

#include "SyntaxTreeNodeTypeDef.h"
#include "../Primitive.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	/// @brief A primitive type, for example "int32"
	class SyntaxTreeNodePrimitive : public SyntaxTreeNodeTypeDef
	{
	public:
		SyntaxTreeNodePrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name)
			: SyntaxTreeNodeTypeDef(SourceCodeView()), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(nullptr),
			_unrefInto(nullptr), _symbol(nullptr) {}

		SyntaxTreeNodePrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name,
			SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto)
			: SyntaxTreeNodeTypeDef(SourceCodeView()), _stackSize(size), _primitiveType(primitiveType), _name(name), _inheritsFrom(inheritsFrom),
			_unrefInto(unrefInto), _symbol(nullptr) {}

		/// @return The name of this primitive
		inline ReadOnlyString GetName() const { return _name; }

		/// @return The underlying language primitive type
		inline PrimitiveType GetPrimitiveType() const { return _primitiveType; }

		/// @return The memory size of this primitive
		inline I32 GetSize() const { return _stackSize; }

		/// @return The builder symbol for this primitive
		inline Builder::Type* GetSymbol() const { return _symbol; }

#pragma region SyntaxTreeNodeTypeDef
		void Compile(Builder::Linker* linker) final;
		void ToString(StringStream& s, int indent) const final;
#pragma endregion

	private:
		const I32 _stackSize;
		const PrimitiveType _primitiveType;
		const ReadOnlyString _name;
		SyntaxTreeNodePrimitive* const _inheritsFrom;
		SyntaxTreeNodePrimitive* const _unrefInto;
		Builder::Type* _symbol;
	};
}
