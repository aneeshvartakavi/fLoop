/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "FeatureExtractor.h"
#include <fstream>
#include <iterator>

FeatureExtractor::FeatureExtractor(const Array<File> &audioLoops, int numFeatures_, int blockSize_, int hopSize_, int fftSizelog2):fftEngine(fftSizelog2)
{
	
	// Create an empty var to hold each feature
	// Change this to an enum that species what features

	numFeatures = numFeatures_+1; // Num features + path to file
	
	featureVector.insertMultiple(0,var(),audioLoops.size());
	
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
    // Insert names as the first entry
	
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
		int64 numBlocks= fileReader->lengthInSamples%hopSize;
		
		// Accounting for non-integer multiples of blockSize
		numBlocks= (fileReader->lengthInSamples+numBlocks)/hopSize;
		// For FFT
		
        std::vector<float> fftData;  // point to all fft data for file
		for (int j=0;j<numBlocks-1;j++)
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
			
            float* blockFFTData = calculateFFT(sampleData);
            for(int i=0; i<blockSize; i++){
                fftData.push_back(blockFFTData[i]);  // faster way?
            }
			
			// Do a check for nan using std::isnan()
			// May need to threshold values, the 10^-38 can be considered 0.
						
			// Uncomment to debug
			//for(int w=0;w<blockSize;w++)
			//DBG(String(res[w]));
						
		}
        
        // Add features to featureVector for this file
        var& element = featureVector.getReference(i);

        // File name
        element.append(loop.getFileNameWithoutExtension());
        
        // Tempo
        element.append(calculateTempo(loop));
        
        // Spectral Crest Factor
        std::pair<float, float> scfDistr = calculateSpectralCrestFactor(fftData, numBlocks-1);
        element.append(scfDistr.first); // mean
        element.append(scfDistr.second); // std
        
        // Beat Spectrum
        std::vector<float> beatSpectrum = calcBeatSpectrum(fftData, numBlocks-1);
                
//        std::ofstream output_file("./example.txt");
//        std::ostreambuf_iterator<std::string> output_iterator(output_file, "\n");
//        std::copy(beatSpectrum.begin(), beatSpectrum.end(), output_iterator);
        
	}
}

void FeatureExtractor::computeFeatures(int index)
{
	// Need some sort of input to decide what features to run
	// Make sure the order of calls is consistent
	// This makes sure that the database and the cache file store data in the right fields
	// Insert names as the first entry
	for(int i=0;i<fileList.size();i++)
	{		
		File tempFile = fileList.getUnchecked(i);
		var& element = featureVector.getReference(i);
		element.append(tempFile.getFileNameWithoutExtension());
	}	

	// Then estimate tempo
//	calculateTempo();
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

float FeatureExtractor::calculateTempo(File loop)
{
//	int numLoops = fileList.size();
	// Preallocating some temporary variables
	int len = 0;
	int Fs = 0;
	float fbpm;

    // Creating a reader for the file, depending on its format
    ScopedPointer<AudioFormatReader> fileReader = formatManager.createReaderFor(loop);

    // Perform out calculations
    len = fileReader->lengthInSamples;
    Fs = fileReader->sampleRate;
    return fbpm = (60*8*Fs)/len; // 60bpm * number of beats * fs /len
    
//    // Get reference to the feature vector
//    var& tempVar = featureVector.getReference(i);
//    // Append the tempo
//    tempVar.append(adjustBPM(fbpm));
}

std::pair<float, float> FeatureExtractor::calculateSpectralCrestFactor(std::vector<float> fftData, int numBlocks){
    // Retruns a pair object with the mean and avg spectral crest factor for the file
    // remember FFT for all blocks are in one vector so watch indexing
    
    std::pair<float, float> distr;
    
    float scfMean = 0.0;
    float scfStd = 0.0;
    
    float runningSumSquares = 0.0;  // used to get running std
    
    for(int j=0; j<numBlocks; j++){  // iterate over each block
        
        float maxFFTVal = fftData[0];  // init
        float spectralSum = fftData[0];  // init
        
        for(int i=0; i<(blockSize/2 - 1); i++){  // iterate over fft data in block
            
            if(fftData[j*blockSize + i] > maxFFTVal){
                maxFFTVal = fftData[j*blockSize + i];
            }            
            spectralSum += fftData[j*blockSize + i];
        }
        
        float scf = maxFFTVal / spectralSum;  // calc scf for flock
        
        //---Optimization: moving avg and std
        runningSumSquares += square(scf);
        scfMean = (scfMean + j*scf)/(j+1);
        scfStd = sqrtf(runningSumSquares - square(scfMean));
        
    }
    
    distr.first = scfMean;
    distr.second = scfStd;
    
    return distr;
}

std::vector<float> FeatureExtractor::calcBeatSpectrum(std::vector<float> fftData, int numBlocks){
    // See paper for description of this
    // THE BEAT SPECTRUM: A NEW APPROACH TO RHYTHM ANALYSIS, Foote, J., Uchihashi, S.
    
    std::vector<float> beatSpectrum;
    
    for(int l=0; l<numBlocks; l++){
        
        float b_l = 0.0;        
        std::vector<float> refVect(fftData.begin() + (numBlocks * l), fftData.begin() + (numBlocks * l) + blockSize);
                
        for(int k=0; k<numBlocks; k++) {
            std::vector<float> otherVect(fftData.begin() + (numBlocks * k), fftData.begin() + (numBlocks * k) + blockSize);
            b_l += calcFFTEuclDist(refVect, otherVect);
        }
        
        beatSpectrum.push_back(b_l);
    }
    
    return beatSpectrum;
}


float FeatureExtractor::calcFFTEuclDist(std::vector<float> vect1, std::vector<float> vect2){
    // Calculate euclidean distance between two vectors
    // Assuming they are the same size, TODO catch this as an error        
    
    int vectSize = vect1.size();
    float euclDist = 0.0;
    
    for(int i=0; i<vectSize; i++){
        float sq = square(vect1[i] - vect2[i]);
        euclDist += sq;
    }
    
    return euclDist;
}



void FeatureExtractor::writeCache(const File& pathToDirectory)
{
	
	DynamicObject* features = new DynamicObject();
	
	Array<var> loopFeatures;

	//var tempFeature;
	for(int i=0;i<fileList.size();i++)
	{
		DynamicObject* loop = new DynamicObject();
		var tempFeature = featureVector[i];
		
		// Add all the properties
		loop->setProperty("Path",tempFeature[0]);
		loop->setProperty("Tempo",tempFeature[1]);
		loop->setProperty("SCF_Mean",tempFeature[2]);
        loop->setProperty("SCF_Std", tempFeature[3]);
		
		loopFeatures.add(loop);
		loop = nullptr;
	}
	features->setProperty("LoopFeatures",loopFeatures);

	// Extract a path
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	//tempPath = tempPath ;
	
	// Create a new cache file
	File cache(tempPath);
	
	// Delete the old file if it exists
	cache.deleteFile();
	// Create a file output stream to write to
	FileOutputStream tempStream(cache);
	// Write to the stream
	JSON::writeToStream(tempStream,features,false);
	
}

bool FeatureExtractor::cacheExists(const File& pathToDirectory)
{
	// Call this function to check if a cache exists
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	File cache(tempPath);
	return cache.exists();
}

void FeatureExtractor::readCache(const File& pathToDirectory)
{
	// Always call this after a call to check if cache exists
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	File cache(tempPath);
	
	// Clear state
	 var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
	 int numBlocks= result.getArray()->size();


	 for(int i=0;i<numBlocks;i++)
	 {
		 // Get the data from the cache file
		 String path = result[i].getProperty(Identifier("Path"),0);
		 int tempo = result[i].getProperty(Identifier("Tempo"),0);
	 
		// Add it to the feature vector
		 var& tempFeatures = featureVector.getReference(i);
		 // Make sure the order is consistent
		 tempFeatures.append(path);
		 tempFeatures.append(tempo);
	 }

}

Array<var> FeatureExtractor::returnFeatureVector()
{
	return featureVector;
}


