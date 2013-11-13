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
		
		// Creating a reader for the file, depending on its format
		ScopedPointer<AudioFormatReader> fileReader = formatManager.createReaderFor(loop);
		// Redundant - Creating a source reader that reads from the fileReader
		//ScopedPointer<AudioFormatReaderSource> sourceReader = new AudioFormatReaderSource(fileReader,true);
				
		ScopedPointer<AudioSampleBuffer> sampleBuffer = new AudioSampleBuffer(1,blockSize);
		sampleBuffer->clear();
		// Declare another buffer to read stereo audio, will not use if loop is mono
		ScopedPointer<AudioSampleBuffer> sampleBuffer2 = new AudioSampleBuffer(1,blockSize);
		sampleBuffer2->clear();

		int numChannels = fileReader->numChannels;
		int64 length = fileReader->lengthInSamples%hopSize;
		
		// Accounting for non-integer multiples of blockSize
		length = (fileReader->lengthInSamples+length)/hopSize;
		// For FFT
		
		for (int j=0;j<length-1;j++)
		{
			// Check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,false);
			
			if(numChannels == 2)
			{
				// Read the right channel into our second buffer
				fileReader->read(sampleBuffer2,0,blockSize,0,false,true);
				// Reducing gain so there is no clipping 		
				sampleBuffer->applyGain(0.5);
				sampleBuffer2->applyGain(0.5);
				// Adding the samples to the final buffer
				sampleBuffer->addFrom(0,0,*sampleBuffer2,0,0,blockSize);
			}
			
			// sampleBuffer now has the audio samples, do something with them
			 
			float* sampleData = sampleBuffer->getSampleData(0);
			
			float *res = calculateFFT(sampleData);
			
			// Do a check for nan using std::isnan()
			// May need to threshold values, the 10^-38 can be considered 0.
						
			// Uncomment to debug
			for(int w=0;w<blockSize;w++)
			DBG(String(res[w]));
						
		}

			
	}
}

float* FeatureExtractor::calculateFFT(float* sampleData)
{
	// Get the samples
	
	// Initialize the FFT stuff
	fftEngine.setWindowType(drow::Window::Hann);
	fftEngine.performFFT (sampleData);
	fftEngine.findMagnitudes();
	
	return fftEngine.getMagnitudesBuffer().getData();
}


float* FeatureExtractor::calculateMFCC(float* sampleData)
{
	return nullptr;
}

