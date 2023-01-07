//
//  LFO.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#include "LFO.h"
#include <cmath>

//"Note that in many programming languages, the % operator
//is a remainder operator (with result the same sign as the dividend),
//not a modulo operator; the modulo operation can be obtained by
//using ((x % p) + p) % p in place of x % p."
//https://en.wikipedia.org/wiki/Triangle_wave
template<typename T>
inline float mod(T a, T b) {
    return ((a % b) + b) % b;
}

LFO::LFO() {}

void LFO::setPeriod(float value) {
    _period = std::fmin(std::fmax(value, 0.0078125), 1.); // 1/128...1
}

void LFO::setCurrentBeat(float value) {
    _currentBeat = value;
    _frame = -1;
}

void LFO::setFramesPerMeasure(long framesPerMeasure) {
    _framesPerMeasure = framesPerMeasure;
    _framesPerBeat = _framesPerMeasure / 4;
}

void LFO::advance() {
    _frame = (_frame + 1) % _framesPerMeasure;
}

float LFO::triangleValue() {
    long fp = _framesPerMeasure * _period / 2;
    float fraction = _currentBeat - int(_currentBeat / (4 * _period)) * (4 * _period);
    long fphase = static_cast<long>(fraction * _framesPerBeat + _frame);
    return (2.0 / fp) * (fp - std::abs(mod(fphase - fp, 2 * fp) - fp)) - 1.0;
}
