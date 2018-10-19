#include "pch.h"
#include <chrono>
#include "Common.h"

std::vector<std::wstring> Split(const std::wstring& line, const std::wstring& dlim)
{
	std::vector<std::wstring> v;
	RemoveWhitespace rmws(line);
	std::wstring line2 = rmws(RemoveWhitespace::LeadingTrailingCondense);

	std::size_t found = line2.find_first_of(dlim);
	int prev = 0;
	while (found != std::wstring::npos)
	{
		v.push_back(line2.substr(prev, found - prev));
		prev = found + dlim.length();
		found = line2.find_first_of(dlim, prev);
	}

	std::wstring x = line2.substr(prev, line2.length() - prev);
	if (x.length() > 0)
		v.push_back(x);

	return v;
}


//---------------------------------------------------------------------------
// RemoveWhitespace implementation
RemoveWhitespace::RemoveWhitespace(const std::wstring _s) : s(_s)
{
	sSize = s.size();
}

void RemoveWhitespace::operator= (const std::wstring _s)
{
	s = _s;
	sSize = s.size();
}

std::wstring RemoveWhitespace::operator() (Mode mode)
{
	pChar = &s[0];
	std::wstring s1;
	s1.resize(sSize);

	int lastChar = 0;	// last char in copy string
	bool bNewWhiteSpaceBlock = true;
	bool bLeadingWhitespace = true;
	while (*pChar)
	{
		if (_istspace((unsigned char)*pChar))
		{
			// Whitespace

			if (mode & Mode::All)
			{
				// skip
			}
			else if ((mode & Mode::Leading) && bLeadingWhitespace)
			{
				// skip
			}
			else if ((mode & Mode::Condense) && !bNewWhiteSpaceBlock)
			{
				// skip if not 1st ws char in contiguous series
			}
			else
				s1[lastChar++] = ' ';

			bNewWhiteSpaceBlock = false;
		}
		else
		{
			// NOT whitespace, so copy
			bLeadingWhitespace = false;
			bNewWhiteSpaceBlock = true;
			s1[lastChar++] = *pChar;
		}

		pChar++;
	}

	if (mode & Trailing)
	{
		// strip trailing spaces
		while (s1[lastChar - 1] == ' ')
			lastChar--;
	}

	s1.resize(lastChar);

	return s1;
}

std::wstring RemoveWhitespace::operator() (const std::wstring _s, Mode mode)
{
	operator=(_s);
	return operator() (mode);
}

//---------------------------------------------------------------------------
// CoutTarget implementation

CoutTarget::CoutTarget() :
coutbuf (nullptr),
out (nullptr)
{
}

void CoutTarget::SendToFile(const std::wstring& fileName)
{
	out = new std::wofstream (fileName);
	coutbuf = std::wcout.rdbuf (out->rdbuf());	// Redirect std::cout to file.
}

void CoutTarget::Reset()
{
	std::wcout.rdbuf (coutbuf);
	if (out != nullptr)
		delete out;
}

