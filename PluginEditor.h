
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "PluginButtons.h"
#include "RotarySliderWithLabels.h"
#include "UtilityComponents.h"
#include "UtilityFunctions.h"
#include "CompressorBandControls.h"
#include "GlobalControls.h"



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
    LookAndFeel lnf;

    MBCompAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer;
    GlobalControls globalControls{ audioProcessor.apvts };
    CompressorBandControls bandControls{ audioProcessor.apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessorEditor)
};

