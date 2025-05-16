/*
  ==============================================================================

    PathProducer.h
    Created: 13 May 2025 9:15:05am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../DSP/ChannelEnum.h"
#include "../PluginProcessor.h"
#include "../DSP/SingleChannelSampleFIFO.h"
#include "../DSP/FIFO.h"
#include "FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"


class PathProducer
{
public:
    explicit PathProducer(SingleChannelSampleFifo<juce::AudioBuffer<float>>& scsf);

    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() const;

private:
    SingleChannelSampleFifo<juce::AudioBuffer<float>>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;
    FFTDataGenerator leftChannelFFTDataGenerator;
    AnalyzerPathGenerator pathProducer;

    juce::Path leftChannelFFTPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathProducer)
};
