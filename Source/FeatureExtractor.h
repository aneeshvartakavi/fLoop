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

#include "dRowAudio.h"

using namespace drow;


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
    
    std::vector<float> fileFFTData;
    int fftBlockSize;
    int fftHopSize;
    int numAudioFrames;
    
    std::vector<float> fileSCF;
    
    AudioSampleBuffer buffer = AudioSampleBuffer(1, 1);  // holds the samples fed to FFTEngine
    FFTEngine* fftEngine;
    AudioFormatReader* fileReader;
    Buffer fftBuffer = Buffer(1); // holds the magnitudes from fft on one block
    
    
    void setReader(AudioFormatReader *reader);
    void getFileFFT();
    void extractAllFeaturesFromFile();
    
    float getBlockSpectralCrestFactor(float* block);
    void getFileSpectralCrestFactor();
    
    
    float getFileFeatureAvg(std::vector<float*> feature);
    float getFileFeatureStd(std::vector<float*> feature);

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeatureExtractor)
};






#endif  // FEATUREEXTRACTOR_H_INCLUDED
