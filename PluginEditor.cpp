

#include "PluginProcessor.h"
#include "PluginEditor.h"

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

void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colours::blueviolet);
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
}



//-----------------------------------------------------------------------//

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& paramsMap = Parameters::GetParams();

    auto& inGainParam = getRangedParam(apvts, paramsMap, Parameters::Input_Gain);
    auto& lowMidParam = getRangedParam(apvts, paramsMap, Parameters::Low_Mid_Crossover_Freq);
    auto& midHighParam = getRangedParam(apvts, paramsMap, Parameters::Mid_High_Crossover_Freq);
    auto& outGainParam = getRangedParam(apvts, paramsMap, Parameters::Output_Gain);

    inputGainSlider = std::make_unique<RotarySliderWithLabels>(&inGainParam, " dB", "Input Gain");
    lowMidCrossoverSlider = std::make_unique<RotarySliderWithLabels>(&lowMidParam, " Hz", "Low Mid Crossover");
    midHighCrossoverSlider = std::make_unique<RotarySliderWithLabels>(&midHighParam, " Hz", "Mid High Crossover");
    outputGainSlider = std::make_unique<RotarySliderWithLabels>(&outGainParam, " dB", "Output Gain");

    makeAttachment(
        inputGainSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Input_Gain,
        *inputGainSlider);

    makeAttachment(
        lowMidCrossoverSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Low_Mid_Crossover_Freq,
        *lowMidCrossoverSlider);

    makeAttachment(
        midHighCrossoverSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Mid_High_Crossover_Freq,
        *midHighCrossoverSlider);

    makeAttachment(
        outputGainSliderAttachment,
        apvts,
        paramsMap,
        Parameters::Output_Gain,
        *outputGainSlider);

    addLabelPairs(inputGainSlider->labels, inGainParam, "dB");
    addLabelPairs(lowMidCrossoverSlider->labels, lowMidParam, "Hz");
    addLabelPairs(midHighCrossoverSlider->labels, midHighParam, "Hz");
    addLabelPairs(outputGainSlider->labels, outGainParam, "dB");

    addAndMakeVisible(*inputGainSlider);
    addAndMakeVisible(*lowMidCrossoverSlider);
    addAndMakeVisible(*midHighCrossoverSlider);
    addAndMakeVisible(*outputGainSlider);
}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);

}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(3);
    auto endCap = juce::FlexItem().withWidth(5);

    flexBox.items.add(endCap);
    flexBox.items.add(juce::FlexItem(*inputGainSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*lowMidCrossoverSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*midHighCrossoverSlider).withFlex(1.0f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(*outputGainSlider).withFlex(1.0f));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}
//==============================================================================
MBCompAudioProcessorEditor::MBCompAudioProcessorEditor(MBCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lnf);
    //addAndMakeVisible(controlBar);
    //addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize(600, 500);
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
