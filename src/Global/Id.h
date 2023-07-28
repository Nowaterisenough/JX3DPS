/**
 * Project: JX3DPS
 * File: Global.h
 * Description:
 * Created Date: 2023-07-21 10:13:54
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 20:46:28
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
    JX3DPS_ID_DEFAULT = 0,           // ---------------------- JX3DPS ID ----------------------

    ERROR_CODE,                      // ---------------------- 错误码 ----------------------
    JX3DPS_SUCCESS,                  // 成功
    JX3DPS_ERROR_INVALID_EXPRESSION, // 无效语句
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
    EXPRESSION_SKILL_PLACE_HOLDERS_END,

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
    TARGET_PLACE_HOLDERS_END,

    SKILL_DEFAULT, // ---------------------- 技能 ----------------------

    SKILL_COMMON,  // ---------------------- 技能 通用 ----------------------
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

    SKILL_END,           // ---------------------- 技能 END ----------------------

    BUFF_DEFAULT,        // ---------------------- Buff ----------------------

    BUFF_COMMON_DEFAULT, // ---------------------- Buff 通用 ----------------------
    BUFF_CLASS_FEATURE,
    BUFF_WEAPON_EFFECT_CW,
    BUFF_WEAPON_EFFECT_CW_2,
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

    BUFF_END,              // ---------------------- Buff END ----------------------

    TALENT_DEFAULT,        // ----------------------  奇穴 ----------------------

    TALENT_TAI_XU_JIAN_YI, // ---------------------- 奇穴 太虚剑意 ----------------------
    TALENT_XIN_GU,
    TALENT_HUAN_YUE,
    TALENT_SHEN_MAI,
    TALENT_BAI_HONG,
    TALENT_YUN_ZHONG_JIAN,
    TALENT_WU_YI,
    TALENT_WU_YI_CLEAR,
    TALENT_JING_HUA_YING,
    TALENT_FENG_SHI,
    TALENT_FENG_SHI_CLEAR,
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

    TALENT_END,                // ----------------------  奇穴 END ----------------------

    SECRET_DEFAULT,            // ---------------------- 秘籍 ----------------------

    SECRET_TAI_XU_JIAN_YI,     // ---------------------- 秘籍 太虚剑意 ----------------------
    SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_2,
    SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_3,
    SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_4,
    SECRET_WU_WO_WU_JIAN_DAMAGE_3,
    SECRET_WU_WO_WU_JIAN_DAMAGE_4,
    SECRET_WU_WO_WU_JIAN_DAMAGE_5,
    SECRET_WU_WO_WU_JIAN_QIDIAN,
    SECRET_BA_HUANG_GUI_YUAN_DAMAGE_3,
    SECRET_BA_HUANG_GUI_YUAN_DAMAGE_4,
    SECRET_BA_HUANG_GUI_YUAN_DAMAGE_5,
    SECRET_BA_HUANG_GUI_YUAN_QIDIAN,
    SECRET_BA_HUANG_GUI_YUAN_COOLDOWN,
    SECRET_SAN_HUAN_TAO_YUE_COOLDOWN,
    SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_2,
    SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_3,
    SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_4,
    SECRET_SAN_HUAN_TAO_YUE_DAMAGE_3,
    SECRET_SAN_HUAN_TAO_YUE_DAMAGE_4,
    SECRET_SAN_HUAN_TAO_YUE_DAMAGE_5,
    SECRET_REN_JIAN_HE_YI_COOLDOWN,
    SECRET_REN_JIAN_HE_YI_RANG_1,
    SECRET_REN_JIAN_HE_YI_RANG_2,
    SECRET_REN_JIAN_HE_YI_RANG_3,
    SECRET_REN_JIAN_HE_YI_DAMAGE_40,
    SECRET_REN_JIAN_HE_YI_DAMAGE_60,
    SECRET_REN_JIAN_HE_YI_EFFECT_DOT,
    SECRET_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN,
    SECRET_SHENG_TAI_JI_PREPARE_1,
    SECRET_SHENG_TAI_JI_PREPARE_2,
    SECRET_SHENG_TAI_JI_PREPARE_3,
    SECRET_SHENG_TAI_JI_EFFECT_COOLDOWN,
    SECRET_TAI_XU_JIAN_YI_END, // ---------------------- 秘籍 太虚剑意 END ----------------------

    SECRET_END,                // ---------------------- 秘籍 END ----------------------

    EQUIP_EFFECT_DEFAULT,      // ---------------------- 装备 ----------------------
    EQUIP_EFFECT_WEAPON_CW,    // 橙武
    EQUIP_EFFECT_WEAPON_WATER,   // 水特效
    EQUIP_EFFECT_SET_ATTRIBUTE,  // 套装属性
    EQUIP_EFFECT_SET_SKILL,      // 套装技能
    EQUIP_EFFECT_ENCHANT_SHOES,  // 大附魔·鞋
    EQUIP_EFFECT_ENCHANT_JACKET, // 大附魔·衣
    EQUIP_EFFECT_ENCHANT_HAT,    // 大附魔·帽
    EQUIP_EFFECT_ENCHANT_BELT,   // 大附魔·腰
    EQUIP_EFFECT_ENCHANT_WRIST,  // 大附魔·腕
    EQUIP_EFFECT_END,            // ---------------------- 装备 END ----------------------

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

     { "----------------------技能语句占位符----------------------" },
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
     { "技能语句占位符_END" },

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
     { "目标占位符_END" },

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

     { "----------------------技能 END----------------------" },

     { "----------------------Buff----------------------" },

     { "----------------------通用----------------------" },
     { "心法特性" },
     { "武器·橙武特效" },
     { "武器·橙武特效2" },
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

     { "----------------------Buff END-----------------------" },

     { "----------------------奇穴-----------------------" },

     { "----------------------奇穴 太虚剑意-----------------------" },
     { "心固" },
     { "环月" },
     { "深埋" },
     { "白虹" },
     { "云中剑" },
     { "无意" },
     { "无意 清除" },
     { "镜花影" },
     { "风逝" },
     { "风逝 清除" },
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
     { "《纯阳诀·生太极》人偶图残页" },
     { "----------------------秘籍 太虚剑意 END-----------------------" },

     { "----------------------秘籍 END-----------------------" },

     { "----------------------装备-----------------------" },
     { "橙武" },
     { "水特效" },
     { "套装属性" },
     { "套装技能" },
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

inline Id_t SecretId(const std::string &name)
{
    for (int id = Id_t::SECRET_DEFAULT; id < Id_t::SECRET_END; ++id) {
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