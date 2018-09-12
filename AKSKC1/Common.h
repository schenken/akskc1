#pragma once

#include <vector>
#include <string>
#include <fstream>

std::vector<std::wstring> Split (const std::wstring& line, const std::wstring& dlim);

// Remove Whitespace Functor.
// 
// Options (Mode enum) to strip leading, trailing, all, or condense blocks of ws to just a single space.
//
// Usage:
//
//	RemoveWhitespace rmws( L"  Andrew Kendall  " );
//
//	wstring z1 = rmws();	// gives "Andrew Kendall" - strips leading & trailing.
//
//	wstring s1 = rmws(static_cast<RemoveWhitespace::Mode>
//		(RemoveWhitespace::Leading | RemoveWhitespace::Trailing));	// also gives "Andrew Kendall"
//
//	wstring s1 = rmws(static_cast<RemoveWhitespace::Mode>(RemoveWhitespace::All));	// gives "AndrewKendall"
//
struct RemoveWhitespace
{
	std::wstring s;
	const WCHAR* pChar;
	int sSize;

	enum Mode { Leading = 1, Trailing = 2, All = 4, Condense = 8, LeadingTrailingCondense = 11 };

	RemoveWhitespace (const std::wstring s);

	// Set new string content.
	void operator= (const std::wstring _s);

	std::wstring operator() (Mode mode = Mode::All);

	// Set new string, process and return immediately.
	std::wstring operator() (const std::wstring _s, Mode mode = Mode::All);
};

// Helper class to control output channel cout.
// Added in order to send output to a file.
class CoutTarget
{
public:
	CoutTarget ();

	void SendToFile (const std::wstring& fileName);
	void Reset ();		// Reset to standard output.

protected:
	std::wstreambuf* coutbuf;
	std::wofstream* out;

};