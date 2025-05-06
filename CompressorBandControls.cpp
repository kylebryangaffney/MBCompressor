/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 6 May 2025 10:35:01am
    Author:  kyleb

  ==============================================================================
*/

#include "CompressorBandControls.h"


CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& tree) :
    apvts(tree),
    attackSlider(nullptr, "ms", "Attack"),
    releaseSlider(nullptr, "ms", "Release"),
    thresholdSlider(nullptr, "dB", "Threshold"),
    ratioSlider(nullptr, "")
{
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(thresholdSlider);

    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);

    bypassButton.setName("b");
    soloButton.setName("s");
    muteButton.setName("m");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    lowBandButton.setName("Low");
    midBandButton.setName("Mid");
    highBandButton.setName("High");

    lowBandButton.setRadioGroupId(1);
    midBandButton.setRadioGroupId(1);
    highBandButton.setRadioGroupId(1);

    auto buttonSwitcher = [safePtr = this->safePtr]()
        {
            if (auto* c = safePtr.getComponent())
            {
                c->updateAttachments();
            }

        };

    lowBandButton.onClick = buttonSwitcher;
    midBandButton.onClick = buttonSwitcher;
    highBandButton.onClick = buttonSwitcher;

    lowBandButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    updateAttachments();

    addAndMakeVisible(lowBandButton);
    addAndMakeVisible(midBandButton);
    addAndMakeVisible(highBandButton);
}

CompressorBandControls::~CompressorBandControls()
{
    bypassButton.removeListener(this);
    soloButton.removeListener(this);
    muteButton.removeListener(this);
}

void CompressorBandControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);

    std::function<juce::FlexBox(const std::vector<juce::Component*>&)> createBandButtonControlBox = [](std::vector<juce::Component*> compressors)
        {
            juce::FlexBox flexBox;
            flexBox.flexDirection = juce::FlexBox::Direction::column;
            flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

            auto spacer = juce::FlexItem().withHeight(2);

            for (auto* c : compressors)
            {
                flexBox.items.add(spacer);
                flexBox.items.add(juce::FlexItem(*c).withFlex(1.0f));
            }
            flexBox.items.add(spacer);

            return flexBox;
        };

    juce::FlexBox bandButtonControlBox = createBandButtonControlBox({ &bypassButton, &soloButton, &muteButton });
    juce::FlexBox bandSelectControlBox = createBandButtonControlBox({ &lowBandButton, &midBandButton, &highBandButton });

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(4);
    auto endCap = juce::FlexItem().withHeight(6);

    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandSelectControlBox).withWidth(55));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(attackSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(releaseSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(ratioSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(thresholdSlider).withFlex(1.0f));
    flexBox.items.add(spacer);

    flexBox.items.add(juce::FlexItem(bandButtonControlBox).withWidth(30));

    flexBox.performLayout(bounds);

}


void CompressorBandControls::buttonClicked(juce::Button* button)
{
    updateSliderEnablements();
    updateSoloMuteBypassToggleStates(*button);
}

void CompressorBandControls::updateSliderEnablements()
{
    bool disabled = muteButton.getToggleState() || bypassButton.getToggleState();
    attackSlider.setEnabled(!disabled);
    releaseSlider.setEnabled(!disabled);
    ratioSlider.setEnabled(!disabled);
    thresholdSlider.setEnabled(!disabled);
}

void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button& clickedButton)
{
    if (&clickedButton == &soloButton && soloButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &muteButton && muteButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &bypassButton && bypassButton.getToggleState())
    {
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
}

void CompressorBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void CompressorBandControls::updateAttachments()
{
    enum BandType { Low, Mid, High } bandType;

    if (lowBandButton.getToggleState()) bandType = Low;
    else if (midBandButton.getToggleState()) bandType = Mid;
    else                                      bandType = High;


    Parameters::Names attackID, releaseID, threshID, ratioID,
        muteID, soloID, bypassID;


    switch (bandType)
    {
    case Low:
        attackID = Parameters::Attack_Low_Band;
        releaseID = Parameters::Release_Low_Band;
        threshID = Parameters::Threshold_Low_Band;
        ratioID = Parameters::Ratio_Low_Band;
        muteID = Parameters::Mute_Low_Band;
        soloID = Parameters::Solo_Low_Band;
        bypassID = Parameters::Bypassed_Low_Band;
        break;

    case Mid:
        attackID = Parameters::Attack_Mid_Band;
        releaseID = Parameters::Release_Mid_Band;
        threshID = Parameters::Threshold_Mid_Band;
        ratioID = Parameters::Ratio_Mid_Band;
        muteID = Parameters::Mute_Mid_Band;
        soloID = Parameters::Solo_Mid_Band;
        bypassID = Parameters::Bypassed_Mid_Band;
        break;

    case High:
        attackID = Parameters::Attack_High_Band;
        releaseID = Parameters::Release_High_Band;
        threshID = Parameters::Threshold_High_Band;
        ratioID = Parameters::Ratio_High_Band;
        muteID = Parameters::Mute_High_Band;
        soloID = Parameters::Solo_High_Band;
        bypassID = Parameters::Bypassed_High_Band;
        break;
    }

    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    muteButtonAttachment.reset();
    soloButtonAttachment.reset();
    bypassButtonAttachment.reset();

    const auto& paramsMap = Parameters::GetParams();

    {
        auto& p = getRangedParam(apvts, paramsMap, attackID);
        attackSlider.changeParam(&p);
        addLabelPairs(attackSlider.labels, p, "ms");
        makeAttachment(attackSliderAttachment, apvts, paramsMap, attackID, attackSlider);
    }
    {
        auto& p = getRangedParam(apvts, paramsMap, releaseID);
        releaseSlider.changeParam(&p);
        addLabelPairs(releaseSlider.labels, p, "ms");
        makeAttachment(releaseSliderAttachment, apvts, paramsMap, releaseID, releaseSlider);
    }
    {
        auto& p = getRangedParam(apvts, paramsMap, ratioID);
        ratioSlider.changeParam(&p);
        ratioSlider.labels.clear();
        if (auto* ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&p))
        {
            auto& choices = ratioParam->choices;
            const int num = choices.size();
            if (num > 0)
            {
                ratioSlider.labels.clear();
                ratioSlider.labels.add({ 0.f, choices[0] });
                ratioSlider.labels.add({ 1.f, choices[num - 1] });
            }
        }
        makeAttachment(ratioSliderAttachment, apvts, paramsMap, ratioID, ratioSlider);
    }
    {
        auto& p = getRangedParam(apvts, paramsMap, threshID);
        thresholdSlider.changeParam(&p);
        addLabelPairs(thresholdSlider.labels, p, "dB");
        makeAttachment(thresholdSliderAttachment, apvts, paramsMap, threshID, thresholdSlider);
    }

    makeAttachment(muteButtonAttachment, apvts, paramsMap, muteID, muteButton);
    makeAttachment(soloButtonAttachment, apvts, paramsMap, soloID, soloButton);
    makeAttachment(bypassButtonAttachment, apvts, paramsMap, bypassID, bypassButton);
}

