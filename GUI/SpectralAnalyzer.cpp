/*
  ==============================================================================

    SpectralAnalyzer.cpp
    Created: 13 May 2025 8:57:41am
    Author:  kyleb

  ==============================================================================
*/

#include "SpectralAnalyzer.h"
#include "../Service/UtilityFunctions.h"
#include "../DSP/Constants.h"


SpectralAnalyzerComponent::SpectralAnalyzerComponent(MBCompAudioProcessor& p) :
    audioProcessor(p),
    leftPathProducer(audioProcessor.leftChannelFifo),
    rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }
    const auto& paramNames = Parameters::GetParams();

    auto floatHelper = [&apvts = audioProcessor.apvts, &paramNames](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramNames.at(paramName)));
            jassert(param != nullptr);
        };

    floatHelper(lowMidCrossoverParam, Parameters::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossoverParam, Parameters::Mid_High_Crossover_Freq);
    floatHelper(lowThresholdParam, Parameters::Threshold_Low_Band);
    floatHelper(midThresholdParam, Parameters::Threshold_Mid_Band);
    floatHelper(highThresholdParam, Parameters::Threshold_High_Band);

    startTimerHz(60);
}

SpectralAnalyzerComponent::~SpectralAnalyzerComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}


void SpectralAnalyzerComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    auto bounds = drawModuleBackground(g, getLocalBounds());

    drawBackgroundGrid(g, bounds);

    if (shouldShowFFTAnalysis)
    {
        drawFFTAnalysis(g, bounds);
    }

    drawCrossovers(g, bounds);
    drawTextLabels(g, bounds);

}

std::vector<float> SpectralAnalyzerComponent::getFrequencies()
{
    return std::vector<float>
    {
        20, /*30, 40,*/ 50, 100,
            200, /*300, 400,*/ 500, 1000,
            2000, /*3000, 4000,*/ 5000, 10000,
            20000
    };
}

std::vector<float> SpectralAnalyzerComponent::getGains()
{
    std::vector<float> values;

    float increment = MAX_DECIBELS; // 12 db steps

    for (float db = NEGATIVE_INFINITY; db <= MAX_DECIBELS; db += increment)
    {
        values.push_back(db);
    }

    return values;
}

std::vector<float> SpectralAnalyzerComponent::getXs(const std::vector<float>& freqs, float left, float width)
{
    std::vector<float> xs;
    for (auto f : freqs)
    {
        auto normX = juce::mapFromLog10(f, MIN_FREQUENCY, MAX_FREQUENCY);
        xs.push_back(left + width * normX);
    }

    return xs;
}

void SpectralAnalyzerComponent::drawBackgroundGrid(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    auto freqs = getFrequencies();

    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto xs = getXs(freqs, left, width);

    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, NEGATIVE_INFINITY, MAX_DECIBELS, float(bottom), float(top));

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}

void SpectralAnalyzerComponent::drawTextLabels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);

    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();

    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);

    for (int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }

        str << f;
        if (addK)
            str << "k";
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;

        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(bounds.getY());

        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, NEGATIVE_INFINITY, MAX_DECIBELS, float(bottom), float(top));

        String str;
        if (gDb > 0)
            str << "+";
        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(bounds.getRight() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);

        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

        r.setX(bounds.getX() + 3);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

void SpectralAnalyzerComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    juce::Rectangle<float> fftBounds = getAnalysisArea(bounds).toFloat();
    float negInf = juce::jmap(bounds.toFloat().getBottom(),
        fftBounds.getBottom(), fftBounds.getY(), NEGATIVE_INFINITY, MAX_DECIBELS);

    DBG("Negatvie Infinity: " << negInf);

    leftPathProducer.setNegativeInfinity(negInf);
    rightPathProducer.setNegativeInfinity(negInf);

}

void SpectralAnalyzerComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}


void SpectralAnalyzerComponent::timerCallback()
{
    if (shouldShowFFTAnalysis)
    {
        juce::Rectangle<int> bounds = getLocalBounds();
        juce::Rectangle<float> fftBounds = getAnalysisArea(bounds).toFloat();
        fftBounds.setBottom(bounds.getBottom());
        int sampleRate = audioProcessor.getSampleRate();

        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }
    repaint();
}


juce::Rectangle<int> SpectralAnalyzerComponent::getRenderArea(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}


juce::Rectangle<int> SpectralAnalyzerComponent::getAnalysisArea(juce::Rectangle<int> bounds)
{
    bounds = getRenderArea(bounds);
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}

void SpectralAnalyzerComponent::drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Rectangle<int> responseArea = getAnalysisArea(bounds);

    juce::Graphics::ScopedSaveState sss(g);
    g.reduceClipRegion(responseArea);

    auto leftChannelFFTPath = leftPathProducer.getPath();
    leftChannelFFTPath.applyTransform(juce::AffineTransform().translation(responseArea.getX(), 0
    ));

    g.setColour(juce::Colour(97u, 18u, 167u)); //purple-
    g.strokePath(leftChannelFFTPath, juce::PathStrokeType(1.f));

    auto rightChannelFFTPath = rightPathProducer.getPath();
    rightChannelFFTPath.applyTransform(juce::AffineTransform().translation(responseArea.getX(), 0
    ));

    g.setColour(juce::Colour(215u, 201u, 134u));
    g.strokePath(rightChannelFFTPath, juce::PathStrokeType(1.f));
}

void SpectralAnalyzerComponent::drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds = getAnalysisArea(bounds);

    const int top = bounds.getY();
    const int bottom = bounds.getBottom();
    const int left = bounds.getX();
    const int right = bounds.getRight();

    std::function<float(float)> mapX = [left = bounds.getX(), width = bounds.getWidth()]
                (float frequency)
        {
            float normX = juce::mapFromLog10(frequency, MIN_FREQUENCY, MAX_FREQUENCY);
            return left + width * normX;
        };

    std::function<float(float)> mapY = [bottom, top](float db)
        {
            return juce::jmap(db, NEGATIVE_INFINITY, MAX_DECIBELS, (float)bottom, (float)top);
        };

    g.setColour(juce::Colours::blue);
    float lowMidX = mapX(lowMidCrossoverParam->get());
    g.drawVerticalLine(lowMidX, top, bottom);
    float midHighX = mapX(midHighCrossoverParam->get());
    g.drawVerticalLine(midHighX, top, bottom);

    g.setColour(juce::Colours::yellow);
    g.drawHorizontalLine(mapY(lowThresholdParam->get()), left, lowMidX);
    g.drawHorizontalLine(mapY(midThresholdParam->get()), lowMidX, midHighX);
    g.drawHorizontalLine(mapY(highThresholdParam->get()), midHighX, right);
}