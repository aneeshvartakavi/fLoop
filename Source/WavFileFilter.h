/*
  ==============================================================================

    WavFileFilter.h
    Created: 16 Nov 2013 12:07:28pm
    Author:  Aneesh, Cameron

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
//	CustomFileFilter(const String& filterDescription);
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
		// For now, all directories are valid
		return true;
	}

};




#endif  // WAVFILEFILTER_H_INCLUDED
