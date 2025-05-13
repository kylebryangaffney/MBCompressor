/*
  ==============================================================================

    SingleChannelSampleFIFO.cpp
    Created: 13 May 2025 9:20:53am
    Author:  kyleb

  ==============================================================================
*/

#include "SingleChannelSampleFIFO.h"


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

    bufferToFill.setSize(1,             // channels
        bufferSize,    // num samples
        false,         // keepExistingContent
        true,          // clear extra space
        true);         // avoid reallocating

    audioBufferFifo.prepare(1, bufferSize);
    fifoIndex = 0;
    prepared.set(true);
}

template<typename BlockType>
bool SingleChannelSampleFifo<BlockType>::isPrepared() const noexcept
{
    return prepared.get();
}

template<typename BlockType>
int SingleChannelSampleFifo<BlockType>::getSize() const noexcept
{
    return size.get();
}

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

    auto* channelPtr = buffer.getReadPointer(channelToUse);
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