#pragma once

#include <assert.h>
#include "dev/sdram.h"


static const int kSliceBufferSeconds { 3 };
static const int kSourceBufferSeconds { 10 };
static const int kSampleRate { 48000 };

static const int kSourceBufferLength = kSourceBufferSeconds * kSampleRate;
static const int kSliceBufferLength = kSliceBufferSeconds * kSampleRate;

static float DSY_SDRAM_BSS _srcBuf1L[kSourceBufferLength];
static float DSY_SDRAM_BSS _srcBuf1R[kSourceBufferLength];

static float DSY_SDRAM_BSS _srcBuf2L[kSourceBufferLength];
static float DSY_SDRAM_BSS _srcBuf2R[kSourceBufferLength];

static float DSY_SDRAM_BSS _slcBuf1L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf1R[kSliceBufferLength];

static float DSY_SDRAM_BSS _slcBuf2L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf2R[kSliceBufferLength];

static float DSY_SDRAM_BSS _slcBuf3L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf3R[kSliceBufferLength];

static float DSY_SDRAM_BSS _slcBuf4L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf4R[kSliceBufferLength];

class Buffers {
public:
    static Buffers& pull() {
        static Buffers instance;
        return instance;
    }
    Buffers(Buffers const&) = delete;
    void operator=(Buffers const&)  = delete;

    float* sourceBuffer() {
        assert(_providedSourceBufCount < _srcBufsCount);
        return _srcBufs[_providedSourceBufCount++];
    };

    float* sliceBuffer() {
        assert(_providedSliceBufCount < _slcBufsCount);
        return _srcBufs[_providedSliceBufCount++];
    };

private:
    Buffers() {};

    int _providedSourceBufCount { 0 };
    static const int _srcBufsCount { 4 };
    float* _srcBufs[_srcBufsCount] = {
        _srcBuf1L,
        _srcBuf1R,
        _srcBuf2L,
        _srcBuf2R
    };

    int _providedSliceBufCount { 0 };
    static const int _slcBufsCount { 8 };
    float* _slcBufs[_slcBufsCount] = {
        _slcBuf1L,
        _slcBuf1R,
        _slcBuf2L,
        _slcBuf2R,
        _slcBuf3L,
        _slcBuf3R,
        _slcBuf4L,
        _slcBuf4R
    };
};
