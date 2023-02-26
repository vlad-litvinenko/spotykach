#pragma once

#include "daisy_seed.h"
#include "../common/fcomp.h"
#include "mi/pitch_shifter.h"
#include "mi/units.h"
#include "../core/Buffers.h"

static clouds::FloatFrame _frames[4];

class PitchShift {
public:
    PitchShift() = default;
    ~PitchShift() = default;

    void initialize(float const sr, float const delay) {
        ps_.Init(Buffers::pool.pitch_buf());
        ps_.set_ratio(stmlib::SemitonesToRatio(0));
	    ps_.set_size(1.0);
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
        ps_.set_ratio(stmlib::SemitonesToRatio(semitones));
    }

    void process(const float* const* in, float **out, size_t size) {
        if (_bypass) {
		    memcpy(out[0], in[0], size * sizeof(float));
		    memcpy(out[1], in[1], size * sizeof(float));
		    return;
        }

        for (size_t i = 0; i < size; i++) {
		    _frames[i].l = in[0][i];
		    _frames[i].r = in[1][i];
	    }
	
	    ps_.Process(_frames, size);

        for (size_t i = 0; i < size; i++) {
            out[0][i] = _frames[i].l;
            out[1][i] = _frames[i].r;
        }
    }

private:
    clouds::PitchShifter ps_;
    bool _bypass = true;
};
