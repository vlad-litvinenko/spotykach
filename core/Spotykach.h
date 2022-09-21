//
//  Spotykach.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 22.05.22.
//

#ifndef Spotykach_h
#define Spotykach_h

#include "Engine.h"
#include "Parameters.h"
#include <array>


namespace vlly {
namespace spotykach {

struct SpotykachRawParameters {
    float vol[2]       = { 1.0, 0.0 };
    float mix          = 1.0;
    float mainVol      = 1.0;
    float jitterRate   = 0.75;
    int mutex           = 0;
    bool cascade[2]     = { false, false };
    bool ownBus[2]      = { false, false };
};

static const int kEnginesCount = 2;

class Spotykach {
public:
    Spotykach();
    ~Spotykach() {};
    
    Engine& engineAt(int index);
    long enginesCount();
    
    void setMutex(int mutex);
    
    void setMix(float normVal);
    
    void setMainVolume(float normVal);
    
    void setVolume(float value, int index);
    
    void setCascade(bool value, int index);
    
    void setJitterRate(float normVal);
    
    void sendToOwnBus(bool value, int index) { _raw.ownBus[index] = value; };
    
    void initialize();
    void preprocess(PlaybackParameters p);
    void process(const float* const* inBuf, bool inMono, float** outBuf[kEnginesCount], bool outMono, int numFrames);
    
    SpotykachRawParameters rawParameters() { return _raw; }
    
    void resetCascadeOf(int index) {
        int nextIndex = index + 1;
        if (_cascade[nextIndex]) engineAt(nextIndex).reset(false);
    }
    
private:
    Engine* _engines[kEnginesCount];
    SpotykachRawParameters _raw;
    
    float _vol[kEnginesCount];
      bool _cascade[kEnginesCount];
    
    float _mix;
    float _mainVol;
    Mutex _mutex;
};
}
}

#endif /* Spotykach_h */
