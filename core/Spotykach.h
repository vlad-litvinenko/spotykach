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
#include "globals.h"
#include <array>
#include <vector>
#include <memory>


namespace vlly {
namespace spotykach {

class Spotykach {
public:
    Spotykach();
    ~Spotykach() {};
    
    Engine& engineAt(int index) const;
    long enginesCount() const;
    
    void advanceTimeline();

    void setMutex(bool mutex);
    
    void setMix(float normVal);
    
    void setVolumeBalance(float value);

    void setPatternBalance(float value);

    void setCascade(bool value);
    
    void setJitterRate(float normVal);
    
    void initialize() const;
    void preprocess(PlaybackParameters p) const;
    void process(const float* const* inBuf, float** outBuf, int numFrames) const;
    
private:
    std::array<std::shared_ptr<Engine>, kEnginesCount> _engines;
    
    float _vol[kEnginesCount];
    float pattern_balance_;
    float _mix;
    bool _mutex;
    bool _cascade;

    std::vector<std::shared_ptr<void>> _releasePool;
};
}
}

#endif /* Spotykach_h */
