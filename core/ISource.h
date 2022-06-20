//
//  ISource.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 14.03.22.
//

#ifndef ISource_h
#define ISource_h

class ISource {
public:
    virtual void setFrozen(bool frozen) = 0;
    virtual bool isFrozen() = 0;
    
    virtual void size(unsigned long size) = 0;
    
    virtual void write(float in0, float in1) = 0;
    virtual unsigned long readHead() = 0;
    
    virtual void read(float& out0, float& out1, unsigned long frameIndex) = 0;
    
    virtual void reset() = 0;
    
    virtual ~ISource() {};
};

#endif /* ISource_h */
