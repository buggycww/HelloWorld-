#pragma once

#include <cstddef>
#include <string>

// Presentation helpers for the console front end.
//
// The game is meant to be read inside a fixed-size window. Text is wrapped by
// this code -- on word boundaries -- so the console never has to break a line in
// the middle of a word just because the window is narrow.
namespace ConsoleUI
{
	constexpr int WINDOW_COLUMNS = 70;
	constexpr int WINDOW_ROWS = 40;
	constexpr int SCROLLBACK_ROWS = 4000;

	// Give the console a predictable size (and some scrollback) before play starts.
	// Also removes the resize frame where the console host allows it.
	void ConfigureWindow();

	// Width, in columns, that wrapped text should stay within. Reserves one column
	// so a full line never triggers the console's own hard wrap.
	std::size_t ContentWidth();

	// Wrap on word boundaries while preserving explicit newlines. The first output
	// line begins with firstIndent; every following line begins with continuationIndent.
	std::string Wrap(const std::string& text,
		std::size_t width,
		const std::string& firstIndent = "",
		const std::string& continuationIndent = "");

	// Print text wrapped to the current console width.
	void WriteWrapped(const std::string& text,
		const std::string& firstIndent = "",
		const std::string& continuationIndent = "");

	void WriteLine(const std::string& text = "");
	void WriteRule(char character = '-');
}
