#include "macro.h"

using namespace JX3DPS;

void Macro::Run(Player &player, Targets &targets)
{
    for (auto &macro : _macros_vec[index]) {
        auto &[condition, action] = macro;
        if (condition.Evaluate()) {
            return;
        }
    }
}