/*
  ==============================================================================

    Parameters.h
    Created: 6 May 2025 10:42:42am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <map>

namespace Parameters
{
    enum Names
    {
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,

        Threshold_Low_Band,
        Threshold_Mid_Band,
        Threshold_High_Band,

        Attack_Low_Band,
        Attack_Mid_Band,
        Attack_High_Band,

        Release_Low_Band,
        Release_Mid_Band,
        Release_High_Band,

        Ratio_Low_Band,
        Ratio_Mid_Band,
        Ratio_High_Band,

        Bypassed_Low_Band,
        Bypassed_Mid_Band,
        Bypassed_High_Band,

        Mute_Low_Band,
        Mute_Mid_Band,
        Mute_High_Band,

        Solo_Low_Band,
        Solo_Mid_Band,
        Solo_High_Band,

        Input_Gain,
        Output_Gain,
    };

    /** Returns a map from each enum to its display name */
    inline const std::map<Names, juce::String>& GetParams();
}