#pragma once
#include <cstddef>
#include <cstdint>

// Shared C ABI; the caller owns one engine, queue and immutable program per worker.
struct LineView
{
    uint32_t begin, count;
    uint8_t  sequential;
};

struct ConditionView
{
    double   value;
    uint32_t yes, no;
    uint8_t  comparator;
};

struct ProgramView
{
    const LineView      *lines;
    const ConditionView *conditions;
    size_t               line_count, condition_count;
    bool                 scan_busy, time_boundaries;
};

struct Hooks
{
    void (*reset)(void *, uint64_t);
    void (*advance)(void *, int32_t);
    void (*timer)(void *, uint32_t);
    bool (*busy)(void *);
    bool (*available)(void *, uint32_t);
    double (*read)(void *, uint32_t);
    bool (*cast)(void *, uint32_t);
    int32_t (*boundary)(void *);
};

using Arm    = void (*)(void *, uint32_t, int32_t);
using Cancel = void (*)(void *, uint32_t);
extern "C"
{
    void *rust_new(const uint16_t *, size_t, const ProgramView *);
    void  rust_delete(void *);
    void  rust_arm(void *, uint32_t, int32_t);
    void  rust_cancel(void *, uint32_t);
    void  rust_run(void *, void *, const Hooks *, uint64_t, int32_t);
    void *cpp_new(const uint16_t *, size_t, const ProgramView *);
    void  cpp_delete(void *);
    void  cpp_arm(void *, uint32_t, int32_t);
    void  cpp_cancel(void *, uint32_t);
    void  cpp_run(void *, void *, const Hooks *, uint64_t, int32_t);
}
