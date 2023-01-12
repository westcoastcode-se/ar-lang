#pragma once

#include "../Common.h"

namespace WestCoastCode::Compilation
{
	// Represents a source code block
	class SourceCode
	{
	public:
		SourceCode(String text, String filename)
			: _text(std::move(text)), _filename(std::move(filename)) {}

		// Get the source code text
		const ReadOnlyString GetText() const {
			return ReadOnlyString(_text);
		}

		// Get the filename of the source code text
		const ReadOnlyString GetFilename() const {
			return ReadOnlyString(_filename);
		}

	private:
		String _text;
		String _filename;
	};
}

