fLoop
=====

fLoop is an cross platform audio file navigation tool that provides a simple interface to navigate a large number of loops by tempo, rhythmic and timbral distances. It is a GPL licensed standalone application, built on the JUCE framework in C++. It also uses the Eigen library, and the dRowAudio module for JUCE.

Build Instructions
==================

-> Open up the .jucer file with the Introjucer, assuming you have the JUCE.
-> Check the paths to the modules, in the config tab.
-> Save and open in your editor of choice!

Mac:

Open the Xcode project in Builds/MacOSX/fLoop.xcodeproject
Build and run the project

If you are unable to build, see the above instructions to generate a new Xcode project in JUCE.

Usage
=====

Upon launch, there is a simple interface for setuping up your audio or selecting a folder for audio loop comparison.

To analyze a directory for audio loop comparison, select "Browse" and use the dialogue box to select the directory containing the audio files (Note: currently the folder is not searched recursively). Depending on how many files are in the directory, this process could take a while, but once a folder has been analyzed the analysis is cached for quick retrieval in the future.

After analysis is compete, the application will display two list boxes and several controls. The list box on the left holds all the audio files analyzed. The get a recommendation, select a reference loop in the left list box and adjust the slide controls. For tempo, the slider provides a range in which the recommended files will be and for rhythm and timbre, a full slider range allows all files through and a narrow range toward the bottom will allow only the most similar files through.

Selecting a file in either list box will populate the preview tool where you can play the audio file.


Troubleshooting
===============

No files showing up for recommendation: In this case make sure to adjust all the sliders to some position. If this does not solve it, then try less narrow filtering on one or more sliders.


Aneesh Vartakavi and Cameron Summers
