/**
 * Project: JX3DPS
 * File: Global.h
 * Description:
 * Created Date: 2023-07-21 10:13:54
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-14 12:29:27
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_GLOBAL_H__
#define __JX3DPS_GLOBAL_H__

#include <array>
#include <string_view>
#include <vector>

#include "JX3.h"

namespace JX3DPS {

enum Id_t
{
    JX3DPS_ID_DEFAULT = 0, // ---------------------- JX3DPS ID ----------------------

    ERROR_CODE,            // ---------------------- 错误码 ----------------------
    JX3DPS_SUCCESS,        // 成功
    JX3DPS_ERROR_INVALID_EXPRESSION,                     // 无效语句
    JX3DPS_ERROR_INVALID_EXPRESSION_SKILL,               // 无效语句技能
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT,               // 无效语句事件
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_TIME,          // 无效语句时间
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_ADD_TARGET,    // 无效语句添加目标
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_CHANGE_TARGET, // 无效语句改变目标
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_ADD_BUFF,      // 无效语句添加Buff
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_CLEAR_BUFF,    // 无效语句清除Buff
    JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_SET_TARGET,    // 无效语句设置目标
    JX3DPS_ERROR_INVALID_JSON,                           // 无效JSON
    ERROR_CODE_END, // ---------------------- 错误码 END ----------------------

    EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT, // ---------------------- 事件语句占位符 ----------------------
    EXPRESSION_EVENT_START,
    EXPRESSION_EVENT_END,

    EXPRESSION_SKILL_PLACE_HOLDERS_DEFAULT, // ---------------------- 技能语句占位符 ----------------------
    EXPRESSION_SKILL_PLACE_HOLDERS_1,
    EXPRESSION_SKILL_PLACE_HOLDERS_2,
    EXPRESSION_SKILL_PLACE_HOLDERS_3,
    EXPRESSION_SKILL_PLACE_HOLDERS_4,
    EXPRESSION_SKILL_PLACE_HOLDERS_5,
    EXPRESSION_SKILL_PLACE_HOLDERS_6,
    EXPRESSION_SKILL_PLACE_HOLDERS_7,
    EXPRESSION_SKILL_PLACE_HOLDERS_8,
    EXPRESSION_SKILL_PLACE_HOLDERS_9,
    EXPRESSION_SKILL_PLACE_HOLDERS_10,
    EXPRESSION_SKILL_PLACE_HOLDERS_11,
    EXPRESSION_SKILL_PLACE_HOLDERS_12,
    EXPRESSION_SKILL_PLACE_HOLDERS_13,
    EXPRESSION_SKILL_PLACE_HOLDERS_14,
    EXPRESSION_SKILL_PLACE_HOLDERS_15,
    EXPRESSION_SKILL_PLACE_HOLDERS_16,
    EXPRESSION_SKILL_PLACE_HOLDERS_17,
    EXPRESSION_SKILL_PLACE_HOLDERS_18,
    EXPRESSION_SKILL_PLACE_HOLDERS_19,
    EXPRESSION_SKILL_PLACE_HOLDERS_END, // ---------------------- 技能语句占位符 END ----------------------

    PLAYER_ID,                    // ---------------------- 角色 ----------------------

    TARGET_PLACE_HOLDERS_DEFAULT, // ---------------------- 目标占位符 ----------------------
    TARGET_PLACE_HOLDERS_1,
    TARGET_PLACE_HOLDERS_2,
    TARGET_PLACE_HOLDERS_3,
    TARGET_PLACE_HOLDERS_4,
    TARGET_PLACE_HOLDERS_5,
    TARGET_PLACE_HOLDERS_6,
    TARGET_PLACE_HOLDERS_7,
    TARGET_PLACE_HOLDERS_8,
    TARGET_PLACE_HOLDERS_9,
    TARGET_PLACE_HOLDERS_10,
    TARGET_PLACE_HOLDERS_11,
    TARGET_PLACE_HOLDERS_12,
    TARGET_PLACE_HOLDERS_13,
    TARGET_PLACE_HOLDERS_14,
    TARGET_PLACE_HOLDERS_15,
    TARGET_PLACE_HOLDERS_16,
    TARGET_PLACE_HOLDERS_17,
    TARGET_PLACE_HOLDERS_18,
    TARGET_PLACE_HOLDERS_19,
    TARGET_PLACE_HOLDERS_20,
    TARGET_PLACE_HOLDERS_21,
    TARGET_PLACE_HOLDERS_22,
    TARGET_PLACE_HOLDERS_23,
    TARGET_PLACE_HOLDERS_24,
    TARGET_PLACE_HOLDERS_25,
    TARGET_PLACE_HOLDERS_26,
    TARGET_PLACE_HOLDERS_27,
    TARGET_PLACE_HOLDERS_28,
    TARGET_PLACE_HOLDERS_29,
    TARGET_PLACE_HOLDERS_END, // ---------------------- 目标占位符 END ----------------------

    SKILL_DEFAULT,            // ---------------------- 技能 ----------------------

    SKILL_COMMON, // ---------------------- 技能 通用 ----------------------
    SKILL_PO_ZHAO,
    SKILL_PENDANT_OVERCOME,

    SKILL_TAI_XU_JIAN_YI, // ---------------------- 技能 太虚剑意 ----------------------
    SKILL_WU_WO_WU_JIAN,
    SKILL_BA_HUANG_GUI_YUAN,
    SKILL_SAN_HUAN_TAO_YUE,
    SKILL_WAN_JIAN_GUI_ZONG,
    SKILL_REN_JIAN_HE_YI,
    SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN,
    SKILL_REN_JIAN_HE_YI_TUN_RI_YUE,
    SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI,
    SKILL_SAN_CHAI_JIAN_FA,
    SKILL_ZI_QI_DONG_LAI,
    SKILL_SUI_XING_CHEN,
    SKILL_TUN_RI_YUE,
    SKILL_SHENG_TAI_JI,
    SKILL_JING_HUA_YING,
    SKILL_TAI_XU_JIAN_YI_END, // ---------------------- 技能 太虚剑意 END ----------------------

    SKILL_MO_WEN, // ---------------------- 技能 莫问 ----------------------
    SKILL_GONG,
    SKILL_BIAN_GONG,
    SKILL_SHANG,
    SKILL_JUE,
    SKILL_ZHI,
    SKILL_BIAN_ZHI,
    SKILL_YU,
    SKILL_GAO_SHAN_LIU_SHUI,
    SKILL_YANG_CHUN_BAI_XUE,
    SKILL_SHU_YING_HENG_XIE,
    SKILL_SHU_YING_HUA_SHUANG,
    SKILL_ZHENG_LV_HE_MING_1,
    SKILL_ZHENG_LV_HE_MING_2,
    SKILL_ZHENG_LV_HE_MING_3,
    SKILL_YI_XING_HUAN_YING,
    SKILL_MO_WEN_END, // ---------------------- 技能 莫问 END ----------------------

    SKILL_END,        // ---------------------- 技能 END ----------------------

    BUFF_DEFAULT,     // ---------------------- Buff ----------------------

    BUFF_COMMON_DEFAULT, // ---------------------- Buff 通用 ----------------------
    BUFF_CLASS_FEATURE,
    BUFF_WEAPON_EFFECT_CW,
    BUFF_WEAPON_EFFECT_WATER,
    BUFF_PENDANT_OVERCOME,
    BUFF_SET_ATTRIBUTE,
    BUFF_ENCHANT_SHOES,
    BUFF_ENCHANT_JACKET,
    BUFF_ENCHANT_HAT,
    BUFF_ENCHANT_BELT,
    BUFF_ENCHANT_WRIST,
    BUFF_COMMON_END, // ---------------------- Buff 通用 END ----------------------

    BUFF_TEAM_CORE,  // ---------------------- Buff 阵法 ----------------------
    BUFF_TEAM_CORE_TAI_XU_JIAN_YI_JING_MIAO,
    BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,

    BUFF_3RD_DEFAULT, // ---------------------- Buff 3rd ----------------------
    BUFF_3RD_HAN_RU_LEI,
    BUFF_3RD_XIU_QI,
    BUFF_3RD_PO_FENG,
    BUFF_3RD_JING_FENG,
    BUFF_3RD_JI_LEI,
    BUFF_3RD_JIE_HUO,
    BUFF_3RD_HAN_CHANG_LIN_LI,
    BUFF_3RD_SHU_KUANG,
    BUFF_3RD_HAN_XIAO_QIAN_JUN,
    BUFF_3RD_ZHEN_FEN,
    BUFF_3RD_SHENG_YU_MING_XIN,
    BUFF_3RD_CHAO_SHENG,
    BUFF_3RD_SHE_SHEN_HONG_FA,
    BUFF_3RD_HAO_LING_SAN_JUN,
    BUFF_3RD_FIELD_SUI_XING_CHEN,
    BUFF_3RD_FIELD_PO_CANG_QIONG,
    BUFF_3RD_SUI_XING_CHEN,
    BUFF_3RD_PO_CANG_QIONG,
    BUFF_3RD_END,        // ---------------------- Buff 3rd END ----------------------

    BUFF_TAI_XU_JIAN_YI, // ---------------------- Buff 太虚剑意 ----------------------
    BUFF_DIE_REN,
    BUFF_REN_JIAN_HE_YI,
    BUFF_WAN_XIANG_GUI_YUAN,
    BUFF_XUAN_MEN,
    BUFF_ZI_QI_DONG_LAI,
    BUFF_FIELD_SUI_XING_CHEN,
    BUFF_FIELD_TUN_RI_YUE,
    BUFF_FIELD_SHENG_TAI_JI,
    BUFF_FIELD_SUI_XING_CHEN_QI_SHENG,
    BUFF_SUI_XING_CHEN,
    BUFF_TUN_RI_YUE,
    BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN,
    BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE,
    BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI,
    BUFF_CHI_YING,
    BUFF_QI_SHENG,
    BUFF_FENG_SHI,
    BUFF_HIDDEN_LIE_YUN,
    BUFF_LIE_YUN,
    BUFF_JING_HUA_YING,
    BUFF_TAI_XU_JIAN_YI_END, // ---------------------- Buff 太虚剑意 END ----------------------

    BUFF_MO_WEN,             // ---------------------- Buff 莫问 ----------------------
    BUFF_SHANG,
    BUFF_JUE,
    BUFF_HUAN_YIN,
    BUFF_SHEN_BING_GONG,
    BUFF_QU_FENG,
    BUFF_LIU_ZHAO,
    BUFF_PO_ZHAO_LIU_ZHAO,
    BUFF_XIAN_FENG,
    BUFF_XIAN_FENG_BIAO_JI,
    BUFF_YUN_HAN,
    BUFF_SHU_LI,
    BUFF_CAN_LIAN,
    BUFF_ZHI_YIN_MIAO_YI,
    BUFF_ZHENG_LV_HE_MING,
    BUFF_GAO_SHAN_LIU_SHUI,
    BUFF_YANG_CHUN_BAI_XUE,
    BUFF_YING_ZI,
    BUFF_MO_WEN_END, // ---------------------- Buff 莫问 END ----------------------

    BUFF_END,        // ---------------------- Buff END ----------------------

    TALENT_DEFAULT,  // ----------------------  奇穴 ----------------------

    TALENT_TAI_XU_JIAN_YI, // ---------------------- 奇穴 太虚剑意 ----------------------
    TALENT_XIN_GU,
    TALENT_HUAN_YUE,
    TALENT_SHEN_MAI,
    TALENT_BAI_HONG,
    TALENT_YUN_ZHONG_JIAN,
    TALENT_WU_YI,
    TALENT_JING_HUA_YING,
    TALENT_FENG_SHI,
    TALENT_DIE_REN,
    TALENT_QIE_YU,
    TALENT_CHANG_SHENG,
    TALENT_LIE_XING,
    TALENT_FU_YIN,
    TALENT_RUO_SHUI,
    TALENT_LIE_YUN,
    TALENT_TONG_GEN,
    TALENT_GU_CHANG,
    TALENT_SUI_WU,
    TALENT_QI_SHENG,
    TALENT_WU_YU,
    TALENT_XU_JI,
    TALENT_XUAN_MEN,
    TALENT_TAI_XU_JIAN_YI_END, // ---------------------- 奇穴 太虚剑意 END ----------------------

    TALENT_MO_WEN, // ---------------------- 奇穴 莫问 ----------------------
    TALENT_HAO_ZHONG,
    TALENT_FEI_FAN,
    TALENT_CHANG_QING,
    TALENT_XIAN_FENG,
    TALENT_LIU_ZHAO,
    TALENT_HAO_QING,
    TALENT_SHI_XIANG,
    TALENT_ZHI_ZHI,
    TALENT_KE_MENG,
    TALENT_SHU_LI,
    TALENT_XIU_QI,
    TALENT_YUN_HAN,
    TALENT_CAN_LIAN,
    TALENT_ZHENG_LV_HE_MING,
    TALENT_MO_WEN_END, // ---------------------- 奇穴 莫问 END ----------------------

    TALENT_END,        // ----------------------  奇穴 END ----------------------

    RECIPE_DEFAULT,    // ---------------------- 秘籍 ----------------------

    RECIPE_TAI_XU_JIAN_YI, // ---------------------- 秘籍 太虚剑意 ----------------------
    RECIPE_WU_WO_WU_JIAN_CRITICAL_STRIKE_2,
    RECIPE_WU_WO_WU_JIAN_CRITICAL_STRIKE_3,
    RECIPE_WU_WO_WU_JIAN_CRITICAL_STRIKE_4,
    RECIPE_WU_WO_WU_JIAN_DAMAGE_3,
    RECIPE_WU_WO_WU_JIAN_DAMAGE_4,
    RECIPE_WU_WO_WU_JIAN_DAMAGE_5,
    RECIPE_WU_WO_WU_JIAN_QIDIAN,
    RECIPE_BA_HUANG_GUI_YUAN_DAMAGE_3,
    RECIPE_BA_HUANG_GUI_YUAN_DAMAGE_4,
    RECIPE_BA_HUANG_GUI_YUAN_DAMAGE_5,
    RECIPE_BA_HUANG_GUI_YUAN_QIDIAN,
    RECIPE_BA_HUANG_GUI_YUAN_COOLDOWN,
    RECIPE_SAN_HUAN_TAO_YUE_COOLDOWN,
    RECIPE_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_2,
    RECIPE_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_3,
    RECIPE_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_4,
    RECIPE_SAN_HUAN_TAO_YUE_DAMAGE_3,
    RECIPE_SAN_HUAN_TAO_YUE_DAMAGE_4,
    RECIPE_SAN_HUAN_TAO_YUE_DAMAGE_5,
    RECIPE_REN_JIAN_HE_YI_COOLDOWN,
    RECIPE_REN_JIAN_HE_YI_RANG_1,
    RECIPE_REN_JIAN_HE_YI_RANG_2,
    RECIPE_REN_JIAN_HE_YI_RANG_3,
    RECIPE_REN_JIAN_HE_YI_DAMAGE_40,
    RECIPE_REN_JIAN_HE_YI_DAMAGE_60,
    RECIPE_REN_JIAN_HE_YI_EFFECT_DOT,
    RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN,
    RECIPE_SHENG_TAI_JI_PREPARE_1,
    RECIPE_SHENG_TAI_JI_PREPARE_2,
    RECIPE_SHENG_TAI_JI_PREPARE_3,
    RECIPE_SHENG_TAI_JI_RANG_1,
    RECIPE_SHENG_TAI_JI_RANG_2,
    RECIPE_SHENG_TAI_JI_RANG_3,
    RECIPE_SHENG_TAI_JI_EFFECT_COOLDOWN,
    RECIPE_TAI_XU_JIAN_YI_END, // ---------------------- 秘籍 太虚剑意 END ----------------------

    RECIPE_MO_WEN, // ---------------------- 秘籍 莫问 ----------------------
    RECIPE_GONG_CRITICAL_STRIKE_2,
    RECIPE_GONG_CRITICAL_STRIKE_3,
    RECIPE_GONG_CRITICAL_STRIKE_4,
    RECIPE_GONG_DAMAGE_3,
    RECIPE_GONG_DAMAGE_4,
    RECIPE_GONG_PREPARE_1,
    RECIPE_GONG_PREPARE_2,
    RECIPE_GONG_RANGE,
    RECIPE_SHANG_CRITICAL_STRIKE_2,
    RECIPE_SHANG_CRITICAL_STRIKE_3,
    RECIPE_SHANG_CRITICAL_STRIKE_4,
    RECIPE_SHANG_DAMAGE_3,
    RECIPE_SHANG_DAMAGE_4,
    RECIPE_SHANG_DAMAGE_5,
    RECIPE_ZHI_CRITICAL_STRIKE_2,
    RECIPE_ZHI_CRITICAL_STRIKE_3,
    RECIPE_ZHI_CRITICAL_STRIKE_4,
    RECIPE_ZHI_DAMAGE_3,
    RECIPE_ZHI_DAMAGE_4,
    RECIPE_ZHI_RANGE,
    RECIPE_YU_CRITICAL_STRIKE_3,
    RECIPE_YU_CRITICAL_STRIKE_4,
    RECIPE_YU_DAMAGE_3,
    RECIPE_YU_DAMAGE_4,
    RECIPE_MO_WEN_END, // ---------------------- 秘籍 莫问 END ----------------------

    RECIPE_END,        // ---------------------- 秘籍 END ----------------------

    EQUIP_EFFECT_DEFAULT,        // ---------------------- 装备 ----------------------
    EQUIP_EFFECT_WEAPON_CW,      // 橙武
    EQUIP_EFFECT_WEAPON_WATER,   // 水特效
    EQUIP_EFFECT_SET_ATTRIBUTE,  // 套装属性
    EQUIP_EFFECT_SET_SKILL,      // 套装技能
    EQUIP_EFFECT_ENCHANT_SHOES,  // 大附魔·鞋
    EQUIP_EFFECT_ENCHANT_JACKET, // 大附魔·衣
    EQUIP_EFFECT_ENCHANT_HAT,    // 大附魔·帽
    EQUIP_EFFECT_ENCHANT_BELT,   // 大附魔·腰
    EQUIP_EFFECT_ENCHANT_WRIST,  // 大附魔·腕
    EQUIP_EFFECT_END, // ---------------------- 装备 END ----------------------

    TRIGGER_DEFAULT,  // ---------------------- 触发效果 ----------------------

    TRIGGER_ENCHANT_WRIST,
    TRIGGER_ENCHANT_BELT,
    TRIGGER_ENCHANT_SHOES,
    TRIGGER_SET_ATTRIBUTE,
    TRIGGER_WEAPON_CW,
    TRIGGER_WEAPON_CW_DOT,
    TRIGGER_WEAPON_CW_DAMAGE,
    TRIGGER_WEAPON_WATER,

    TRIGGER_TAI_XU_JIAN_YI, // ---------------------- 触发效果 太虚剑意 ----------------------
    TRIGGER_WU_YI,          // 无意
    TRIGGER_FENG_SHI_ADD,   // 风逝
    TRIGGER_FENG_SHI_CLEAR, // 风逝
    TRIGGER_SHEN_MAI,       // 深埋
    TRIGGER_XU_JI,
    TRIGGER_XUAN_MEN,
    TRIGGER_CHANG_SHENG,
    TRIGGER_BAI_HONG,
    TRIGGER_WU_YU,
    TRIGGER_DIE_REN,
    TRIGGER_QIE_YU,
    TRIGGER_HUAN_YUE,
    TRIGGER_LIE_YUN,
    TRIGGER_JING_HUA_YING,
    TRIGGER_GU_CHANG,
    TRIGGER_REN_JIAN_HE_YI_DOT,
    TRIGGER_SHENG_TAI_JI_EFFECT_COOLDOWN,
    TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
    TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
    TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
    TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
    TRIGGER_FIELD_QI_SHENG,
    TRIGGER_QI_SHENG,
    TRIGGER_TAI_XU_JIAN_YI_END, // ---------------------- 触发效果 太虚剑意 END ----------------------

    TRIGGER_MO_WEN, // ---------------------- 触发效果 莫问 ----------------------
    TRIGGER_XIAN_FENG,
    TRIGGER_XIAN_FENG_BIAO_JI,
    TRIGGER_XIAN_FENG_DAMAGE,
    TRIGGER_XIAN_FENG_CLEAR,
    TRIGGER_YUN_HAN,
    TRIGGER_CAN_LIAN_ADD,
    TRIGGER_CAN_LIAN_CLEAR,
    TRIGGER_SHU_LI,
    TRIGGER_SHI_XIANG,
    TRIGGER_ZHI_ZHI,
    TRIGGER_HAO_QING_ZHI,
    TRIGGER_HAO_QING_BIAN_ZHI,
    TRIGGER_LIU_ZHAO_DAMAGE,
    TRIGGER_LIU_ZHAO_SURPLUS_DAMAGE,
    TRIGGER_MO_WEN_END, // ---------------------- 触发效果 莫问 END ----------------------

    TRIGGER_END,        // ---------------------- 触发效果 END ----------------------

    COUNT
}

typedef Error_t;

constexpr std::array<std::string_view, Id_t::COUNT> JX3DPS_NAME = {
    {{ "----------------------JX3DPS ID----------------------" },

     { "----------------------错误码-----------------------" },
     { "JX3DPS 成功" },
     { "JX3DPS 错误 无效语句" },
     { "JX3DPS 错误 无效语句技能" },
     { "JX3DPS 错误 无效语句事件" },
     { "JX3DPS 错误 无效语句时间" },
     { "JX3DPS 错误 无效语句添加目标" },
     { "JX3DPS 错误 无效语句改变目标" },
     { "JX3DPS 错误 无效语句添加Buff" },
     { "JX3DPS 错误 无效语句清除Buff" },
     { "JX3DPS 错误 无效语句设置目标" },
     { "JX3DPS 错误 无效JSON" },
     { "----------------------错误码 END-----------------------" },

     { "----------------------事件语句占位符----------------------" },
     { "模拟开始" },
     { "模拟结束" },

     { "----------------------技能语句占位符----------------------" },
     { "技能语句占位符_1" },
     { "技能语句占位符_2" },
     { "技能语句占位符_3" },
     { "技能语句占位符_4" },
     { "技能语句占位符_5" },
     { "技能语句占位符_6" },
     { "技能语句占位符_7" },
     { "技能语句占位符_8" },
     { "技能语句占位符_9" },
     { "技能语句占位符_10" },
     { "技能语句占位符_11" },
     { "技能语句占位符_12" },
     { "技能语句占位符_13" },
     { "技能语句占位符_14" },
     { "技能语句占位符_15" },
     { "技能语句占位符_16" },
     { "技能语句占位符_17" },
     { "技能语句占位符_18" },
     { "技能语句占位符_19" },
     { "----------------------技能语句占位符 END----------------------" },

     { "----------------------角色 ID----------------------" },

     { "----------------------目标占位符----------------------" },
     { "目标占位符_1" },
     { "目标占位符_2" },
     { "目标占位符_3" },
     { "目标占位符_4" },
     { "目标占位符_5" },
     { "目标占位符_6" },
     { "目标占位符_7" },
     { "目标占位符_8" },
     { "目标占位符_9" },
     { "目标占位符_10" },
     { "目标占位符_11" },
     { "目标占位符_12" },
     { "目标占位符_13" },
     { "目标占位符_14" },
     { "目标占位符_15" },
     { "目标占位符_16" },
     { "目标占位符_17" },
     { "目标占位符_18" },
     { "目标占位符_19" },
     { "目标占位符_20" },
     { "目标占位符_21" },
     { "目标占位符_22" },
     { "目标占位符_23" },
     { "目标占位符_24" },
     { "目标占位符_25" },
     { "目标占位符_26" },
     { "目标占位符_27" },
     { "目标占位符_28" },
     { "目标占位符_29" },
     { "----------------------目标占位符 END----------------------" },

     { "----------------------技能----------------------" },

     { "----------------------通用----------------------" },
     { "破招" },
     { "腰坠·破防" },

     { "----------------------技能 太虚剑意----------------------" },
     { "无我无剑" },
     { "八荒归元" },
     { "三环套月" },
     { "万剑归宗" },
     { "人剑合一" },
     { "人剑合一·空爆·碎星辰" },
     { "人剑合一·空爆·吞日月" },
     { "人剑合一·空爆·生太极" },
     { "三柴剑法" },
     { "紫气东来" },
     { "碎星辰" },
     { "吞日月" },
     { "生太极" },
     { "镜花影" },
     { "----------------------技能 太虚剑意 END----------------------" },

     { "----------------------技能 莫问----------------------" },
     { "宫" },
     { "变宫" },
     { "商" },
     { "角" },
     { "徵" },
     { "变徵" },
     { "羽" },
     { "高山流水" },
     { "阳春白雪" },
     { "疏影横斜" },
     { "疏影化双" },
     { "正律和鸣·1" },
     { "正律和鸣·2" },
     { "正律和鸣·3" },
     { "移形换影" },
     { "----------------------技能 莫问 END----------------------" },

     { "----------------------技能 END----------------------" },

     { "----------------------Buff----------------------" },

     { "----------------------Buff 通用----------------------" },
     { "心法特性" },
     { "武器·橙武特效" },
     { "武器·水特效" },
     { "腰坠·破防" },
     { "套装·属性" },
     { "刃凌" },
     { "大附魔·衣" },
     { "大附魔·头" },
     { "大附魔·腰" },
     { "昆吾·弦刃" },
     { "----------------------通用 END----------------------" },

     { "----------------------阵法----------------------" },
     { "北斗七星阵·精妙" },
     { "北斗七星阵·游刃" },

     { "----------------------3rd·Buff----------------------" },
     { "3rd·撼如雷" },
     { "3rd·袖气" },
     { "3rd·破风" },
     { "3rd·劲风" },
     { "3rd·激雷" },
     { "3rd·戒火" },
     { "3rd·酣畅淋漓" },
     { "3rd·疏狂" },
     { "3rd·寒啸千军" },
     { "3rd·振奋" },
     { "3rd·圣浴明心" },
     { "3rd·朝圣" },
     { "3rd·舍身弘法" },
     { "3rd·号令三军" },
     { "3rd·气场·碎星辰" },
     { "3rd·气场·破苍穹" },
     { "3rd·碎星辰" },
     { "3rd·破苍穹" },
     { "----------------------3rd·Buff END----------------------" },

     { "----------------------Buff 太虚剑意----------------------" },
     { "Dot·叠刃" },
     { "Dot·人剑合一" },
     { "Dot·万象归元" },
     { "玄门" },
     { "紫气东来" },
     { "气场·碎星辰" },
     { "气场·碎星辰·期声" },
     { "气场·生太极" },
     { "气场·吞日月" },
     { "碎星辰" },
     { "吞日月" },
     { "云中剑·碎星辰" },
     { "云中剑·生太极" },
     { "云中剑·吞日月" },
     { "持盈" },
     { "期声" },
     { "风逝" },
     { "隐藏Buff·裂云" },
     { "裂云" },
     { "镜花影" },
     { "----------------------Buff 太虚剑意 END----------------------" },

     { "----------------------Buff 莫问----------------------" },
     { "Dot·商" },
     { "Dot·角" },
     { "Dot·幻音" },
     { "Dot·神兵·宫" },
     { "曲风" },
     { "流照" },
     { "破招·流照" },
     { "弦风" },
     { "弦风·标记" },
     { "云汉" },
     { "书离" },
     { "参连" },
     { "知音妙意" },
     { "正律和鸣" },
     { "高山流水" },
     { "阳春白雪" },
     { "影子" },
     { "----------------------Buff 莫问 END----------------------" },

     { "----------------------Buff END-----------------------" },

     { "----------------------奇穴-----------------------" },

     { "----------------------奇穴 太虚剑意-----------------------" },
     { "心固" },
     { "环月" },
     { "深埋" },
     { "白虹" },
     { "云中剑" },
     { "无意" },
     { "镜花影" },
     { "风逝" },
     { "叠刃" },
     { "切玉" },
     { "长生" },
     { "裂云" },
     { "负阴" },
     { "若水" },
     { "裂云" },
     { "同根" },
     { "故长" },
     { "随物" },
     { "期声" },
     { "无欲" },
     { "虚极" },
     { "玄门" },
     { "----------------------奇穴 太虚剑意 END-----------------------" },

     { "----------------------奇穴 莫问-----------------------" },
     { "号钟" },
     { "飞帆" },
     { "长清" },
     { "弦风" },
     { "流照" },
     { "豪情" },
     { "师襄" },
     { "知止" },
     { "刻梦" },
     { "书离" },
     { "修齐" },
     { "云汉" },
     { "参连" },
     { "正律和鸣" },
     { "----------------------奇穴 莫问 END-----------------------" },

     { "----------------------奇穴 END-----------------------" },

     { "----------------------秘籍-----------------------" },

     { "----------------------秘籍 太虚剑意-----------------------" },
     { "《天道剑势·无我无剑》秘诀残页" },
     { "《天道剑势·无我无剑》秘诀断篇" },
     { "《天道剑势·无我无剑》秘诀绝章" },
     { "《天道剑势·无我无剑》参悟残页" },
     { "《天道剑势·无我无剑》参悟断篇" },
     { "《天道剑势·无我无剑》参悟绝章" },
     { "《天道剑势·无我无剑》人偶图残页" },
     { "《天道剑势·八荒归元》参悟残页" },
     { "《天道剑势·八荒归元》参悟断篇" },
     { "《天道剑势·八荒归元》参悟绝章" },
     { "《天道剑势·八荒归元》人偶图残页" },
     { "《天道剑势·八荒归元》穴位图残页" },
     { "《天道剑势·三环套月》穴位图残页" },
     { "《天道剑势·三环套月》秘诀残页" },
     { "《天道剑势·三环套月》秘诀断篇" },
     { "《天道剑势·三环套月》秘诀绝章" },
     { "《天道剑势·三环套月》参悟残页" },
     { "《天道剑势·三环套月》参悟断篇" },
     { "《天道剑势·三环套月》参悟绝章" },
     { "《天道剑势·人剑合一》穴位图残页" },
     { "《天道剑势·人剑合一》真传残页" },
     { "《天道剑势·人剑合一》真传残页" },
     { "《天道剑势·人剑合一》真传残页" },
     { "《天道剑势·人剑合一》参悟残页" },
     { "《天道剑势·人剑合一》参悟断篇" },
     { "《天道剑势·人剑合一》人偶图残页" },
     { "《天道剑势·人剑合一》人偶图断篇" },
     { "《纯阳诀·生太极》经脉图残页" },
     { "《纯阳诀·生太极》经脉图断篇" },
     { "《纯阳诀·生太极》经脉图绝章" },
     { "《纯阳诀·生太极》真传残页" },
     { "《纯阳诀·生太极》真传断篇" },
     { "《纯阳诀·生太极》真传绝章" },
     { "《纯阳诀·生太极》人偶图残页" },
     { "----------------------秘籍 太虚剑意 END-----------------------" },

     { "----------------------秘籍 莫问-----------------------" },
     { "《音·宫》秘诀断篇" },
     { "《音·宫》秘诀断篇" },
     { "《音·宫》秘诀绝章" },
     { "《音·宫》参悟残页" },
     { "《音·宫》参悟断篇" },
     { "《音·宫》经脉图残页" },
     { "《音·宫》经脉图断篇" },
     { "《音·宫》真传残页" },
     { "《音·商》秘诀残页" },
     { "《音·商》秘诀断篇" },
     { "《音·商》秘诀绝章" },
     { "《音·商》参悟残页" },
     { "《音·商》参悟断篇" },
     { "《音·商》参悟绝章" },
     { "《音·徵》秘诀残页" },
     { "《音·徵》秘诀断篇" },
     { "《音·徵》秘诀绝章" },
     { "《音·徵》参悟残页" },
     { "《音·徵》参悟断篇" },
     { "《音·徵》真传残页" },
     { "《音·羽》秘诀残页" },
     { "《音·羽》秘诀断篇" },
     { "《音·羽》参悟残页" },
     { "《音·羽》参悟断篇" },
     { "----------------------秘籍 莫问 END-----------------------" },

     { "----------------------秘籍 END-----------------------" },

     { "----------------------装备-----------------------" },
     { "武器效果·橙武" },
     { "武器效果·水特效" },
     { "套装·属性" },
     { "套装·技能" },
     { "大附魔·鞋" },
     { "大附魔·衣" },
     { "大附魔·帽" },
     { "大附魔·腰" },
     { "大附魔·腕" },
     { "----------------------装备 END-----------------------" }}
};

inline Id_t SkillId(const std::string &name)
{
    for (int id = Id_t::SKILL_DEFAULT; id < Id_t::SKILL_END; ++id) {
        if (name == "变宫") {
            return SKILL_GONG;
        }
        if (name == "变徵") {
            return SKILL_ZHI;
        }
        if (name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    return Id_t::JX3DPS_ID_DEFAULT;
}

inline Id_t BuffId(const std::string &name)
{
    for (int id = Id_t::BUFF_DEFAULT; id < Id_t::BUFF_END; ++id) {
        if (name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    for (int id = Id_t::BUFF_DEFAULT; id < Id_t::BUFF_END; ++id) {
        if ("Dot·" + name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    return Id_t::JX3DPS_ID_DEFAULT;
}

inline Id_t TalentId(const std::string &name)
{
    for (int id = Id_t::TALENT_DEFAULT; id < Id_t::TALENT_END; ++id) {
        if (name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    return Id_t::JX3DPS_ID_DEFAULT;
}

inline Id_t RecipeId(const std::string &name)
{
    for (int id = Id_t::RECIPE_DEFAULT; id < Id_t::RECIPE_END; ++id) {
        if (name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    return Id_t::JX3DPS_ID_DEFAULT;
}

inline Id_t EquipEffectId(const std::string &name)
{
    for (int id = Id_t::EQUIP_EFFECT_DEFAULT; id < Id_t::EQUIP_EFFECT_END; ++id) {
        if (name == JX3DPS_NAME[id]) {
            return static_cast<Id_t>(id);
        }
    }
    return Id_t::JX3DPS_ID_DEFAULT;
}

} // namespace JX3DPS

#endif // __JX3DPS_GLOBAL_H__