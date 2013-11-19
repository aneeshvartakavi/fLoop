/*
  ==============================================================================

    LoopSimilarity.cpp
    
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "LoopSimilarity.h"

LoopSimilarity::LoopSimilarity()
{
	// Preallocate memory
	similarFiles.ensureStorageAllocated(100);
}

//LoopSimilarity::LoopSimilarity(Array<var> featureVector_)
//{
//	featureVector = featureVector_;
//}

LoopSimilarity::~LoopSimilarity()
{
	audioLoops = nullptr;
	//similarFiles = nullptr;
}

void LoopSimilarity::returnSimilar(int sliderIndex, int sliderMax, int sliderMin,const File &referenceFile)
{
	// sliderIndex is a parameter to specify which slider was changed
	
}

StringArray LoopSimilarity::returnSimilarTempo(int sliderMax, int sliderMin,const File &referenceFile)
{
	// Get filename
	String fileName = referenceFile.getFileNameWithoutExtension();
	
	//similarFiles = new StringArray();
	//similarFiles->ensureStorageAllocated(20);
	int referenceIndex = 0;
	int referenceTempo = 0;
	// Look for the file in the featurevector
	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
//		DBG(JSON::toString(tempVar));
		if(fileName == tempVar[0].toString())
		{
			// Get the tempo
			referenceTempo = tempVar[1].toString().getIntValue();
			referenceIndex = i;
			break;
		}

	}
	// Find similar loops

	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
		//DBG(JSON::toString(tempVar));
		if(tempVar.size()>0)
		{
			int currentTempo =  tempVar[1].toString().getIntValue();
			if(currentTempo>=referenceTempo+sliderMin && currentTempo<= referenceTempo+sliderMax && i!= referenceIndex)
			{
				// If condition is satisfied, add the fileName into a stringArray
				//DBG(tempVar[0].toString());
				//similarFiles->add(tempVar[0].toString());
				similarFiles.add(tempVar[0].toString());
			}

		}


	}

	return similarFiles;

}

void LoopSimilarity::readCache(const File& pathToDirectory)
{
	// Always call this after a call to check if cache exists
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	File cache(tempPath);

	// Clear state
	 var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
	 
	 int length = result.size();
	 // Initialize the feature vector, do we need this
	 featureVector.insertMultiple(0,var(),length);

	 for(int i=0;i<length;i++)
	 {
		 // Get the data from the cache file
		 String path = result[i].getProperty(Identifier("Path"),0);
		 int tempo = result[i].getProperty(Identifier("Tempo"),0);
	 
		// Add it to the feature vector
		 var& tempFeatures = featureVector.getReference(i);
		 // Make sure the order is consistent
		 tempFeatures.append(path);
		 tempFeatures.append(tempo);
	 }

}

