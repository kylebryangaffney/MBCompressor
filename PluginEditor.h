
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/PluginButtons.h"
#include "GUI/RotarySliderWithLabels.h"
#include "GUI/UtilityComponents.h"
#include "Service/UtilityFunctions.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/GlobalControls.h"



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

