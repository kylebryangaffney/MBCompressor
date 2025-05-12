/*
  ==============================================================================

    UtilityComponents.h
    Created: 6 May 2025 8:54:12am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



struct Placeholder : juce::Component
{

    Placeholder();
    void paint(juce::Graphics& g) override;
    juce::Colour customColor;
};


struct RotarySlider : juce::Slider
{
    RotarySlider() :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
    }
};
