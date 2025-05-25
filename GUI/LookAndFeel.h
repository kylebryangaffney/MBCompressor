/*
  ==============================================================================

    LookAndFeel.h
    Created: 6 May 2025 8:39:03am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "PluginButtons.h"

#define USE_LIVE_CONSTANT true

#if USE_LIVE_CONSTANT
#define colorHelper(c) JUCE_LIVE_CONSTANT(c);
#else
#define colorHelper(c) c;
#endif

namespace ColorScheme
{
    inline juce::Colour getSliderBorderColor()
    {
        return colorHelper(juce::Colour(0xff00fff9));
    }
    inline juce::Colour getModuleBorderColor()
    {
        return colorHelper(juce::Colour(0xffb14996);
    }
}


struct LookAndFeel : juce::LookAndFeel_V4
{
    LookAndFeel() = default;
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};
