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
#include "qm_dsp\dsp\mfcc\MFCC.h"

class FeatureExtractor
{

public:
	FeatureExtractor(const Array<File> &audioLoops, int numFeatures, int blockSize_,int hopSize_, int fftSizeLog2);
	~FeatureExtractor();
	void computeFeatures(const Array<File> &audioLoops);
	void computeFeatures(int index);
	// Index is used to place the element in the appropriate row of the var
	void calculateTempo();
	// Old code, has to be modified
	float* calculateFFT(float* sampleData);
	float* calculateMFCC(float* sampleData);
	float* spectralCentroid(float* magSpectrum);
	
	// Writes a text file in the parent directory
	void writeFile(const File &pathToDirectory);
	// Reads the parent directory to see if there
	bool readFile(const File& pathToDirectory);
	

private:
	// The variable that stores all the features.
	// Initialize it with number of features and loops
	Array<var> featureVector;
	
	// This stores the list of files
	Array<File> fileList;

	AudioFormatManager formatManager;
	int blockSize;
	int hopSize;
	int numFeatures;
	drow::FFTEngine fftEngine;
	enum features
	{

	};

	// feature is a pointer to an array of float pointers
	// All the feature fuctions should return a float pointer, which can be appended to the array.
	// The pointer to the array can then effeciently be sent to other classes.

	//ScopedPointer<Array<float*>> featureVector;

	// Returns adjusted BPM
	inline int adjustBPM(float fbpm)
	{
		int bpm = roundFloatToInt(fbpm);
		// Round off tempo to closest multiple of 5
		bpm = (bpm + 5-1) - (bpm - 1)%5;
		// Define anything from 80-160 as range of tempo
		if(bpm>160)
		{
			return bpm/2;
		}

		else
		{
			return bpm;
		}

	}

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeatureExtractor)
};






#endif  // FEATUREEXTRACTOR_H_INCLUDED
