/*
  ==============================================================================

    LoopSimilarity.cpp
    
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "LoopSimilarity.h"

LoopSimilarity::LoopSimilarity(Array<var> featureVector_)
{
	featureVector = featureVector_;
}

LoopSimilarity::~LoopSimilarity()
{

}

void LoopSimilarity::readFile(File* path)
{
	if(path->exists())
	{
		// Replace with an alert
		DBG("Path does not exist");
	}

	else
	{
		// Read the file
		
	}


}

