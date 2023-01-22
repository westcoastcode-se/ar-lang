#include "SyntaxTreeNodeOp.h"
#include "SyntaxTreeNodePrimitive.h"

namespace WestCoastCode::Compilation
{
	/// @brief A constant
	class SyntaxTreeNodeConstant : public SyntaxTreeNodeOp
	{
	public:
		SyntaxTreeNodeConstant(SourceCodeView view, SyntaxTreeNodeFuncBody* body,
			const PrimitiveValue& value, SyntaxTreeNodePrimitive* stackType);

		/// @return The constant value
		inline const PrimitiveValue& GetValue() const { return _value; }

#pragma region SyntaxTreeNodeOp
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& instructions) final;
		inline SyntaxTreeNodeTypeDef* GetType() final { return _stackType; }
#pragma endregion

	public:
		// Try to cast this constant into a new constant
		SyntaxTreeNodeConstant* Cast(SyntaxTreeNodeTypeDef* newType);

		// Parse the supplied state and return a constant
		static SyntaxTreeNodeConstant* Parse(const ParserState* state);

	private:
		const PrimitiveValue _value;
		SyntaxTreeNodePrimitive* const _stackType;
	};
}
