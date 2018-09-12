#pragma once

#include <vector>
#include <string>

enum class MajorModes { IONIAN, DORIAN, PHRYGIAN, LYDIAN, MIXOLYDIAN, AEOLIAN, LOCRIAN, NUL };

struct ScaleKeyChord
{
	std::vector<std::wstring> vScale;

	int semitones [7];		// Represents the mode intervals between scale notes,
							// eg. for C major scale: D  E  F  G  A  B  C
							//						  2  2  1  2  2  2  1

	std::wstring chordName[7];
	std::wstring triads[7];		// The notes comprising the triad chord.
	std::wstring chordNum[7];	// In roman numerals, eg. I, ii, iii, IV.

	MajorModes mode;

	bool invalidRequest;
};

class CMusicData
{
public:
	CMusicData();
	~CMusicData();

	ScaleKeyChord GetScale (const std::wstring& key, MajorModes mode);

	static const std::wstring& GetModeName (MajorModes mode);


	// Public Class members.
	static std::vector<std::wstring> chromaticScale;		// Basic 12-note scale.
	static std::vector<std::wstring> chromaticScaleSharps;	// Basic 12-note scale.
	static std::wstring modeNames[7];

protected:

	// Class members.
	static int intervals[7];
	static std::wstring romanNums[7];
	static std::wstring romanNumsLower[7];
};
