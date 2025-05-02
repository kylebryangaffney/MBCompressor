
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter* rap, const juce::String& unitSuffix, const juce::String& title) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rap),
        suffix(unitSuffix)
    {
        setName(title);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    virtual juce::String getDisplayString() const;
    void changeParam(juce::RangedAudioParameter* p);
protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct RatioSlider : RotarySliderWithLabels
{
    RatioSlider(juce::RangedAudioParameter* rap,
        const juce::String& unitSuffix) : RotarySliderWithLabels(rap, unitSuffix, "Ratio")
    {
    }

    virtual juce::String getDisplayString() const override;
};

struct PowerButton : juce::ToggleButton {};

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto insetRect = bounds.reduced(4);

        randomPath.clear();

        juce::Random r;

        randomPath.startNewSubPath(insetRect.getX(),
            insetRect.getY() + insetRect.getHeight() * r.nextFloat());

        for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
        {
            randomPath.lineTo(x,
                insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        }
    }

    juce::Path randomPath;
};

//***************************************************************//

struct Placeholder : juce::Component
{

    Placeholder();
    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }

    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider() :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
    }
};

inline void makeAttachment(std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>&
    attachment, juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, const Parameters::Names paramID, juce::Slider& slider) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramsMap.at(paramID), slider);
}

inline void makeAttachment(std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>&
    attachment, juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, const Parameters::Names paramID, juce::Button& button) noexcept
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramsMap.at(paramID), button);
}

static juce::RangedAudioParameter& getRangedParam(juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, Parameters::Names paramID)
{
    auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts.getParameter(paramsMap.at(paramID)));
    jassert(param != nullptr);

    return *param;

}

juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow, const juce::String& suffix);

bool truncateKiloValue(float& value);

void addLabelPairs(juce::Array<RotarySliderWithLabels::LabelPos>& labels,
    const juce::RangedAudioParameter& param,
    const juce::String& suffix);

struct CompressorBandControls : juce::Component
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& tree);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
    RatioSlider ratioSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attackSliderAttachment, releaseSliderAttachment, thresholdSliderAttachment, ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBandButton, midBandButton, highBandButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    void updateAttachments();

};

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<RotarySliderWithLabels> inputGainSlider, lowMidCrossoverSlider, midHighCrossoverSlider, outputGainSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        inputGainSliderAttachment, lowMidCrossoverSliderAttachment, midHighCrossoverSliderAttachment, outputGainSliderAttachment;
};
/**
*/
class MBCompAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MBCompAudioProcessorEditor(MBCompAudioProcessor&);
    ~MBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LookAndFeel lnf;

    MBCompAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer;
    GlobalControls globalControls{ audioProcessor.apvts };
    CompressorBandControls bandControls{ audioProcessor.apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessorEditor)
};

