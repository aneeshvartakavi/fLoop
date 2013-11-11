/*
  ==============================================================================

    FeatureExtractor.h
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh

  ==============================================================================
*/

#ifndef FEATUREEXTRACTOR_H_INCLUDED
#define FEATUREEXTRACTOR_H_INCLUDED

#include "JuceHeader.h"

class FeatureExtractor
{

public:
	FeatureExtractor(int blockSize_,int hopSize_);
	~FeatureExtractor();
	void computeFeatures(const Array<File> &audioLoops);

private:
	AudioFormatManager formatManager;
	int blockSize;
	int hopSize;
	
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeatureExtractor)
};






#endif  // FEATUREEXTRACTOR_H_INCLUDED
