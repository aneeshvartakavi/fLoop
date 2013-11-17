
#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent (AudioFormatManager& formatManager,
                    AudioTransportSource& transportSource_,
                    Slider& zoomSlider_)
    : transportSource (transportSource_),
        zoomSlider (zoomSlider_),
        thumbnailCache (5),
        thumbnail (512, formatManager, thumbnailCache)
{
    startTime = endTime = 0;
    thumbnail.addChangeListener (this);

    currentPositionMarker.setFill (Colours::purple.withAlpha (0.7f));
    addAndMakeVisible (&currentPositionMarker);
}

ThumbnailComponent::~ThumbnailComponent()
{
    thumbnail.removeChangeListener (this);
}

void ThumbnailComponent::setFile (const File& file)
{
    if (! file.isDirectory())
    {
        thumbnail.setSource (new FileInputSource (file));
        startTime = 0;
        endTime = thumbnail.getTotalLength();
        startTimer (1000 / 40);
    }
}

void ThumbnailComponent::setZoomFactor (double amount)
{
    if (thumbnail.getTotalLength() > 0)
    {
        const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
        const double timeAtCentre = xToTime (getWidth() / 2.0f);
        startTime = timeAtCentre - newScale * 0.5;
        endTime = timeAtCentre + newScale * 0.5;
        repaint();
    }
}

void ThumbnailComponent::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
    if (thumbnail.getTotalLength() > 0)
    {
        double newStart = startTime - wheel.deltaX * (endTime - startTime) / 10.0;
        newStart = jlimit (0.0, jmax (0.0, thumbnail.getTotalLength() - (endTime - startTime)), newStart);
        endTime = newStart + (endTime - startTime);
        startTime = newStart;

        if (wheel.deltaY != 0)
            zoomSlider.setValue (zoomSlider.getValue() - wheel.deltaY);

        repaint();
    }
}

void ThumbnailComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    g.setColour (Colours::lightblue);

    if (thumbnail.getTotalLength() > 0)
    {
        thumbnail.drawChannels (g, getLocalBounds().reduced (2),
                                startTime, endTime, 1.0f);
    }
    else
    {
        g.setFont (14.0f);
        g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
    }
}

void ThumbnailComponent::changeListenerCallback (ChangeBroadcaster*)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    repaint();
}

bool ThumbnailComponent::isInterestedInFileDrag (const StringArray& /*files*/)
{
    return true;
}

void ThumbnailComponent::filesDropped (const StringArray& files, int /*x*/, int /*y*/)
{
        
	LoopPlayer* loopPage = findParentComponentOfClass<LoopPlayer>();

    if (loopPage != nullptr)
        loopPage->showLeftFile (File (files[0])); // Changed this
}

void ThumbnailComponent::mouseDown (const MouseEvent& e)
{
    mouseDrag (e);
}

void ThumbnailComponent::mouseDrag (const MouseEvent& e)
{
    transportSource.setPosition (jmax (0.0, xToTime ((float) e.x)));
}

void ThumbnailComponent::mouseUp (const MouseEvent&)
{
    transportSource.start();
}

void ThumbnailComponent::timerCallback()
{
    currentPositionMarker.setVisible (transportSource.isPlaying() || isMouseButtonDown());

    double currentPlayPosition = transportSource.getCurrentPosition();

    currentPositionMarker.setRectangle (Rectangle<float> (timeToX (currentPlayPosition) - 0.75f, 0,
                                                            1.5f, (float) getHeight()));
}

float ThumbnailComponent::timeToX (const double time) const
{
    return getWidth() * (float) ((time - startTime) / (endTime - startTime));
}

double ThumbnailComponent::xToTime (const float x) const
{
    return (x / getWidth()) * (endTime - startTime) + startTime;
}

