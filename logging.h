#pragma once
#ifndef LOGGING_H
#define LOGGING_H

#include "daisy_seed.h"

using namespace daisy;

static DaisySeed* _hw;

static void initLogger(DaisySeed hw) {
    _hw = &hw;
    _hw->StartLog(true);
}

template <typename... VA>
static void printLog(const char* format, VA... va) {
    _hw->PrintLine(format, va...);
}

#endif
