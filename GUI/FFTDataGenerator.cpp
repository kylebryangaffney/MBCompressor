/*
  ==============================================================================

    FFTDataGenerator.cpp
    Created: 14 May 2025 9:06:57am
    Author:  kyleb

  ==============================================================================
*/

#include "FFTDataGenerator.h"

FFTDataGenerator::FFTDataGenerator()
{
    changeOrder(order2048); // default constructor behavior
}

void FFTDataGenerator::changeOrder(FFTOrder newOrder)
{
    order = newOrder;
    auto fftSize = getFFTSize();

    forwardFFT = std::make_unique<juce::dsp::FFT>(static_cast<int>(order));
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(
        fftSize,
        juce::dsp::WindowingFunction<float>::blackmanHarris
    );

    fftData.clear();
    fftData.resize(fftSize * 2, 0);

    fftDataFifo.prepare(fftData.size());
}

void FFTDataGenerator::produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, float negativeInfinity)
{
    const auto fftSize = getFFTSize();

    fftData.assign(fftData.size(), 0);
    const float* readPointer = audioData.getReadPointer(0);
    std::copy(readPointer, readPointer + fftSize, fftData.begin());

    window->multiplyWithWindowingTable(fftData.data(), fftSize);
    forwardFFT->performFrequencyOnlyForwardTransform(fftData.data());

    const int numBins = fftSize / 2;

    for (int i = 0; i < numBins; ++i)
    {
        float v = fftData[i];
        v = (!std::isinf(v) && !std::isnan(v)) ? v / static_cast<float>(numBins) : 0.0f;
        fftData[i] = v;
    }

    float max = negativeInfinity;

    for (int i = 0; i < numBins; ++i)
    {
        auto data = juce::Decibels::gainToDecibels(fftData[i], negativeInfinity);
        fftData[i] = data;
        max = juce::jmax(data, max);
    }

    fftDataFifo.push(fftData);
}

int FFTDataGenerator::getFFTSize() const
{
    return 1 << static_cast<int>(order);
}

int FFTDataGenerator::getNumAvailableFFTDataBlocks() const
{
    return fftDataFifo.getNumAvailableForReading();
}

bool FFTDataGenerator::getFFTData(std::vector<float>& outputData)
{
    return fftDataFifo.pull(outputData);
}