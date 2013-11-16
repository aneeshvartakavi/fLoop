/*
  ==============================================================================

    CustomFileFilter.cpp
    Created: 16 Nov 2013 12:07:28pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "CustomFileFilter.h"

CustomFileFilter::CustomFileFilter(const String& filterDescription):FileFilter(filterDescription)
{

}

CustomFileFilter::~CustomFileFilter()
{
	// Nothing to do here
}

bool CustomFileFilter::isFileSuitable(const File &file) const
{
	bool returnVal = false;
	String ext = file.getFileExtension();
	if(ext == ".wav")
		returnVal = true;

	return returnVal;
}

bool CustomFileFilter::isDirectorySuitable(const File &file) const
{
	// For now, all directories are val
	return true;
}
