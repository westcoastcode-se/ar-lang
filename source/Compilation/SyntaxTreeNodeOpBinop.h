#pragma once

#include "SyntaxTreeNodeOp.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeTypeDef.h"

namespace WestCoastCode::Compilation
{
	// Binary operator node
	class SyntaxTreeNodeOpBinop : public SyntaxTreeNodeOp
	{
	public:
		enum Op {
			Plus,
			Minus,
			Mult,
			Div,
			Equals,
			NotEquals,
			LessThen,
			LessThenEquals,
			GreaterThen,
			GreaterThenEquals,
			BitAnd,
			BitOr,
			BitXor,
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
			case TokenType::OpMult:
				return Mult;
			case TokenType::OpDiv:
				return Div;
			case TokenType::TestEquals:
				return Equals;
			case TokenType::TestNotEquals:
				return NotEquals;
			case TokenType::TestLt:
				return LessThen;
			case TokenType::TestLte:
				return	LessThenEquals;
			case TokenType::TestGt:
				return GreaterThen;
			case TokenType::TestGte:
				return GreaterThenEquals;
			case TokenType::BitAnd:
				return BitAnd;
			case TokenType::BitOr:
				return BitOr;
			case TokenType::BitXor:
				return BitXor;
			default:
				return Unknown;
			}
		}

		// Stringify the operator
		static const ReadOnlyString ToString(const Op op) {
			ENUM_STRING_START(op);
			ENUM_STRING(Plus);
			ENUM_STRING(Minus);
			ENUM_STRING(Mult);
			ENUM_STRING(Div);
			ENUM_STRING(Equals);
			ENUM_STRING(NotEquals);
			ENUM_STRING(LessThen);
			ENUM_STRING(LessThenEquals);
			ENUM_STRING(GreaterThen);
			ENUM_STRING(GreaterThenEquals);
			ENUM_STRING(BitAnd);
			ENUM_STRING_END();
		}

		SyntaxTreeNodeOpBinop(SourceCodeView view, SyntaxTreeNodeFuncBody* body, Op op);

		/// @return The operation at the left side of this binop
		inline SyntaxTreeNodeOp* GetLeft() const { return static_cast<SyntaxTreeNodeOp*>(GetChildren()[0]); }
		
		/// @return The operation at the right side of this binop
		inline SyntaxTreeNodeOp* GetRight() const { return static_cast<SyntaxTreeNodeOp*>(GetChildren()[1]); }

		/// @return What type of operator this binop does
		inline Op GetOperator() const { return _op; }

#pragma region SyntaxTreeNodeOp
		SyntaxTreeNodeTypeDef* GetType() final;
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
#pragma endregion

	private:
		const Op _op;

	public:
		/// @brief Optimizer that merges a binop if the two children are constants
		class Optimize0_Merge : public ISyntaxTreeNodeOptimizer {
		public:
			I32 count = 0;
			Vector<SyntaxTreeNodeOp*> Optimize(SyntaxTreeNodeOp* node) final;
		};
	};
}
