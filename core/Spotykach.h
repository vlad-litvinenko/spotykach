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
    
    void set_is_playing(bool value);
    void set_is_playing(bool value, int index, bool reset = false);

    void step();

    void setMutex(bool mutex);
    
    void setSplit(bool value);
    
    void setVolumeBalance(float value);

    void set_pattern_balance(float value);

    void setCascade(bool value);
    
    void setJitterRate(float normVal);
    
    void initialize() const;
    void preprocess(PlaybackParameters p) const;
    void process(const float* const* inBuf, float** outBuf, int numFrames) const;
    
private:
    std::array<std::shared_ptr<Engine>, kEnginesCount> _engines;
    
    float _vol[kEnginesCount];
    float pattern_balance_;
    bool _split;
    bool _mutex;
    bool _cascade;
    bool _is_playing;

    std::vector<std::shared_ptr<void>> _releasePool;
};
}
}

#endif /* Spotykach_h */
