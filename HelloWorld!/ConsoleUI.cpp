#include "ConsoleUI.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace
{
	std::vector<std::string> SplitParagraphs(const std::string& text)
	{
		std::vector<std::string> paragraphs;
		std::string current;

		for (char c : text)
		{
			if (c == '\n')
			{
				paragraphs.push_back(current);
				current.clear();
			}
			else
			{
				current += c;
			}
		}

		paragraphs.push_back(current);
		return paragraphs;
	}

	std::vector<std::string> SplitWords(const std::string& paragraph)
	{
		std::vector<std::string> words;
		std::size_t pos = 0;

		while (pos < paragraph.size())
		{
			while (pos < paragraph.size() && paragraph[pos] == ' ')
				++pos;

			const std::size_t start = pos;
			while (pos < paragraph.size() && paragraph[pos] != ' ')
				++pos;

			if (pos > start)
				words.push_back(paragraph.substr(start, pos - start));
		}

		return words;
	}

#ifdef _WIN32
	std::atomic<bool> g_watchWindow{ false };
	std::atomic<bool> g_watcherStarted{ false };
	int g_fixedColumns = ConsoleUI::WINDOW_COLUMNS;
	int g_fixedRows = ConsoleUI::WINDOW_ROWS;

	// Best effort: classic console windows are ordinary Win32 windows, so removing
	// the sizing frame and maximizing box stops the user from dragging them larger.
	// Hosts such as Windows Terminal own their own window and may ignore this.
	void MakeWindowFixed()
	{
		HWND window = GetConsoleWindow();
		if (!window)
			return;

		LONG_PTR style = GetWindowLongPtr(window, GWL_STYLE);
		style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLongPtr(window, GWL_STYLE, style);
		SetWindowPos(window, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		HMENU menu = GetSystemMenu(window, FALSE);
		if (menu)
		{
			EnableMenuItem(menu, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(menu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
		}
	}

	// Some console hosts quietly restore the resizing frame, and the window can be
	// resized through paths that ignore the frame style. Re-assert the fixed size
	// periodically so a resize snaps back. This has no effect in terminal hosts
	// that own their own window, such as Windows Terminal.
	void WatchConsoleWindow()
	{
		int failures = 0;

		while (g_watchWindow.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			if (!g_watchWindow.load())
				break;

			HWND window = GetConsoleWindow();
			if (!window)
				break;

			const LONG_PTR style = GetWindowLongPtr(window, GWL_STYLE);
			const LONG_PTR fixedStyle = style & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
			if (fixedStyle != style)
			{
				SetWindowLongPtr(window, GWL_STYLE, fixedStyle);
				SetWindowPos(window, nullptr, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}

			if (IsZoomed(window))
				ShowWindow(window, SW_RESTORE);

			HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO info{};
			if (!GetConsoleScreenBufferInfo(output, &info))
			{
				if (++failures >= 3)
					break;
				continue;
			}

			const int columns = info.srWindow.Right - info.srWindow.Left + 1;
			const int rows = info.srWindow.Bottom - info.srWindow.Top + 1;
			if (columns == g_fixedColumns && rows == g_fixedRows)
			{
				failures = 0;
				continue;
			}

			const SMALL_RECT fixedRect = {
				0,
				0,
				static_cast<SHORT>(g_fixedColumns - 1),
				static_cast<SHORT>(g_fixedRows - 1)
			};
			if (!SetConsoleWindowInfo(output, TRUE, &fixedRect))
			{
				if (++failures >= 3)
					break;
			}
			else
			{
				failures = 0;
			}
		}

		g_watchWindow.store(false);
	}
#endif
}

void ConsoleUI::ConfigureWindow()
{
#ifdef _WIN32
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	if (output == INVALID_HANDLE_VALUE)
		return;

	SetConsoleTitleW(L"Hello, World! -- a tiny story");

	// If the console started maximized, bring it back before sizing it.
	HWND consoleWindow = GetConsoleWindow();
	if (consoleWindow)
		ShowWindow(consoleWindow, SW_RESTORE);

	CONSOLE_SCREEN_BUFFER_INFO info{};
	if (!GetConsoleScreenBufferInfo(output, &info))
		return;

	// Do not ask for a window larger than the screen/font can display, or the
	// resize fails and the console is left exactly as it was.
	int columns = WINDOW_COLUMNS;
	int rows = WINDOW_ROWS;
	const COORD largest = GetLargestConsoleWindowSize(output);
	if (largest.X > 0 && largest.X < columns)
		columns = largest.X;
	if (largest.Y > 0 && largest.Y < rows)
		rows = largest.Y;
	if (columns < 20)
		columns = 20;
	if (rows < 10)
		rows = 10;

	// A console buffer must be at least as large as its window. Shrink the window
	// first, size the buffer, then set the final window rectangle. If the user's
	// screen or font cannot fit the requested window, restore the original console
	// instead of leaving a tiny window behind.
	const COORD oldBuffer = info.dwSize;
	const SMALL_RECT oldWindow = info.srWindow;

	const SMALL_RECT tiny = { 0, 0, 1, 1 };
	if (!SetConsoleWindowInfo(output, TRUE, &tiny))
		return;

	const COORD bufferSize = {
		static_cast<SHORT>(columns),
		static_cast<SHORT>(SCROLLBACK_ROWS)
	};
	if (!SetConsoleScreenBufferSize(output, bufferSize))
	{
		SetConsoleWindowInfo(output, TRUE, &oldWindow);
		return;
	}

	SMALL_RECT windowSize = {
		0,
		0,
		static_cast<SHORT>(columns - 1),
		static_cast<SHORT>(rows - 1)
	};
	if (!SetConsoleWindowInfo(output, TRUE, &windowSize))
	{
		// Last try with a conservative size; some consoles report a largest size
		// that their current window cannot quite reach.
		const int safeColumns = columns > 80 ? 80 : columns;
		const int safeRows = rows > 25 ? 25 : rows;
		const SMALL_RECT safeSize = {
			0,
			0,
			static_cast<SHORT>(safeColumns - 1),
			static_cast<SHORT>(safeRows - 1)
		};

		if (!SetConsoleWindowInfo(output, TRUE, &safeSize))
		{
			SetConsoleScreenBufferSize(output, oldBuffer);
			SetConsoleWindowInfo(output, TRUE, &oldWindow);
			return;
		}

		columns = safeColumns;
		rows = safeRows;
	}

	g_fixedColumns = columns;
	g_fixedRows = rows;
	MakeWindowFixed();

	g_watchWindow.store(true);
	if (!g_watcherStarted.exchange(true))
		std::thread(WatchConsoleWindow).detach();
#endif
}

std::size_t ConsoleUI::ContentWidth()
{
#ifdef _WIN32
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info{};

	if (output != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(output, &info))
	{
		int width = static_cast<int>(info.srWindow.Right) - info.srWindow.Left + 1;
		const int bufferWidth = static_cast<int>(info.dwSize.X);

		if (bufferWidth > 0 && bufferWidth < width)
			width = bufferWidth;

		if (width > 20)
			return static_cast<std::size_t>(width - 1);
	}
#endif

	return static_cast<std::size_t>(WINDOW_COLUMNS - 1);
}

std::string ConsoleUI::Wrap(const std::string& text,
	std::size_t width,
	const std::string& firstIndent,
	const std::string& continuationIndent)
{
	if (width < 20)
		width = 20;

	std::string result;
	bool firstOutputLine = true;
	const std::vector<std::string> paragraphs = SplitParagraphs(text);

	for (const std::string& rawParagraph : paragraphs)
	{
		std::string paragraph = rawParagraph;
		if (!paragraph.empty() && paragraph.back() == '\r')
			paragraph.pop_back();

		if (paragraph.empty())
		{
			result += '\n';
			firstOutputLine = false;
			continue;
		}

		const auto indent = [&]() -> const std::string&
		{
			return firstOutputLine ? firstIndent : continuationIndent;
		};
		const auto available = [&]() -> std::size_t
		{
			const std::size_t used = indent().size();
			return width > used ? width - used : 1;
		};

		std::string line;
		const auto emit = [&]()
		{
			result += indent();
			result += line;
			result += '\n';
			line.clear();
			firstOutputLine = false;
		};

		for (std::string word : SplitWords(paragraph))
		{
			// A single word wider than the window has to be split somewhere; this is
			// the only case where a break can happen inside a word.
			while (word.size() > available())
			{
				if (!line.empty())
					emit();

				const std::size_t take = available();
				line = word.substr(0, take);
				emit();
				word.erase(0, take);
			}

			if (word.empty())
				continue;

			if (line.empty())
			{
				line = word;
			}
			else if (line.size() + 1 + word.size() <= available())
			{
				line += ' ';
				line += word;
			}
			else
			{
				emit();
				line = word;
			}
		}

		if (!line.empty())
			emit();
	}

	if (result.empty())
		result += '\n';

	return result;
}

void ConsoleUI::WriteWrapped(const std::string& text,
	const std::string& firstIndent,
	const std::string& continuationIndent)
{
	std::cout << Wrap(text, ContentWidth(), firstIndent, continuationIndent);
}

void ConsoleUI::WriteLine(const std::string& text)
{
	std::cout << text << '\n';
}

void ConsoleUI::WriteRule(char character)
{
	std::cout << std::string(ContentWidth(), character) << '\n';
}
