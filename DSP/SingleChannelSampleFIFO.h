#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
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
};
