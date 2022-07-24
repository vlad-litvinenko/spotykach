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
    
    double amplitude() override { return _amp; }
    void setAmplitude(double) override;
    void setPeriod(double) override;
    void setCurrentBeat(double) override;
    void setFramesPerMeasure(long) override;
    float triangleValueAt(int) override;
    
private:
    bool _isOn { false };
    long _framesPerMeasure { 0 };
    long _framesPerBeat { 0 };
    double _currentBeat { 0 };
    double _period { 0.25 }; // 1/4
    double _amp { 1.0 }; // +1.0...-1.0
 };

#endif /* LFO_h */
