/*
  ==============================================================================

    UtilityFunctions.h
    Created: 6 May 2025 10:01:57am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotarySliderWithlabels.h"
#include "Parameters.h"


inline void makeAttachment(std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>&
    attachment, juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, const Parameters::Names paramID, juce::Slider& slider) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramsMap.at(paramID), slider);
}

inline void makeAttachment(std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>&
    attachment, juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, const Parameters::Names paramID, juce::Button& button) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramsMap.at(paramID), button);
}

static juce::RangedAudioParameter& getRangedParam(juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, Parameters::Names paramID);

void addLabelPairs(juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix);


bool truncateKiloValue(float& value);

juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow, const juce::String& suffix);

void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds);