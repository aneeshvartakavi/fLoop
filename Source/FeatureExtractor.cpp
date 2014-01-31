/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Authors:  Aneesh Vartakavi, Cameron Summers
	
	A class that computes MIR features from the audio files.
  ==============================================================================
*/
#if JUCE_MAC
#include "Dense.h"
#include "FFT.h"
#endif

#if JUCE_WINDOWS
#include "Eigen\Dense.h"
#include "Eigen\FFT.h"
#endif

#include "FeatureExtractor.h"
#define _USE_MATH_DEFINES
#include <math.h>





FeatureExtractor::FeatureExtractor(const Array<File> &audioLoops, int numFeatures_, int blockSize_, int hopSize_)
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
	// Iterate through all our loops
	for(int i=0;i<audioLoops.size();i++)
	{
		File loop(audioLoops.getUnchecked(i));
		
		// Creating a reader for the file, depending on its format
		ScopedPointer<AudioFormatReader> fileReader = formatManager.createReaderFor(loop);
	
		ScopedPointer<AudioSampleBuffer> sampleBuffer = new AudioSampleBuffer(1,blockSize);
		sampleBuffer->clear();
		// Declare another buffer to read stereo audio, will not use if loop is mono
		ScopedPointer<AudioSampleBuffer> sampleBuffer2 = new AudioSampleBuffer(1,blockSize);
		sampleBuffer2->clear();

		// Should mix stereo to mono
		int numChannels = fileReader->numChannels;
		
		int sampleRate = static_cast<int>(fileReader->sampleRate);
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

		// Add features to featureVector for this file
        var& element = featureVector.getReference(i);
		// File name
		var fName(loop.getFileNameWithoutExtension());
		element.append(fName);
        
        // Tempo
		int tempo = static_cast<int>(calculateTempo(loop));
		element.append(var(tempo));
		
		var mfcc;
		computeMFCC(stft,mfcc);
		element.append(mfcc);

		var beatSpec;
		computeBeatSpectrum(stft,beatSpec,static_cast<int>(numBlocks),tempo,sampleRate);
		element.append(beatSpec);
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
    len = static_cast<int>(fileReader->lengthInSamples);
    Fs = static_cast<int>(fileReader->sampleRate);
    fbpm = static_cast<float>((60*8*Fs))/len; // 60 sec/min * number of beats * fs /len
	return static_cast<float>(adjustBPM(fbpm));
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
			}

		// Preprocessing
		// Remove slope, the line of best fit in least squared sense
		Eigen::VectorXf time = Eigen::VectorXf::Zero(numBlocks,1);
		Eigen::VectorXf timeSq = Eigen::VectorXf::Zero(numBlocks,1);
		for(int k=0; k<numBlocks; k++)
		{
			time[k] = static_cast<float>(k);
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
		float firstBeatTime = 60.0f/tempo;
		int firstBeatIndex = static_cast<int>(ceilf(((firstBeatTime*sampleRate) - blockSize/2)/hopSize));
		
		Eigen::Vector4f instBeat = Eigen::VectorXf::Zero(4,1);	
		for (int k=0;k<=4;k+=4)
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

}

void FeatureExtractor::computeMFCC(const Eigen::MatrixXf &spec, var& tempVar)
{
	// Hardcoding all filters
	Eigen::MatrixXf filters = Eigen::MatrixXf::Zero(26,513);
	filters.block(0,5,1,5) << 0.3333f , 0.6667f , 1.0000f , 0.6667f , 0.3333f;
	filters.block(1,8,1,4) << 0.2500f, 0.7500f, 0.7500f, 0.2500f;
	filters.block(2,10,1,5) <<  0.3333f , 0.6667f , 1.0000f , 0.6667f , 0.3333f;
	filters.block(3,12,1,5) << 0.3333f,0.6667f,1.0000f,0.6667f,0.3333f;
	filters.block(4,15,1,6) << 0.1667f,0.5000f,0.8333f,0.8333f,0.5000f,0.1667f;
	filters.block(5,18,1,7) << 0.25f,0.05f,0.75f,1,0.75f,0.5f,0.25f;
	filters.block(6,21,1,8) << 0.125f,0.375f,0.625f,0.875f,0.875f,0.625f,0.375f,0.125f;
	filters.block(7,25,1,8) << 0.125,0.375,0.625f,0.875f,0.875f,0.625f,0.375f,0.125f;
	filters.block(8,29,1,8) << 0.125f,0.375f,0.625f,0.875f,0.875f,0.625f,0.375f,0.125f;
	filters.block(9,33,1,9) << 0.2f,0.4f,0.6f,0.8f,1.0f,0.8f,0.6f,0.4f,0.2f;
	filters.block(10,37,1,11) << 0.1667f,0.333f,0.5f,0.6667f,0.8333f,1.0f,0.8333f,0.6667f,0.5f,0.333f,0.1667f;
	filters.block(11,42,1,12) << 0.0833f,0.25f,0.4167f,0.5833f,0.7500f,0.9167f,0.9167f,0.7500f,0.5833f,0.4167f,0.25f,0.0833f;
	filters.block(12,48,1,12) << 0.0833f,0.25f,0.4167f,0.5833f,0.7500f,0.9167f,0.9167f,0.7500f,0.5833f,0.4167f,0.25f,0.0833f;
	filters.block(13,54,1,13) <<  0.1429f,0.2857f,0.4286f,0.5714f, 0.7143f, 0.8571f, 1.0000f,0.8571f,0.7143f,0.5714f,0.4286f,0.2857f,0.1429f;
	filters.block(14,60,1,15) <<  0.1250f,0.2500f,0.3750f,0.5000f,0.6250f,0.7500f,0.8750f,1.0000f,0.8750f,0.7500f,0.6250f,0.5000f,0.3750f,0.2500f,0.1250f;
	filters.block(15,67,1,16) <<  0.0625f,0.1875f,0.3125f,0.4375f,0.5625f,0.6875f,0.8125f, 0.9375f,0.9375f,0.8125f,0.6875f,0.5625f,0.4375f,0.3125f,0.1875f,0.0625f;
	filters.block(16,75,1,17) <<  0.1111f,0.2222f,0.3333f,0.4444f,0.5556f,0.6667f,0.7778f,0.8889f,1.0000f,0.8889f,0.7778f, 0.6667f,0.5556f,0.4444f,0.3333f,0.2222f,0.1111f;
	filters.block(17,83,1,19) << 0.1f,0.2f,0.3f,0.4f,0.5f,0.6f,0.7f,0.8f,0.9f,1.0f,0.9f,0.8f,0.7f,0.6f,0.5f,0.4f,0.3f,0.2f,0.1f;
	filters.block(18,92,1,21) << 0.0909f,0.1818f, 0.2727f,0.3636f,0.4545f,0.5455f,0.6364f,0.7273f,0.8182f,0.9091f,1.0000f,0.9091f,0.8182f,0.7273f,0.6364f,0.5455f,0.4545f,0.3636f,0.2727f,0.1818f,0.0909f;
	filters.block(19,102,1,23) <<0.0833f,0.1667f,0.2500f,0.3333f,0.4167f,0.5000f,0.5833f,0.6667f,0.7500f,0.8333f,0.9167f,1.0000f,0.9167f,0.8333f,0.7500f,0.6667f,0.5833f,0.5000f,0.4167f,0.3333f,0.2500f,0.1667f,0.0833f;
	filters.block(20,113,1,25) <<0.0769f,0.1538f,0.2308f,0.3077f,0.3846f,0.4615f,0.5385f,0.6154f,0.6923f,0.7692f,0.8462f,0.9231f,1.0000f,0.9231f,0.8462f,0.7692f,0.6923f,0.6154f,0.5385f,0.4615f,0.3846f,0.3077f,0.2308f,0.1538f,0.0769f;
	filters.block(21,125,1,27) <<0.0714f,0.1429f,0.2143f,0.2857f,0.3571f,0.4286f,0.5000f,0.5714f,0.6429f,0.7143f,0.7857f,0.8571f,0.9286f,1.0000f,0.9286f,0.8571f,0.7857f,0.7143f,0.6429f,0.5714f,0.5000f,0.4286f,0.3571f,0.2857f,0.2143f,0.1429f,0.0714f;
	filters.block(22,138,1,30) << 0.0333f,0.1000f,0.1667f,0.2333f,0.3000f,0.3667f,0.4333f,0.5000f,0.5667f,0.6333f,0.7000f,0.7667f,0.8333f, 0.9000f, 0.9667f,0.9667f,0.9000f, 0.8333f,0.7667f,0.7000f,0.6333f,0.5667f,0.5000f,0.4333f,0.3667f,0.3000f,0.2333f,0.1667f,0.1000f,0.0333f;
	filters.block(23,152,1,32) << 0.0313f,0.0938f,0.1563f,0.2188f,0.2813f,0.3438f,0.4063f,0.4688f,0.5313f,0.5938f,0.6563f,0.7188f,0.7813f,0.8438f,0.9063f,0.9688f,0.9688f,0.9063f,0.8438f,0.7813f,0.7188f,0.6563f,0.5938f,0.5313f,0.4688f,0.4063f,0.3438f,0.2813f,0.2188f,0.1563f,0.0938f,0.0313f;
	filters.block(24,168,1,35) << 0.0556f,0.1111f,0.1667f,0.2222f,0.2778f,0.3333f,0.3889f,0.4444f,0.5000f,0.5556f,0.6111f,0.6667f,0.7222f,0.7778f,0.8333f,0.8889f,0.9444f,1.0000f,0.9444f,0.8889f,0.8333f,0.7778f,0.7222f,0.6667f,0.6111f,0.5556f,0.5000f,0.4444f,0.3889f,0.3333f,0.2778f,0.2222f,0.1667f,0.1111f,0.0556f;
	filters.block(25,185,1,38) << 0.0263f,0.0789f,0.1316f,0.1842f,0.2368f,0.2895f,0.3421f,0.3947f,0.4474f,0.5000f,0.5526f,0.6053f,0.6579f,0.7105f,0.7632f,0.8158f,0.8684f,0.9211f,0.9737f,0.9737f,0.9211f,0.8684f,0.8158f,0.7632f,0.7105f,0.6579f,0.6053f,0.5526f,0.5000f,0.4474f,0.3947f,0.3421f,0.2895f,0.2368f,0.1842f,0.1316f,0.0789f,0.0263f;

	// Compute periodogram
	Eigen::MatrixXf periodogram = spec.array() * spec.array();
	periodogram = periodogram.array()/512; // 512 = size(periodogram,1)

	// Find the products
	Eigen::MatrixXf energy = filters*periodogram;
	// Account for log(0) errors
	energy = energy.array() + 1e-20f;
	//Take log
	Eigen::MatrixXf logEnergy = energy.array().log();
	
	// Inverse DCT
	// First stage, create symmetric matrix
	Eigen::MatrixXf input = Eigen::MatrixXf::Zero(52,logEnergy.cols());
	int n1 = logEnergy.rows();
	
	for(int k=0;k<n1;k++)
	{
		input.row(k) = logEnergy.row(k);
		input.row(n1+k) = logEnergy.row(n1-k-1);
	}

	// Take the 2*n1 dimensional fft per column
	Eigen::FFT<float> mfcc_fft;
	
	Eigen::MatrixXcf dctc(input.rows(),input.cols());
	Eigen::MatrixXf dct(input.rows()/2,input.cols());
	//Eigen::MatrixXf stft(halfBlockSize,numBlocks);
	for (int k=0;k<input.cols();++k)
	{
		dctc.col(k) = mfcc_fft.fwd(input.col(k),input.rows());
	}

	// Multiply by W -k,2N
	std::complex<float> i1(0,-1);
	for (int k=0;k<26;++k)
	{
		std::complex<float> t((M_1_PI*k)/(2*n1),0);
		t = t*i1;
		t = exp(t);
		dctc.row(k) = dctc.row(k).array()*t;
		dct.row(k)= dctc.row(k).real();
	}
	
	// Extract 13 dimensional MFCC from 1-13
	Eigen::MatrixXf mfcc(13,input.cols());
	for (int k=0;k<13;++k)
	{
		mfcc.row(k) = dct.row(k);
	}
	
	// Compute derivative
	Eigen::MatrixXf mfcc_d = Eigen::MatrixXf::Zero(13,input.cols());
		
	for (int k=0;k<mfcc_d.cols()-1;++k)
	{
		mfcc_d.col(k) = mfcc.col(k+1) - mfcc.col(k);
	}
	
	// Compute mean
	Eigen::VectorXf mean = Eigen::VectorXf::Zero(13,1);
	Eigen::VectorXf mean_d = Eigen::VectorXf::Zero(13,1);
	Eigen::VectorXf std = Eigen::VectorXf::Zero(13,1);
	Eigen::VectorXf std_d = Eigen::VectorXf::Zero(13,1);
	
	for (int k=0;k<13;++k)
	{
		mean(k) = mfcc.row(k).mean();
		mean_d(k) = mfcc_d.row(k).mean();
	
		for (int t=0;t<mfcc.cols();++t)
		{
			std(k) += pow(mfcc(k,t) - mean(k),2);
			std_d(k) += pow(mfcc_d(k,t) - mean_d(k),2);
		}
	}

	// Append it to the var
	for (int k = 0; k < 13; k++)
	{
		tempVar.append(mean(k));
	}
	
	for (int k = 0; k < 13; k++)
	{
		tempVar.append(mean_d(k));
	}
	for (int k = 0; k < 13; k++)
	{
		tempVar.append(sqrtf(std(k)/mfcc.cols()));
	}
	for (int k = 0; k < 13; k++)
	{
		tempVar.append(sqrtf(std(k)/mfcc.cols()));
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
		loop->setProperty("MFCC",tempFeature[2]);
		loop->setProperty("Beat_Spectrum",tempFeature[3]);
		loopFeatures.add(loop);
		loop = nullptr;
	}
	features->setProperty("LoopFeatures",loopFeatures);

	// Extract a path
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
		
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
		var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
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


