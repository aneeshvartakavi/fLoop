/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "FeatureExtractor.h"
#include <math.h>
//#include <cmath>
#include "Eigen\Dense.h"
#include "Eigen\FFT.h"


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
		int sampleRate = fileReader->sampleRate;
		int64 numSamples = fileReader->lengthInSamples;
		int64 numBlocks= numSamples%hopSize;
		
		// Accounting for non-integer multiples of blockSize
		numBlocks= (numSamples+numBlocks)/hopSize;
		
		numBlocks+=1;

		// Define a matrix to hold all the audio
		Eigen::MatrixXf audioBuffer(blockSize,numBlocks);

		
		// For FFT
		for (int j=0;j<numBlocks;j++)
		{
			// Check if blockSize, hopSize implementation is correct
			fileReader->read(sampleBuffer,0,blockSize,j*hopSize,true,false);
			float minVal;
			float maxVal;
			sampleBuffer->findMinMax(0,0,blockSize,minVal,maxVal);
			
			sampleBuffer->applyGain(1.0/maxVal);
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

	/*	if(stftc.allFinite())
		{

		}
		else
		{*/
			//("NAN found");
			//for(int k1=0;k1<stft.rows();k1++)
			//{
			//	for(int k2=0;k2<stft.cols();k2++)
			//	{
			//		if(stft(k1,k2)!=stft(k1,k2))
			//		{
			//			stft(k1,k2) = 0;
			//		}

			//	}

			//}

		//}


		//stft = stft.array().log();
		// Compute beat spectrum
		
		//DBG(JSON::toString(beatSpec));

		// Add features to featureVector for this file
        var& element = featureVector.getReference(i);
		// File name
		var fName(loop.getFileNameWithoutExtension());
		element.append(fName);
        
        // Tempo
		int tempo = calculateTempo(loop);
		element.append(var(tempo));
	//	DBG(JSON::toString(element));
			
		var beatSpec;
		computeBeatSpectrum(stft,beatSpec,numBlocks,tempo,sampleRate);
		//DBG(JSON::toString(beatSpec));
		element.append(beatSpec);

	//	DBG(JSON::toString(element));
		      
  //      // Spectral Crest Factor
  //      //std::pair<float, float> scfDistr = calculateSpectralCrestFactor(fftData, numBlocks-1);
  //      //element.append(scfDistr.first); // mean
  //      //element.append(scfDistr.second); // std
  //      
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

    // Perform calculations
    len = fileReader->lengthInSamples;
    Fs = fileReader->sampleRate;
    fbpm = (60*8*Fs)/len; // 60bpm * number of beats * fs /len
	return adjustBPM(fbpm);
//    // Get reference to the feature vector
//    var& tempVar = featureVector.getReference(i);
//    // Append the tempo
//    tempVar.append(adjustBPM(fbpm));
}

void FeatureExtractor::computeBeatSpectrum(const Eigen::MatrixXf &stft, var& tempVar,int numBlocks, int tempo, int sampleRate)
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
				if(mags(k)!=0 && mags(l)!=0)
					dist = dist/(mags(k) * mags(l));
				else
					dist = 0;
				similarityMatrix(l,k) = dist;
			
			}
		}
		
		// Find diagonal sums
		Eigen::VectorXf colSums = Eigen::VectorXf::Zero(numBlocks,1);
		
		for (int k=0;k<numBlocks;++k)
			{
				colSums(k) = similarityMatrix.diagonal(-k).sum();
				//DBG(String(colSums(k)));
				//tempVar.append(diagSums(k));
			}

		// Preprocessing
		// Remove slope, the line of best fit in least squared sense
		Eigen::VectorXf time = Eigen::VectorXf::Zero(numBlocks,1);
		Eigen::VectorXf timeSq = Eigen::VectorXf::Zero(numBlocks,1);
		for(int k=0; k<numBlocks; k++)
		{
			time[k] = k;
			//timeSq[k] = k*k;
		}

		float meanX = time.mean();
		float meanY = colSums.mean();

		float sumX = time.sum();
		float sumY = colSums.sum();

		timeSq = time.array() * time.array();

		float sumX2 = timeSq.sum();
		float sumXY = (time.cwiseProduct(colSums)).sum();

		float slope = (sumXY - ((sumX*sumY)/numBlocks))/(sumX2 - (pow(sumX,2)/numBlocks));
		float yIntercept = meanY - slope*meanX;

		Eigen::VectorXf trend = Eigen::VectorXf::Zero(numBlocks,1);
		trend = ((time.array() * slope) + yIntercept);

		Eigen::VectorXf beatSpectrum = colSums - trend;
		// Normalize
		float maxVal = beatSpectrum.cwiseAbs().maxCoeff();
		beatSpectrum = beatSpectrum.array()/maxVal;

		// Moving average to smooth out signal

		for(int k=2; k<numBlocks; k++)
		{
			beatSpectrum[k] = (beatSpectrum[k] + beatSpectrum[k-1] + beatSpectrum[k-2])/3;
		}

		// Add the slope to the feature vector
		tempVar.append(slope);
		tempVar.append(yIntercept);
		
		// For now, let the features go here, and append to tempVar
		// To do, combine the for loops to one, should improve performance
		// Mean
		
		
		float meanBeatSpectrum = beatSpectrum.mean();
		//tempVar.append(meanBeatSpectrum);
		// STD
		float stdBeatSpectrum=0.0;
		for (int k=0;k<numBlocks;++k)
		{
			stdBeatSpectrum += pow((beatSpectrum[k] - meanBeatSpectrum),2);
		}
		stdBeatSpectrum = sqrtf(stdBeatSpectrum/numBlocks);
		tempVar.append(stdBeatSpectrum);

		// Skewness
		float skewnessBeatSpectrum = 0.0;
		for (int k=0;k<numBlocks;++k)
		{
			skewnessBeatSpectrum += pow((beatSpectrum[k] - meanBeatSpectrum),3);
		}

		skewnessBeatSpectrum = skewnessBeatSpectrum / (pow(stdBeatSpectrum,3) *numBlocks);
		tempVar.append(skewnessBeatSpectrum);

		// Kurtosis
		float kurtosisBeatSpectrum = 0.0;
		for (int k=0;k<numBlocks;++k)
		{
			kurtosisBeatSpectrum += pow((beatSpectrum[k] - meanBeatSpectrum),4);
		}

		kurtosisBeatSpectrum = ((kurtosisBeatSpectrum/numBlocks)/pow(stdBeatSpectrum,4)-3);
		tempVar.append(kurtosisBeatSpectrum);
		
		// Amplitude of first peak to second peak
		//float ppRatio = 0.0;
		//for (int k=1;k<numBlocks;++k)
		//{
		//	float slope = beatSpectrum(k) - beatSpectrum(k-1);

		//	if(slope>0)
		//	{
		//		// Find the max value in the remaining vector
		//		maxVal = beatSpectrum(k);
		//		for(int t=k+1;t<numBlocks; ++t)
		//		{
		//			if(beatSpectrum(t)>maxVal)
		//				maxVal = beatSpectrum(t);
		//		}

		//		// Fill this in
		//		ppRatio = beatSpectrum(0)/maxVal;

		//	}

		//}

		//tempVar.append(ppRatio);
		//tempVar.append();

		// Cumulative difference between BS and straight line through signal
		float lineSlope = (beatSpectrum(numBlocks-1) - beatSpectrum(0))/numBlocks;
		float cumulativeDifference = 0.0;

		for (int k=1;k<numBlocks;++k)
		{
			float refPt = lineSlope * k + beatSpectrum(0);
			cumulativeDifference += abs(beatSpectrum(k)-refPt);
		}
		tempVar.append(cumulativeDifference);

		// Number of changes in slope

		int numSlopeChanges = 0;
		int slopeSign = -1;

		for (int k=1;k<numBlocks;++k)
		{
			float slope = beatSpectrum(k) - beatSpectrum(k-1);
			if((slope >0 && slopeSign == -1)|| (slope <0 && slopeSign == 1))
			{
				slopeSign = -1 * slopeSign;
				numSlopeChanges++;
			}

		}

		// Instantaneous energy per beat
		float firstBeatTime = 60.0/tempo;
		int firstBeatIndex = ceilf(((sampleRate*firstBeatTime) - blockSize/2)/hopSize);
		
		Eigen::Vector4f instBeat = Eigen::VectorXf::Zero(4,1);	
		for (int k=0;k<=12;k+=4)
		{
			instBeat[0]+= beatSpectrum(((k * firstBeatIndex)+ 1));
			instBeat[1]+= beatSpectrum((((k+1) * firstBeatIndex)+ 1));
			instBeat[2]+= beatSpectrum((((k+2) * firstBeatIndex)+ 1));
			instBeat[3]+= beatSpectrum((((k+3) * firstBeatIndex)+ 1));
		}

		float coeff = instBeat.cwiseAbs().sum();
		instBeat = instBeat.array()/ coeff;
		
		for(int k=0;k<4;k++)
		{
			tempVar.append(instBeat(k));
		}

		// Energy per beat
		Eigen::Vector4f enBeat = Eigen::VectorXf::Zero(4,1);	



		//DBG(JSON::toString(tempVar));
	
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
		//DBG(JSON::toString(tempFeature[2]));
		//DynamicObject* beatSpec = new DynamicObject();
		//beatSpec->setProperty("Beat_spec",tempFeature[2]);
		//loop->setProperty("Beat_Spectrum",beatSpec);
		loop->setProperty("Beat_Spectrum",tempFeature[2]);
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
	bool returnVal = false;
	if(cache.existsAsFile())
	{
		var tempR = JSON::parse(cache);
	//	DBG(JSON::toString(tempR));
		var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
//		DBG(JSON::toString(result));
		int numLoops = result.getArray()->size();
		if(numLoops==fileList.size())
		{
			returnVal = true;
			for(int i=0;i<numLoops;i++)
			 {
				 // Get the data from the cache file
				 String path = result[i].getProperty(Identifier("Path"),0);
				 if(path==fileList[i].getFileNameWithoutExtension())
				 continue;
				 else
				 {
					 DBG("Files don't match");
					 returnVal = false;
					 break;
				 }
			}
		}
	}

	return returnVal;
}

void FeatureExtractor::readCache(const File& pathToDirectory)
{
	// Always call this after a call to check if cache exists
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	File cache(tempPath);
	
	// Clear state
	 var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
	 int numLoops = result.getArray()->size();

	 // Initialize the feature vector, do we need this
	 //featureVector.insertMultiple(0,var(),length);

	 for(int i=0;i<numLoops;i++)
	 {
		 // Get the data from the cache file
		 String path = result[i].getProperty(Identifier("Path"),0);
		 int tempo = result[i].getProperty(Identifier("Tempo"),0);
		 var beatSpec = result[i].getProperty(Identifier("Beat_Spectrum"),0);
		// Add it to the feature vector
		 var& tempFeatures = featureVector.getReference(i);
		 // Make sure the order is consistent
		 tempFeatures.append(path);
		 tempFeatures.append(tempo);
		 tempFeatures.append(beatSpec);
	 }

}

Array<var> FeatureExtractor::returnFeatureVector()
{
	return featureVector;
}


