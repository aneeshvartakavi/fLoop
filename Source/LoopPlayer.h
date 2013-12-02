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
#include "WavFileFilter.h"
#include "CustomFileFilter.h"
#include "FeatureExtractor.h"
#include "LoopSimilarity.h"

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
    LoopPlayer (AudioDeviceManager& deviceManager, const File& pathtoDirectory, WavFileFilter* wavFilter, Array<File> &audioLoops_, ScopedPointer<CustomFileFilter> customFileFilter_);
    ~LoopPlayer();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	// These methods are our FileBrowserListener implementation:
    void selectionChanged();
    void fileClicked (const File& file, const MouseEvent& e);
    void fileDoubleClicked (const File& file);
    void browserRootChanged (const File&) {}
    void showLeftFile (const File& file);
	void showRightFile (const File& file);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    AudioFormatManager formatManager;
	TimeSliceThread thread;
    DirectoryContentsList directoryList;

    AudioSourcePlayer leftAudioSourcePlayer;
	AudioSourcePlayer rightAudioSourcePlayer;
    AudioTransportSource leftTransportSource;
	AudioTransportSource rightTransportSource;
    ScopedPointer<AudioFormatReaderSource> currentLeftAudioFileSource;
	ScopedPointer<AudioFormatReaderSource> currentRightAudioFileSource;

	void loadLeftFileIntoTransport (const File& audioFile);
	void loadRightFileIntoTransport (const File& audioFile);

	ScopedPointer<LoopSimilarity> loopSimilarity;
	//ScopedPointer<StringArray> similarLoops;
	StringArray similarLoops;
	// For the second FileBrowserComponent

	DirectoryContentsList customDirectoryList;

	//TimeSliceThread thread1;
	ScopedPointer<CustomFileFilter> customFileFilter1;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<FileTreeComponent> fileTreeComp;
    ScopedPointer<ThumbnailComponent> thumbnailComponent;
    ScopedPointer<Label> explanation;
    ScopedPointer<TextButton> startStopButton;
    ScopedPointer<drow::CpuMeter> cpuMeter;
    ScopedPointer<Label> label;
    ScopedPointer<FileTreeComponent> fileTreeComp2;
    ScopedPointer<ThumbnailComponent> thumbnailComponent2;
    ScopedPointer<TextButton> startStopButton2;
    ScopedPointer<Slider> tempoSlider;
    ScopedPointer<Slider> rhythmSlider;
    ScopedPointer<Slider> zoomSlider;
    ScopedPointer<Label> label2;
    ScopedPointer<Label> label3;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopPlayer)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_3C59D62F8505C3A6__
