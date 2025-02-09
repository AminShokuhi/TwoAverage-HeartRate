// Copyright (c) 2025 Amin Shokuhi
// This software is licensed under the MIT License with attribution.
// See LICENSE file for more details.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define FS 250              //Sampling Frequency, Change it based on your own needs
#define MAX_SAMPLES 1000    // Maximum samples read from the array

int calculate_heart_rate(const int16_t* ecg_samples);