//
//  Source.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Source__
#define __Spotykach__Source__

#include <vector>
#include "ISource.h"

class Source: public ISource {
public:
    Source();
    void setFrozen(bool) override;
    bool isFrozen() override { return _frozen; }
    
    void size(unsigned long) override;
    
    void write(float, float) override;
    unsigned long readHead() override;
    
    void read(float&, float&, unsigned long) override;
    
    void reset() override;
    
private:
    std::vector<float> _buffer[2];
    unsigned long _bufferLength;
    bool _frozen;
    unsigned long _writeHead;
    unsigned long _readHead;
    
};

#endif
