/*
  ==============================================================================

    WavFileFilter.h
    Created: 16 Nov 2013 12:07:28pm
    Author:  Aneesh Vartakavi, Cameron Summers

	An extension of the FileFilter class, to display only wav files
  ==============================================================================
*/

#ifndef WAVFILEFILTER_H_INCLUDED
#define WAVFILEFILTER_H_INCLUDED

#include "JuceHeader.h"

class WavFileFilter : public FileFilter
{

public:
	WavFileFilter(const String& filterDescription):FileFilter(filterDescription)
	{

	}

	~WavFileFilter()
	{

	}

	// Implementing virtual functions
	bool isFileSuitable(const File &file) const
	{
		bool returnVal = false;
		String ext = file.getFileExtension();
		if(ext == ".wav")
			returnVal = true;

		return returnVal;
	}

	bool isDirectorySuitable(const File &file) const
	{
		if(file.exists())
			return true;
		else
			return false;
	}

};




#endif  // WAVFILEFILTER_H_INCLUDED
