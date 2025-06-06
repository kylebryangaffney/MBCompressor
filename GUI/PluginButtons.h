/*
  ==============================================================================

    PluginButtons.h
    Created: 6 May 2025 8:47:45am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct PowerButton : juce::ToggleButton {};

struct AnalyzerButton : juce::ToggleButton
{
    AnalyzerButton() = default;
    void resized() override;
    juce::Path randomPath;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerButton)
};

