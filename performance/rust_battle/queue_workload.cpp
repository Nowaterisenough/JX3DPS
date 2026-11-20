// Independent queue-only timing control. Both standalone executables consume
// the same pre-recorded operations and validate the event order on every pass.
#include <windows.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "src/core/runtime/scheduler.hpp"
using namespace JX3DPS::runtime;

struct Op
{
    uint32_t type, id;
    int32_t  value;
};

int main(int argc, char **argv)
{
    if (argc != 3 || !SetThreadAffinityMask(GetCurrentThread(), 4)) {
        return 2;
    }
    std::ifstream input(argv[1], std::ios::binary);
    uint32_t      count = 0;
    input.read(reinterpret_cast<char *>(&count), 4);
    DeadlineQueue queue;
    for (uint32_t i = 0; i < count; ++i) {
        uint16_t priority;
        input.read(reinterpret_cast<char *>(&priority), 2);
        queue.Create({}, priority);
    }
    std::vector<Op> ops;
    Op              op;
    while (input.read(reinterpret_cast<char *>(&op), sizeof(op))) {
        ops.push_back(op);
    }
    if (ops.empty() || !input.eof() || input.gcount()) {
        return 2;
    }
    uint64_t check  = 0;
    auto     replay = [&] {
        size_t i       = 0;
        auto   updates = [&] {
            while (i < ops.size() && (ops[i].type == 1 || ops[i].type == 2)) {
                auto &o = ops[i++];
                if (o.type == 1) {
                    queue.ArmAt(o.id, o.value);
                } else {
                    queue.Cancel(o.id);
                }
            }
        };
        while (i < ops.size()) {
            updates();
            if (i == ops.size()) {
                break;
            }
            const auto &o = ops[i++];
            switch (o.type) {
                case 0: queue.Reset(); break;
                case 3:
                    queue.DispatchDue([&](TimerHandle, uint32_t id) {
                        const auto &event = ops.at(i++);
                        if (event.type != 4 || id != event.id) {
                            throw std::logic_error("event diverged");
                        }
                        check = check * 31 + id;
                        updates();
                    });
                    if (ops.at(i++).type != 5) {
                        throw std::logic_error("dispatch diverged");
                    }
                    break;
                case 6:
                    if (queue.NextDelta().value_or(-1) != o.value) {
                        throw std::logic_error("next diverged");
                    }
                    break;
                case 7: queue.Advance(o.value); break;
                default: return false;
            }
        }
        return true;
    };
    if (!replay()) {
        return 3;
    }
    const auto repetitions = std::stoul(argv[2]);
    check                  = 0;
    auto start             = std::chrono::steady_clock::now();
    for (unsigned j = 0; j < repetitions; ++j) {
        if (!replay()) {
            return 3;
        }
    }
    std::cout << std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() << ' ' << check << '\n';
}
