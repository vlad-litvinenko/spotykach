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
    double vol[4]  = { 1.0, 1.0, 1.0, 1.0 };
    double mix      = 1.0;
    double mainVol  = 1.0;
    int mutex       = 0;
    bool cascade[4]  = { false, false, false, false };
    bool ownBus[4]   = { false, false, false, false };
};

const int kEnginesCount = 4;

class Spotykach {
public:
    Spotykach();
    ~Spotykach();
    
    Engine& engineAt(int index);
    long enginesCount();
    
    void setMutex(int mutex);
    
    void setMix(double normVal);
    
    void setMainVolume(double normVal);
    
    void setVolume(double value, int index);
    
    void setCascade(bool value, int index);
    
    void sendToOwnBus(bool value, int index) { _raw.ownBus[index] = value; };
    
    void preprocess(PlaybackParameters p);
    void process(float** inBuf, bool inMono, float** outBuf[kEnginesCount], bool outMono, int numFrames);
    
    SpotykachRawParameters rawParameters() { return _raw; }
    
    void resetCascadeOf(int index) {
        int nextIndex = index + 1;
        if (_cascade[nextIndex]) engineAt(nextIndex).reset();
    }
    
private:
    std::array<Engine*, kEnginesCount> _engines;
    
    SpotykachRawParameters _raw;
    
    double _vol[kEnginesCount];
    bool _cascade[kEnginesCount];
    
    double _mix;
    double _mainVol;
    Mutex _mutex;
};

}
}

#endif /* Spotykach_h */
