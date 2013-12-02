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
			returnVal = false;
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

	void combine()
	{
		validFiles.clear();

		if(validTempoFiles.size()>0 && validRhythmFiles.size()==0)
		{
			//validFiles = validTempoFiles;
		}

		else
		{
		
				// Search for the files that are present in both
				for(int k=0;k<validTempoFiles.size();k++)
				{
					//String temp = validTempoFiles[k];
					for(int k1=0;k1<validRhythmFiles.size();k1++)
					{
						if(validTempoFiles[k] == validRhythmFiles[k1])
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
};




#endif  // CUSTOMFILEFILTER_H_INCLUDED
