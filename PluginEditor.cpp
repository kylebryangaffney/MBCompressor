/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}
//==============================================================================
MBCompAudioProcessorEditor::MBCompAudioProcessorEditor(MBCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize(600, 500);
}

MBCompAudioProcessorEditor::~MBCompAudioProcessorEditor()
{
}

//==============================================================================
void MBCompAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MBCompAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));
    analyzer.setBounds(bounds.removeFromTop(225));
    bandControls.setBounds(bounds.removeFromBottom(135));
    globalControls.setBounds(bounds);

}
