/*
  ==============================================================================

    FIFO.h
    Created: 13 May 2025 9:57:34am
    Author:  kyleb

  ==============================================================================
*/

#pragma once


#pragma once
#include <array>
#include <vector>
#include <JuceHeader.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <type_traits>


template<typename T>
struct Fifo
{
    void prepare(int numChannels, int numSamples);
    void prepare(size_t numElements);

    bool push(const T& t);
    bool pull(T& t);

    int getNumAvailableForReading() const;

private:
    static constexpr int Capacity = 30;
    std::array<T, Capacity> buffers;
    juce::AbstractFifo fifo{ Capacity };
};
