#pragma once

#include "../SyntaxTreeNodeType.h"
#include "../../Primitive.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	/// @brief A primitive type, for example "int32"
	class SyntaxTreeNodePrimitive : public SyntaxTreeNodeType
	{
	public:
		SyntaxTreeNodePrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name)
			: SyntaxTreeNodeType(SourceCodeView()), _stackSize(size), _primitiveType(primitiveType), _inheritsFrom(nullptr),
			_unrefInto(nullptr), _symbol(nullptr) {
			_name = name;
		}

		SyntaxTreeNodePrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name,
			SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto)
			: SyntaxTreeNodeType(SourceCodeView()), _stackSize(size), _primitiveType(primitiveType), _inheritsFrom(inheritsFrom),
			_unrefInto(unrefInto), _symbol(nullptr) {
			_name = name;
		}

		/// @return The underlying language primitive type
		inline PrimitiveType GetPrimitiveType() const { return _primitiveType; }

		/// @return The memory size of this primitive
		inline I32 GetSize() const { return _stackSize; }

		/// @return The builder symbol for this primitive
		inline Builder::Type* GetSymbol() const { return _symbol; }

#pragma region SyntaxTreeNodeType
		void Compile(Builder::Linker* linker) final;
		void ToString(StringStream& s, int indent) const final;
		bool IsCompatibleWith(SyntaxTreeNodeType* def) final;
#pragma endregion

	private:
		const I32 _stackSize;
		const PrimitiveType _primitiveType;
		SyntaxTreeNodePrimitive* const _inheritsFrom;
		SyntaxTreeNodePrimitive* const _unrefInto;
		Builder::Type* _symbol;
	};
}
