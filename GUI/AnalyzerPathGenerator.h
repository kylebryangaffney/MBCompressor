/*
  ==============================================================================

    AnalyzerPathGenerator.h
    Created: 14 May 2025 9:08:32am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../DSP/FIFO.h"

class AnalyzerPathGenerator
{
public:
    AnalyzerPathGenerator() = default;
    void generatePath(const std::vector<float>& renderData,
        juce::Rectangle<float> fftBounds,
        int fftSize,
        float binWidth,
        float negativeInfinity);

    int getNumPathsAvailable() const;
    bool getPath(juce::Path& path);

private:
    Fifo<juce::Path> pathFifo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerPathGenerator)
};