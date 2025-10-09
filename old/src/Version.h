#ifndef JX3DPS_VERSION_H
#define JX3DPS_VERSION_H

#include <array>

namespace JX3DPS {

/*------------------ 剑网3 - 历代资料片 ------------------*/
enum class JX3Version
{
    OPEN_BATE             = 0,  // 2009年08月28日 公测
    PO_XIAO_ZHI_LU        = 1,  // 2009年10月22日 破晓之路
    HAN_SHI_CHAN_JIE      = 2,  // 2010年02月08日 撼世禅劫
    CANG_JIAN_SHAN_ZHUANG = 3,  // 2010年05月26日 藏剑山庄
    ZHAN_HUN_JIE          = 4,  // 2010年10月28日 战魂劫
    LONG_ZHENG_HU_DOU     = 5,  // 2011年01月20日 龙争虎斗
    XUE_LONG_CHU_YUAN     = 6,  // 2011年04月28日 血龙出渊
    BA_SHU_FENG_YUN       = 7,  // 2011年05月19日 巴蜀风云
    YI_DAI_ZONG_SHI       = 8,  // 2011年11月16日 一代宗师
    ZHU_HUO_LIAO_TIAN     = 9,  // 2012年04月19日 烛火燎天
    ER_HAI_JING_LONG      = 10, // 2012年07月12日 洱海惊龙
    RI_YUE_MING_ZUN       = 11, // 2012年11月22日 日月明尊
    AN_SHI_ZHI_LUAN       = 12, // 2013年06月08日 安史之乱
    LUAN_SHI_CHANG_AN     = 13, // 2013年09月16日 乱世长安
    XUE_ZHAN_TIAN_CE      = 14, // 2013年11月21日 血战天策
    ZHU_LU_ZHONG_YUAN     = 15, // 2014年04月23日 逐鹿中原
    DIAO_ZHUO_BAN         = 16, // 2014年07月21日 雕琢版
    CANG_LONG_XUE_CHENG   = 17, // 2014年11月24日 苍雪龙城
    XIA_YI_QIAN_KUN       = 18, // 2015年04月20日 侠义乾坤
    JIAN_DAN_QIN_XIN      = 19, // 2015年10月26日 剑胆琴心
    ZHUANG_ZHI_LING_YUN   = 20, // 2016年04月15日 壮志凌云
    BAI_JIA_ZHENG_MING    = 21, // 2016年07月14日 百家争鸣
    FENG_GU_BA_DAO        = 22, // 2016年10月17日 风骨霸刀
    RI_YUE_LING_KONG      = 23, // 2017年04月20日 日月凌空
    REMAKE                = 24, // 2017年12月29日 重制版
    CHANG_FENG_PO_XIAO    = 25, // 2018年06月21日 长风破晓
    SHI_WAI_PENG_LAI      = 26, // 2018年12月21日 世外蓬莱
    NU_HAI_ZHENG_FENG     = 27, // 2019年06月20日 怒海争锋
    LING_XUE_CANG_FENG    = 28, // 2019年10月15日 凌雪藏锋
    JIE_LU_ZAI_JIANG_HU   = 29, // 2020年05月22日 结庐在江
    TONG_ZHU_SHAN_SHUI_JU = 30, // 2020年07月17日 同筑山水
    FENG_TIAN_ZHENG_DAO   = 31, // 2020年10月15日 奉天证道
    YUE_MAN_GUI_XIANG     = 32, // 2020年11月30日 月满归乡
    BAI_DI_FENG_YUN       = 33, // 2021年04月22日 白帝风云
    BEI_TIAN_YAO_ZONG     = 34, // 2021年10月28日 北天药宗
    JIANG_HU_WU_XIAN      = 35, // 2022年04月28日 江湖无限
    HENG_DAO_DUAN_LANG    = 36, // 2022年10月27日 横刀断浪
    QUN_XIA_WAN_BIAN      = 37, // 2023年04月27日 群侠万变
    WAN_LING_DANG_GE      = 38, // 2023年10月27日 万灵当歌
    WU_HAI_XUN_LONG       = 39, // 2024年05月01日 雾海寻龙

    COUNT,
};

constinit std::array<const char *const, static_cast<std::size_t>(JX3Version::COUNT)> JX3_VERSIONS = {
    "公测",     "破晓之路",   "撼世禅劫",   "藏剑山庄", "战魂劫",   "龙争虎斗", "血龙出渊",
    "巴蜀风云", "一代宗师",   "烛火燎天",   "洱海惊龙", "日月明尊", "安史之乱", "乱世长安",
    "血战天策", "逐鹿中原",   "雕琢版",     "苍雪龙城", "侠义乾坤", "剑胆琴心", "壮志凌云",
    "百家争鸣", "风骨霸刀",   "日月凌空",   "重制版",   "长风破晓", "世外蓬莱", "怒海争锋",
    "凌雪藏锋", "结庐在江湖", "同筑山水居", "奉天证道", "月满归乡", "白帝风云", "北天药宗",
    "江湖无限", "横刀断浪",   "群侠万变",   "万灵当歌", "雾海寻龙",
};

} // namespace JX3DPS

/*------------------ 版本号 ------------------*/

#define JX3_LEVEL_VERSION 120             // 剑网3 - 等级
#define JX3_VERSION       WU_HAI_XUN_LONG // 剑网3 - 资料片
#define VERSION_MAJOR     5               // JX3DPS - 主版本号
#define VERSION_MINOR     JX3_VERSION     // JX3DPS - 次版本号

#define AUX_STR(_STR)     #_STR
#define STR(_STR)         AUX_STR(_STR)

const char *const VERSION =
    STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH) "." STR(VERSION_TWEAK);
const char *const BRANCH = STR(VERSION_BRANCH);
const char *const NAME   = STR(PROJECT_NAME);

#endif // JX3DPS_VERSION_H
