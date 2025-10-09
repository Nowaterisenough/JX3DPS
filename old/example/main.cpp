#include <iostream>
#include <vector>

#include "Parser.hpp"

int main()
{
    MacroInterpreter interpreter;

    std::vector<std::string> macros = {
        "<循环宏>",
        "/switch [buff:紫气东来] 爆发宏",
        "/cast 无我无剑",
        "/cast [cd:八荒归元<3&nobuff:碎星辰] 三环套月",
        "/cast [tbuff:叠刃|skill_energy:紫气>=2] 八荒归元"
        "<爆发宏>"
        "/cast [nobuff:碎星辰] 碎星辰",
        "/cast 紫气东来",
        "/switch [nobuff:紫气东来] 循环宏"
        "/break 碎星辰",
    };

    for (const auto &macro : macros) {
        std::cout << "Parse: " << macro << std::endl;
        auto [node, error] = interpreter.ParseMacro(macro);

        if (error == ErrorCode::SUCCESS && node != nullptr) {
            bool        condition_met = node->Evaluate();
            std::string action        = node->GetAction();

            std::cout << "Condition: " << (condition_met ? "true" : "false") << std::endl;
            std::cout << "Action: " << action << std::endl;
        } else {
            std::cout << "Error" << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}