/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh

  ==============================================================================
*/

#include "FeatureExtractor.h"

FeatureExtractor::FeatureExtractor(const Array<File> &audioLoops, int numFeatures_, int blockSize_, int hopSize_, int fftSizelog2):fftEngine(fftSizelog2)
{
	
	// Create an empty var to hold each feature
	//var tempVar = var();
	
	numFeatures = numFeatures_+1; // Num features + path to file
	
	//for(int i=0; i<numFeatures; i++) 
	//{
	//	tempVar.insert(i,var());
	//}
	
	//DBG(JSON::toString(tempVar));
	// Create an array of vars for each file
	featureVector.insertMultiple(0,var(),audioLoops.size());
	/*for(int i=0; i<audioLoops.size(); i++) 
		featureVector.insert(i,var());
	*/
	
	blockSize = blockSize_;
	hopSize = hopSize_;
	// Register basic formats to read
	formatManager.registerBasicFormats();

	// Initialize pointer to the list of files
	fileList = audioLoops;
	
}

FeatureExtractor::~FeatureExtractor()
{
	//fileList = nullptr;
}

void FeatureExtractor::computeFeatures(const Array<File> &audioLoops)
{
	// Will be rewritten
	
	
	// First thing to do is write the filenames to var

		
		
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
			//for(int w=0;w<blockSize;w++)
			//DBG(String(res[w]));
						
		}

			
	}
}

void FeatureExtractor::computeFeatures(int index)
{
	// Insert names as the first entry
	for(int i=0;i<fileList.size();i++)
	{
		
		File tempFile = fileList.getUnchecked(i);
		var& element = featureVector.getReference(i);
		element.append(tempFile.getFileNameWithoutExtension());
	}	

	// Then estimate tempo
	calculateTempo();
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
	//MFCCConfig config(44100);
	//config.fftsize=2048;
	//config.nceps = 13;
	//config.logpower = false;
	//
	////MFCC a(config);

	return nullptr;
}

void FeatureExtractor::calculateTempo()
{
	int numLoops = fileList.size();
	// Preallocating some temporary variables
	int len = 0;
	int Fs = 0;
	float fbpm;
	
	for(int i=0;i<numLoops;i++)
	{
		File loop(fileList.getUnchecked(i));
		// Creating a reader for the file, depending on its format
		ScopedPointer<AudioFormatReader> fileReader = formatManager.createReaderFor(loop);

		// Perform out calculations
		len = fileReader->lengthInSamples;
		Fs = fileReader->sampleRate;
		fbpm = (60*8*Fs)/len; // 60bpm * number of beats * fs /len
		
		// Get reference to the feature vector
		var& tempVar = featureVector.getReference(i);
		// Append the tempo
		tempVar.append(adjustBPM(fbpm));
	}

	
}

void FeatureExtractor::writeFile(const File& pathToDirectory)
{
	
	DynamicObject* features = new DynamicObject();
	
	Array<var> loopFeatures;

	//var tempFeature;
	for(int i=0;i<fileList.size();i++)
	{
		DynamicObject* loop = new DynamicObject();
		var tempFeature = featureVector[i];
		
//		DBG(String(tempFeature[0]));
		
		// Add all the properties
		loop->setProperty("Path",tempFeature[0]);
		loop->setProperty("Tempo",tempFeature[1]);
		
		loopFeatures.add(loop);
		loop = nullptr;
	}
	features->setProperty("LoopFeatures",loopFeatures);

	// Extract a path
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	//tempPath = tempPath ;
	
	File cache(tempPath);
	FileOutputStream tempStream(cache);
	JSON::writeToStream(tempStream,features,false);
	

}
