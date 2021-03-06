/*
  ==============================================================================

    LoopSimilarity.h
   
    Authors:  Aneesh Vartakavi, Cameron Summers
	
	This class will get the info from FeatureExtractor and interface with
	LoopPlayer to suggest similar loops.
	
  ==============================================================================
*/

#ifndef LOOPSIMILARITY_H_INCLUDED
#define LOOPSIMILARITY_H_INCLUDED

#include "JuceHeader.h"
#include "FeatureExtractor.h"
#if JUCE_MAC
#include "Dense.h"
#endif

#if JUCE_WINDOWS
#include "Eigen\Dense.h"
#endif


class LoopSimilarity
{
	
public:
	LoopSimilarity();

	~LoopSimilarity();

	StringArray returnSimilarTempo(int sliderMax, int sliderMin, const File &referenceFile);
	StringArray returnSimilarRhythm(float sliderMax, float sliderMin, const File &referenceFile);
	StringArray returnSimilarTimbre(float sliderMax, float sliderMin, const File &referenceFile);
	
	// Previously, featureExtractor used to read the cache, and pass a large data structure
	// over to loopSimilarity, which is kinda pointless. Migrating that function over here.
	void readCache(const File &pathToDirectory);
private:
	
	Array<var> featureVector;

	ScopedPointer<Array<File>> audioLoops;

	StringArray similarFiles;
	void readFile(File* path);
	Eigen::MatrixXf bsFeatures;
	Eigen::MatrixXf mfccFeatures;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopSimilarity);
};




#endif  // LOOPSIMILARITY_H_INCLUDED
