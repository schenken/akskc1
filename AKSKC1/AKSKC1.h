#pragma once

#include "resource.h"
#include <vector>
#include <string>
#include <iomanip>
#include <thread>

#include "CMusicData.h"

//-------------------------------------------------------------------------
// Macros

#define DIRECT_OUTPUT(outFile) \
	CoutTarget ct; \
	if (outFile.size()) \
		ct.SendToFile (outFile);

#define RESET_OUTPUT(outFile) \
	if (outFile.size()) \
		ct.Reset();

//-------------------------------------------------------------------------
const WCHAR *helpText =
	L"\nUsage:\n"
	L"\n"
	L"    <key> <mode>\n"
	L"\n"
	L"        Displays the musical data for the specified key and mode\n"
	L"        where <key> is a chromatic note value and <mode> is one of\n"
	L"        \"ionian\", \"dorian\", \"phrygian\", \"lydian\", \"mixolydian\",\n"
	L"        \"aeolian\" or \"locrian\" - without the quotes. (Ionian is the\n"
	L"        Major scale and Aeolian is the (Natural) Minor scale.)\n"
	L"\n"
	L"        Indicate flat notes by using lowercase \"b\", eg. Eb.\n"
	L"        Or you can indicate sharp notes using the hash symbol, eg. D#.\n"
	L"\n"
	L"        You can abbreviate the mode names, eg. \"io\", \"ae\".\n"
	L"\n"
	L"        You can also enter * for either/both key and mode to output\n"
	L"        data for all keys and/or modes.\n"
	L"\n"
	L"        -f outfile is an optional filename if you want to send the output\n"
	L"        to a file.\n"
	L"\n"
	L"    s [chordname] [chordname]...\n"
	L"\n"
	L"        Chord search to identify scales which use the specified chords.\n"
	L"\n"
	L"    search modes [all | basic]\n"
	L"\n"
	L"        For the search command (s), this sets whether to output data for\n"
	L"        all seven modes (all) or just Ionian and Aeolian (basic).\n"
	L"\n"
	L"    -f outfile\n"
	L"\n"
	L"        Optional switch to write output to a file. Add this switch at the\n"
	L"        end of the command, eg.\n"
	L"\n"
	L"            Eb ionian -f fred.txt\n"
	L"\n"
	L"    m [bpm]\n"
	L"\n"
	L"        Start and stop the metronome. Specify a bpm to start the metronome\n"
	L"        and enter m again to stop it.\n"
	L"\n"
	L"    ? or h[elp]\n"
	L"\n"
	L"        Display this page.\n"
	L"\n"
	L"    q[uit]\n"
	L"\n"
	L"        Exit the program.\n"
	L"\n";

void PrintHelp();

MajorModes CheckModeName (const std::wstring& modeName);

// Handler for a given note (key) and mode. If the search chords arg is specified,
// only those scales which include the search chords are identified.
// vOutBuf is a buffer into which the the functions 'writes' its output data.
// pScaleCount maintains a count of the number of scales the function finds.
void ProcessNoteAndMode (std::vector<std::wstring>& vOutBuf, int* pScaleCount, const std::wstring& note, 
	std::wstring modeName, const std::vector<std::wstring>* pvSearchChords = nullptr);

// vOutBuf is a buffer into which the the functions 'writes' its output data.
// pScaleCount maintains a count of the number of scales the function finds.
void PrintNotes (std::vector<std::wstring>& vOutBuf, int* pScaleCount, const ScaleKeyChord& skc);

void WriteOutBuf (std::vector<std::wstring>& vOutBuf);

// For a given count of values (numValues) return all possible selectCount combinations.
// For example, if numValues is 5, and selectCount is 3, we want all combinations of
// 3 of the items from a set of 5 values.
//
// It's intended for something like...
// If the set is Tea, Cake, Milk, Sugar, Cream - we can have:
// 
//		Tea, Cake, Milk.
//		Tea, Cake, Sugar.
//		Tea, Cake, Cream.
//		Tea, Milk, Sugar.
//		Tea, Milk, Cream.
//		Tea, Sugar, Cream.
//		Cake, Milk, Sugar.
//		Cake, Sugar, Cream.
//		Cake, Milk, Cream.
//		Milk, Sugar, Cream.
//
//		ie. 10 combinations of 3 from a set of 5.
//
std::vector<int> GetSelectionCombinations (int numValues, int selectCount);

struct ThreadData
{
	ScaleKeyChord skc;
	int bpm;
	int bars;
	bool finish;
};

void PlayAlong (std::shared_ptr<ThreadData> thrData);
