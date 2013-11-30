/*
  ==============================================================================

    FeatureExtractor.h
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#ifndef FEATUREEXTRACTOR_H_INCLUDED
#define FEATUREEXTRACTOR_H_INCLUDED

#include "Eigen/Dense.h"
#include "JuceHeader.h"
//#include "Eigen/Dense.h"


class FeatureExtractor
{

public:
	FeatureExtractor(const Array<File> &audioLoops, int numFeatures, int blockSize_,int hopSize_, int fftSizeLog2);
	~FeatureExtractor();
	void computeFeatures(const Array<File> &audioLoops);
	
	
	float calculateTempo(File loop);
    
	void computeBeatSpectrum(const Eigen::MatrixXf &spec, var& tempVar,int num_blocks,int tempo, int sampleRate);
	
	std::pair<float, float> calculateSpectralCrestFactor(std::vector<float> fftData, int length);
	
    
	// Old code, has to be modified
    float* calculateMFCC(std::vector<float> &fftVector);
	
	float* spectralCentroid(float* magSpectrum);
	
	// Writes a text file in the parent directory
	void writeCache(const File &pathToDirectory);
	// Returns true if a cache exists, no check if the files in the cache match the files of the directory
	bool cacheExists(const File &pathToDirectory);
	// Reads the parent directory to see if there
	void readCache(const File& pathToDirectory);
	
	// Returns some data elements
	Array<var> returnFeatureVector();

private:
	// The variable that stores all the features.
	// Initialize it with number of features and loops
	Array<var> featureVector;
	
	// This stores the list of files
	Array<File> fileList;
	
	// To manage reading audio formats
	AudioFormatManager formatManager;
	
	// For the FFT
	int blockSize;
	int hopSize;
	int numFeatures;
	int halfBlockSize;
	drow::FFTEngine fftEngine;
	
	// Returns adjusted BPM
	inline int adjustBPM(float fbpm)
	{
		int bpm = roundFloatToInt(fbpm);
		// Round off tempo to closest multiple of 5
		//bpm = (bpm + 5-1) - (bpm - 1)%5;
		// Define anything from 80-160 as range of tempo
		if(bpm>160)
		{
			return bpm/2;
		}

		if(bpm<50)
		{
			
			return bpm*2;
		}


		else
		{
			return bpm;
		}

	}

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeatureExtractor)
};






#endif  // FEATUREEXTRACTOR_H_INCLUDED
