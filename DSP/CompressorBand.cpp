/*
  ==============================================================================

    CompressorBand.cpp
    Created: 6 May 2025 10:51:51am
    Author:  kyleb

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    float inputRMS = computeRMSLevel(buffer);
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = bypassed->get();
    compressor.process(context);

    float outputRMS = computeRMSLevel(buffer);

    rmsInputLevelDb.store(juce::Decibels::gainToDecibels(inputRMS));
    rmsOutputLevelDb.store(juce::Decibels::gainToDecibels(outputRMS));
}

float CompressorBand::computeRMSLevel(const juce::AudioBuffer<float>& buffer)
{
    int numChannels = static_cast<int>(buffer.getNumChannels());
    int numSamples = static_cast<int>(buffer.getNumSamples());
    float rms = 0.0f;

    for (int chan = 0; chan < numChannels; ++chan)
    {
        rms += buffer.getRMSLevel(chan, 0, numSamples);
    }

    rms /= static_cast<float>(numChannels);
    return rms;
}
