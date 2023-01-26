#pragma once

#include "../SyntaxTreeNodeOp.h"

namespace WestCoastCode::Compilation
{
	// Unary operator node
	class SyntaxTreeNodeOpUnaryop : public SyntaxTreeNodeOp
	{
	public:
		enum Op {
			Minus,
			Plus,
			BitNot,
			Unknown
		};

		static Op FromTokenType(TokenType type)
		{
			switch (type)
			{
			case TokenType::OpPlus:
				return Plus;
			case TokenType::OpMinus:
				return Minus;
			case TokenType::BitNot:
				return BitNot;
			default:
				return Unknown;
			}
		}

		// Stringify the operator
		static const ReadOnlyString ToString(const Op op) {
			ENUM_STRING_START(op);
			ENUM_STRING(Plus);
			ENUM_STRING(Minus);
			ENUM_STRING(BitNot);
			ENUM_STRING_END();
		}

		SyntaxTreeNodeOpUnaryop(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body, Op op);

		/// @return The operation at the right side of this binop
		inline SyntaxTreeNodeOp* GetRight() const { return static_cast<SyntaxTreeNodeOp*>(GetChildren()[0]); }

		/// @return The operator
		inline Op GetOperator() const { return _op; }

#pragma region SyntaxTreeNodeOp
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
		SyntaxTreeNodeType* GetType() final;
#pragma endregion

	private:
		Op _op;

	public:
		/// @brief Optimizer that merges a unaryop
		class Optimize0_Merge : public TSyntaxTreeNodeOptimizer<SyntaxTreeNodeOpUnaryop> {
		public:
			I32 count = 0;
			Vector<SyntaxTreeNodeOp*> Optimize(SyntaxTreeNodeOpUnaryop* node) final;
		};
	};
}
