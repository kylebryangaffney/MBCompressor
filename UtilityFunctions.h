#pragma once

#include <JuceHeader.h>
#include <map>
#include <memory>
#include "PluginProcessor.h"
#include "RotarySliderWithLabels.h"
#include "Parameters.h"

//==============================================================================
/// Creates a slider attachment for the given parameter ID.
inline void makeAttachment(
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
    juce::AudioProcessorValueTreeState& apvts,
    const std::map<Parameters::Names, juce::String>& paramsMap,
    Parameters::Names paramID,
    juce::Slider& slider) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramsMap.at(paramID), slider);
}

/// Creates a button attachment for the given parameter ID.
inline void makeAttachment(
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& attachment,
    juce::AudioProcessorValueTreeState& apvts,
    const std::map<Parameters::Names, juce::String>& paramsMap,
    Parameters::Names paramID,
    juce::Button& button) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, paramsMap.at(paramID), button);
}

//==============================================================================
/// Retrieves a RangedAudioParameter by enum ID; asserts if not found.
juce::RangedAudioParameter& getRangedParam(
    juce::AudioProcessorValueTreeState& apvts,
    const std::map<Parameters::Names, juce::String>& paramsMap,
    Parameters::Names paramID);

//==============================================================================
/// Populates label positions at 0.0f and 1.0f with formatted strings.
void addLabelPairs(
    juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix);

//==============================================================================
/// Truncates values > 999 down by 1000 and returns true if truncated.
bool truncateKiloValue(float& value);

//==============================================================================
/// Builds a display string for the low or high parameter value, adding "k" and suffix.
juce::String getValString(
    const juce::RangedAudioParameter& param,
    bool getLow,
    const juce::String& suffix);

//==============================================================================
/// Draws a module background: full fill, rounded inner rect, and outline.
void drawModuleBackground(
    juce::Graphics& g,
    juce::Rectangle<int> bounds);
