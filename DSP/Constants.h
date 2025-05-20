/*
  ==============================================================================

    Constants.h
    Created: 13 May 2025 10:35:05am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#define MIN_FREQUENCY 20.0f
#define MAX_FREQUENCY 20000.0f

#define NEGATIVE_INFINITY -72.0f
#define MAX_DECIBELS 12.0f

#define MIN_THRESHOLD -60.0f

enum Channel
{
    Right, //effectively 0
    Left //effectively 1
};


enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};


