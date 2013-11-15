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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "AudioSetup.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AudioSetup::AudioSetup (AudioDeviceManager& deviceManager_)
    : deviceManager (deviceManager_)
{
    addAndMakeVisible (deviceSelector = new AudioDeviceSelectorComponent (deviceManager, 0, 2, 0, 2, true, true, true, false));

    addAndMakeVisible (backButton = new TextButton ("backButton"));
    backButton->setButtonText ("Back");
    backButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1024, 768);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AudioSetup::~AudioSetup()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deviceSelector = nullptr;
    backButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioSetup::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::lightgrey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioSetup::resized()
{
    deviceSelector->setBounds (8, 8, getWidth() - 176, getHeight() - 140);
    backButton->setBounds (200, 688, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioSetup::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == backButton)
    {
        //[UserButtonCode_backButton] -- add your button handler code here..
		setVisible(false);
        //[/UserButtonCode_backButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioSetup" componentName=""
                 parentClasses="public Component" constructorParams="AudioDeviceManager&amp; deviceManager_"
                 variableInitialisers="deviceManager (deviceManager_)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="0"
                 initialWidth="1024" initialHeight="768">
  <BACKGROUND backgroundColour="ffd3d3d3"/>
  <GENERICCOMPONENT name="" id="a04c56de9f3fc537" memberName="deviceSelector" virtualName=""
                    explicitFocusOrder="0" pos="8 8 176M 140M" class="AudioDeviceSelectorComponent"
                    params="deviceManager, 0, 2, 0, 2, true, true, true, false"/>
  <TEXTBUTTON name="backButton" id="b4447ff6df4f9f33" memberName="backButton"
              virtualName="" explicitFocusOrder="0" pos="200 688 150 24" buttonText="Back"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
