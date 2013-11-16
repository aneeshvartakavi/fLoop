/*
  ==============================================================================

    CustomFileFilter.h
    Created: 16 Nov 2013 12:07:28pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#ifndef CUSTOMFILEFILTER_H_INCLUDED
#define CUSTOMFILEFILTER_H_INCLUDED

#include "JuceHeader.h"

class CustomFileFilter : public FileFilter
{

public:
	CustomFileFilter();
	CustomFileFilter(const String& filterDescription);
	~CustomFileFilter();
	// Implementing virtual functions
	bool isFileSuitable(const File &file) const;
	bool isDirectorySuitable(const File &file) const;
	//void dummy();

private:
	

};




#endif  // CUSTOMFILEFILTER_H_INCLUDED
