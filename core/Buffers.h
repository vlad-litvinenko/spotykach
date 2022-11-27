#pragma once

#include <assert.h>
#include "dev/sdram.h"


static const int kSliceBufferSeconds { 2 };
static const int kSourceBufferSeconds { 10 };
static const int kSampleRate { 48000 };

static const size_t kSourceBufferLength = kSourceBufferSeconds * kSampleRate;
static const size_t kSliceBufferLength = kSliceBufferSeconds * kSampleRate;

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

static float DSY_SDRAM_BSS _slcBuf5L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf5R[kSliceBufferLength];

static float DSY_SDRAM_BSS _slcBuf6L[kSliceBufferLength];
static float DSY_SDRAM_BSS _slcBuf6R[kSliceBufferLength];

class Buffers {
public:
    static Buffers& pool() {
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
        return _slcBufs[_providedSliceBufCount++];
    };

private:
    Buffers() {
        auto srcbs = kSourceBufferLength * sizeof(float);
        for (size_t i = 0; i < _srcBufsCount; i++) {
            memset(_srcBufs[i], 0, srcbs);
        }

        auto slcbs = kSliceBufferLength * sizeof(float);
        for (size_t i = 0; i < _slcBufsCount; i++) {
            memset(_slcBufs[i], 0, slcbs);
        }
    };

    int _providedSourceBufCount { 0 };
    static const int _srcBufsCount { 4 };
    float* _srcBufs[_srcBufsCount] = {
        _srcBuf1L,
        _srcBuf1R,
        _srcBuf2L,
        _srcBuf2R
    };

    int _providedSliceBufCount { 0 };
    static const int _slcBufsCount { 12 };
    float* _slcBufs[_slcBufsCount] = {
        _slcBuf1L,
        _slcBuf1R,
        _slcBuf2L,
        _slcBuf2R,
        _slcBuf3L,
        _slcBuf3R,
        _slcBuf4L,
        _slcBuf4R,
        _slcBuf5L,
        _slcBuf5R,
        _slcBuf6L,
        _slcBuf6R
    };
};
