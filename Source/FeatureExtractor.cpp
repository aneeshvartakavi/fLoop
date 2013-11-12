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
    fftEngine = new FFTEngine(blockSize);

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
				// Create a new sample buffer to convert from stereo to mono
				AudioSampleBuffer monoBuffer(1,blockSize);
				// Important to clear, since we are adding from other buffers
				monoBuffer.clear();
				monoBuffer.addFrom(0,0,*sampleBuffer,0,0,blockSize,0.5);
				monoBuffer.addFrom(0,0,*sampleBuffer,1,0,blockSize,0.5);
				sampleBuffer = new AudioSampleBuffer(monoBuffer);
			}


			//Send the sampleBuffer to feature functions
			// 
			//float* sampleData = audioSamples->getSampleData(0);
			//float minum = findMaximum(sampleData,blockSize);
			
		}			
	}
}

void FeatureExtractor::setReader(AudioFormatReader *reader)
{
    buffer.setSize(1, fftBlockSize);  // TODO get channels
    fftBuffer.setSize(fftBlockSize);
    fileReader = reader;
    numAudioFrames = reader->lengthInSamples;
    
    fileFFTData.resize(numAudioFrames*fftBlockSize);
    fileSCF.resize(numAudioFrames);
}

void FeatureExtractor::extractAllFeaturesFromFile()
{
    
    getFileFFT();
    getFileSpectralCrestFactor();
    
    
}

void FeatureExtractor::getFileFFT()
{
    int n=0;
    
    for(int i=0; i<fileReader->lengthInSamples; i += fftHopSize)
    {
        
        int bufferLocation = 0;
        int sampleLocation = i;
        
        fileReader->read(&buffer, bufferLocation, fftBlockSize, sampleLocation, true, false);
        
        float* samples = buffer.getSampleData(0);  // TODO just getting one channel, need to handle multiple channels
        
        fftEngine->performFFT(samples);
        fftEngine->findMagnitudes(&fftBuffer);
        
        float* fftMags = fftBuffer.getData();
        
        for(int j=0; j<fftBlockSize; j++)
        {
            std::cout << fftMags[j] << std::endl;
            fileFFTData.push_back(fftMags[j]);
        }
        // TODO store pointer to magnitudes in array for each block
        n++;
    }
}


float FeatureExtractor::getBlockSpectralCrestFactor(float* block)
{
    float maxFFTVal = block[0];
    float spectralSum = block[0];
    
    for(int i=1; i<(fftBlockSize/2 - 1); i++){
        
        if(block[i] > maxFFTVal){
            maxFFTVal = block[i];
        }
        
        spectralSum += block[i];
    }
    
    return maxFFTVal / spectralSum;
}

void FeatureExtractor::getFileSpectralCrestFactor()
{
    for(int i=0; i<numAudioFrames; i++)
    {
        fileSCF[i] = getBlockSpectralCrestFactor(&(fileFFTData[i]));
    }
    
}

float FeatureExtractor::getFileFeatureAvg(std::vector<float*> feature)
{
    float avgScf = 0.0;
    for(int i=0; i<numAudioFrames; i++)
    {
        avgScf += *(feature[i]);
    }
    avgScf = avgScf/numAudioFrames;
    
    return avgScf;
}

float FeatureExtractor::getFileFeatureStd(std::vector<float*> feature)
{
    float stdScf = 0.0;
    for(int i=0; i<numAudioFrames; i++)
    {
        // Std
    }
    
    return stdScf;
}

