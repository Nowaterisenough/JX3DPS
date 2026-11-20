#pragma once
#include <cstddef>
#include <cstdint>
#ifdef CAPTURE_QUEUE
#    include <fstream>
// Capture executable runs one worker and one scenario per process.
inline std::ofstream capture;

inline void capture_priorities(const uint16_t *p, size_t n)
{
    capture.exceptions(std::ios::failbit | std::ios::badbit);
    capture.open("queue.bin", std::ios::binary);
    auto size = static_cast<uint32_t>(n);
    capture.write(reinterpret_cast<const char *>(&size), sizeof(size));
    capture.write(reinterpret_cast<const char *>(p), n * sizeof(*p));
}

inline void capture_op(uint32_t type, uint32_t id = 0, int32_t value = 0)
{
    capture.write(reinterpret_cast<const char *>(&type), 4);
    capture.write(reinterpret_cast<const char *>(&id), 4);
    capture.write(reinterpret_cast<const char *>(&value), 4);
}
#else
inline void capture_priorities(const uint16_t *, size_t) { }

inline void capture_op(uint32_t, uint32_t = 0, int32_t = 0) { }
#endif
