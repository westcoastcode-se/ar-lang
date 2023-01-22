#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;
	class SyntaxTreeNodeFuncBody;
	class SyntaxTreeNodeTypeDef;

	/// @brief Represents an operation executed inside a function
	class ARLANG_API SyntaxTreeNodeOp : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeOp(SourceCodeView view, SyntaxTreeNodeFuncBody* body);

		/// @return The function definition where this operation is part of
		SyntaxTreeNodeFuncDef* GetFunction() const;

		/// @return The function body this operation is part of
		inline SyntaxTreeNodeFuncBody* GetBody() const { return _body; }

		/// @brief Compile this operation and put the bytecode into the supplied instructions target
		/// @param linker 
		/// @param target 
		virtual void Compile(Builder::Linker* linker, Builder::Instructions& target);

		/// @brief Optimize and return zero or more operations that should replace this node
		/// @param optimizer 
		/// @return 
		virtual Vector<SyntaxTreeNodeOp*> OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer);

		/// @return The type which this operator resolves into
		virtual SyntaxTreeNodeTypeDef* GetType() = 0;

#pragma region SyntaxTreeNode
		void Compile(Builder::Linker* linker) final {
			assert(false && "You should call the Compile(linker, instructions) method instead");
		}
		void Optimize(ISyntaxTreeNodeOptimizer* optimizer) final {
			assert(false && "You should call the OptimizeOp(optimizer) method instead");
		}
		void OnChildAdded(SyntaxTreeNode* child) override;
#pragma endregion

	private:
		/// @brief The function body this operation is part of
		SyntaxTreeNodeFuncBody* const _body;
	};
}
