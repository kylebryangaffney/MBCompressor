
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

struct Placeholder : juce::Component
{

    Placeholder();
    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }

    juce::Colour customColor;
};
/**
*/
class MBCompAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MBCompAudioProcessorEditor(MBCompAudioProcessor&);
    ~MBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBCompAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer, globalControls, bandControls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessorEditor)
};
