/*
  ==============================================================================

    GlobalControls.cpp
    Created: 6 May 2025 10:39:29am
    Author:  kyleb

  ==============================================================================
*/

#include "GlobalControls.h"

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& paramsMap = Parameters::GetParams();

    auto& inGainParam = getRangedParam(apvts, paramsMap, Parameters::Input_Gain);
    auto& lowMidParam = getRangedParam(apvts, paramsMap, Parameters::Low_Mid_Crossover_Freq);
    auto& midHighParam = getRangedParam(apvts, paramsMap, Parameters::Mid_High_Crossover_Freq);
    auto& outGainParam = getRangedParam(apvts, paramsMap, Parameters::Output_Gain);

    inputGainSlider = std::make_unique<RotarySliderWithLabels>(&inGainParam, " dB", "Input Gain");
    lowMidCrossoverSlider = std::make_unique<RotarySliderWithLabels>(&lowMidParam, " Hz", "Low Mid Crossover");
    midHighCrossoverSlider = std::make_unique<RotarySliderWithLabels>(&midHighParam, " Hz", "Mid High Crossover");
    outputGainSlider = std::make_unique<RotarySliderWithLabels>(&outGainParam, " dB", "Output Gain");

    makeAttachment(
        inputGainSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Input_Gain,
        *inputGainSlider);

    makeAttachment(
        lowMidCrossoverSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Low_Mid_Crossover_Freq,
        *lowMidCrossoverSlider);

    makeAttachment(
        midHighCrossoverSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Mid_High_Crossover_Freq,
        *midHighCrossoverSlider);

    makeAttachment(
        outputGainSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Output_Gain,
        *outputGainSlider);

    addLabelPairs(inputGainSlider->labels, inGainParam, "dB");
    addLabelPairs(lowMidCrossoverSlider->labels, lowMidParam, "Hz");
    addLabelPairs(midHighCrossoverSlider->labels, midHighParam, "Hz");
    addLabelPairs(outputGainSlider->labels, outGainParam, "dB");

    addAndMakeVisible(*inputGainSlider);
    addAndMakeVisible(*lowMidCrossoverSlider);
    addAndMakeVisible(*midHighCrossoverSlider);
    addAndMakeVisible(*outputGainSlider);
}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);

}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(3);
    auto endCap = juce::FlexItem().withWidth(5);

    flexBox.items.add(endCap);
    flexBox.items.add(juce::FlexItem(*inputGainSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*lowMidCrossoverSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*midHighCrossoverSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*outputGainSlider).withFlex(1.0f));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}