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
	// Example of feature function
	float* calculateFFT(const AudioSampleBuffer& sampleBuffer);
	float* spectralCentroid(float* magSpectrum);
	
private:
	AudioFormatManager formatManager;
	int blockSize;
	int hopSize;
	// feature is a pointer to an array of float pointers
	// All the feature fuctions should return a float pointer, which can be appended to the array.
	// The pointer to the array can then effeciently be sent to other classes.

	ScopedPointer<Array<float*>> featureVector;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeatureExtractor)
};






#endif  // FEATUREEXTRACTOR_H_INCLUDED
