/*
  ==============================================================================

    FIFO.h
    Created: 13 May 2025 9:57:34am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <array>
#include <vector>
#include <JuceHeader.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <type_traits>
#include "Constants.h"


template<typename T>
struct Fifo
{
    Fifo() : fifo(Capacity) {}
    void prepare(int numChannels, int numSamples);
    void prepare(size_t numElements);

    bool push(const T& t);
    bool pull(T& t);

    int getNumAvailableForReading() const;

private:
    static constexpr int Capacity = 30;
    std::array<T, Capacity> buffers;
    juce::AbstractFifo fifo{ Capacity };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fifo)
};

//----------------------------------------------//

template<typename T>
inline void Fifo<T>::prepare(int numChannels, int numSamples)
{
    static_assert(std::is_same_v<T, juce::AudioBuffer<float>>,
        "prepare(numChannels, numSamples) valid only for juce::AudioBuffer<float>");
    for (auto& buffer : buffers)
    {
        buffer.setSize(numChannels,
            numSamples,
            false,  // keepExistingContent
            true,   // clear extra space
            true);  // avoid reallocating
        buffer.clear();
    }
}

template<typename T>
inline void Fifo<T>::prepare(size_t numElements)
{
    static_assert(std::is_same_v<T, std::vector<float>>,
        "prepare(numElements) valid only for std::vector<float>");
    for (auto& buffer : buffers)
    {
        buffer.clear();
        buffer.resize(numElements, 0.0f);
    }
}

template<typename T>
inline bool Fifo<T>::push(const T& t)
{
    auto write = fifo.write(1);
    if (write.blockSize1 > 0)
    {
        buffers[write.startIndex1] = t;
        return true;
    }
    return false;
}

template<typename T>
inline bool Fifo<T>::pull(T& t)
{
    auto read = fifo.read(1);
    if (read.blockSize1 > 0)
    {
        t = buffers[read.startIndex1];
        return true;
    }
    return false;
}

template<typename T>
inline int Fifo<T>::getNumAvailableForReading() const
{
    return fifo.getNumReady();
}