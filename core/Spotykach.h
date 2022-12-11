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
#include <vector>
#include <memory>


namespace vlly {
namespace spotykach {

static const int kEnginesCount = 2;

class Spotykach {
public:
    Spotykach();
    ~Spotykach() {};
    
    Engine& engineAt(int index) const;
    long enginesCount() const;
    
    void setMutex(bool mutex);
    
    void setMix(float normVal);
    
    void setMainVolume(float normVal);
    
    void setVolume(float value, int index);
    
    void setCascade(bool value);
    
    void setJitterRate(float normVal);
    
    void initialize() const;
    void preprocess(PlaybackParameters p) const;
    void process(const float* const* inBuf, float** outBuf, int numFrames) const;
    
    void resetCascadingEngine() {
        if (_cascade) engineAt(1).reset(false);
    }
    
private:
    std::array<std::shared_ptr<Engine>, kEnginesCount> _engines;
    
    float _vol[kEnginesCount];
    float _mix;
    float _mainVol;
    bool _mutex;
    bool _cascade;

    std::vector<std::shared_ptr<void>> _releasePool;
};
}
}

#endif /* Spotykach_h */
