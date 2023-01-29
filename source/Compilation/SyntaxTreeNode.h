#pragma once

#include "../Memory.h"
#include "../Array.h"
#include "SourceCodeView.h"
#include "SourceCodeParser.h"
#include "ParseError.h"
#include "CompileError.h"
#include "../Primitive.h"
#include "../Builder/Linker.h"
#include "ID.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTree;
	class SyntaxTreeNode;

	// Flags used to help configure the search algorithm when using the Query functionality
	enum class QuerySearchFlag : int
	{
		// Searching backwards from the current node. Useful for searching for variables
		// declared before the current node in a function
		Backwards = 1 << 0,

		// Include imports when searching for nodes
		TraverseImports = 1 << 1,

		// Traverse child-nodes
		TraverseChildren = 1 << 2,

		// Traverse upwards to parent etc.
		TraverseParent = 1 << 3
	};
	typedef int QuerySearchFlags;

	// Flags used to help configure the search algorithm when using the Visit functionality
	enum class VisitFlag : int
	{
		// Include more children
		IncludeChildren = 1 << 0
	};
	typedef int VisitFlags;

	enum class VisitorResult
	{
		// Stop querying for any more nodes
		Stop,

		// Continue querying for more nodes
		Continue,

		// Continue query but also exclude children
		ContinueExcludeChildren,
	};

	enum class VisitResult
	{
		// Stop querying for any more nodes
		Stop,

		// Continue querying for more nodes
		Continue,
	};

	class SyntaxTreeNode;

	/// Interface used for implementing the visitor pattern when you want to traverse the syntax tree
	class ARLANG_API ISyntaxTreeNodeVisitor
	{
	public:
		virtual ~ISyntaxTreeNodeVisitor() = default;

		/// @brief Method called when a node is visited
		/// @param node The node we are visiting
		/// @return If we should continue visit more nodes or stop querying
		virtual VisitorResult Visit(SyntaxTreeNode* node) = 0;
	};

	class SyntaxTreeNodeOp;

	/// @brief Override this to add additional optimization capabilities. The optimization is always done bottom up
	class ARLANG_API ISyntaxTreeNodeOptimizer
	{
	public:
		virtual ~ISyntaxTreeNodeOptimizer() = default;

		/// @brief Check to see if the supplied node is accepted by this optimizer
		/// @param node 
		/// @return 
		virtual bool Accept(SyntaxTreeNodeOp* node) = 0;

		/// @brief Optimize the supplied node. If optimized then a list containing the new/optimized values are returned
		/// @param node The node we are trying to optimize
		/// @return The new optimized nodes
		virtual Vector<SyntaxTreeNodeOp*> Optimize(SyntaxTreeNodeOp* node) = 0;
	};

	/// @brief 
	/// @tparam T 
	template<class T>
	class ARLANG_API TSyntaxTreeNodeOptimizer : public ISyntaxTreeNodeOptimizer
	{
	public:
		bool Accept(SyntaxTreeNodeOp* node) final { return dynamic_cast<T*>(node) != nullptr; }
		Vector<SyntaxTreeNodeOp*> Optimize(SyntaxTreeNodeOp* node) final { return Optimize(static_cast<T*>(node)); }
		
		/// @brief Specialized optimize method
		/// @param node 
		/// @return 
		virtual Vector<SyntaxTreeNodeOp*> Optimize(T* node) = 0;
	};

	/// @brief Implement this interface if you want to have support for converting to a string
	class ARLANG_API IStringify : public IMemoryTracked
	{
	public:
		/// @brief Stringify this object
		/// @param s 
		/// @param indent 
		virtual void ToString(StringStream& s, int indent) const = 0;

		/// @brief Helper function for indentations. Returns a string with the appropriate spaces
		/// @param indent How many spaces should be added
		/// @return 
		inline static String Indent(int indent) {
			String s;
			for (int i = 0; i < indent; ++i)
				s += "  ";
			return s;
		}

		/// @brief Indent the supplied stream
		/// @param ss 
		/// @param indent 
		inline static void Indent(StringStream& ss, int indent) {
			for (int i = 0; i < indent; ++i)
				ss << " ";
		}
	};

	/// @brief Implement this interface if you want something to be able to be compiled
	///        into bytecode
	class ARLANG_API ICompilable
	{
	public:
		virtual ~ICompilable() = default;

		/// @brief Compile this package into symbols that can be linked into bytecode later on
		/// @param linker The linker
		virtual void Compile(Builder::Linker* linker) = 0;
	};

	/// @brief Type that helps with detecting when recursion between two nodes happens
	struct ARLANG_API RecursiveDetector
	{
		RecursiveDetector* root;
		RecursiveDetector* parent;
		SyntaxTreeNode* node;

		RecursiveDetector(SyntaxTreeNode* node)
			: root(nullptr), parent(nullptr), node(node) {}

		RecursiveDetector(RecursiveDetector* parent, SyntaxTreeNode* node)
			: root(parent->root), parent(parent), node(node) {}

		/// @brief Search for the supplied node
		/// @param node 
		/// @return true if recursion is detected
		SyntaxTreeNode* Find(const SyntaxTreeNode* node) const;

		/// @brief Raise an error if recursion is detected
		/// @param node 
		void RaiseErrorIfRecursion(const SyntaxTreeNode* node) const;
	};

	/// @brief Base class for a syntax tree nodes
	class ARLANG_API SyntaxTreeNode : public IStringify, public ICompilable
	{
	public:
		SyntaxTreeNode(const SourceCodeView view);

		virtual ~SyntaxTreeNode();

		/// @return A runtime-unique ID
		inline const ID& GetID() const { return _id; }

		/// @return Get the source code which this node is created from
		inline const SourceCodeView& GetSourceCode() const { return _sourceCode; }

		/// @return The tree that this node is part of - if any. Returns nullptr if this node isn't part of any tree
		virtual SyntaxTree* GetSyntaxTree() const { 
			if (_parent)
				return _parent->GetSyntaxTree();
			return nullptr; 
		}

		/// @return The root node
		inline SyntaxTreeNode* GetRootNode() {
			if (_parent)
				return _parent->GetRootNode();
			return this;
		}

		/// @return The parent node - if any
		inline SyntaxTreeNode* GetParent() const { return _parent; }

		/// @return A list of all children under this node
		ReadOnlyArray<SyntaxTreeNode*> GetChildren() const;

		/// @return All sibling nodes that's before this node in the parent
		Vector<SyntaxTreeNode*> GetSiblingsBefore() const;

		/// @brief Add a new child-node under this node
		/// @param node The node
		void AddChild(SyntaxTreeNode* node);

		/// @brief Visit all children in the entire tree. Is useful for searching for nodes
		/// @param visitor 
		/// @param flags 
		/// @return 
		virtual VisitResult Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags);

		/// @brief Query for nodes in an upwards/revsersed manner, from this node's point of view
		/// @param visitor 
		/// @param flags 
		/// @return 
		virtual VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags);

		/// @brief Resolves this node and all it's children.
		/// @return true if we should continue resolve this node
		virtual bool Resolve(RecursiveDetector* detector);

#pragma region IStringify
		void ToString(StringStream& s, int indent) const override;
#pragma endregion

#pragma region ICompilable
		void Compile(Builder::Linker* linker) override;
#pragma endregion

		/// @brief Optimize this node
		/// @param optimizer
		virtual void Optimize(ISyntaxTreeNodeOptimizer* optimizer);

	protected:
		/// @brief Validates the parent node to see that it's compatible. This is normally
		///        used as a way to lessen the chance of bugs being introduced
		virtual void OnAddedToParent(SyntaxTreeNode* parent) {}

		/// @brief Validates the child node to see that it's compatible. This is normally
		///        used as a way to lessen the chance of bugs being introduced
		virtual void OnChildAdded(SyntaxTreeNode* parent) {}

		/// @brief Destroy all child-nodes
		void DestroyChildren();

		/// @brief Replace the child node at the supplied index with a new child node
		/// @param index 
		/// @param node 
		void ReplaceChild(I32 index, SyntaxTreeNode* node);

		/// @brief Replace all childes node at the supplied index with a new child node
		/// @param index The index we are replacing
		/// @param node The nodes we are putting in the node's place
		/// @return The next index, if we are calling this method at the
		///         same time as we are iterating over all child-nodes
		I32 ReplaceChildren(I32 index, ReadOnlyArray<SyntaxTreeNode*> nodes);

	private:
		void SetParent(SyntaxTreeNode* node);

	private:
		/// @brief The id
		const ID _id;
		/// @brief The source code this node is created from
		const SourceCodeView _sourceCode;
		/// @brief The parent
		SyntaxTreeNode* _parent;
		/// @brief The children
		Vector<SyntaxTreeNode*, 2> _children;
	};
}
