/*
  ==============================================================================

    CompressorBand.h
    Created: 6 May 2025 10:51:51am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Constants.h";


struct CompressorBand
{
    CompressorBand() = default;

    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec);
    void updateCompressorSettings();
    void process(juce::AudioBuffer<float>& buffer);

    float getRmsInputLevelDb() const { return rmsInputLevelDb; }
    float getRmsOutputLevelDb() const { return rmsOutputLevelDb; }

private:
    juce::dsp::Compressor<float> compressor;

    std::atomic<float> rmsInputLevelDb{ NEGATIVE_INFINITY };
    std::atomic<float> rmsOutputLevelDb{ NEGATIVE_INFINITY };

    float computeRMSLevel(const juce::AudioBuffer<float>& buffer);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorBand)
};
