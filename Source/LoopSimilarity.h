/*
  ==============================================================================

    LoopSimilarity.h
   
    Author:  Aneesh, Cameron
	
	This class will get the info from FeatureExtractor and interface with
	LoopPlayer to suggest similar loops.
	
  ==============================================================================
*/

#ifndef LOOPSIMILARITY_H_INCLUDED
#define LOOPSIMILARITY_H_INCLUDED

#include "JuceHeader.h"
#include "FeatureExtractor.h"

class LoopSimilarity
{
	
public:
	LoopSimilarity(Array<var> featureVector_);
	~LoopSimilarity();

	void returnSimilar(int sliderIndex, int sliderMax, int sliderMin,const File &referenceFile);
	StringArray* returnSimilarTempo(int sliderMax, int sliderMin, const File &referenceFile);
private:
	
	Array<var> featureVector;

	ScopedPointer<Array<File>> audioLoops;

	ScopedPointer<StringArray> similarFiles;

	void readFile(File* path);



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopSimilarity);
};




#endif  // LOOPSIMILARITY_H_INCLUDED
