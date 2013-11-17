/*
  ==============================================================================

    CustomFileFilter.h
    Created: 16 Nov 2013 6:23:55pm
    Author:  Aneesh

  ==============================================================================
*/

#ifndef CUSTOMFILEFILTER_H_INCLUDED
#define CUSTOMFILEFILTER_H_INCLUDED

#include "JuceHeader.h"

class CustomFileFilter: public FileFilter
{
public:
	CustomFileFilter(const String& filterDescription):FileFilter(filterDescription)
	{

	}

	~CustomFileFilter()
	{
		// Nothing here yet
	}

	bool isFileSuitable(const File &file) const
	{
		// Read valid files, and return bool
		return true;

	}

	bool isDirectorySuitable(const File &file) const
	{
		// For now, all directories are valid
		return true;
	}

private:
	// Store valid files

};




#endif  // CUSTOMFILEFILTER_H_INCLUDED
