#pragma once
#include "setup.h"
#include <type_traits>

#define MV_IMPL_COUNTER_READ(tag, rank, acc) mv::_counter_crumb(tag(), mv::_constant_index<rank>(), mv::_constant_index<acc>())
#define MV_COUNTER_READ(tag)\
    MV_IMPL_COUNTER_READ(tag, 1 << 0,\
    MV_IMPL_COUNTER_READ(tag, 1 << 1,\
    MV_IMPL_COUNTER_READ(tag, 1 << 2,\
    MV_IMPL_COUNTER_READ(tag, 1 << 3,\
    MV_IMPL_COUNTER_READ(tag, 1 << 4,\
    MV_IMPL_COUNTER_READ(tag, 1 << 5,\
    MV_IMPL_COUNTER_READ(tag, 1 << 6,\
    MV_IMPL_COUNTER_READ(tag, 1 << 7,\
    MV_IMPL_COUNTER_READ(tag, 1 << 8,\
    MV_IMPL_COUNTER_READ(tag, 1 << 9,\
    MV_IMPL_COUNTER_READ(tag, 1 << 10,\
    MV_IMPL_COUNTER_READ(tag, 1 << 11,\
    MV_IMPL_COUNTER_READ(tag, 1 << 12,\
    MV_IMPL_COUNTER_READ(tag, 1 << 13,\
    MV_IMPL_COUNTER_READ(tag, 1 << 14,\
    MV_IMPL_COUNTER_READ(tag, 1 << 15,\
0)))))))))))))))) // can count up to 65535

#define MV_COUNTER_INC(tag) namespace mv {\
constexpr mv::_constant_index<MV_COUNTER_READ(tag) + 1> \
_counter_crumb(tag, mv::_constant_index<(MV_COUNTER_READ(tag) + 1) & ~MV_COUNTER_READ(tag)>, \
mv::_constant_index<(MV_COUNTER_READ(tag) + 1) & MV_COUNTER_READ(tag)>) { return {}; } }

namespace mv
{
    template <mv::uint n>
    struct _constant_index : std::integral_constant<mv::uint, n> {};

    template <typename T, mv::uint rank, mv::uint acc>
    constexpr _constant_index<acc> _counter_crumb(T, _constant_index<rank>, _constant_index<acc>) { return {}; }
}
