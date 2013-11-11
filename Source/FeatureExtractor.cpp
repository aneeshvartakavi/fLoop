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
		int numChannels = fileReader->numChannels;
		ScopedPointer<AudioSampleBuffer> sampleBuffer = new AudioSampleBuffer(numChannels,blockSize);
		
		// Clearing buffer, may not be necessary
		sampleBuffer->clear();
		
		int64 length = fileReader->lengthInSamples%hopSize;
		// Accounting for non-integer multiples of blockSize
		length = (fileReader->lengthInSamples+length)/hopSize;

		for (int j=0;j<length-1;j++)
		{
			// Not sure of the last two arguments, check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,true);
			
			if(numChannels==2)
			{
				
				// There's a bug in here somewhere
				//// Create a new sample buffer to convert from stereo to mono
				//AudioSampleBuffer monoBuffer(1,blockSize);
				//// Important to clear, since we are adding from other buffers
				//monoBuffer.clear();
				//monoBuffer.addFrom(0,0,*sampleBuffer,0,0,blockSize,0.5);
				//monoBuffer.addFrom(0,0,*sampleBuffer,1,0,blockSize,0.5);
				//sampleBuffer = new AudioSampleBuffer(monoBuffer);
			}


			//Send the sampleBuffer to feature functions
			// 
			//float* sampleData = audioSamples->getSampleData(0);
			//float minum = findMaximum(sampleData,blockSize);
			
		}

			
	}
}

