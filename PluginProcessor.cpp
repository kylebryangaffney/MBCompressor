/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBCompAudioProcessor::MBCompAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    const auto& params = Parameters::GetParams();

    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };

    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };
    
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };


    floatHelper(compressor.attack, Parameters::Names::Attack_Low_Band);
    floatHelper(compressor.release, Parameters::Names::Release_Low_Band);
    floatHelper(compressor.threshold, Parameters::Names::Threshold_Low_Band);
    floatHelper(lowCrossover, Parameters::Names::Low_Mid_Crossover_Freq);

    choiceHelper(compressor.ratio, Parameters::Names::Ratio_Low_Band);
    boolHelper(compressor.bypassed, Parameters::Names::Bypassed_Low_Band);

    LPFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HPFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

}

MBCompAudioProcessor::~MBCompAudioProcessor()
{
}

//==============================================================================
const juce::String MBCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MBCompAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MBCompAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MBCompAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MBCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MBCompAudioProcessor::getNumPrograms()
{
    return 1;   
}

int MBCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MBCompAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MBCompAudioProcessor::getProgramName(int index)
{
    return {};
}

void MBCompAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MBCompAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    compressor.prepare(spec);

    LPFilter.prepare(spec);
    HPFilter.prepare(spec);

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void MBCompAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MBCompAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MBCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //compressor.updateCompressorSettings();
    //compressor.process(buffer);

    for (auto& fb : filterBuffers)
    {
        fb = buffer;
    }

    auto cutoff = lowCrossover->get();
    LPFilter.setCutoffFrequency(cutoff);
    HPFilter.setCutoffFrequency(cutoff);

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);

    LPFilter.process(fb0Ctx);
    HPFilter.process(fb1Ctx);

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    buffer.clear();
    

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (int i = 0; i < nc; ++i)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            }
        };

    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);

}

//==============================================================================
bool MBCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MBCompAudioProcessor::createEditor()
{
    return new MBCompAudioProcessorEditor(*this);
}

//==============================================================================
void MBCompAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void MBCompAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}


juce::AudioProcessorValueTreeState::ParameterLayout MBCompAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const auto& params = Parameters::GetParams();

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Threshold_Low_Band), 
        params.at(Parameters::Names::Threshold_Low_Band),
        juce::NormalisableRange<float>{ -18.f, 12.f },
        0));

    auto attackRange = juce::NormalisableRange<float>{ 1.f, 100.f, 1, 1 };
    auto releaseRange = juce::NormalisableRange<float>{ 5.f, 500.f, 1, 1 };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Attack_Low_Band), 
        params.at(Parameters::Names::Attack_Low_Band),
        attackRange, 50
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Release_Low_Band),
        params.at(Parameters::Names::Release_Low_Band),
        releaseRange, 250
    ));

    auto ratioChoices = std::vector<double>{ 1, 1.5, 2, 3, 4, 7, 10, 15, 20, 50 };

    juce::StringArray ratioChoicesString;
    for (auto choice : ratioChoices)
    {
        ratioChoicesString.add(juce::String(choice, 1));
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        params.at(Parameters::Names::Ratio_Low_Band),
        params.at(Parameters::Names::Ratio_Low_Band),
        ratioChoicesString, 3));


    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Bypassed_Low_Band),
        params.at(Parameters::Names::Bypassed_Low_Band), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Low_Mid_Crossover_Freq),
        params.at(Parameters::Names::Low_Mid_Crossover_Freq),
        juce::NormalisableRange<float>(20, 20000, 1, 1), 500));

    return layout;

}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBCompAudioProcessor();
}
