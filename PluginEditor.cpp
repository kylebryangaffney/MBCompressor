

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MBCompAudioProcessorEditor::MBCompAudioProcessorEditor(MBCompAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    analyzer(p),
    globalControls(audioProcessor.apvts),
    bandControls(audioProcessor.apvts)
{
    setLookAndFeel(&lnf);
    //addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize(600, 500);

    startTimerHz(60);
}

MBCompAudioProcessorEditor::~MBCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MBCompAudioProcessorEditor::paint(juce::Graphics& g)
{

}

void MBCompAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));
    analyzer.setBounds(bounds.removeFromTop(225));
    bandControls.setBounds(bounds.removeFromBottom(135));
    globalControls.setBounds(bounds);

}

void MBCompAudioProcessorEditor::timerCallback()
{
    std::vector<float> rmsValues
    {
        audioProcessor.lowBandComp.getRmsInputLevelDb(),
        audioProcessor.lowBandComp.getRmsOutputLevelDb(),
        audioProcessor.midBandComp.getRmsInputLevelDb(),
        audioProcessor.midBandComp.getRmsOutputLevelDb(),
        audioProcessor.highBandComp.getRmsInputLevelDb(),
        audioProcessor.highBandComp.getRmsOutputLevelDb()
    };

}
