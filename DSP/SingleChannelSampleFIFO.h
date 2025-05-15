/*
  ==============================================================================

    SingleChannelSampleFIFO.h
    Created: 13 May 2025 9:20:53am
    Author:  kyleb

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>
#include "../DSP/ChannelEnum.h"
#include "FIFO.h"


template<typename BlockType>
struct SingleChannelSampleFifo
{
    explicit SingleChannelSampleFifo(Channel channelToUse);

    // lifecycle
    void prepare(int bufferSize);
    bool isPrepared() const noexcept;
    int  getSize() const noexcept;
    int  getNumCompleteBuffersAvailable() const noexcept;

    // operation
    void update(const BlockType& buffer);
    bool getAudioBuffer(BlockType& buf);

private:
    void pushNextSampleIntoFifo(float sample);

    Channel channelToUse;
    int fifoIndex = 0;
    Fifo<BlockType> audioBufferFifo;
    BlockType bufferToFill;
    juce::Atomic<bool> prepared{ false };
    juce::Atomic<int> size{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SingleChannelSampleFifo)
};

//==============================================================================
// Template definitions must be in the header:

template<typename BlockType>
SingleChannelSampleFifo<BlockType>::SingleChannelSampleFifo(Channel ch)
    : channelToUse(ch)
{
    prepared.set(false);
}

template<typename BlockType>
void SingleChannelSampleFifo<BlockType>::prepare(int bufferSize)
{
    prepared.set(false);
    size.set(bufferSize);

    bufferToFill.setSize(1, bufferSize, false, true, true);
    audioBufferFifo.prepare(1, bufferSize);
    fifoIndex = 0;
    prepared.set(true);
}

template<typename BlockType>
bool SingleChannelSampleFifo<BlockType>::isPrepared() const noexcept { return prepared.get(); }

template<typename BlockType>
int SingleChannelSampleFifo<BlockType>::getSize() const noexcept { return size.get(); }

template<typename BlockType>
int SingleChannelSampleFifo<BlockType>::getNumCompleteBuffersAvailable() const noexcept
{
    return audioBufferFifo.getNumAvailableForReading();
}

template<typename BlockType>
void SingleChannelSampleFifo<BlockType>::update(const BlockType& buffer)
{
    jassert(prepared.get());
    jassert(buffer.getNumChannels() > channelToUse);

    const float* channelPtr = buffer.getReadPointer(channelToUse);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        pushNextSampleIntoFifo(channelPtr[i]);
}

template<typename BlockType>
bool SingleChannelSampleFifo<BlockType>::getAudioBuffer(BlockType& buf)
{
    return audioBufferFifo.pull(buf);
}

template<typename BlockType>
void SingleChannelSampleFifo<BlockType>::pushNextSampleIntoFifo(float sample)
{
    if (fifoIndex == bufferToFill.getNumSamples())
    {
        bool ok = audioBufferFifo.push(bufferToFill);
        juce::ignoreUnused(ok);
        fifoIndex = 0;
    }

    bufferToFill.setSample(0, fifoIndex, sample);
    ++fifoIndex;
}
