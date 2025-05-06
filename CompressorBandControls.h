/*
  ==============================================================================

    CompressorBandControls.h
    Created: 6 May 2025 10:35:01am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithlabels.h"
#include "UtilityComponents.h"
#include "UtilityFunctions.h"

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& tree);
    ~CompressorBandControls() override;
    void resized() override;
    void paint(juce::Graphics& g) override;

    void buttonClicked(juce::Button* button) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
    RatioSlider ratioSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attackSliderAttachment, releaseSliderAttachment, thresholdSliderAttachment, ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBandButton, midBandButton, highBandButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    void updateAttachments();
    void updateSliderEnablements();
    void updateSoloMuteBypassToggleStates(juce::Button& clickedButton);

};