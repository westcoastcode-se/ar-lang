#pragma once

#include "../SyntaxTreeNodeOp.h"

namespace WestCoastCode::Compilation
{
	/// @brief Cast one type into another
	class SyntaxTreeNodeOpTypeCast : public SyntaxTreeNodeOp
	{
		SyntaxTreeNodeOpTypeCast(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body);

	public:
		/// @return From which type are we casting
		SyntaxTreeNodeType* FromType();

#pragma region SyntaxTreeNodeOp
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
		SyntaxTreeNodeType* GetType() final;
#pragma endregion

		/// @brief Create a node which casts the supplied operator with the supplied type
		static SyntaxTreeNodeOpTypeCast* Cast(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body,
			SyntaxTreeNodeType* type, SyntaxTreeNodeOp* op);

	public:
		// Optimizer that merges this type-cast with the underlying node
		class Optimize0_Merge : public TSyntaxTreeNodeOptimizer<SyntaxTreeNodeOpTypeCast> {
		public:
			I32 count = 0;
			Vector<SyntaxTreeNodeOp*> Optimize(SyntaxTreeNodeOpTypeCast* node) final;
		};
	};
}
