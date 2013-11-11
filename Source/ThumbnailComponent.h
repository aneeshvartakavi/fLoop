
#ifndef THUMBNAIL_COMPONENT
#define THUMBNAIL_COMPONENT

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoopPlayer.h"

class ThumbnailComponent: public Component,public ChangeListener,
							public FileDragAndDropTarget,private Timer
{
public:
	ThumbnailComponent (AudioFormatManager& formatManager,
                       AudioTransportSource& transportSource_,
                       Slider& zoomSlider_);
	
	~ThumbnailComponent();
	void setFile (const File& file);
	void setZoomFactor (double amount);
	void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel) override;
	void paint (Graphics& g) override;
	void changeListenerCallback (ChangeBroadcaster*) override;
	bool isInterestedInFileDrag (const StringArray& /*files*/) override;
	void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent&) override;
    void timerCallback() override;

private:
	AudioTransportSource& transportSource;
    Slider& zoomSlider;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    double startTime, endTime;

    DrawableRectangle currentPositionMarker;

	 float timeToX (const double time) const;
	 double xToTime (const float x) const;

};




#endif // THUMBNAIL_COMPONENT