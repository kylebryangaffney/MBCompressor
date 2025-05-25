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
    bypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::blue);
    bypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    soloButton.setName("s");
    soloButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
    soloButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    muteButton.setName("m");
    muteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    muteButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    lowBandButton.setName("Low");
    lowBandButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::grey);
    lowBandButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    midBandButton.setName("Mid");
    midBandButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::grey);
    midBandButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    highBandButton.setName("High");
    highBandButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::grey);
    highBandButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);

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
    updateSliderEnablements();
    updateBandSelectButtonStates();

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
void CompressorBandControls::updateBandSelectButtonStates()
{
    const auto& paramsMap = Parameters::GetParams();

    std::vector<std::array<Parameters::Names, 3>> parametersToCheck
    {
        {{ Parameters::Solo_Low_Band,   Parameters::Mute_Low_Band,   Parameters::Bypassed_Low_Band   }},
        {{ Parameters::Solo_Mid_Band,   Parameters::Mute_Mid_Band,   Parameters::Bypassed_Mid_Band   }},
        {{ Parameters::Solo_High_Band,  Parameters::Mute_High_Band,  Parameters::Bypassed_High_Band  }}
    };

    for (size_t i = 0; i < parametersToCheck.size(); ++i)
    {
        juce::ToggleButton* bandButton = (i == 0) ? &lowBandButton
            : (i == 1) ? &midBandButton
            : &highBandButton;

        auto& ids = parametersToCheck[i];

        if (auto* p = getBoolParam(apvts, paramsMap, ids[0]); p && p->get())
        {
            refreshBandButtonColors(*bandButton, soloButton);
            continue;
        }
        if (auto* p = getBoolParam(apvts, paramsMap, ids[1]); p && p->get())
        {
            refreshBandButtonColors(*bandButton, muteButton);
            continue;
        }
        if (auto* p = getBoolParam(apvts, paramsMap, ids[2]); p && p->get())
        {
            refreshBandButtonColors(*bandButton, bypassButton);
            continue;
        }
        resetActiveBandColors();
    }
}

void CompressorBandControls::toggleAllBands(bool isBypassed)
{
    std::vector<juce::Component*> bandButtonss{ &lowBandButton, &midBandButton, &highBandButton };

    for (auto* band : bandButtonss)
    {
        band->setColour(juce::TextButton::ColourIds::buttonOnColourId, isBypassed ?
            bypassButton.findColour(juce::TextButton::ColourIds::buttonOnColourId) : juce::Colours::grey);

        band->setColour(juce::TextButton::ColourIds::buttonColourId, isBypassed ?
            bypassButton.findColour(juce::TextButton::ColourIds::buttonOnColourId) : juce::Colours::black);

        band->repaint();
    }

}

void CompressorBandControls::buttonClicked(juce::Button* button)
{
    updateSliderEnablements();
    updateSoloMuteBypassToggleStates(*button);
    updateActiveBandFillColors(*button);
}

void CompressorBandControls::updateActiveBandFillColors(juce::Button& clickedButton)
{
    jassert(activeBand != nullptr);
    DBG("active band: " << activeBand->getName());

    if (clickedButton.getToggleState() == false)
    {
        resetActiveBandColors();
    }
    else
    {
        refreshBandButtonColors(*activeBand, clickedButton);
    }
}

void CompressorBandControls::resetActiveBandColors()
{
    activeBand->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::grey);
    activeBand->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    activeBand->repaint();
}

void CompressorBandControls::refreshBandButtonColors(juce::Button& band, juce::Button& colorSource)
{
    band.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        colorSource.findColour(juce::TextButton::ColourIds::buttonOnColourId));
    band.setColour(juce::TextButton::ColourIds::buttonColourId,
        colorSource.findColour(juce::TextButton::ColourIds::buttonOnColourId));
    band.repaint();
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
        activeBand = &lowBandButton;
        break;

    case Mid:
        attackID = Parameters::Attack_Mid_Band;
        releaseID = Parameters::Release_Mid_Band;
        threshID = Parameters::Threshold_Mid_Band;
        ratioID = Parameters::Ratio_Mid_Band;
        muteID = Parameters::Mute_Mid_Band;
        soloID = Parameters::Solo_Mid_Band;
        bypassID = Parameters::Bypassed_Mid_Band;
        activeBand = &midBandButton;
        break;

    case High:
        attackID = Parameters::Attack_High_Band;
        releaseID = Parameters::Release_High_Band;
        threshID = Parameters::Threshold_High_Band;
        ratioID = Parameters::Ratio_High_Band;
        muteID = Parameters::Mute_High_Band;
        soloID = Parameters::Solo_High_Band;
        bypassID = Parameters::Bypassed_High_Band;
        activeBand = &highBandButton;
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

juce::AudioParameterBool* getBoolParam(
    juce::AudioProcessorValueTreeState& apvts,
    const std::map<Parameters::Names, juce::String>& paramsMap,
    Parameters::Names paramID)
{
    auto* baseParam = apvts.getParameter(paramsMap.at(paramID));

    auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(baseParam);
    jassert(boolParam != nullptr);
    return boolParam;
}
