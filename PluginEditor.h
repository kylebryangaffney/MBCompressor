
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
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
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
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String suffix;
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
    { }
};

inline void makeAttachment(std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>&
    attachment, juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, const Parameters::Names paramID, juce::Slider& slider)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramsMap.at(paramID), slider);
}

static juce::RangedAudioParameter& getRangedParam(juce::AudioProcessorValueTreeState& apvts, const std::map<Parameters::Names, juce::String>& paramsMap, Parameters::Names paramID)
{
    auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts.getParameter(paramsMap.at(paramID)));
    jassert(param != nullptr);

    return *param;

}

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBCompAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer, bandControls;
    GlobalControls globalControls{ audioProcessor.apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessorEditor)
};
