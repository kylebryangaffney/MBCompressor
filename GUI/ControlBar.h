/*
  ==============================================================================

    ControlBar.h
    Created: 25 May 2025 6:22:32am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginButtons.h"


struct ControlBar : juce::Component
{
    ControlBar();
    void resized() override;
    AnalyzerButton analyzerButton;
    PowerButton globalBypassButton;
};

