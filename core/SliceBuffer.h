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
    SliceBuffer();

    void initialize() override;
    float read(int, uint32_t) override;
    void write(float, float) override;
    unsigned long writeHead() override { return _writeHead; }
    void rewind() override;
    bool isFull() override;
    void reset() override;
    
    ~SliceBuffer() {};
    
private:
    uint32_t _size;
    uint32_t _writeHead;
    float* _buffer[2];
};

#endif /* SliceBuffer_h */
