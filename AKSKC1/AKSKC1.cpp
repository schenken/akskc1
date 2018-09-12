// AKSKC1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <algorithm>
#include <cwctype>
#include <fstream>
#include <sstream>
#include "AKSKC1.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
			bool searchModesAll = false;	// True = all seven modes. False = just major and minor.

			std::wcout << "\n\nAKSKC1: Scales, Keys and Chords Program\n";
			std::wcout << "\n(c) 2018 Andrew Kendall\n\nEnter ? for help\n\n";

			while (1) {

				wcout << L"> ";
				std::wstring inputLine;
				getline (wcin, inputLine);

				if (inputLine.size() == 0)
					continue;

				size_t pos;
				MajorModes mode = MajorModes::NUL;

				pos = std::wstring(L"help").find(inputLine);
				if (inputLine == L"?" || (pos != std::wstring::npos && pos == 0))
				{
					PrintHelp();
					continue;
				}

				pos = std::wstring(L"quit").find(inputLine);
				if (pos != std::wstring::npos && pos == 0)
					break;

				std::vector<wstring> vInput = Split (inputLine, L" ");

				// Check we've got at least two args
				if (vInput.size() < 2)
				{
					std::wcout << L"Invalid command - enter ? for help\n";
					continue;
				}

				// Search modes set command
				if (vInput.size() == 3 && vInput[0] == L"search" && vInput[1] == L"modes")
				{
					if (vInput[2] == L"all")
						searchModesAll = true;
					else if (vInput[2] == L"basic")
						searchModesAll = false;
					continue;
				}

				// Check for output filename: -f switch followed by filename.
				std::wstring outFile = L"";
				if (vInput.size() > 3)	// must be at least 4 args
				{
					for (size_t i = 1; i < vInput.size(); ++i)
					{
						if (vInput[i] == L"-f")
						{
							// Remove the -f <file> args from the input vector.
							vInput.erase (vInput.begin() + i);
							if (i < vInput.size())
							{
								outFile = vInput[i];
								vInput.erase (vInput.begin() + i);
							}
							break;
						}
					}
				}

				int scaleCount = 0;
				std::vector<std::wstring> vOutBuf;

				if (vInput[0] == L"s")
				{
					// Chord search.

					// For a chord search, this is used for iterative re-searching when nothing is found.
					// We lose one of the chords and retry the search. If still nothing found, we try excluding
					// each of the chords. If still nothing found after excluding one chord, then we go on to
					// try excluding any TWO of the chords and, again, retrying the search with all
					// combinations of two chords excluded. If still nothing found, try three chords
					// excluded. We stop trying once we get down to only searching for two chords.
					int countOfChordsExcluded = 0;

					std::vector<std::wstring> vSearchChords;
					for (size_t i = 1; i < vInput.size(); ++i)
					{
						vSearchChords.push_back (vInput[i]);
					}

					DIRECT_OUTPUT (outFile)

					// Lambda ---------------------------------------------------------------------
					// Perform for ALL notes of the chromatic scale.
					auto ChordSearch = [&]()
					{
						scaleCount = 0;

						for (std::vector<std::wstring>::const_iterator it = CMusicData::chromaticScale.begin(); it < CMusicData::chromaticScale.end(); it++)
						{
							if (searchModesAll)
							{
								// ALL modes.
								for each (auto modeName in CMusicData::modeNames)
									ProcessNoteAndMode (vOutBuf, &scaleCount, *it, modeName, &vSearchChords);
							}
							else
							{
								ProcessNoteAndMode (vOutBuf, &scaleCount, *it, L"Ionian", &vSearchChords);
								ProcessNoteAndMode (vOutBuf, &scaleCount, *it, L"Aeolian", &vSearchChords);
							}
						}
					};
					//--------------------------------------------------------------------------------------------

					std::wcout << L"\nSearch for chords: ";
					for each  (auto s in vSearchChords)
						std::wcout << s << L" ";
					std::wcout << "\n";

					bool searchSuccessful = false;

					ChordSearch();
					if (vOutBuf.size())
					{
						WriteOutBuf (vOutBuf);
						searchSuccessful = true;
					}
					else
						std::wcout << L"\n    Those chords do not appear together in any key.\n";


					// Did we find anything? If not, exclude chords and retry.
					while ((!searchSuccessful) && vSearchChords.size() > 2)
					{
						// Repeat loop until search is completed on only two chords.
						countOfChordsExcluded++;

						int selectCount = vSearchChords.size() - 1;
						std::vector<int> vSelect = GetSelectionCombinations (vInput.size() - 1, selectCount);

						for (size_t i = 0; i < vSelect.size(); ++i)
						{
							vSearchChords.clear();
							int chordSelect = vSelect[i];
							int flag = 1;
							for (size_t i = 1; i < vInput.size(); ++i)
							{
								if (chordSelect & flag)
									vSearchChords.push_back (vInput[i]);
								flag <<= 1;
								int ak = 1;
							}

							ChordSearch();

							if (vOutBuf.size())
							{
								std::wcout << L"\nFound for " << vSearchChords.size() << L" of the chords: ";
								for each  (auto s in vSearchChords)
									std::wcout << s << L" ";
								std::wcout << "\n";
								WriteOutBuf (vOutBuf);
								searchSuccessful = true;
							}

						}

						if (!searchSuccessful)
							std::wcout << L"    Nothing found for any combination of " << vSearchChords.size() << L" of the specified chords.\n";
					}

					std::wcout << L"\n------------------------------------------------------------\n\n";
					RESET_OUTPUT (outFile)
					continue;
				}

				if (vInput[0] == L"*")
				{
					// Perform for ALL notes of the chromatic scale.
					DIRECT_OUTPUT (outFile)
					for (std::vector<std::wstring>::const_iterator it = CMusicData::chromaticScale.begin(); it < CMusicData::chromaticScale.end(); it++)
					{
						if (vInput[1] == L"*")
						{
							// ALL modes.
							for each (auto modeName in CMusicData::modeNames)
							{
								ProcessNoteAndMode (vOutBuf, &scaleCount, *it, modeName);
								WriteOutBuf (vOutBuf);
							}
							std::wcout << L"\n------------------------------------------------------------\n";
							continue;
						}
						ProcessNoteAndMode (vOutBuf, &scaleCount, *it, vInput[1]);
						WriteOutBuf (vOutBuf);
					}
					RESET_OUTPUT (outFile)
					continue;
				}

				if (vInput[1] == L"*")
				{
					// Single note, but all modes.
					DIRECT_OUTPUT (outFile)
					for each (auto modeName in CMusicData::modeNames)
						ProcessNoteAndMode (vOutBuf, &scaleCount, vInput[0], modeName);
					WriteOutBuf (vOutBuf);
					std::wcout << L"\n------------------------------------------------------------\n\n";
					RESET_OUTPUT (outFile)
					continue;
				}

				// Single note and mode.
				DIRECT_OUTPUT (outFile)
				ProcessNoteAndMode (vOutBuf, &scaleCount, vInput[0], vInput[1]);
				WriteOutBuf (vOutBuf);
				std::wcout << L"\n";
				RESET_OUTPUT (outFile)
			}
		}
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}

//---------------------------------------------------------------------------

void PrintHelp()
{
	std::wcout << helpText;
}

MajorModes CheckModeName(const std::wstring& modeName)
{
	MajorModes mode = MajorModes::NUL;

	std::size_t pos = std::wstring(L"IONIAN").find (modeName);		// Major scale
	if (pos != std::wstring::npos && pos == 0)
		mode = MajorModes::IONIAN;

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"DORIAN").find (modeName);
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::DORIAN;
	}

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"PHRYGIAN").find (modeName);
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::PHRYGIAN;
	}

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"LYDIAN").find (modeName);
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::LYDIAN;
	}

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"MIXOLYDIAN").find (modeName);
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::MIXOLYDIAN;
	}

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"AEOLIAN").find (modeName);		// Natural minor
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::AEOLIAN;
	}

	if (mode == MajorModes::NUL)
	{
		pos = std::wstring(L"LOCRIAN").find (modeName);
		if (pos != std::wstring::npos && pos == 0)
			mode = MajorModes::LOCRIAN;
	}

	return mode;
}

void ProcessNoteAndMode(std::vector<wstring>& vOutBuf, int* pScaleCount, const std::wstring& note, std::wstring modeName, const std::vector<std::wstring>* pvSearchChords)
{
	// Convert input mode name to uppercase.
	std::transform (modeName.begin(), modeName.end(), modeName.begin(), std::towupper);

	// Get the scale and chord values.
	MajorModes mode = CheckModeName (modeName);

	CMusicData md;
	ScaleKeyChord skc = md.GetScale (note, mode);

	if (skc.invalidRequest)
	{
		vOutBuf.push_back (L"Invalid parameters(s) - enter ? for help\n"); 
		return;
	}

	// Check if the key contains the search chords.
	if (pvSearchChords != nullptr)
	{
		int chordsFound = 0;
		for (int i = 0; i < 7; ++i)
		{
			for each  (auto s in *pvSearchChords)
			{
				if (skc.chordName[i] == s)
				{
					chordsFound++;
					break;
				}
			}
		}
		if (chordsFound == pvSearchChords->size())
			PrintNotes (vOutBuf, pScaleCount, skc);
		return;
	}

	PrintNotes (vOutBuf, pScaleCount, skc);
}

void PrintNotes (std::vector<wstring>& vOutBuf, int* pScaleCount, const ScaleKeyChord& skc)
{
	(*pScaleCount)++;

	std::wostringstream ss;

	// Print the notes of the scale.
	ss << L"\n    (" << *pScaleCount << L") " << CMusicData::GetModeName (skc.mode) << L" scale: ";

	for (auto v : skc.vScale)
		ss << v << L" ";

	// List the triads (chords).
	ss << L"\n\n";
	for (int i = 0; i < 7; ++i)
	{
		ss << L"      " << std::right << std::setw(3) << skc.chordNum[i] << L"  ";
		ss << std::left << std::setw(6) << std::setfill(L' ') << skc.chordName[i];
		ss << std::left << std::setw(6) << std::setfill(L' ') << skc.triads[i] << L"\n";
	}

	vOutBuf.push_back (ss.str());
}

void WriteOutBuf(std::vector<std::wstring>& vOutBuf)
{
	for each (auto s in vOutBuf)
		std::wcout << s;
	vOutBuf.clear();
}

std::vector<int> GetSelectionCombinations (int numValues, int selectCount)
{
	// Find all combinations of a fixed selection choice from a set of values.
	// For example, if there are five values (Eb G Bm Am Db) and we must choose
	// any three, find all combinations.
	//
	// selectCount represents a fixed selection choice of items.
	// Number of bits that must be set in the result.
	//
	// So it's a case of: Find all <selectCount> combinations from <numValues>.

	int max = static_cast<int> (pow (2.0, numValues));
	int count = 1;

	std::vector<int> vSelect;
	do
	{
		int selected = 0;
		int tempCount = count;
		for (int i = 0; i < numValues; ++i)
		{
			if (tempCount & 1)
				selected++;
			tempCount >>= 1;
		}

		if (selected == selectCount)
			// This is a valid number, since it's got selectCount bits set.
			// We should save this to a vector.
			vSelect.push_back (count);

		count++;
	} while (count < max);

	return vSelect;
}
