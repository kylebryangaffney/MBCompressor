/*
  ==============================================================================

    GlobalControls.h
    Created: 6 May 2025 10:39:29am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "UtilityComponents.h"
#include "../Service/UtilityFunctions.h"

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<RotarySliderWithLabels> inputGainSlider, lowMidCrossoverSlider, midHighCrossoverSlider, outputGainSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        inputGainSliderAttachment, lowMidCrossoverSliderAttachment, midHighCrossoverSliderAttachment, outputGainSliderAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalControls)
};