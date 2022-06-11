//
//  Parameters.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 27.02.22.
//

#ifndef Parameters_h
#define Parameters_h
#include <array>
#include <math.h>


namespace vlly {
namespace spotykach {
    
    static constexpr double logVolume(double value) {
        return (pow(10.0, value) - 1) / 9.0;
    }

    inline static std::pair<int, int> decodePID(int pid) {
        int unit = 100 * (pid / 100);
        int param = pid - unit;
        return { unit, param };
    };

    enum SUnit {
        u1  = 100,
        u2  = 200,
        u3  = 300,
        u4  = 400,
        UnitsCount = 4
    };

    enum UnitParam {
        kP_Slice_Start      = 1,
        kP_Slice_Length     = 2,
        kP_Slice_Step       = 3,
        kP_Direction        = 4,
        kP_Retrigger        = 5,
        kP_Declick          = 6,
        kP_Grid             = 7,
        kP_Repeats          = 8,
        kP_Level            = 9,
        kP_On               = 10,
        kP_OwnBus           = 11,
        kP_Mix              = 12,
        kP_Shift            = 13,
        kP_Cascade          = 14,
        kP_RetriggerChance  = 15
    };

    enum GlobalParam {
        kP_Volume     = 73,
        kP_Mutex      = 74
    };

    enum class Mutex {
        off,
        cascade,
        any
    };

    const int kMutexCount = 2;
    
    

    enum Direction {
        kDirection_Forward,
        kDirection_Reverse,
        kDirection_Pendulum,
        kDirections_Count
    };

    enum Grid {
        kGrid_Even,
        kGrid_Odd,
        kGrid_CWord
    };

    static const int kGrids_Count = 3;

    struct Step {
        double value;
        const char* title;
    };

    struct CWord {
        int onsets;
        const char* title;
    };

    static constexpr std::array<Step, 15> EvenSteps {{
        { 0.03125, "1 / 32" },
        { 0.041666666666666664, "1 / 16T" },
        { 0.046875, "1 / 32." },
        { 0.0625, "1 / 16" },
        { 0.08333333333333333, "1 / 8T" },
        { 0.09375, "1 / 16." },
        { 0.125, "1 / 8" },
        { 0.16666666666666666, "1 / 4T" },
        { 0.1875, "1 / 8." },
        { 0.25, "1 / 4" },
        { 0.3333333333333333, "1 / 2T" },
        { 0.375, "1 / 4." },
        { 0.5, "1 / 2" },
        { 0.6666666666666666, "1 / 1T" },
        { 0.75, "1 / 2." }
    }};

    static constexpr std::array<Step, 9> OddSteps {{
        { 0.09523809523809523, "1 / 7T" },
        { 0.13333333333333333, "1 / 5T" },
        { 0.14285714285714285, "1 / 7" },
        { 0.2, "1 / 5" },
        { 0.21428571428571427, "1 / 7." },
        { 0.2222222222222222, "1 / 3T" },
        { 0.3, "1 / 5." },
        { 0.3333333333333333, "1 / 3" },
        { 0.5, "1 / 3." }
    }};

    static constexpr std::array<CWord, 9> CWords {{
        { 3, "3" },
        { 5, "5" },
        { 6, "6" },
        { 7, "7" },
        { 9, "9" },
        { 10, "10" },
        { 11, "11" },
        { 12, "12" },
        { 13, "13" }
    }};

    static constexpr int EvenStepsCount                 = EvenSteps.size();
    static constexpr int OddStepsCount                  = OddSteps.size();
    static constexpr int CWordsCount                    = CWords.size();

    inline static double findNearestStepGridPosition(int newStepsCount, double oldPosition) {
        double newStep = 1. / newStepsCount;
        return round(oldPosition / newStep) * newStep;
    }
    
    inline static Step step(int index, Grid grid) {
        switch (grid) {
            case (kGrid_Even): return EvenSteps[std::min(index, EvenStepsCount - 1)];
            case (kGrid_Odd): return OddSteps[std::min(index, OddStepsCount - 1)];
            case (kGrid_CWord): return { 0, "" };
        }
    }
}
}
#endif /* Parameters_h */
