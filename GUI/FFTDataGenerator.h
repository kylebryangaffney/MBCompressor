/*
  ==============================================================================

    FFTDataGenerator.h
    Created: 14 May 2025 9:06:57am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../DSP/Constants.h"
#include "../PluginProcessor.h"
#include "../DSP/SingleChannelSampleFIFO.h"
#include "../DSP/FIFO.h"

class FFTDataGenerator
{
public:
    FFTDataGenerator();
    void changeOrder(FFTOrder newOrder);

    void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, float negativeInfinity);

    int getFFTSize() const;
    int getNumAvailableFFTDataBlocks() const;
    bool getFFTData(std::vector<float>& fftData);

private:
    FFTOrder order;

    std::vector<float> fftData;
    std::unique_ptr<juce::dsp::FFT> forwardFFT;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    Fifo<std::vector<float>> fftDataFifo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTDataGenerator)
};