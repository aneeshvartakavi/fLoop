/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh

  ==============================================================================
*/

#include "FeatureExtractor.h"

FeatureExtractor::FeatureExtractor(int blockSize_, int hopSize_)
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

		//ScopedPointer<AudioSubsectionReader> subReader = new AudioSubsectionReader(fileReader,0,fileReader->lengthInSamples, true);
		int numChannels = fileReader->numChannels;
		if(numChannels==2)
		{
			AudioSampleBuffer stereoBuffer(2,blockSize);
			sampleBuffer->clear();
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

		for (int j=0;j<length-1;j++)
		{
			// Not sure of the last two arguments, check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,true);
			
			// sampleBuffer now has the audio samples, do something with them
			 
			float* sampleData = sampleBuffer->getSampleData(0);
			DBG(String(sampleData[0]));
			
		}

			
	}
}

