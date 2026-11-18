/**
 * @file example_api.c
 * @brief JX3DPS动态库 C API 使用示例
 */

#include "jx3dps.h"
#include <stdio.h>
#include <stdlib.h>

// 进度回调函数
void progress_callback(double progress) {
    printf("\r模拟进度: %.1f%%", progress * 100.0);
    fflush(stdout);
}

int main(void) {
    printf("=== JX3DPS 动态库使用示例 ===\n\n");

    // ========================================
    // 1. 获取版本信息
    // ========================================
    printf("1. 版本信息\n");
    printf("   库版本: %s\n", jx3dps_version());
    printf("   剑网3版本: %s\n", jx3dps_jx3_version());
    printf("\n");

    // ========================================
    // 2. 准备模拟配置
    // ========================================
    printf("2. 准备模拟配置\n");

    const char* input_json =
        "{"
        "  \"player\": {"
        "    \"name\": \"测试玩家\","
        "    \"class\": \"太虚剑意\","
        "    \"level\": 130,"
        "    \"attributes\": {"
        "      \"attack_power\": 15000,"
        "      \"critical_strike\": 0.45,"
        "      \"haste\": 0.25"
        "    },"
        "    \"talents\": [6670, 6667, 21836],"
        "    \"equipment\": {"
        "      \"weapon\": \"橙武\","
        "      \"set\": \"CW套装\""
        "    }"
        "  },"
        "  \"target\": {"
        "    \"name\": \"木桩\","
        "    \"level\": 130,"
        "    \"armor\": 20000"
        "  },"
        "  \"simulation\": {"
        "    \"duration\": 300,"
        "    \"iterations\": 10,"
        "    \"threads\": 4"
        "  },"
        "  \"macro\": \"/cast [buff:破] 无我无剑\\n/cast 碎星辰\\n/cast 三环套月\""
        "}";

    printf("   配置已准备\n\n");

    // ========================================
    // 3. 执行模拟（带进度回调）
    // ========================================
    printf("3. 执行DPS模拟\n");

    int result = jx3dps_simulate(input_json, progress_callback);

    printf("\n");

    if (result != 0) {
        printf("   ❌ 模拟失败，错误码: %d\n", result);
        return 1;
    }

    printf("   ✅ 模拟成功\n\n");

    // ========================================
    // 4. 获取模拟结果
    // ========================================
    printf("4. 获取模拟结果\n");

    const char* result_json = jx3dps_get_result();

    if (result_json && result_json[0] != '\0') {
        printf("%s\n\n", result_json);
    } else {
        printf("   ⚠️ 结果为空\n\n");
    }

    // ========================================
    // 5. 调试模式示例
    // ========================================
    printf("5. 调试模式示例\n");

    const char* debug_json =
        "{"
        "  \"player\": {"
        "    \"name\": \"调试玩家\","
        "    \"class\": \"太虚剑意\""
        "  },"
        "  \"target\": {"
        "    \"name\": \"木桩\""
        "  },"
        "  \"simulation\": {"
        "    \"duration\": 30"
        "  },"
        "  \"debug\": {"
        "    \"mode\": \"step\","
        "    \"breakpoints\": [\"无我无剑\", \"碎星辰\"]"
        "  }"
        "}";

    // 设置断点
    jx3dps_debugger_set_breakpoint("无我无剑");
    jx3dps_debugger_set_breakpoint("碎星辰");

    // 启动调试
    int debug_result = jx3dps_debug(debug_json, NULL);

    if (debug_result == 0) {
        printf("   调试器已启动\n");

        // 单步进入
        const char* debug_info = jx3dps_debugger_step_in();
        printf("   单步进入:\n%s\n", debug_info);

        // 单步越过
        debug_info = jx3dps_debugger_step_over();
        printf("   单步越过:\n%s\n", debug_info);

        // 继续执行
        debug_info = jx3dps_debugger_continue();
        printf("   继续执行:\n%s\n", debug_info);
    } else {
        printf("   ❌ 调试启动失败: %d\n", debug_result);
    }

    printf("\n");

    // ========================================
    // 6. 多线程模拟示例
    // ========================================
    printf("6. 多线程模拟示例\n");
    printf("   执行10次独立模拟...\n");

    for (int i = 0; i < 10; ++i) {
        int iter_result = jx3dps_simulate(input_json, NULL);
        double progress = jx3dps_get_progress();

        printf("   迭代 %d/10: %s (进度: %.1f%%)\n",
            i + 1,
            iter_result == 0 ? "✅" : "❌",
            progress * 100.0);
    }

    printf("\n");

    // ========================================
    // 完成
    // ========================================
    printf("=== 示例完成 ===\n");

    return 0;
}
