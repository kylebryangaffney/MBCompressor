

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

    controlBar.analyzerButton.onClick = [this]()
        {
            auto isOn = controlBar.analyzerButton.getToggleState();
            analyzer.toggleAnalysisEnablement(isOn);
        };

    controlBar.globalBypassButton.onClick = [this]()
        {
            toggleGlobalBypassState();
        };
    setLookAndFeel(&lnf);
    addAndMakeVisible(controlBar);
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
    analyzer.update(rmsValues);
    updateGlobalBypassButton();

}

void MBCompAudioProcessorEditor::toggleGlobalBypassState()
{
    bool isBypassEnabled = !controlBar.globalBypassButton.getToggleState();

    std::array<juce::AudioParameterBool*, 3> bypParamsArray = getBypassParameters();

    auto bypassParamHelper = [](auto* param, bool isBypassed)
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(isBypassed ? 1.0f : 0.0f);
            param->endChangeGesture();
        };

    for (auto* param : bypParamsArray)
    {
        bypassParamHelper(param, !isBypassEnabled);
    }

    bandControls.toggleAllBands(!isBypassEnabled);
}

std::array<juce::AudioParameterBool*, 3> MBCompAudioProcessorEditor::getBypassParameters()
{
    const auto& parameters = Parameters::GetParams();
    auto& apvts = audioProcessor.apvts;

    auto boolHelper = [&apvts, &parameters](const auto& paramName)
        {
            juce::AudioParameterBool* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(parameters.at(paramName)));
            jassert(param != nullptr);

            return param;
        };

    juce::AudioParameterBool* lowBypassParam = boolHelper(Parameters::Names::Bypassed_Low_Band);
    juce::AudioParameterBool* midBypassParam = boolHelper(Parameters::Names::Bypassed_Mid_Band);
    juce::AudioParameterBool* highBypassParam = boolHelper(Parameters::Names::Bypassed_High_Band);

    return { lowBypassParam, midBypassParam, highBypassParam };

}

void MBCompAudioProcessorEditor::updateGlobalBypassButton()
{
    std::array<juce::AudioParameterBool*, 3> bypassParams = getBypassParameters();

    bool allBandsBypassed = std::all_of(bypassParams.begin(), bypassParams.end(),
        [](juce::AudioParameterBool* param) {return param->get(); });

    controlBar.globalBypassButton.setToggleState(allBandsBypassed, juce::NotificationType::dontSendNotification);


}
