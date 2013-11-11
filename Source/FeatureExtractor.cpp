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
		//ScopedPointer<AudioSubsectionReader> subReader = new AudioSubsectionReader(fileReader,0,fileReader->lengthInSamples, true);
		// Set frame length to 2048 for now
		ScopedPointer<AudioSampleBuffer> audioSamples = new AudioSampleBuffer(fileReader->numChannels,blockSize);
		// Clearing buffer, may not be needed
		audioSamples->clear();
		
		int64 length = fileReader->lengthInSamples%hopSize;
		// Accounting for non-integer multiples of blockSize
		length = (fileReader->lengthInSamples+length)/hopSize;

		for (int j=0;j<length-1;j++)
		{
			// Not sure of the last two arguments, check if blockSize, hopSize implementation is correct
			fileReader->read(audioSamples,0,blockSize,j*hopSize,true,true);
			//Process the samples
			float* sampleData = audioSamples->getSampleData(0);
			float minum = findMaximum(sampleData,blockSize);
			
		}

			
	}
}

