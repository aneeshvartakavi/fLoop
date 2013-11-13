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

#ifndef __JUCE_HEADER_3C59D62F8505C3A6__
#define __JUCE_HEADER_3C59D62F8505C3A6__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "ThumbnailComponent.h"
class ThumbnailComponent;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LoopPlayer  : public Component,
                    public FileBrowserListener,
                    public SliderListener,
                    public ButtonListener
{
public:
    //==============================================================================
    LoopPlayer (AudioDeviceManager& deviceManager, const File& pathtoDirectory);
    ~LoopPlayer();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	// These methods are our FileBrowserListener implementation:
    void selectionChanged();
    void fileClicked (const File& file, const MouseEvent& e);
    void fileDoubleClicked (const File& file);
    void browserRootChanged (const File&) {}
    void showFile (const File& file);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    AudioFormatManager formatManager;
	TimeSliceThread thread;
    DirectoryContentsList directoryList;

    AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;

    void loadFileIntoTransport (const File& audioFile);
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> zoomSlider;
    ScopedPointer<FileTreeComponent> fileTreeComp;
    ScopedPointer<ThumbnailComponent> thumbnailComponent;
    ScopedPointer<Label> zoomLabel;
    ScopedPointer<Label> explanation;
    ScopedPointer<TextButton> startStopButton;
    ScopedPointer<drow::CpuMeter> cpuMeter;
    ScopedPointer<Label> label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopPlayer)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_3C59D62F8505C3A6__
