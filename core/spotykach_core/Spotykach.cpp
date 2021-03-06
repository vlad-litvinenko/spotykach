//
//  Spotykach.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 22.05.22.
//

#include "Spotykach.h"

using namespace vlly;
using namespace spotykach;

Spotykach::Spotykach() {
    for (int i = 0; i < enginesCount(); i++) {
        _engines[i] = new Engine();
        setVolume(_raw.vol[i], i);
        setCascade(_raw.cascade[i], i);
    }
    
    _engines[0]->setIsOn(true);
    setMutex(_raw.mutex);
    setMix(_raw.mix);
    setMainVolume(_raw.mainVol);
}

Engine& Spotykach::engineAt(int index) {
    assert(index < enginesCount());
    return *(_engines[index]);
}

long Spotykach::enginesCount() {
    return kEnginesCount;
}

void Spotykach::setMutex(int mutex) {
    _raw.mutex = mutex;
    switch (mutex) {
        case 1: { _mutex = Mutex::cascade; break; }
        case 2: { _mutex = Mutex::any; break; }
        default: { _mutex = Mutex::off; break; }
    }
}

void Spotykach::setMix(double normVal) {
    _raw.mix = normVal;
    _mix = logVolume(normVal);
}

void Spotykach::setMainVolume(double normVal) {
    _raw.mainVol = normVal;
    _mainVol = logVolume(normVal);
}

void Spotykach::setVolume(double value, int index) {
    assert(index < enginesCount());
    _raw.vol[index] = value;
    _vol[index] = logVolume(value);
}

void Spotykach::setCascade(bool value, int index) {
    assert(index < enginesCount());
    _raw.cascade[index] = value;
    _cascade[index] = value;
    
    Engine& e = engineAt(index);
    if (value) e.setFrozen(false);
    e.reset();
}

void Spotykach::preprocess(PlaybackParameters p) {
    std::for_each(_engines.begin(), _engines.end(), [p](Engine* e) { e->preprocess(p); });
}

void Spotykach::process(float** inBuf, bool inMono, float** outBuf[kEnginesCount], bool outMono, int numFrames) {
    for (int f = 0; f < numFrames; f++) {
        float in0Ext = inBuf[0][f];
        float in1Ext = inMono ? in0Ext : inBuf[1][f];
        float out0Summ = 0;
        float out1Summ = 0;
        float cascadeSum0 = 0;
        float cascadeSum1 = 0;
        float in0 = 0;
        float in1 = 0;
        float out0 = 0;
        float out1 = 0;
        char locking = 0;
        bool previousLocking = false;
        bool engaged = true;
        for (int i = 0; i < kEnginesCount; i++) {
            Engine& e = engineAt(i);
            in0 = _cascade[i] ? cascadeSum0 : in0Ext;
            in1 = _cascade[i] ? cascadeSum1 : in0Ext;
            
            previousLocking = i > 0 && (locking >> (i - 1) & 1);
            switch (_mutex) {
                case Mutex::off: { engaged = true; break; }
                case Mutex::cascade: { engaged = !previousLocking; break; }
                case Mutex::any: { engaged = (locking == 0); break; }
            }
            
            e.process(in0, in1, &out0, &out1, engaged);
            
            if (e.isLocking() || (_mutex == Mutex::cascade && previousLocking)) {
                locking |= 1 << i;
            }
            else {
                locking &= ~(1 << i);
            }
            
            cascadeSum0 += out0;
            cascadeSum1 += out1;
            
            if (_raw.ownBus[i]) {
                outBuf[i][0][f] = out0;
                if (!outMono) outBuf[i][1][f] = out1;
            }
            else {
                out0Summ += out0 * _vol[i];
                out1Summ += out1 * _vol[i];
            }
        }
        
        outBuf[0][0][f] = (out0Summ * _mix + in0Ext * (1 - _mix)) * _mainVol;
        if (!outMono) outBuf[0][1][f] = (out1Summ * _mix + in1Ext * (1 - _mix)) * _mainVol;
    }
}
