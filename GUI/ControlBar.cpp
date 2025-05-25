/*
  ==============================================================================

    ControlBar.cpp
    Created: 25 May 2025 6:22:32am
    Author:  kyleb

  ==============================================================================
*/

#include "ControlBar.h"

ControlBar::ControlBar()
{
    analyzerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(analyzerButton);
    addAndMakeVisible(globalBypassButton);
}

void ControlBar::resized()
{
    auto bounds = getLocalBounds();
    analyzerButton.setBounds(bounds.removeFromLeft(50).withTrimmedTop(4).withTrimmedBottom(4));
    globalBypassButton.setBounds(bounds.removeFromRight(60).withTrimmedTop(4).withTrimmedBottom(4));
}
