
#pragma once

#include <JuceHeader.h>
#include "DSP/Constants.h"
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/PluginButtons.h"
#include "GUI/RotarySliderWithLabels.h"
#include "GUI/UtilityComponents.h"
#include "Service/UtilityFunctions.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/GlobalControls.h"
#include "GUI/SpectralAnalyzer.h"
#include "GUI/ControlBar.h"


/**
*/
class MBCompAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    MBCompAudioProcessorEditor(MBCompAudioProcessor&);
    ~MBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    LookAndFeel lnf;

    MBCompAudioProcessor& audioProcessor;

    //Placeholder controlBar;
    ControlBar controlBar;
    GlobalControls globalControls{ audioProcessor.apvts };
    CompressorBandControls bandControls{ audioProcessor.apvts };
    SpectralAnalyzerComponent analyzer{ audioProcessor };

    void toggleGlobalBypassState();

    std::array<juce::AudioParameterBool*, 3> getBypassParameters();

    void updateGlobalBypassButton();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessorEditor)
};
