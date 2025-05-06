/*
  ==============================================================================

    PluginButtons.cpp
    Created: 6 May 2025 8:47:45am
    Author:  kyleb

  ==============================================================================
*/

#include "PluginButtons.h"


void AnalyzerButton::resized()
{
    auto bounds = getLocalBounds();
    auto insetRect = bounds.reduced(4);

    randomPath.clear();

    juce::Random r;

    randomPath.startNewSubPath(insetRect.getX(),
        insetRect.getY() + insetRect.getHeight() * r.nextFloat());

    for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
    {
        randomPath.lineTo(x,
            insetRect.getY() + insetRect.getHeight() * r.nextFloat());
    }
}
