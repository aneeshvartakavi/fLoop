/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh

  ==============================================================================
*/

#include "FeatureExtractor.h"

FeatureExtractor::FeatureExtractor(int blockSize_, int hopSize_, int fftSizelog2):fftEngine(fftSizelog2)
{
	blockSize = blockSize_;
	hopSize = hopSize_;
}

FeatureExtractor::~FeatureExtractor()
{

}

void FeatureExtractor::computeFeatures(const Array<File> &audioLoops)
{
	formatManager.registerBasicFormats();
	
	// Iterate through all our loops
	for(int i=0;i<audioLoops.size();i++)
	{
		File loop(audioLoops.getUnchecked(i));
		ScopedPointer<AudioFormatReader> fileReader = formatManager.createReaderFor(loop);
		
		ScopedPointer<AudioSampleBuffer> sampleBuffer = new AudioSampleBuffer(1,blockSize);
		sampleBuffer->clear();

		if(fileReader->numChannels==2)
		{
			// Convert Stereo to mono
			AudioSampleBuffer stereoBuffer(2,blockSize);
			stereoBuffer.clear();
			sampleBuffer->addFrom(0,0,stereoBuffer,0,0,blockSize,0.5);
			sampleBuffer->addFrom(0,0,stereoBuffer,1,0,blockSize,0.5);
		}
		else
		{
			DBG("More than 2 audio channels!");
		}

		int64 length = fileReader->lengthInSamples%hopSize;
		// Accounting for non-integer multiples of blockSize
		length = (fileReader->lengthInSamples+length)/hopSize;

		// For FFT
		
		for (int j=0;j<length-1;j++)
		{
			// Not sure of the last two arguments, check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,true);
			
			// sampleBuffer now has the audio samples, do something with them
			 
			float* sampleData = sampleBuffer->getSampleData(0);
			
			float *res = calculateFFT(sampleData);
			// Uncomment to debug
			for(int w=0;w<blockSize;w++)
			DBG(String(res[w]));
						
		}

			
	}
}

float* FeatureExtractor::calculateFFT(float* sampleData)
{
	// Get the samples
	//float *sampleData = sampleBuffer->getSampleData(0);
	// Initialize the FFT stuff
	fftEngine.setWindowType(drow::Window::Hann);
	fftEngine.performFFT (sampleData);
	fftEngine.findMagnitudes();
	
	return sampleData;
}


