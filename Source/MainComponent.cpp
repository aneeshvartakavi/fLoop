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
#include "AudioSetup.h"
//[/Headers]

#include "MainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MainComponent::MainComponent ()
{
    addAndMakeVisible (browseButton = new TextButton ("browseButton"));
    browseButton->setExplicitFocusOrder (20);
    browseButton->setButtonText ("Browse");
    browseButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight);
    browseButton->addListener (this);
    browseButton->setColour (TextButton::buttonColourId, Colours::grey);
    browseButton->setColour (TextButton::buttonOnColourId, Colour (0xab000000));

    addAndMakeVisible (setupButton = new TextButton ("setupButton"));
    setupButton->setButtonText ("Setup");
    setupButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight);
    setupButton->addListener (this);
    setupButton->setColour (TextButton::buttonColourId, Colours::grey);
    setupButton->setColour (TextButton::buttonOnColourId, Colour (0xfb000000));
    setupButton->setColour (TextButton::textColourOnId, Colours::black);
    setupButton->setColour (TextButton::textColourOffId, Colours::black);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1024, 768);


    //[Constructor] You can add your own custom stuff here..
	//
	deviceManager.initialise(2, 2, 0, true, String::empty, 0);

	AudioIODeviceType* const audioDeviceType = deviceManager.getCurrentDeviceTypeObject();
	StringArray audioInputDevices (audioDeviceType->getDeviceNames(true));
    StringArray audioOutputDevices (audioDeviceType->getDeviceNames(false));
	AudioDeviceManager::AudioDeviceSetup deviceConfig;
    deviceManager.getAudioDeviceSetup(deviceConfig);

	deviceConfig.inputDeviceName = audioInputDevices[0];
	deviceConfig.outputDeviceName= audioOutputDevices[0];
    String result = deviceManager.setAudioDeviceSetup (deviceConfig, true);
	DBG(result);

	wavFilter = new WavFileFilter(".wav");
	customFileFilter = new CustomFileFilter("new");
	customFileFilter->clearFilters();
    //[/Constructor]
}

MainComponent::~MainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    browseButton = nullptr;
    setupButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

	audioSetup = nullptr;
	wavFilter = nullptr;
	featureExtractor = nullptr;
	loopPlayer = nullptr;
	customFileFilter = nullptr;
    //[/Destructor]
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xecdfdfdf));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainComponent::resized()
{
    browseButton->setBounds (200, 688, 150, 24);
    setupButton->setBounds (696, 688, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == browseButton)
    {
        //[UserButtonCode_browseButton] -- add your button handler code here..
		readDirectory();
		if(pathToDirectory.exists())
		{
			featureExtractor = new FeatureExtractor(audioLoops,1,1024,512,10);

			if(featureExtractor->cacheExists(pathToDirectory))
			{
				bool cacheSelected = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon,"Cache Found","fLoop detected a pre-processed cache for this directory. Would you like to use it and save time?",
					"Yes","No",this);
				if (cacheSelected)
				{
					featureExtractor->readCache(pathToDirectory);
				}
				else
				{
					// Else do the same thing as you do when cache is not found
					featureExtractor->computeFeatures(audioLoops);
//					featureExtractor->writeCache(pathToDirectory);
				}
			}
			else
			{
				// Cache not found
				featureExtractor->computeFeatures(audioLoops);
				//featureExtractor->writeCache(pathToDirectory);
			}

			addAndMakeVisible(loopPlayer = new LoopPlayer(deviceManager,pathToDirectory,wavFilter,audioLoops,customFileFilter));
			//addAndMakeVisible(loopPlayer = new LoopPlayer(deviceManager,pathToDirectory,wavFilter,featureExtractor->returnFeatureVector(),audioLoops,customFileFilter));

		}
        //[/UserButtonCode_browseButton]
    }
    else if (buttonThatWasClicked == setupButton)
    {
        //[UserButtonCode_setupButton] -- add your button handler code here..
		addAndMakeVisible(audioSetup = new AudioSetup(deviceManager));
        //[/UserButtonCode_setupButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void MainComponent::readDirectory()
{
	FileChooser fileChooser ("Select directory to load...", File::getSpecialLocation (File::userHomeDirectory));
		if(fileChooser.browseForDirectory())
		{

			pathToDirectory = fileChooser.getResult();
			if(pathToDirectory.exists())
			{
				Array<File> wavFiles;
				pathToDirectory.findChildFiles(wavFiles,3,true,"*.wav");
				audioLoops.addArray(wavFiles);

				Array<File> mp3Files;
				pathToDirectory.findChildFiles(mp3Files,3,true,"*.mp3");
				audioLoops.addArray(mp3Files);

				/*File directories = fileChooser.getResult();
				Array<File> wavFiles;
				directories.findChildFiles(wavFiles,3,true,"*.wav");
				audioLoops.addArray(wavFiles);

				Array<File> mp3Files;
				directories.findChildFiles(mp3Files,3,true,"*.mp3");
				audioLoops.addArray(mp3Files);
				pathToDirectory = directories;*/
			}

		}
		else
		{
			pathToDirectory = File();
		}
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="1024" initialHeight="768">
  <BACKGROUND backgroundColour="ecdfdfdf"/>
  <TEXTBUTTON name="browseButton" id="82045a96457e5019" memberName="browseButton"
              virtualName="" explicitFocusOrder="20" pos="200 688 150 24" bgColOff="ff808080"
              bgColOn="ab000000" buttonText="Browse" connectedEdges="3" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="setupButton" id="9321cd912ce2e88b" memberName="setupButton"
              virtualName="" explicitFocusOrder="0" pos="696 688 150 24" bgColOff="ff808080"
              bgColOn="fb000000" textCol="ff000000" textColOn="ff000000" buttonText="Setup"
              connectedEdges="3" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
