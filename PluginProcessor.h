/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Service/Parameters.h"
#include "DSP/CompressorBand.h"
#include "DSP/ChannelEnum.h"               
#include "DSP/FIFO.h"                      
#include "DSP/SingleChannelSampleFIFO.h"


class MBCompAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MBCompAudioProcessor();
    ~MBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    SingleChannelSampleFifo<juce::AudioBuffer<float>> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<juce::AudioBuffer<float>> rightChannelFifo{ Channel::Right };


private:
    //==============================================================================

    std::array<CompressorBand, 3> compressorArray;

    CompressorBand& lowBandComp = compressorArray[0];
    CompressorBand& midBandComp = compressorArray[1];
    CompressorBand& highBandComp = compressorArray[2];

    juce::dsp::LinkwitzRileyFilter<float>
        LPFilter1, APFilter2,
        HPFilter1, LPFilter2,
        HPFilter2;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    std::array<juce::AudioBuffer<float>, 3> filterBufferArray;

    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam{ nullptr };
    juce::AudioParameterFloat* outputGainParam{ nullptr };

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }

    void updateState();
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);

    juce::dsp::Oscillator<float> osc;
    juce::dsp::Gain<float> oscGain;

    //================================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBCompAudioProcessor)
};
