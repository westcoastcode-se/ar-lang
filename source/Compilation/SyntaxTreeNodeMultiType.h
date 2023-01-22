#pragma once

#include "SyntaxTreeNodeTypeDef.h"

namespace WestCoastCode::Compilation
{
	/// @brief A type that contains multiple types, for example: (int32, bool)
	class SyntaxTreeNodeMultiType : public SyntaxTreeNodeTypeDef
	{
	public:
		SyntaxTreeNodeMultiType(SourceCodeView view);

#pragma region SyntaxTreeNodeTypeDef
		inline ReadOnlyArray<SyntaxTreeNodeTypeDef*> GetTypes() final {
			ASSERT_NOT_IMPLEMENTED();
			return ReadOnlyArray<SyntaxTreeNodeTypeDef*>();
		}
		void ToString(StringStream& s, int indent) const final;
#pragma endregion

		/// @brief Parse the source code and turn it into a MultiType
		/// @param  
		/// @return 
		static SyntaxTreeNodeMultiType* Parse(const ParserState*);
	};
}
