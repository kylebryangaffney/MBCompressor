/*
  ==============================================================================
    UtilityFunctions.cpp
    Created: 6 May 2025 10:01:57am
    Author:  kyleb
  ==============================================================================
*/

#include "UtilityFunctions.h"

//==============================================================================
// Truncate a value above 999 down by 1000 and signal with 'k'
bool truncateKiloValue(float& value)
{
    if (value > 999.0f)
    {
        value /= 1000.0f;
        return true;
    }

    return false;
}

//==============================================================================
// Build a display string for the low or high end of a parameter, adding "k"
juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow,
    const juce::String& suffix)
{
    float v = getLow
        ? param.getNormalisableRange().start
        : param.getNormalisableRange().end;

    const bool useK = truncateKiloValue(v);

    juce::String s;
    s << v;
    if (useK)
        s << "k";
    if (suffix.isNotEmpty())
        s << " " << suffix;
    return s;
}

//==============================================================================
// Populate label positions at 0.0f and 1.0f with formatted values
void addLabelPairs(juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix)
{
    labels.clear();
    labels.add({ 0.0f, getValString(param, true,  suffix) });
    labels.add({ 1.0f, getValString(param, false, suffix) });
}

//==============================================================================
// Internal helper to grab a RangedAudioParameter by ID
juce::RangedAudioParameter& getRangedParam(
    juce::AudioProcessorValueTreeState& apvts,
    const std::map<Parameters::Names, juce::String>& paramsMap,
    Parameters::Names paramID)
{
    auto* p = dynamic_cast<juce::RangedAudioParameter*>(
        apvts.getParameter(paramsMap.at(paramID)));
    jassert(p != nullptr);
    return *p;
}

//==============================================================================
// Draw a module background with full fill, rounded inner rect, and outline
juce::Rectangle<int> drawModuleBackground(juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    g.setColour(ColorScheme::getModuleBorderColor());
    g.fillAll();

    auto localBounds = bounds;
    bounds.reduce(3, 3);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);

    return bounds;
}
