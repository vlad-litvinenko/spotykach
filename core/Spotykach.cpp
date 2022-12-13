//
//  Spotykach.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 22.05.22.
//

#include "Spotykach.h"
#include "Envelope.h"
#include "Source.h"
#include "Generator.h"
#include "Trigger.h"
#include "LFO.h"

using namespace vlly;
using namespace spotykach;

Spotykach::Spotykach() {
    auto l = std::make_shared<LFO>();
    _releasePool.emplace_back(l);
    for (int i = 0; i < enginesCount(); i++) {
        auto e = std::make_shared<Envelope>();
        auto s = std::make_shared<Source>();
        auto g = std::make_shared<Generator>(*s, *e);
        auto t = std::make_shared<Trigger>(*g, *l);
        _engines[i] = std::make_shared<Engine>(*t, *s, *e, *g, *l);
        setVolume(0, i);
        setCascade(false);

        _releasePool.emplace_back(e);
        _releasePool.emplace_back(s);
        _releasePool.emplace_back(g);
        _releasePool.emplace_back(t);
    }
    
    setMutex(false);
    setMix(1);
    setMainVolume(1);
    setJitterRate(0);
}

Engine& Spotykach::engineAt(int index) const {
    return *(_engines[index]);
}

long Spotykach::enginesCount() const {
    return kEnginesCount;
}

void Spotykach::setJitterRate(float normVal) {
    for (int i = 0; i < enginesCount(); i++) {
        engineAt(i).setJitterRate(normVal);
    }
}

void Spotykach::setMutex(bool mutex) {
    _mutex = (mutex > 0);
}

void Spotykach::setMix(float normVal) {
    _mix = logVolume(normVal);
}

void Spotykach::setMainVolume(float normVal) {
    _mainVol = logVolume(normVal);
}

void Spotykach::setVolume(float value, int index) {
    _vol[index] = logVolume(value);
}

void Spotykach::setCascade(bool value) {
    _cascade = value;
    if (!value) return;

    Engine& e = engineAt(1);
    e.setFrozen(false);
    e.reset();
}

void Spotykach::initialize() const {
    for (auto e: _engines) e->initialize();
}

void Spotykach::preprocess(PlaybackParameters p) const {
    for (auto e: _engines) e->preprocess(p);
}

void Spotykach::process(const float* const* inBuf, float** outBuf, int numFrames) const {
    auto& e1 = engineAt(0);
    auto& e2 = engineAt(1);
    auto e1_vol = _vol[0];
    auto e2_vol = _vol[1];

    float out0 = 0;
    float out1 = 0;

    for (int f = 0; f < numFrames; f++) {
        float out0Summ = 0;
        float out1Summ = 0;
        float in0Ext = inBuf[0][f];
        float in1Ext = inBuf[1][f];

        e1.process(in0Ext, in1Ext, &out0, &out1, true);
        out0Summ += out0 * e1_vol;
        out1Summ += out1 * e1_vol;

        float e2_in0 = _cascade ? out0 : in0Ext;
        float e2_in1 = _cascade ? out1 : in1Ext;
        bool engaged = !_cascade || !e1.isLocking();
        e2.process(e2_in0, e2_in1, &out0, &out1, engaged);
        out0Summ += out0 * e2_vol;
        out1Summ += out1 * e2_vol;
        
        outBuf[0][f] = (out0Summ * _mix + in0Ext * (1 - _mix)) * _mainVol;
        outBuf[1][f] = (out1Summ * _mix + in1Ext * (1 - _mix)) * _mainVol;
    }
}
