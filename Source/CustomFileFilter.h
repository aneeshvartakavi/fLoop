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
		clearFilters();
	}

	~CustomFileFilter()
	{
		clearFilters();
		// Nothing here yet
	}

	CustomFileFilter(const String& filterDescription,CustomFileFilter* filterToCopyFrom):FileFilter(filterDescription)
	{
		validFiles = filterToCopyFrom->returnValidFiles();
	}


	bool isFileSuitable(const File &file) const
	{
		bool returnVal = false;
		if(validFiles.size()==0 && file.getFileExtension() == ".wav")
		{
			// If filters have not been defined yet, return true by default
			returnVal = true;
		}
		else
		{
			//String tempFileName = file.getFileNameWithoutExtension();
			String tempName = file.getFileNameWithoutExtension();
			
			for(int k=0;k<validFiles.size();k++)
			{
				String validFile = validFiles[k];
				if (validFile == tempName && file.getFileExtension() == ".wav")
				{
					returnVal = true;
					break;
				}
			}

			/*if(validFiles.contains(tempName)) // Something going wrong here, this is not working as it should.
			{
				returnVal = true;
				
			}*/

		}

		return returnVal;
		
	}

	bool isDirectorySuitable(const File &file) const
	{
		// For now, all directories are valid
		return true;
	}

	void updateFilters(const StringArray &validFiles_)
	{
		// Add array because we can potentially have multiple criteria selected
		validFiles.addArray(validFiles_);
	}

	void clearFilters()
	{
		validFiles.clear();
	}

	StringArray returnValidFiles()
	{
		return validFiles;
	}



private:
	// Store valid files
	StringArray validFiles;
};




#endif  // CUSTOMFILEFILTER_H_INCLUDED
