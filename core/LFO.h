//
//  LFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#ifndef LFO_h
#define LFO_h

#include "ILFO.h"

class LFO: public ILFO {
public:
    LFO();
    
    float amplitude() override { return _amp; }
    void setAmplitude(float) override;
    void setPeriod(float) override;
    void setCurrentBeat(float) override;
    void setFramesPerMeasure(long) override;
    float triangleValueAt(int) override;
    
private:
    bool _isOn { false };
    long _framesPerMeasure { 0 };
    long _framesPerBeat { 0 };
    float _currentBeat { 0 };
    float _period { 0.25 }; // 1/4
    float _amp { 1.0 }; // +1.0...-1.0
 };

#endif /* LFO_h */
