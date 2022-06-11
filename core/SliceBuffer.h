//
//  SliceBuffer.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 17.05.22.
//

#ifndef SliceBuffer_h
#define SliceBuffer_h

#include <vector>
#include "ISliceBuffer.h"

class SliceBuffer: public ISliceBuffer {
public:
    void size(unsigned long) override;
    float read(int, long) override;
    void write(float, float) override;
    unsigned long writeHead() override { return _writeHead; }
    void rewind() override;
    bool isFull() override;
    void reset() override;
    
    ~SliceBuffer() {};
    
private:
    unsigned long _size;
    unsigned long _writeHead;
    std::vector<float> _buffer[2];
};

#endif /* SliceBuffer_h */
