/*
  ==============================================================================

    LoopSimilarity.cpp
    
    Author:  Aneesh, Cameron

  ==============================================================================
*/

#include "LoopSimilarity.h"

LoopSimilarity::LoopSimilarity()
{
	// Preallocate memory
	similarFiles.ensureStorageAllocated(100);
}

//LoopSimilarity::LoopSimilarity(Array<var> featureVector_)
//{
//	featureVector = featureVector_;
//}

LoopSimilarity::~LoopSimilarity()
{
	audioLoops = nullptr;
	//similarFiles = nullptr;
}

void LoopSimilarity::returnSimilar(int sliderIndex, int sliderMax, int sliderMin,const File &referenceFile)
{
	// sliderIndex is a parameter to specify which slider was changed
	
}

StringArray LoopSimilarity::returnSimilarTempo(int sliderMax, int sliderMin,const File &referenceFile)
{
	// Get filename
	String fileName = referenceFile.getFileNameWithoutExtension();
	
	//similarFiles = new StringArray();
	//similarFiles->ensureStorageAllocated(20);
	int referenceIndex = 0;
	int referenceTempo = 0;
	// Look for the file in the featurevector
	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
//		DBG(JSON::toString(tempVar));
		if(fileName == tempVar[0].toString())
		{
			// Get the tempo
			referenceTempo = tempVar[1].toString().getIntValue();
			referenceIndex = i;
			break;
		}

	}
	// Find similar loops

	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
		//DBG(JSON::toString(tempVar));
		if(tempVar.size()>0)
		{
			int currentTempo =  tempVar[1].toString().getIntValue();
			if(currentTempo>=referenceTempo+sliderMin && currentTempo<= referenceTempo+sliderMax && i!= referenceIndex)
			{
				// If condition is satisfied, add the fileName into a stringArray
				//DBG(tempVar[0].toString());
				//similarFiles->add(tempVar[0].toString());
				similarFiles.add(tempVar[0].toString());
			}

		}


	}

	return similarFiles;

}

StringArray LoopSimilarity::returnSimilarRhythm(float sliderMax, float sliderMin, const File &referenceFile)
{
	String fileName = referenceFile.getFileNameWithoutExtension();
	
	StringArray similarFiles;
	similarFiles.ensureStorageAllocated(20);

	int referenceIndex = 0;
	// Look for the file in the featurevector
	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
//		DBG(JSON::toString(tempVar));
		if(fileName == tempVar[0].toString())
		{
			// Get the tempo
			referenceIndex = i;
			break;
		}

	}

	// Select the index
	Eigen::VectorXf refVector = bsFeatures.col(referenceIndex);
	
	int numCols = bsFeatures.cols();


	// Create a copy of featureVector
	Eigen::MatrixXf tempFeature = bsFeatures;
	//tempFeature.col(referenceIndex) *= 100; // Make the vector itself large, so it will not turn up in the results
	Eigen::VectorXf distVector = Eigen::VectorXf::Zero(numCols);

	for(int k=0;k<tempFeature.cols();k++)
	{
		Eigen::VectorXf distVector1 = tempFeature.col(k) -refVector;
		distVector(k) = distVector1.norm();
				
	}

	float maxVal = distVector.maxCoeff();
	// Normalizing distance to 1
	distVector = distVector/maxVal;

	//for(int k=0; k<distVector.rows();k++)
	//{
//		DBG(String(distVector(k)));
//	}


	for(int k=0; k<distVector.rows();k++)
	{
		//DBG(String(distVector(k)));
		if(distVector(k)<=sliderMax && distVector(k)>=sliderMin)
		{
			var tempVar = featureVector.getUnchecked(k); 

			//String tempName = tempVar[0].toString();
			similarFiles.add(tempVar[0].toString());
		}

	}

	
	return similarFiles;

}

StringArray LoopSimilarity::returnSimilarTimbre(float sliderMax, float sliderMin, const File &referenceFile)
{
	String fileName = referenceFile.getFileNameWithoutExtension();
	
	StringArray similarFiles;
	similarFiles.ensureStorageAllocated(40);

	int referenceIndex = 0;
	// Look for the file in the featurevector
	for (int i=0;i<featureVector.size();i++)
	{
		var tempVar = featureVector.getUnchecked(i); 
		if(fileName == tempVar[0].toString())
		{
			// Get the tempo
			referenceIndex = i;
			break;
		}

	}

	// Select the index
	Eigen::VectorXf refVector = mfccFeatures.col(referenceIndex);
	
	int numCols = mfccFeatures.cols();


	// Create a copy of featureVector
	Eigen::MatrixXf tempFeature = mfccFeatures;
	//tempFeature.col(referenceIndex) *= 100; // Make the vector itself large, so it will not turn up in the results
	Eigen::VectorXf distVector = Eigen::VectorXf::Zero(numCols);

	for(int k=0;k<tempFeature.cols();k++)
	{
		Eigen::VectorXf distVector1 = tempFeature.col(k) -refVector;
			
		distVector(k) = distVector1.norm();
				
	}

	float maxVal = distVector.maxCoeff();
	// Normalizing distance to 1
	distVector = distVector/maxVal;

	for(int k=0; k<distVector.rows();k++)
	{
		DBG(String(distVector(k)));
	}


	for(int k=0; k<distVector.rows();k++)
	{
		
		if(distVector(k)<=sliderMax && distVector(k)>=sliderMin)
		{
			var tempVar = featureVector.getUnchecked(k); 

			//String tempName = tempVar[0].toString();
			similarFiles.add(tempVar[0].toString());
		}

	}
	
	return similarFiles;

}


void LoopSimilarity::readCache(const File& pathToDirectory)
{
		// Always call this after a call to check if cache exists
	String tempPath = pathToDirectory.getFullPathName() + String("\\floop_cache.txt");
	File cache(tempPath);
	
	// Clear state
	 var result = JSON::parse(cache).getProperty(Identifier("LoopFeatures"),0);
	// DBG(JSON::toString(result));
	 int numLoops = result.getArray()->size();
	 bsFeatures = Eigen::MatrixXf::Zero(10,numLoops); // Ten for now
	 mfccFeatures = Eigen::MatrixXf::Zero(52,numLoops);
	 // Initialize the feature vector, do we need this?
	 featureVector.insertMultiple(0,var(),numLoops);

	 for(int i=0;i<numLoops;i++)
	 {
		 // Get the data from the cache file
		 String path = result[i].getProperty(Identifier("Path"),0);
		 int tempo = result[i].getProperty(Identifier("Tempo"),0);
		 var beatSpec = result[i].getProperty(Identifier("Beat_Spectrum"),0);
		 var mfcc = result[i].getProperty(Identifier("MFCC"),0);
		 // Add it to the feature vector
		 var& tempFeatures = featureVector.getReference(i);
		 // Make sure the order is consistent
		 tempFeatures.append(path);
		// DBG(JSON::toString(tempFeatures));
		// DBG(JSON::toString(mfcc));
		 tempFeatures.append(tempo);
		 // Add the beat spectrum features to the matrix
		 for(int k=0;k<beatSpec.size();k++)
		 {
			 bsFeatures(k,i) = beatSpec[k];
		 }
		 
		 for(int k=0;k<mfcc.size();k++)
		 {
			 mfccFeatures(k,i) = mfcc[k];
		 }
		 //tempFeatures.append(beatSpec);
	 }

	 // Normalize bsfeature vectors
	 for(int i=0;i<bsFeatures.rows();i++)
	 {
		 // Extract rows, set mean to 0
		 Eigen::VectorXf rowVec = bsFeatures.row(i);
		 
		 float meanVal = rowVec.mean();
		 float stdVal=0.0;
		 for (int k=0;k<rowVec.rows();++k)
		 {
			stdVal += pow((rowVec[k] - meanVal),2);
		 }

		 stdVal = sqrtf(stdVal/rowVec.rows());

		 bsFeatures.row(i) = (rowVec.array()-meanVal)/stdVal;

	}
	
	 
	// Normalize mfcc feature vectors
	 for(int i=0;i<mfccFeatures.rows();i++)
	 {
		 // Extract rows, set mean to 0
		 Eigen::VectorXf rowVec = mfccFeatures.row(i);
		 
		 float meanVal = rowVec.mean();
		 float stdVal=0.0;
		 for (int k=0;k<rowVec.rows();++k)
		 {
			stdVal += pow((rowVec[k] - meanVal),2);
		 }

		 stdVal = sqrtf(stdVal/rowVec.rows());

		 mfccFeatures.row(i) = (rowVec.array()-meanVal)/stdVal;

	}

	 //DBG(JSON::toString(featureVector));

}

