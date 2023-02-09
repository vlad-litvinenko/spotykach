#pragma once

#include "daisy_seed.h"
#include "daisysp.h"
#include "../common/fcomp.h"

class PitchShift {
public:
    PitchShift() = default;
    ~PitchShift() = default;

    void initialize(float const sr, float const delay) {
        // _ps.Init(sr);
        // _ps.SetDelSize(delay);
    }

    void setShift(float s) {
        if (s > 1.0) s = 1.0;
        if (s < 0) s = 0;

        float semitones = 0;
        if (fcomp(s, 0.5)) {
            _bypass = true;
        }
        else if (s < 0.5) {
            semitones = 22.0 * (s - 0.5);
            _bypass = false;
        }
        else {
            semitones = 24.0 * (s - 0.5);
            _bypass = false;
        }
        // _ps.SetTransposition(semitones);
    }

    void process(const float* const* in, float **out, size_t size) {
        if (_bypass) {
		    memcpy(out[0], in[0], size * sizeof(float));
		    memcpy(out[1], in[1], size * sizeof(float));
		    return;
        }

        for (size_t i = 0; i < size; i++) {
            float in0 = in[0][i];
            float in1 = in[1][i];
            // out[0][i] = _ps.Process(in0);
            // out[1][i] = _ps.Process(in1);
        }
    }

private:
    // daisysp::PitchShifter _ps;
    bool _bypass = true;
};
