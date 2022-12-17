//
//  ISource.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 14.03.22.
//

#ifndef ISource_h
#define ISource_h

#include <stdint.h>

class ISource {
public:
    virtual void setFrozen(bool frozen) = 0;
    virtual bool isFrozen() = 0;
    virtual void setAntifreeze(bool) = 0;
    
    virtual void setCycleStart(uint32_t start) = 0;

    virtual void initialize() = 0;
    
    virtual void write(float in0, float in1) = 0;
    virtual uint32_t readHead() = 0;
    
    virtual void read(float& out0, float& out1, uint32_t frameIndex) = 0;
    
    virtual bool isFilled() = 0;
    
    virtual void reset() = 0;
    
    virtual ~ISource() {};
};

#endif /* ISource_h */
