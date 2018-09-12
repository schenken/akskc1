#include "pch.h"
#include <algorithm>
#include <cwctype>
#include "CMusicData.h"
#include "Common.h"

CMusicData::CMusicData()
{
}

CMusicData::~CMusicData()
{
}

ScaleKeyChord CMusicData::GetScale (const std::wstring& key, MajorModes mode)
{
	ScaleKeyChord skc;

	// Major scale interval: T T S T T T (S)

	// mode is a number from 0 to 6 representing the seven mode of the major scale:
	// 0 = Ionian, 1 = Dorian, 2 = Phrygian, 3 = Lydian, 4 = Mixolydian, 5 = Aeolian, 6 = Locrian
	//
	//    0  ->  T   T   S   T   T   T   S
	//    1  ->      T   S   T   T   T   S   T
	//    2  ->          S   T   T   T   S   T   T
	//    3  ->              T   T   T   S   T   T   S
	//    4  ->                  T   T   S   T   T   S   T
	//    5  ->                      T   S   T   T   S   T   T
	//    6  ->	                         S   T   T   S   T   T   T
	//
	// So we can use the mode value as an offset into the intervals array to determine
	// the seven notes in the sacle for the specified mode.

	// Locate the key note in the chromatic scale.
	skc.invalidRequest = true;
	int index = -1;

	// Conveniently return the mode that is requested
	skc.mode = mode;

	// Convert input key to uppercase for searching.
	std::wstring key1 (key);
	std::transform (key1.begin(), key1.end(), key1.begin(), std::towupper);

	// Lambda --------------------------------------------------------------------
	auto FindNoteInChromaticScale = [&] (auto& chromScale)
	{
		for (std::vector<std::wstring>::const_iterator it = chromScale.begin(); it < chromScale.end(); it++)
		{
			// Convert listed chromatic scale note to uppercase, for searching.
			std::wstring cs (*it);
			std::transform (cs.begin(), cs.end(), cs.begin(), std::towupper);

			if (key1 == cs)
			{
				index = it - chromScale.begin();
				skc.invalidRequest = false;
				break;
			}
		}
    };
	//----------------------------------------------------------------------------

	// If user specified a sharp note, that's what we'll search for and report back with.
	// If ordinary note (ie. not sharp or flat) specified, we report back with flats.
	std::vector<std::wstring>* pChromScale = &chromaticScale;
	FindNoteInChromaticScale (chromaticScale);
	if (skc.invalidRequest)
	{
		FindNoteInChromaticScale (chromaticScaleSharps);
		pChromScale = &chromaticScaleSharps;
	}
	// If valid, pChromScale will point to appropriate chromatic scale list.

	if (skc.invalidRequest)
		return skc;

	if (mode == MajorModes::NUL)
		skc.invalidRequest = true;

	//skc.vScale.push_back(chromaticScale[index]);		// Always the root.
	skc.vScale.push_back((*pChromScale)[index]);		// Always the root.

	// For the remaining six notes.
	int step = static_cast<int>(mode);
	int semitones = index;
	for (int i = 0; i < 6; ++i)
	{
		semitones = (semitones + intervals[step]) % 12;	// Round the chromatic horn.

		//skc.vScale.push_back(chromaticScale[semitones]);
		skc.vScale.push_back((*pChromScale)[semitones]);
		skc.semitones[i] = intervals[step];

		step = (step + 1) % 7;	// Round the horn.
	}

	skc.semitones[6] = intervals[step];

	// Determine the triads: 1st, 3rd and 5th notes of the scale.
	// From the 3rd interval we can tell whether the chords are major or minor.
	for (int i = 0; i < 7; ++i)
	{
		// Provide the chord name.
		skc.chordName[i] = skc.vScale[i];

		int third_interval = skc.semitones[i] + skc.semitones[(i + 1) % 7];
		int fifth_interval = third_interval + skc.semitones[(i + 2) % 7] + skc.semitones[(i + 3) % 7];

		if (fifth_interval == 6)
			skc.chordName[i].append(L"dim");
		else if (third_interval == 3)
			skc.chordName[i].append (L"m");

		// Get the notes of the triad.
		skc.triads[i] = L"(";
		skc.triads[i].append (skc.vScale[i]);
		skc.triads[i].append (L"-");
		skc.triads[i].append (skc.vScale[(i + 2) % 7]);
		skc.triads[i].append (L"-");
		skc.triads[i].append (skc.vScale[(i + 4) % 7]);
		skc.triads[i].append (L")");

		// Assign Roman numeral symbol.
		skc.chordNum[i] = third_interval == 3 ? romanNumsLower[i] : romanNums[i];
	}

	return skc;
}

const std::wstring& CMusicData::GetModeName(MajorModes mode)
{
	return modeNames[static_cast<int>(mode)];
}


// Initialise class members.
int CMusicData::intervals[7] = { 2, 2, 1, 2, 2, 2, 1 };
std::wstring CMusicData::modeNames[7] = { L"Ionian", L"Dorian", L"Phrygian", L"Lydian", L"Mixolydian", L"Aeolian", L"Locrian"};

// Brace initialisation (C++11)
std::vector<std::wstring> CMusicData::chromaticScale = { L"C", L"Db", L"D", L"Eb", L"E", L"F", L"Gb", L"G", L"Ab", L"A", L"Bb", L"B", };
std::vector<std::wstring> CMusicData::chromaticScaleSharps = { L"C", L"C#", L"D", L"D#", L"E", L"F", L"F#", L"G", L"G#", L"A", L"A#", L"B", };

std::wstring CMusicData::romanNums[7] = { L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII" };
std::wstring CMusicData::romanNumsLower[7] = { L"i", L"ii", L"iii", L"iv", L"v", L"vi", L"vii" };
