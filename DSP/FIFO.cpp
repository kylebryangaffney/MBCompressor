/*
  ==============================================================================

    FIFO.cpp
    Created: 13 May 2025 9:57:34am
    Author:  kyleb

  ==============================================================================
*/

#include "FIFO.h"


// prepare for AudioBuffer<float>
template<typename T>
void Fifo<T>::prepare(int numChannels, int numSamples)
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

// prepare for std::vector<float>
template<typename T>
void Fifo<T>::prepare(size_t numElements)
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
bool Fifo<T>::push(const T& t)
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
bool Fifo<T>::pull(T& t)
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
int Fifo<T>::getNumAvailableForReading() const
{
    return fifo.getNumReady();
}