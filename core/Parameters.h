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
    
    static constexpr float logVolume(float value) {
        return (pow(10.0, value) - 1) / 9.0;
    }

    enum Direction {
        kDirection_Forward,
        kDirection_Reverse,
        kDirection_Pendulum,
        kDirections_Count
    };

    enum Grid {
        kGrid_Even,
        kGrid_CWord,
        kGrid_Count
    };

    struct Step {
        float value;
        const char* title;
    };

    struct CWord {
        int onsets;
        const char* title;
    };

    static constexpr std::array<Step, 15> EvenSteps {{
        { 0.75, "1 / 2." },
        { 0.6666666666666666, "1 / 1T" },
        { 0.5, "1 / 2" },
        { 0.375, "1 / 4." },
        { 0.3333333333333333, "1 / 2T" },
        { 0.25, "1 / 4" },
        { 0.1875, "1 / 8." },
        { 0.16666666666666666, "1 / 4T" },
        { 0.125, "1 / 8" },
        { 0.09375, "1 / 16." },
        { 0.08333333333333333, "1 / 8T" },
        { 0.0625, "1 / 16" },
        { 0.046875, "1 / 32." },
        { 0.041666666666666664, "1 / 16T" },
        { 0.03125, "1 / 32" }
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

    static constexpr int EvenStepsCount = EvenSteps.size();
    static constexpr int CWordsCount = CWords.size();

    inline static float findNearestStepGridPosition(int newStepsCount, float oldPosition) {
        float newStep = 1. / newStepsCount;
        return round(oldPosition / newStep) * newStep;
    }
    
    inline static Step step(int index, Grid grid) {
        switch (grid) {
            case (kGrid_Even): return EvenSteps[std::min(index, EvenStepsCount - 1)];
            case (kGrid_CWord): return { 0, "" };
        }
    }
}
}
#endif /* Parameters_h */
