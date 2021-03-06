/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_9002020A4DD09B20__
#define __JUCE_HEADER_9002020A4DD09B20__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "AudioSetup.h"
#include "LoopPlayer.h"
#include "FeatureExtractor.h"
#include "WavFileFilter.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
   A minimilastic GUI for fLoop.

   Authors:  Aneesh Vartakavi, Cameron Summers
                                                                    //[/Comments]
*/
class MainComponent  : public Component,
                       public ButtonListener
{
public:
    //==============================================================================
    MainComponent ();
    ~MainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void readDirectory();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Array<File> audioLoops;
	ScopedPointer<AudioSetup> audioSetup;
	ScopedPointer<LoopPlayer> loopPlayer;
	ScopedPointer<FeatureExtractor> featureExtractor;
	AudioDeviceManager deviceManager;
	File pathToDirectory;
	ScopedPointer<WavFileFilter> wavFilter;
	ScopedPointer<CustomFileFilter> customFileFilter;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TextButton> browseButton;
    ScopedPointer<TextButton> setupButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_9002020A4DD09B20__
