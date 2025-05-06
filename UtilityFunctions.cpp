/*
  ==============================================================================

    UtilityFunctions.cpp
    Created: 6 May 2025 10:01:57am
    Author:  kyleb

  ==============================================================================
*/

#include "UtilityFunctions.h"


bool truncateKiloValue(float& value)
{
    if (value > 999.0f)
    {
        value /= 1000.0f;
        return true;
    }

    return false;
}

juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow, const juce::String& suffix)
{
    // pick the range endpoint
    float v = getLow
        ? param.getNormalisableRange().start
        : param.getNormalisableRange().end;

    // truncate to “k” if needed
    const bool useK = truncateKiloValue(v);

    // build string: number, optional “k”, then suffix
    juce::String s;
    s << v;
    if (useK)
        s << "k";
    if (suffix.isNotEmpty())
        s << " " << suffix;
    return s;
}

void addLabelPairs(juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix)
{
    labels.clear();
    labels.add({ 0.0f, getValString(param, true,  suffix) });
    labels.add({ 1.0f, getValString(param, false, suffix) });
}

static juce::RangedAudioParameter& getRangedParam(juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, Parameters::Names paramID)
{
    auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts.getParameter(paramsMap.at(paramID)));
    jassert(param != nullptr);

    return *param;

}