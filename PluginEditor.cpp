

#include "PluginProcessor.h"
#include "PluginEditor.h"

bool truncateKiloValue(float& value)
{
    if (value > 999.0f)
    {
        value /= 1000.0f;
        return true;
    }

    return false;
}

juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow, const juce::String& suffix)
{
    // pick the range endpoint
    float v = getLow
        ? param.getNormalisableRange().start
        : param.getNormalisableRange().end;

    // truncate to “k” if needed
    const bool useK = truncateKiloValue(v);

    // build string: number, optional “k”, then suffix
    juce::String s;
    s << v;
    if (useK)
        s << "k";
    if (suffix.isNotEmpty())
        s << " " << suffix;
    return s;
}

void addLabelPairs(juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix)
{
    labels.clear();
    labels.add({ 0.0f, getValString(param, true,  suffix) });
    labels.add({ 1.0f, getValString(param, false, suffix) });
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    auto enabled = slider.isEnabled();

    g.setColour(enabled ? Colour(97u, 18u, 167u) : Colours::darkgrey);
    g.fillEllipse(bounds);

    g.setColour(enabled ? Colour(255u, 154u, 1u) : Colours::grey);
    g.drawEllipse(bounds, 1.f);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;

        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);

        p.addRoundedRectangle(r, 2.f);

        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.fillPath(p);

        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);

        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());

        g.setColour(enabled ? Colours::black : Colours::darkgrey);
        g.fillRect(r);

        g.setColour(enabled ? Colours::white : Colours::lightgrey);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;

        auto bounds = toggleButton.getLocalBounds();

        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

        float ang = 30.f; //30.f;

        size -= 6;

        powerButton.addCentredArc(r.getCentreX(),
            r.getCentreY(),
            size * 0.5,
            size * 0.5,
            0.f,
            degreesToRadians(ang),
            degreesToRadians(360.f - ang),
            true);

        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());

        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = !toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

        g.setColour(color);

        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);

        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);
        bool buttonIsOn = toggleButton.getToggleState();

        const int cornerSize = 4;

        g.setColour(buttonIsOn ? juce::Colours::white : juce::Colours::black);
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colours::white);
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);
        g.drawFittedText(toggleButton.getName(), bounds, juce::Justification::centred, 1);
    }
}
//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    auto bounds = getLocalBounds();

    g.setColour(juce::Colours::blueviolet);
    g.drawFittedText(getName(), bounds.removeFromTop(getTextHeight() + 2), juce::Justification::centredBottom, 1);


    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());

    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; ++i)
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);

        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);

        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);

        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(getTextHeight() * 1.5);

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 1.5;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getY());

    return r;

}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();

        if (val > 999.f)
        {
            val /= 1000.f; //1001 / 1000 = 1.001
            addK = true;
        }

        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse;
    }

    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";

        str << suffix;
    }

    return str;
}

void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter* p)
{
    param = p;
    repaint();
}

juce::String RatioSlider::getDisplayString() const
{
    auto choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
    jassert(choiceParam != nullptr);

    auto currentChoice = choiceParam->getCurrentChoiceName();
    if (currentChoice.contains(".0"))
        currentChoice = currentChoice.substring(0, currentChoice.indexOf("."));

    currentChoice << ":1";

    return currentChoice;

}

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

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
