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

#ifndef __JUCE_HEADER_6160AAC046B72536__
#define __JUCE_HEADER_6160AAC046B72536__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
   Controls the Audio Device Setup for fLoop, a window where the user can
   select the input and output audio devices.

   Authors:  Aneesh Vartakavi, Cameron Summers
                                                                    //[/Comments]
*/
class AudioSetup  : public Component,
                    public ButtonListener
{
public:
    //==============================================================================
    AudioSetup (AudioDeviceManager& deviceManager_);
    ~AudioSetup();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<AudioDeviceSelectorComponent> deviceSelector;
    ScopedPointer<TextButton> backButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSetup)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_6160AAC046B72536__
