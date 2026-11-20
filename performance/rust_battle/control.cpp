#include <vector>

#include "src/core/runtime/simulation.hpp"

#include "bridge.hpp"
#include "capture.hpp"
using namespace JX3DPS::runtime;

namespace {
struct Control
{
    DeadlineQueue        queue;
    ProgramView          program;
    std::vector<uint8_t> consumed;
};

bool scan(Control &control, void *context, const Hooks &hooks)
{
    const auto &program = control.program;
    for (uint32_t i = 0; i < program.line_count; ++i) {
        const auto &line = program.lines[i];
        if (control.consumed[i] != 0u) {
            continue;
        }
        if (!hooks.available(context, i)) {
            if (line.sequential != 0u) {
                return false;
            }
            continue;
        }
        auto pc = (line.count != 0u) ? 0U : ACCEPT;
        while (pc < REJECT) {
            const auto  IDX = line.begin + pc;
            const auto &c   = program.conditions[IDX];
            pc = Compare(hooks.read(context, IDX), static_cast<Comparator>(c.comparator), c.value) ? c.yes : c.no;
        }
        if (pc == ACCEPT && hooks.cast(context, i)) {
            if (line.sequential != 0u) {
                control.consumed[i] = 1;
            }
            return true;
        }
        if (line.sequential != 0u) {
            return false;
        }
    }
    return false;
}
} // namespace

extern "C" void *cpp_new(const uint16_t *priorities, size_t n, const ProgramView *program)
{
    capture_priorities(priorities, n);
    auto *c = new Control{ {}, *program, std::vector<uint8_t>(program->line_count) };
    for (size_t i = 0; i < n; ++i) {
        c->queue.Create({}, priorities[i]);
    }
    return c;
}

extern "C" void cpp_delete(void *p)
{
    delete static_cast<Control *>(p);
}

extern "C" void cpp_arm(void *p, uint32_t id, int32_t at)
{
    capture_op(1, id, at);
    static_cast<Control *>(p)->queue.ArmAt(id, at);
}

extern "C" void cpp_cancel(void *p, uint32_t id)
{
    capture_op(2, id);
    static_cast<Control *>(p)->queue.Cancel(id);
}

extern "C" void cpp_run(void *p, void *context, const Hooks *h, uint64_t seed, int32_t end)
{
    auto &c = *static_cast<Control *>(p);
    capture_op(0);
    c.queue.Reset();
    std::fill(c.consumed.begin(), c.consumed.end(), 0);
    h->reset(context, seed);
    unsigned actions = 0;
    for (;;) {
        capture_op(3);
        c.queue.DispatchDue([&](TimerHandle, uint32_t id) {
            capture_op(4, id);
            h->timer(context, id);
        });
        capture_op(5);
        if ((c.program.scan_busy || !h->busy(context)) && scan(c, context, *h)) {
            if (++actions > 10000) {
                std::terminate();
            }
            continue;
        }
        auto delta = c.queue.NextDelta();
        capture_op(6, 0, delta.value_or(-1));
        auto next = delta ? c.queue.Now() + *delta : INT32_MAX;
        if (c.program.time_boundaries) {
            next = std::min(next, h->boundary(context));
        }
        if (next >= end) {
            h->advance(context, end);
            return;
        }
        capture_op(7, 0, next - c.queue.Now());
        c.queue.Advance(next - c.queue.Now());
        h->advance(context, next);
        actions = 0;
    }
}
