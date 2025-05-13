/*
  ==============================================================================

    PathProducer.h
    Created: 13 May 2025 9:15:05am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../DSP/SingleChannelSampleFIFO.h"
#include "../DSP/FIFO.h"

struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<MBCompAudioProcessor::juce::Audiobuffer<float>>& scsf) :
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }
private:
    SingleChannelSampleFifo<MBCompAudioProcessor::juce::Audiobuffer<float>>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;
};