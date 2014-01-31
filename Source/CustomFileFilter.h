/*
  ==============================================================================

    CustomFileFilter.h
    Created: 16 Nov 2013 6:23:55pm
    Author:  Aneesh Vartakavi, Cameron Summers

	This class is an extension of the JUCE FileFilter class, that checks for
	files of similar rhythm and timbre.

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
		validFiles.clear();
		validRhythmFiles.clear();
		validTempoFiles.clear();
		validTimbreFiles.clear();
		clearFilters();
		
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
			returnVal = false;
		}
		else
		{
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

		}

		return returnVal;
		
	}

	bool isDirectorySuitable(const File &file) const
	{
		if(file.exists())
			return true;
		else
			return false;
	}

	void updateTempoFilters(const StringArray &validFiles_)
	{
		if(validTempoFiles.size()==0)
			validTempoFiles.addArray(validFiles_);
		else
		{
			// Add array because we can potentially have multiple criteria selected
			validTempoFiles.clear();
			validTempoFiles = validFiles_;
		}
		combine();
	}

	void updateRhythmFilters(const StringArray &validFiles_)
	{
		if(validRhythmFiles.size()==0)
			validRhythmFiles.addArray(validFiles_);
		else
		{
			validRhythmFiles.clear();
			validRhythmFiles = validFiles_;
		}
		
		// Check for similar loops in both
		combine();
			
	}

	void updateTimbreFilters(const StringArray &validFiles_)
	{
		if(validTimbreFiles.size()==0)
			validTimbreFiles.addArray(validFiles_);
		else
		{
			validTimbreFiles.clear();
			validTimbreFiles = validFiles_;
		}
		
		// Check for similar loops in both
		combine();
			
	}

	void combine()
	{
		validFiles.clear();

		if(validTempoFiles.size()>0 && validRhythmFiles.size()==0)
		{
			//validFiles = validTempoFiles;
		}

		else
		{
			// Search for the files that are present in all three
			for(int k=0;k<validTempoFiles.size();k++)
			{
				StringRef temp = validTempoFiles[k];
				if(validRhythmFiles.contains(temp,true))
				{
					if(validTimbreFiles.contains(temp,true))
					{
						validFiles.add(validTempoFiles[k]);
					}

				}
			}
		}
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
	StringArray validTempoFiles;
	StringArray validRhythmFiles;
	StringArray validTimbreFiles;

};




#endif  // CUSTOMFILEFILTER_H_INCLUDED
