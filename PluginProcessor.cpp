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


    floatHelper(lowBandComp.attack, Parameters::Names::Attack_Low_Band);
    floatHelper(lowBandComp.release, Parameters::Names::Release_Low_Band);
    floatHelper(lowBandComp.threshold, Parameters::Names::Threshold_Low_Band);

    floatHelper(midBandComp.attack, Parameters::Names::Attack_Mid_Band);
    floatHelper(midBandComp.release, Parameters::Names::Release_Mid_Band);
    floatHelper(midBandComp.threshold, Parameters::Names::Threshold_Mid_Band);

    floatHelper(highBandComp.attack, Parameters::Names::Attack_High_Band);
    floatHelper(highBandComp.release, Parameters::Names::Release_High_Band);
    floatHelper(highBandComp.threshold, Parameters::Names::Threshold_High_Band);

    floatHelper(lowMidCrossover, Parameters::Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Parameters::Names::Mid_High_Crossover_Freq);

    floatHelper(inputGainParam, Parameters::Names::Input_Gain);
    floatHelper(outputGainParam, Parameters::Names::Output_Gain);

    choiceHelper(lowBandComp.ratio, Parameters::Names::Ratio_Low_Band);
    boolHelper(lowBandComp.bypassed, Parameters::Names::Bypassed_Low_Band);
    boolHelper(lowBandComp.mute, Parameters::Names::Mute_Low_Band);
    boolHelper(lowBandComp.solo, Parameters::Names::Solo_Low_Band);

    choiceHelper(midBandComp.ratio, Parameters::Names::Ratio_Mid_Band);
    boolHelper(midBandComp.bypassed, Parameters::Names::Bypassed_Mid_Band);
    boolHelper(midBandComp.mute, Parameters::Names::Mute_Mid_Band);
    boolHelper(midBandComp.solo, Parameters::Names::Solo_Mid_Band);

    choiceHelper(highBandComp.ratio, Parameters::Names::Ratio_High_Band);
    boolHelper(highBandComp.bypassed, Parameters::Names::Bypassed_High_Band);
    boolHelper(highBandComp.mute, Parameters::Names::Mute_High_Band);
    boolHelper(highBandComp.solo, Parameters::Names::Solo_High_Band);

    LPFilter1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HPFilter1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    APFilter2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LPFilter2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HPFilter2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);


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

    for (auto& comp : compressorArray)
        comp.prepare(spec);

    LPFilter1.prepare(spec);
    HPFilter1.prepare(spec);
    APFilter2.prepare(spec);
    LPFilter2.prepare(spec);
    HPFilter2.prepare(spec);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    inputGain.setRampDurationSeconds(0.05); // 50ms
    outputGain.setRampDurationSeconds(0.05); // 50ms

    for (auto& buffer : filterBufferArray)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
    
    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);


    osc.initialise([](float x) {return std::sin(x); });
    osc.prepare(spec);
    float oscFreq = getSampleRate() / ((2 << FFTOrder::order2048) - 1) * 50;
    osc.setFrequency(oscFreq);

    oscGain.prepare(spec);
    oscGain.setGainDecibels(-12.0f);
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


void MBCompAudioProcessor::updateState()
{
    for (auto& comp : compressorArray)
    {
        comp.updateCompressorSettings();
    }

    auto lowMidCutOffFreq = lowMidCrossover->get();
    LPFilter1.setCutoffFrequency(lowMidCutOffFreq);
    HPFilter1.setCutoffFrequency(lowMidCutOffFreq);

    auto midHighCutoffFreq = midHighCrossover->get();
    APFilter2.setCutoffFrequency(midHighCutoffFreq);
    LPFilter2.setCutoffFrequency(midHighCutoffFreq);
    HPFilter2.setCutoffFrequency(midHighCutoffFreq);

    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());
    }

void MBCompAudioProcessor::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{
    for (auto& filterBuffer : filterBufferArray)
    {
        filterBuffer = inputBuffer;
    }

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBufferArray[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBufferArray[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBufferArray[2]);

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    LPFilter1.process(fb0Ctx);
    APFilter2.process(fb0Ctx);

    HPFilter1.process(fb1Ctx);
    filterBufferArray[2] = filterBufferArray[1];
    LPFilter2.process(fb1Ctx);

    HPFilter2.process(fb2Ctx);

}


void MBCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateState();

    if (false)
    {
        buffer.clear();
        juce::dsp::AudioBlock<float> audioBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);
        osc.process(context);

        oscGain.setGainDecibels(JUCE_LIVE_CONSTANT(-12));
        oscGain.process(context);
    }


    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);

    applyGain(buffer, inputGain);

    for (auto& filterBuffer : filterBufferArray)
    {
        filterBuffer = buffer;
    }

    splitBands(buffer);

    for (size_t i = 0; i < filterBufferArray.size(); ++i)
    {
        compressorArray[i].process(filterBufferArray[i]);
    }

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

    const bool bandIsSoloed = std::any_of(compressorArray.begin(), compressorArray.end(),
        [](const auto& comp) { return comp.solo->get(); });

    for (size_t i = 0; i < compressorArray.size(); ++i)
    {
        const auto& comp = compressorArray[i];

        if ((bandIsSoloed && comp.solo->get()) ||
            (!bandIsSoloed && !comp.mute->get()))
        {
            addFilterBand(buffer, filterBufferArray[i]);
        }
    }

    applyGain(buffer, outputGain);

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

    auto gainRange = juce::NormalisableRange<float>{ -24.f, 18.f, 0.1f };
    gainRange.setSkewForCentre(0.f);

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Input_Gain),
        params.at(Parameters::Names::Input_Gain),
        gainRange, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Output_Gain),
        params.at(Parameters::Names::Output_Gain),
        gainRange, 0));

    auto thresholdRange = juce::NormalisableRange<float>{ MIN_THRESHOLD, MAX_DECIBELS, 0.1f, 1 };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Threshold_Low_Band), 
        params.at(Parameters::Names::Threshold_Low_Band),
        thresholdRange, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Threshold_Mid_Band),
        params.at(Parameters::Names::Threshold_Mid_Band),
        thresholdRange, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Threshold_High_Band),
        params.at(Parameters::Names::Threshold_High_Band),
        thresholdRange, 0));

    auto attackRange = juce::NormalisableRange<float>{ 0.1f, 100.f, 0.1f };
    attackRange.setSkewForCentre(10.f);
    auto releaseRange = juce::NormalisableRange<float>{ 5.f, 500.f, 0.1f, 1 };
    releaseRange.setSkewForCentre(55.f);

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Attack_Low_Band), 
        params.at(Parameters::Names::Attack_Low_Band),
        attackRange, 50
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Attack_Mid_Band),
        params.at(Parameters::Names::Attack_Mid_Band),
        attackRange, 50
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Attack_High_Band),
        params.at(Parameters::Names::Attack_High_Band),
        attackRange, 50
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Release_Low_Band),
        params.at(Parameters::Names::Release_Low_Band),
        releaseRange, 250
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Release_Mid_Band),
        params.at(Parameters::Names::Release_Mid_Band),
        releaseRange, 250
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Release_High_Band),
        params.at(Parameters::Names::Release_High_Band),
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

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        params.at(Parameters::Names::Ratio_Mid_Band),
        params.at(Parameters::Names::Ratio_Mid_Band),
        ratioChoicesString, 3));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        params.at(Parameters::Names::Ratio_High_Band),
        params.at(Parameters::Names::Ratio_High_Band),
        ratioChoicesString, 3));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Bypassed_Low_Band),
        params.at(Parameters::Names::Bypassed_Low_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Bypassed_Mid_Band),
        params.at(Parameters::Names::Bypassed_Mid_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Bypassed_High_Band),
        params.at(Parameters::Names::Bypassed_High_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Mute_Low_Band),
        params.at(Parameters::Names::Mute_Low_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Mute_Mid_Band),
        params.at(Parameters::Names::Mute_Mid_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Mute_High_Band),
        params.at(Parameters::Names::Mute_High_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Solo_Low_Band),
        params.at(Parameters::Names::Solo_Low_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Solo_Mid_Band),
        params.at(Parameters::Names::Solo_Mid_Band), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        params.at(Parameters::Names::Solo_High_Band),
        params.at(Parameters::Names::Solo_High_Band), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Low_Mid_Crossover_Freq),
        params.at(Parameters::Names::Low_Mid_Crossover_Freq),
        juce::NormalisableRange<float>(20, 999, 1, 1), 450));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        params.at(Parameters::Names::Mid_High_Crossover_Freq),
        params.at(Parameters::Names::Mid_High_Crossover_Freq),
        juce::NormalisableRange<float>(1000, 20000, 1, 1), 2000));

    return layout;

}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBCompAudioProcessor();
}
