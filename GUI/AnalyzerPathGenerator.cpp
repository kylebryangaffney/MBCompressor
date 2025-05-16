/*
  ==============================================================================

    AnalyzerPathGenerator.cpp
    Created: 14 May 2025 9:08:32am
    Author:  kyleb

  ==============================================================================
*/

#include "AnalyzerPathGenerator.h"

void AnalyzerPathGenerator::generatePath(const std::vector<float>& renderData,
    juce::Rectangle<float> fftBounds,
    int fftSize,
    float binWidth,
    float negativeInfinity)
{
    const float top = fftBounds.getY();
    const float bottom = fftBounds.getBottom();
    const float width = fftBounds.getWidth();

    const int numBins = fftSize / 2;

    juce::Path p;
    p.preallocateSpace(3 * static_cast<int>(width));

    auto map = [bottom, top, negativeInfinity](float v)
        {
            return juce::jmap(v, negativeInfinity, 0.f, bottom, top);
        };

    float y = map(renderData[0]);
    if (std::isnan(y) || std::isinf(y))
        y = bottom;

    p.startNewSubPath(0, y);

    const int pathResolution = 2;

    for (int binNum = 1; binNum < numBins; binNum += pathResolution)
    {
        y = map(renderData[binNum]);

        if (!std::isnan(y) && !std::isinf(y))
        {
            float binFreq = static_cast<float>(binNum) * binWidth;
            float normalizedX = juce::mapFromLog10(binFreq, 20.f, 20000.f);
            int binX = static_cast<int>(std::floor(normalizedX * width));

            p.lineTo(binX, y);
        }
    }

    pathFifo.push(p);
}

int AnalyzerPathGenerator::getNumPathsAvailable() const
{
    return pathFifo.getNumAvailableForReading();
}

bool AnalyzerPathGenerator::getPath(juce::Path& path)
{
    return pathFifo.pull(path);
}