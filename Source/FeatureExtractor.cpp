/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/
//#include "Eigen/

#include "Eigen/Dense.h"
#include "Eigen/FFT.h"
#include "FeatureExtractor.h"
//#include <fstream>
//#include <iterator>



FeatureExtractor::FeatureExtractor(const Array<File> &audioLoops, int numFeatures_, int blockSize_, int hopSize_, int fftSizelog2):fftEngine(fftSizelog2)
{
	
	// Create an empty var to hold each feature
	// Change this to an enum that species what features

	numFeatures = numFeatures_+1; // Num features + path to file
	
	featureVector.insertMultiple(0,var(),audioLoops.size());
	
	blockSize = blockSize_;
	hopSize = hopSize_;
	
	halfBlockSize = (blockSize/2)+1;
	
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
		
		// Define a matrix to hold all the audio
		Eigen::MatrixXf audioBuffer(blockSize,numBlocks);

		
		// For FFT
		for (int j=0;j<numBlocks-1;j++)
		{
			// Check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,false);
			
			//if(numChannels == 2)
			//{
			//	// Read the right channel into our second buffer
			//	fileReader->read(sampleBuffer2,0,blockSize,0,false,true);
			//	// Reducing gain so there is no clipping 		
			//	sampleBuffer->applyGain(0.5);
			//	sampleBuffer2->applyGain(0.5);
			//	// Adding the samples to the final buffer
			//	sampleBuffer->addFrom(0,0,*sampleBuffer2,0,0,blockSize);
			//}
			
			// sampleBuffer now has the audio samples, do something with them
			 
			float* sampleData = sampleBuffer->getSampleData(0);
			
			for(int k=0;k<blockSize;k++)
			{
				// Copy the samples into the matrix
				// Can potentially use Eigen::Map to speed up
				audioBuffer(k,j) = sampleData[k];
			}
		}
		// We now have all the samples for the audio file
		
		// Compute FFT
		Eigen::FFT<float> fft;
		fft.SetFlag(fft.HalfSpectrum);
		

		Eigen::MatrixXcf stftc(halfBlockSize,numBlocks);
		Eigen::MatrixXf stft(halfBlockSize,numBlocks);
		for (int k=0;k<audioBuffer.cols();++k)
		{
			stftc.col(k) = fft.fwd(audioBuffer.col(k),blockSize);
			stft.col(k) = stftc.col(k).real().cwiseAbs();
		
		}

		// Compute beat spectrum
//		var beatSpec;
////		computeBeatSpectrum(stft,beatSpec,numBlocks);
////
//		DBG(JSON::toString(beatSpec));

		 // Add features to featureVector for this file
  //      var& element = featureVector.getReference(i);
		////element.convertToArray();
  //      // File name
		//var fName(loop.getFileNameWithoutExtension());
		//element.append(fName);
  //      
  //      // Tempo
		//var tempo = calculateTempo(loop);
		//element.append(tempo);
  //      
  //      // Spectral Crest Factor
  //      //std::pair<float, float> scfDistr = calculateSpectralCrestFactor(fftData, numBlocks-1);
  //      //element.append(scfDistr.first); // mean
  //      //element.append(scfDistr.second); // std
  //      

  //      // Beat Spectrum
		//var beatSpectrum = calcBeatSpectrum(fftData, numBlocks-1);
		////var beatSpectrum = calcBeatSpectrum(fftData, numBlocks-1);
		////Array<var> beatSpec;
		//DBG(JSON::toString(beatSpectrum));
		//var beatSpec;
		//beatSpec.ensureStorageAllocated(beatSpectrum.size());
	/*	for(int kw = 0; kw<beatSpectrum.size();kw++)
		{
			beatSpec.insert(kw,beatSpectrum[kw]);
			DBG(String(beatSpectrum[kw]) + " " + JSON::toString(beatSpec[kw]));
		}

		
		element.append(beatSpec);*/


		//var beatSpec(beatSpectrum);

//        std::ofstream output_file("./example.txt");
//        std::ostreambuf_iterator<std::string> output_iterator(output_file, "\n");
//        std::copy(beatSpectrum.begin(), beatSpectrum.end(), output_iterator);
        
	}
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

void FeatureExtractor::computeBeatSpectrum(const Eigen::MatrixXf &stft, var& tempVar,int numBlocks)
{
	//Compute similarity matrix for beatspectrum
		
		Eigen::MatrixXf similarityMatrix = Eigen::MatrixXf::Zero(numBlocks,numBlocks);
		Eigen::VectorXf mags = stft.colwise().norm();
			
		for(int k=0;k<numBlocks;k++)
		{
			// Start from k, exploit symmetry
			for(int l=k;l<numBlocks;l++)
			{
				float dist = stft.col(k).transpose() * stft.col(l);
				//float dist = stft.col(k).dot(stft.col(l));
				dist = dist/(mags(k) * mags(l));
				similarityMatrix(l,k) = dist;
			}
		}
		
		// Find diagonal sums
		Eigen::VectorXf diagSums = Eigen::VectorXf::Zero(numBlocks,1);
		diagSums(0) = similarityMatrix.diagonal(0).sum();
		for (int k=0;k<numBlocks;++k)
        {
            diagSums(k) = similarityMatrix.diagonal(-k).sum();
//				tempVar.append(diagSums(k));
        }
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

	 // Initialize the feature vector, do we need this
	 //featureVector.insertMultiple(0,var(),length);

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


