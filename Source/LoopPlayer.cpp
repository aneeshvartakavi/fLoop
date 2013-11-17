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

#include "LoopPlayer.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LoopPlayer::LoopPlayer (AudioDeviceManager& deviceManager, const File& pathtoDirectory, WavFileFilter* wavFilter, Array<var> featureVector, Array<File> audioLoops_, CustomFileFilter* customFileFilter)
    : deviceManager (deviceManager), thread ("Audio file preview"), directoryList (wavFilter, thread), customDirectoryList(customFileFilter,thread)
{
    addAndMakeVisible (zoomSlider = new Slider ("zoomSlider"));
    zoomSlider->setRange (0, 1, 0);
    zoomSlider->setSliderStyle (Slider::LinearHorizontal);
    zoomSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    zoomSlider->addListener (this);

    addAndMakeVisible (fileTreeComp = new FileTreeComponent (directoryList));
    fileTreeComp->setName ("FileTreeComp1");

    addAndMakeVisible (thumbnailComponent = new ThumbnailComponent (formatManager, transportSource, *zoomSlider));
    thumbnailComponent->setName ("thumbnail");

    addAndMakeVisible (zoomLabel = new Label ("zoomLabel",
                                              "Zoom"));
    zoomLabel->setFont (Font (15.00f, Font::plain));
    zoomLabel->setJustificationType (Justification::centredLeft);
    zoomLabel->setEditable (false, false, false);
    zoomLabel->setColour (TextEditor::textColourId, Colours::black);
    zoomLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (explanation = new Label ("explanation",
                                                "Select an Audio File to play!"));
    explanation->setFont (Font (15.00f, Font::plain));
    explanation->setJustificationType (Justification::centredLeft);
    explanation->setEditable (false, false, false);
    explanation->setColour (TextEditor::textColourId, Colours::black);
    explanation->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (startStopButton = new TextButton ("startStopButton"));
    startStopButton->setButtonText ("Start/Stop");
    startStopButton->addListener (this);

    addAndMakeVisible (cpuMeter = new drow::CpuMeter (&deviceManager));
    cpuMeter->setName ("cpuMeter");

    addAndMakeVisible (label = new Label ("new label",
                                          "CPU Usage"));
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (fileTreeComp2 = new FileTreeComponent (customDirectoryList));
    fileTreeComp2->setName ("FileTreeComp");


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1024, 768);


    //[Constructor] You can add your own custom stuff here..
	formatManager.registerBasicFormats();

	directoryList.setDirectory (pathtoDirectory, true, true);
    thread.startThread (3);


    fileTreeComp->setColour (FileTreeComponent::backgroundColourId, Colours::white);
    fileTreeComp->addListener (this);

    deviceManager.addAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (&transportSource);

	loopSimilarity = new LoopSimilarity(featureVector);

	// Handling the second component on our own

	customDirectoryList.setDirectory(pathtoDirectory,true,true);
	//thread1.startThread(4);

	fileTreeComp2->setColour (FileTreeComponent::backgroundColourId, Colours::white);
    fileTreeComp->addListener (this);
    //[/Constructor]
}

LoopPlayer::~LoopPlayer()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	transportSource.setSource (nullptr);
    audioSourcePlayer.setSource (nullptr);

    deviceManager.removeAudioCallback (&audioSourcePlayer);
    fileTreeComp->removeListener (this);
	currentAudioFileSource = nullptr;
	//fileTreeComp2->
	//thread1.stopThread(1000);
    //[/Destructor_pre]

    zoomSlider = nullptr;
    fileTreeComp = nullptr;
    thumbnailComponent = nullptr;
    zoomLabel = nullptr;
    explanation = nullptr;
    startStopButton = nullptr;
    cpuMeter = nullptr;
    label = nullptr;
    fileTreeComp2 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LoopPlayer::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xfff0f0f0));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LoopPlayer::resized()
{
    zoomSlider->setBounds (32, 439, 150, 24);
    fileTreeComp->setBounds (32, 64, 320, 320);
    thumbnailComponent->setBounds (24, 487, 944, 128);
    zoomLabel->setBounds (32, 407, 48, 24);
    explanation->setBounds (32, 16, 150, 24);
    startStopButton->setBounds (24, 648, 150, 24);
    cpuMeter->setBounds (928, 26, 64, 24);
    label->setBounds (920, 2, 150, 24);
    fileTreeComp2->setBounds (632, 64, 320, 320);
    //[UserResized] Add your own custom resize handling here..

    //[/UserResized]
}

void LoopPlayer::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == zoomSlider)
    {
        //[UserSliderCode_zoomSlider] -- add your slider handling code here..
		thumbnailComponent->setZoomFactor (zoomSlider->getValue());
        //[/UserSliderCode_zoomSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void LoopPlayer::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == startStopButton)
    {
        //[UserButtonCode_startStopButton] -- add your button handler code here..
		if (transportSource.isPlaying())
        {
            transportSource.stop();
        }
        else
        {
            transportSource.setPosition (0);
            transportSource.start();
        }
        //[/UserButtonCode_startStopButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void LoopPlayer::showFile (const File& file)
{
    loadFileIntoTransport (file);

    zoomSlider->setValue (0, dontSendNotification);
	thumbnailComponent->setFile (file);
}

void LoopPlayer::loadFileIntoTransport (const File& audioFile)
{
    // unload the previous file source and delete it..
    transportSource.stop();
    transportSource.setSource (nullptr);
    currentAudioFileSource = nullptr;

    AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource (reader, true);

        //featureExtractor->processFile(reader);
//        featureExtractor.getBlockSpectralCrestFactor(<#float *data#>)
        // ..and plug it into our transport source
        transportSource.setSource (currentAudioFileSource,
                                   32768, // tells it to buffer this many samples ahead
                                   &thread, // this is the background thread to use for reading-ahead
                                   reader->sampleRate);
    }
}

void LoopPlayer::selectionChanged()
{
    showFile (fileTreeComp->getSelectedFile());
}

void LoopPlayer::fileClicked (const File&, const MouseEvent&)
{
	DBG("FileClicked");
}

void LoopPlayer::fileDoubleClicked (const File&)
{
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LoopPlayer" componentName=""
                 parentClasses="public Component, public FileBrowserListener, public SliderListener, public ButtonListener"
                 constructorParams="AudioDeviceManager&amp; deviceManager, const File&amp; pathtoDirectory, WavFileFilter* wavFilter, Array&lt;var&gt; featureVector, Array&lt;File&gt; audioLoops_, CustomFileFilter* customFileFilter"
                 variableInitialisers="deviceManager (deviceManager), thread (&quot;Audio file preview&quot;), directoryList (wavFilter, thread), customDirectoryList(customFileFilter,thread)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="1024" initialHeight="768">
  <BACKGROUND backgroundColour="fff0f0f0"/>
  <SLIDER name="zoomSlider" id="57b0a0502dcd3cd9" memberName="zoomSlider"
          virtualName="" explicitFocusOrder="0" pos="32 439 150 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <GENERICCOMPONENT name="FileTreeComp1" id="28b803a9ec0db835" memberName="fileTreeComp"
                    virtualName="" explicitFocusOrder="0" pos="32 64 320 320" class="FileTreeComponent"
                    params="directoryList"/>
  <GENERICCOMPONENT name="thumbnail" id="9da459d8045d031" memberName="thumbnailComponent"
                    virtualName="" explicitFocusOrder="0" pos="24 487 944 128" class="ThumbnailComponent"
                    params="formatManager, transportSource, *zoomSlider"/>
  <LABEL name="zoomLabel" id="6b306247a4bb8a6c" memberName="zoomLabel"
         virtualName="" explicitFocusOrder="0" pos="32 407 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Zoom" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="explanation" id="94523f3466d25467" memberName="explanation"
         virtualName="" explicitFocusOrder="0" pos="32 16 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Select an Audio File to play!" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="startStopButton" id="60a0719e5a96e44c" memberName="startStopButton"
              virtualName="" explicitFocusOrder="0" pos="24 648 150 24" buttonText="Start/Stop"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="cpuMeter" id="47442aafe0aa91da" memberName="cpuMeter" virtualName="drow::CpuMeter"
                    explicitFocusOrder="0" pos="928 26 64 24" class="Component" params="&amp;deviceManager"/>
  <LABEL name="new label" id="1a1155b3b685297c" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="920 2 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CPU Usage" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="FileTreeComp" id="c2172d74b9138c92" memberName="fileTreeComp2"
                    virtualName="" explicitFocusOrder="0" pos="632 64 320 320" class="FileTreeComponent"
                    params="customDirectoryList"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
