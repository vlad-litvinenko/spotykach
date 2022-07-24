//
//  ILFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#ifndef ILFO_h
#define ILFO_h

class ILFO {
public:
    virtual double amplitude() = 0;
    virtual void setAmplitude(double amplitude) = 0;
    virtual void setPeriod(double period) = 0;
    virtual void setCurrentBeat(double beat) = 0;
    virtual void setFramesPerMeasure(long frames) = 0;
    virtual float triangleValueAt(int frame) = 0;
};

#endif /* ILFO_h */
