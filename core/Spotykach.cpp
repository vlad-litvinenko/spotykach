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
#include "../common/fcomp.h"

using namespace vlly;
using namespace spotykach;

Spotykach::Spotykach() {
    auto l = std::make_shared<LFO>();
    _releasePool.emplace_back(l);

    auto e_a = std::make_shared<Envelope>();
    auto s_a = std::make_shared<Source>();
    auto g_a = std::make_shared<Generator>(*s_a, *e_a, *l);
    auto t_a = std::make_shared<Trigger>(*g_a);
    _engines[0] = std::make_shared<Engine>(*t_a, *s_a, *e_a, *g_a, *l);
    _engines[0]->index = 1;
    _releasePool.emplace_back(e_a);
    _releasePool.emplace_back(s_a);
    _releasePool.emplace_back(g_a);
    _releasePool.emplace_back(t_a);

    auto e_b = std::make_shared<Envelope>();
    auto s_b = std::make_shared<Source>();
    auto g_b = std::make_shared<Generator>(*s_b, *e_b, *l);
    auto t_b = std::make_shared<Trigger>(*g_b);
    _engines[1] = std::make_shared<Engine>(*t_b, *s_b, *e_b, *g_b, *l);
    _engines[1]->index = 2;
    _releasePool.emplace_back(e_b);
    _releasePool.emplace_back(s_b);
    _releasePool.emplace_back(g_b);
    _releasePool.emplace_back(t_b);
    

    g_a->set_on_update([this, g_b]{ if (this->_cascade) g_b->reset(); });

    setMutex(false);
    setCascade(false);
    setSplit(false);
    setJitterRate(0);
    set_pattern_balance(0.5);
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

void Spotykach::setVolumeBalance(float value) {
    float amp = 1.7;
    if (fcomp(value, 0.5)) {
        _vol[0] = amp;
        _vol[1] = amp;
    }
    else if (value < 0.5) {
        _vol[0] = amp;
        _vol[1] = logVolume(2 * value) * amp;    
    } 
    else {
        _vol[0] = logVolume(2 * (1 - value)) * amp;
        _vol[1] = amp;
    }
}

void Spotykach::set_pattern_balance(float value) {
    if (fcomp(value, pattern_balance_)) return;
    pattern_balance_ = value;

    auto& e1 = engineAt(0);
    auto& e2 = engineAt(1);
    //center
    if (fcomp(value, 0.5)) {
        e1.setShift(0);
        e1.setRepeats(1.0);
        e2.setShift(0);
        e2.setRepeats(1.0);
        return;
    }
    //0...0.24
    if (value < 0.25) {
        e1.setShift(0);
        e1.setRepeats(1 - 2 * value);
        e2.setShift(1 - 2 * value);
        e2.setRepeats(2 * value);
        return;
    }
    //0.25...center
    if (value < 0.5) {
        e1.setShift(0);
        e1.setRepeats(2 * value);
        e2.setShift(1 - 2 * value);
        e2.setRepeats(2 * value);
        return;
    }

    //0.76...1.0
    if (value > 0.75) {
        e1.setShift(2 * value - 1);
        e1.setRepeats(2 * (1 - value));
        e2.setShift(0);
        e2.setRepeats(2 * value - 1);
        return;
    }

    //0.5...0.75
    if (value > 0.5) {
        e1.setShift(2 * value - 1);
        e1.setRepeats(2 * (1 - value));
        e2.setShift(0);
        e2.setRepeats(2 * (1 - value));
    }
}

void Spotykach::setCascade(bool value) {
    auto needsReset = !_cascade && value;
    _cascade = value;
    Engine& e = engineAt(1);
    e.setAntifreeze(value);
    
    if (needsReset) e.reset();
}

void Spotykach::setSplit(bool value) {
    _split = value;
}

void Spotykach::initialize() const {
    for (auto e: _engines) e->initialize();
}

void Spotykach::step() {
    auto& e1 = engineAt(0);
    e1.step();
    
    auto& e2 = engineAt(1);
    bool engaged = !(_mutex && e1.isLocking());
    e2.step(engaged);
}

void Spotykach::preprocess(PlaybackParameters p) const {
    for (auto e: _engines) e->preprocess(p);
}

void Spotykach::process(const float* const* in_buf, float** out_buf, int num_frames) const {
    auto& e1 = engineAt(0);
    auto& e2 = engineAt(1);
    auto e1_vol = _vol[0];
    auto e2_vol = _vol[1];

    for (int f = 0; f < num_frames; f++) {
        float in_0_ext = in_buf[0][f];
        float in_1_ext = in_buf[0][f]; //Note! Both are taken from 0, i.e. mono

        float out_0_a = 0;
        float out_1_a = 0;
        e1.process(in_0_ext, in_1_ext, &out_0_a, &out_1_a);
        out_0_a *= e1_vol;
        out_1_a *= e1_vol;

        float out_0_b = 0;
        float out_1_b = 0;
        float e2_in0 = _cascade ? out_0_a : in_0_ext;
        float e2_in1 = _cascade ? out_0_a : in_1_ext;
        e2.process(e2_in0, e2_in1, &out_0_b, &out_1_b);
        out_0_b *= e2_vol;
        out_1_b *= e2_vol;

        if (_split) {
            out_buf[0][f] = out_0_a;
            out_buf[1][f] = out_0_b;    
        }
        else {
            out_buf[0][f] = out_0_a + out_0_b;
            out_buf[1][f] = out_0_a + out_0_b;
        }
    }
}
