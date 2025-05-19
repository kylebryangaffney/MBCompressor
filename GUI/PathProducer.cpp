/*
  ==============================================================================

    PathProducer.cpp
    Created: 13 May 2025 9:15:05am
    Author:  kyleb

  ==============================================================================
*/

#include "PathProducer.h"
PathProducer::PathProducer(SingleChannelSampleFifo<juce::AudioBuffer<float>>& scsf)
    : leftChannelFifo(&scsf)
{
    leftChannelFFTDataGenerator.changeOrder(order2048);
    monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;

    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))
        {
            int size = tempIncomingBuffer.getNumSamples();
            int monoSize = monoBuffer.getNumSamples();

            auto writePointer = monoBuffer.getWritePointer(0, 0);
            auto readPointer = monoBuffer.getReadPointer(0, size);

            jassert(size <= monoBuffer.getNumSamples());
            size = juce::jmin(size, monoBuffer.getNumSamples());

            std::copy(readPointer, readPointer + (monoBuffer.getNumSamples() - size), writePointer);

            juce::FloatVectorOperations::copy(
                monoBuffer.getWritePointer(0, monoSize - size),
                tempIncomingBuffer.getReadPointer(0, 0),
                size);

            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, negativeInfinity);
        }
    }

    const int fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const double binWidth = sampleRate / static_cast<double>(fftSize);

    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, negativeInfinity);
        }
    }

    while (pathProducer.getNumPathsAvailable() > 0)
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

juce::Path PathProducer::getPath() const
{
    return leftChannelFFTPath;
}

void PathProducer::setNegativeInfinity(float newValue)
{
    negativeInfinity = newValue;
}
