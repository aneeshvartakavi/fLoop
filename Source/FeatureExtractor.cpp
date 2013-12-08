/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 10 Nov 2013 7:34:20pm
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "FeatureExtractor.h"
#define _USE_MATH_DEFINES
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
			/*float minVal;
			float maxVal;
			sampleBuffer->findMinMax(0,0,blockSize,minVal,maxVal);
			
			sampleBuffer->applyGain(1.0/maxVal);*/
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
			
		var mfcc;
		computeMFCC(stft,mfcc,numBlocks,tempo,sampleRate);
		element.append(mfcc);

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



		//DBG(JSON::toString(tempVar));
	
}

void FeatureExtractor::computeMFCC(const Eigen::MatrixXf &spec, var& tempVar,int num_blocks,int tempo, int sampleRate)
{
	// Hardcoding all filters
	Eigen::MatrixXf filters = Eigen::MatrixXf::Zero(26,513);
	filters.block(0,5,1,5) << 0.3333 , 0.6667 , 1.0000 , 0.6667 , 0.3333;
	filters.block(1,8,1,4) << 0.2500, 0.7500, 0.7500, 0.2500;
	filters.block(2,10,1,5) <<  0.3333 , 0.6667 , 1.0000 , 0.6667 , 0.3333;
	filters.block(3,12,1,5) << 0.3333,0.6667,1.0000,0.6667,0.3333;
	filters.block(4,15,1,6) << 0.1667,0.5000,0.8333,0.8333,0.5000,0.1667;
	filters.block(5,18,1,7) << 0.25,0.05,0.75,1,0.75,0.5,0.25;
	filters.block(6,21,1,8) << 0.125,0.375,0.625,0.875,0.875,0.625,0.375,0.125;
	filters.block(7,25,1,8) << 0.125,0.375,0.625,0.875,0.875,0.625,0.375,0.125;
	filters.block(8,29,1,8) << 0.125,0.375,0.625,0.875,0.875,0.625,0.375,0.125;
	filters.block(9,33,1,9) << 0.2,0.4,0.6,0.8,1.0,0.8,0.6,0.4,0.2;
	filters.block(10,37,1,11) << 0.1667,0.333,0.5,0.6667,0.8333,1.0,0.8333,0.6667,0.5,0.333,0.1667;
	filters.block(11,42,1,12) << 0.0833,0.25,0.4167,0.5833,0.7500,0.9167,0.9167,0.7500,0.5833,0.4167,0.25,0.0833;
	filters.block(12,48,1,12) << 0.0833,0.25,0.4167,0.5833,0.7500,0.9167,0.9167,0.7500,0.5833,0.4167,0.25,0.0833;
	filters.block(13,54,1,13) <<  0.1429,0.2857,0.4286,0.5714, 0.7143, 0.8571, 1.0000,0.8571,0.7143,0.5714,0.4286,0.2857,0.1429;
	filters.block(14,60,1,15) <<  0.1250,0.2500,0.3750,0.5000,0.6250,0.7500,0.8750,1.0000,0.8750,0.7500,0.6250,0.5000,0.3750,0.2500,0.1250;
	filters.block(15,67,1,16) <<  0.0625,0.1875,0.3125,0.4375,0.5625,0.6875,0.8125, 0.9375,0.9375,0.8125,0.6875,0.5625,0.4375,0.3125,0.1875,0.0625;
	filters.block(16,75,1,17) <<  0.1111,0.2222,0.3333,0.4444,0.5556,0.6667,0.7778,0.8889,1.0000,0.8889,0.7778, 0.6667,0.5556,0.4444,0.3333,0.2222,0.1111;
	filters.block(17,83,1,19) << 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1;
	filters.block(18,92,1,21) << 0.0909,0.1818, 0.2727,0.3636,0.4545,0.5455,0.6364,0.7273,0.8182,0.9091,1.0000,0.9091,0.8182,0.7273,0.6364,0.5455,0.4545,0.3636,0.2727,0.1818,0.0909;
	filters.block(19,102,1,23) <<0.0833,0.1667,0.2500,0.3333,0.4167,0.5000,0.5833,0.6667,0.7500,0.8333,0.9167,1.0000,0.9167,0.8333,0.7500,0.6667,0.5833,0.5000,0.4167,0.3333,0.2500,0.1667,0.0833;
	filters.block(20,113,1,25) <<0.0769,0.1538,0.2308,0.3077,0.3846,0.4615,0.5385,0.6154,0.6923,0.7692,0.8462,0.9231,1.0000,0.9231,0.8462,0.7692,0.6923,0.6154,0.5385,0.4615,0.3846,0.3077,0.2308,0.1538,0.0769;
	filters.block(21,125,1,27) <<0.0714,0.1429,0.2143,0.2857,0.3571,0.4286,0.5000,0.5714,0.6429,0.7143,0.7857,0.8571,0.9286,1.0000,0.9286,0.8571,0.7857,0.7143,0.6429,0.5714,0.5000,0.4286,0.3571,0.2857,0.2143,0.1429,0.0714;
	filters.block(22,138,1,30) << 0.0333,0.1000,0.1667,0.2333,0.3000,0.3667,0.4333,0.5000,0.5667,0.6333,0.7000,0.7667,0.8333, 0.9000, 0.9667,0.9667,0.9000, 0.8333,0.7667,0.7000,0.6333,0.5667,0.5000,0.4333,0.3667,0.3000,0.2333,0.1667,0.1000,0.0333;
	filters.block(23,152,1,32) << 0.0313,0.0938,0.1563,0.2188,0.2813,0.3438,0.4063,0.4688,0.5313,0.5938,0.6563,0.7188,0.7813,0.8438,0.9063,0.9688,0.9688,0.9063,0.8438,0.7813,0.7188,0.6563,0.5938,0.5313,0.4688,0.4063,0.3438,0.2813,0.2188,0.1563,0.0938,0.0313;
	filters.block(24,168,1,35) << 0.0556,0.1111,0.1667,0.2222,0.2778,0.3333,0.3889,0.4444,0.5000,0.5556,0.6111,0.6667,0.7222,0.7778,0.8333,0.8889,0.9444,1.0000,0.9444,0.8889,0.8333,0.7778,0.7222,0.6667,0.6111,0.5556,0.5000,0.4444,0.3889,0.3333,0.2778,0.2222,0.1667,0.1111,0.0556;
	filters.block(25,185,1,38) << 0.0263,0.0789,0.1316,0.1842,0.2368,0.2895,0.3421,0.3947,0.4474,0.5000,0.5526,0.6053,0.6579,0.7105,0.7632,0.8158,0.8684,0.9211,0.9737,0.9737,0.9211,0.8684,0.8158,0.7632,0.7105,0.6579,0.6053,0.5526,0.5000,0.4474,0.3947,0.3421,0.2895,0.2368,0.1842,0.1316,0.0789,0.0263;

	// Compute periodogram
	Eigen::MatrixXf periodogram = spec.array() * spec.array();
	periodogram = periodogram.array()/512; // 512 = size(periodogram,1)

	// Find the products
	Eigen::MatrixXf energy = filters*periodogram;
	// Account for log(0) errors
	energy = energy.array() + 1e-20;
	//Take log
	Eigen::MatrixXf logEnergy = energy.array().log();
	
	//DBG(String(logEnergy.rows()));
	//DBG(String(logEnergy.cols()));
	// Inverse DCT
	// First stage, create symmetric matrix
	Eigen::MatrixXf input = Eigen::MatrixXf::Zero(52,logEnergy.cols());
	int n1 = logEnergy.rows();
	
	for(int k=0;k<n1;k++)
	{
		input.row(k) = logEnergy.row(k);
		//DBG(String(n1+k+1) + String(" to ") +  String(n1-k-1));
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
	
	//for(int k=0;k<dctc.rows();k++)
	//{
	//	DBG(String(dctc(k,0).real()) + " + " + String(dctc(k,0).imag()));
	//}

	/*for(int k=0;k<dct.rows();k++)
	{
		DBG(String(dct(k,0)));
	}*/

	// Extract 13 dimensional MFCC from 1-13
	Eigen::MatrixXf mfcc(13,input.cols());
	for (int k=0;k<13;++k)
	{
		mfcc.row(k) = dct.row(k);
	}
	
	/*for (int k=0;k<13;++k)
	{
		DBG(String(mfcc(k,0)));
	}*/

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
		loop->setProperty("MFCC",tempFeature[2]);
		loop->setProperty("Beat_Spectrum",tempFeature[3]);
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


