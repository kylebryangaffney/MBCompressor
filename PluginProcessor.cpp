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
    attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
    jassert(attack != nullptr);

    release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
    jassert(release != nullptr);

    threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
    jassert(threshold != nullptr);

    ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
    jassert(ratio != nullptr);

    bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypassed"));
    jassert(ratio != nullptr);

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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
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

    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = bypassed->get();

    compressor.process(context);
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

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Threshold", "Threshold",
        juce::NormalisableRange<float>{ -18.f, 12.f },
        0));

    auto attackRange = juce::NormalisableRange<float>{ 1.f, 100.f, 1, 1 };
    auto releaseRange = juce::NormalisableRange<float>{ 5.f, 500.f, 1, 1 };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Attack", "Attack",
        attackRange, 50
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Release", "Release",
        releaseRange, 250
    ));

    auto ratioChoices = std::vector<double>{ 1, 1.5, 2, 3, 4, 7, 10, 15, 20, 50 };

    juce::StringArray ratioChoicesString;
    for (auto choice : ratioChoices)
    {
        ratioChoicesString.add(juce::String(choice, 1));
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "Ratio", "Ratio",
        ratioChoicesString, 3));


    layout.add(std::make_unique<juce::AudioParameterBool>(
        "Bypassed", "Bypassed", false));


    return layout;

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBCompAudioProcessor();
}
