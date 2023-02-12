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

    static const int kDirections_Count = 2;
    enum class Direction {
        forward,
        reverse
    };

    static const int kGrid_Count = 2;
    enum class Grid {
        even,
        c_word
    };

    using Step24ppgn = int;
    static constexpr std::array<Step24ppgn, 13> EvenSteps {{
        64, //1 / 1T
        48, //1 / 2
        36, //1 / 4.
        32, //1 / 2T
        24, //1 / 4
        18, //1 / 8.
        16, //1 / 4T
        12, //1 / 8"
        9,  //1 / 16.
        8,  //1 / 8T
        6,  //1 / 16
        4,  //1 / 16T
        3   //1 / 32
    }};

    using CWordOnsets = int;
    static constexpr std::array<CWordOnsets, 9> CWords {{
        3,
        5,
        6,
        7,
        9,
        10,
        11,
        12,
        13
    }};

    static constexpr int EvenStepsCount = EvenSteps.size();
    static constexpr int CWordsCount = CWords.size();
}
}
#endif /* Parameters_h */
