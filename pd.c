// Copyright (c) 2025 Amin Shokuhi
// This software is licensed under the MIT License with attribution.
// See LICENSE file for more details.

#include "pd.h"

int calculate_heart_rate(const int16_t* ecg_samples) {
    int qrs_window = (int)(0.12 * FS);
    int beat_window = (int)(0.6 * FS);
    int min_distance = (int)(0.25 * FS);
    int len = MAX_SAMPLES;

    double ecg_double[MAX_SAMPLES];
    double filtered_ecg[MAX_SAMPLES] = {0};
    double mwa_qrs[MAX_SAMPLES] = {0};
    double mwa_beat[MAX_SAMPLES] = {0};
    double detection_signal[MAX_SAMPLES] = {0};
    int QRS_detections[MAX_SAMPLES] = {0};

    for (int i = 0; i < len; i++) {
        ecg_double[i] = (double)ecg_samples[i];
    }

    for (int i = 1; i < len - 1; i++) {
        filtered_ecg[i] = pow(ecg_double[i + 1] - ecg_double[i - 1], 2);
    }

    double sum_qrs = 0.0, sum_beat = 0.0;
    for (int i = 0; i < len; i++) {
        sum_qrs += filtered_ecg[i];
        sum_beat += filtered_ecg[i];
        if (i >= qrs_window) sum_qrs -= filtered_ecg[i - qrs_window];
        if (i >= beat_window) sum_beat -= filtered_ecg[i - beat_window];
        mwa_qrs[i] = sum_qrs / (i < qrs_window ? (i + 1) : qrs_window);
        mwa_beat[i] = sum_beat / (i < beat_window ? (i + 1) : beat_window);
    }

    double signal_mean = 0.0, signal_std = 0.0;
    for (int i = 0; i < len; i++) signal_mean += filtered_ecg[i];
    signal_mean /= len;
    for (int i = 0; i < len; i++) signal_std += pow(filtered_ecg[i] - signal_mean, 2);
    signal_std = sqrt(signal_std / len);
    double dynamic_threshold = signal_mean + 0.05 * signal_std;

    int count = 0, last_peak_index = -min_distance;
    for (int i = 1; i < len - 1; i++) {
        if (mwa_qrs[i] - mwa_beat[i] > dynamic_threshold) {
            detection_signal[i] = 1.0;
        }
        if (detection_signal[i - 1] == 0.0 && detection_signal[i] == 1.0) {
            if (i - last_peak_index > min_distance) {
                QRS_detections[count++] = i;
                last_peak_index = i;
            }
        }
    }

    if (count < 2) return 0;

    double rr_intervals[MAX_SAMPLES] = {0};
    for (int i = 1; i < count; i++) {
        rr_intervals[i - 1] = (QRS_detections[i] - QRS_detections[i - 1]) / (double)FS;
    }

    double rr_mean = 0.0;
    for (int i = 0; i < count - 1; i++) {
        rr_mean += rr_intervals[i];
    }
    rr_mean /= (count - 1);

    double heart_rate = 60.0 / rr_mean;
    return (int)heart_rate;
}