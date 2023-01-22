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
#include "fcomp.h"

using namespace vlly;
using namespace spotykach;

Spotykach::Spotykach() {
    auto l = std::make_shared<LFO>();
    _releasePool.emplace_back(l);
    for (int i = 0; i < enginesCount(); i++) {
        auto e = std::make_shared<Envelope>();
        auto s = std::make_shared<Source>();
        auto g = std::make_shared<Generator>(*s, *e, *l);
        auto t = std::make_shared<Trigger>(*g);
        _engines[i] = std::make_shared<Engine>(*t, *s, *e, *g, *l);

        _releasePool.emplace_back(e);
        _releasePool.emplace_back(s);
        _releasePool.emplace_back(g);
        _releasePool.emplace_back(t);
    }
    
    setMutex(false);
    setCascade(false);
    setMix(1);
    setJitterRate(0);
    setVolumeBalance(0.5);
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

void Spotykach::setVolumeBalance(float value) {
    float amp = 1.7;
    if (value < 0.5) {
        _vol[0] = logVolume(2 * value) * amp;
        _vol[1] = amp;    
    } 
    else {
        _vol[0] = amp;
        _vol[1] = logVolume(2 * (1 - value)) * amp;
    }
}

void Spotykach::setMute(bool value, int index) {
    _mute[index] = value;
}

void Spotykach::setCascade(bool value) {
    auto needsReset = !_cascade && value;
    _cascade = value;
    Engine& e = engineAt(1);
    e.setAntifreeze(value);
    
    if (needsReset) e.reset();
}

void Spotykach::initialize() const {
    for (auto e: _engines) e->initialize();
}

void Spotykach::advanceTimeline() {
    auto& e1 = engineAt(0);
    e1.advanceTimeline();
    
    auto& e2 = engineAt(1);
    bool engaged = !(_mutex && e1.isLocking());
    e2.advanceTimeline(engaged);
}

void Spotykach::preprocess(PlaybackParameters p) const {
    for (auto e: _engines) e->preprocess(p);
}

void Spotykach::process(const float* const* inBuf, float** outBuf, int numFrames) const {
    auto& e1 = engineAt(0);
    auto& e2 = engineAt(1);
    auto e1_vol = _mute[0] ? 0 : _vol[0];
    auto e2_vol = _mute[1] ? 0 : _vol[1];

    float out0 = 0;
    float out1 = 0;

    for (int f = 0; f < numFrames; f++) {
        float out0Summ = 0;
        float out1Summ = 0;
        float in0Ext = inBuf[0][f];
        float in1Ext = inBuf[1][f];

        e1.process(in0Ext, in1Ext, &out0, &out1);
        out0Summ += out0 * e1_vol;
        out1Summ += out1 * e1_vol;

        float e2_in0 = _cascade ? out0 : in0Ext;
        float e2_in1 = _cascade ? out1 : in1Ext;
        e2.process(e2_in0, e2_in1, &out0, &out1);
        out0Summ += out0 * e2_vol;
        out1Summ += out1 * e2_vol;
        
        outBuf[0][f] = (out0Summ * _mix + in0Ext * (1 - _mix));
        outBuf[1][f] = (out1Summ * _mix + in1Ext * (1 - _mix));
    }
}
