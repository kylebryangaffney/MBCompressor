/*
  ==============================================================================

    SpectralAnalyzer.h
    Created: 13 May 2025 8:57:41am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "PathProducer.h"


struct SpectralAnalyzerComponent : juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
    SpectralAnalyzerComponent(MBCompAudioProcessor&);
    ~SpectralAnalyzerComponent();

    void parameterValueChanged(int parameterIndex, float newValue) override;

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void toggleAnalysisEnablement(bool enabled)
    {
        shouldShowFFTAnalysis = enabled;
    }
private:
    MBCompAudioProcessor& audioProcessor;

    bool shouldShowFFTAnalysis = true;

    juce::Atomic<bool> parametersChanged{ false };

    void drawBackgroundGrid(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTextLabels(juce::Graphics& g, juce::Rectangle<int> bounds);

    std::vector<float> getFrequencies();
    std::vector<float> getGains();
    std::vector<float> getXs(const std::vector<float>& freqs, float left, float width);

    juce::Rectangle<int> getRenderArea(juce::Rectangle<int> bounds);

    juce::Rectangle<int> getAnalysisArea(juce::Rectangle<int> bounds);

    PathProducer leftPathProducer, rightPathProducer;

    void drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds);

    void drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds);

    juce::AudioParameterFloat* lowMidCrossoverParam{ nullptr };
    juce::AudioParameterFloat* midHighCrossoverParam{ nullptr };

    juce::AudioParameterFloat* lowThresholdParam{ nullptr };
    juce::AudioParameterFloat* midThresholdParam{ nullptr };
    juce::AudioParameterFloat* highThresholdParam{ nullptr };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralAnalyzerComponent)
};