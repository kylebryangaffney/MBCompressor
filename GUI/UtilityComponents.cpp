/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 6 May 2025 8:54:12am
    Author:  kyleb

  ==============================================================================
*/

#include "UtilityComponents.h"


Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

void Placeholder::paint(juce::Graphics& g)
{
    g.fillAll(customColor);
}
