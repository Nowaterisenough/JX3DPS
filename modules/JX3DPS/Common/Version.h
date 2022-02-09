/**
 * @Description : JX3DPS版本相关设定
 * @Author      : NoWats
 * @Date        : 2022-01-26 22:25:28
 * @Update      : NoWats
 * @LastTime    : 2022-02-01 15:17:09
 * @FilePath    : \JX3DPS\modules\Common\Version.h
 */

#ifndef VERSION_H
#define VERSION_H

/*------------------ 剑网3 - 历代资料片 ------------------*/

#define OPEN_BATE             0  // 2009年08月28日 公测
#define PO_XIAO_ZHI_LU        1  // 2009年10月22日 破晓之路
#define HAN_SHI_CHAN_JIE      2  // 2010年02月08日 撼世禅劫
#define CANG_JIAN_SHAN_ZHUANG 3  // 2010年05月26日 藏剑山庄
#define ZHAN_HUN_JIE          4  // 2010年10月28日 战魂劫
#define LONG_ZHENG_HU_DOU     5  // 2011年01月20日 龙争虎斗
#define XUE_LONG_CHU_YUAN     6  // 2011年04月28日 血龙出渊
#define BA_SHU_FENG_YUN       7  // 2011年05月19日 巴蜀风云
#define YI_DAI_ZONG_SHI       8  // 2011年11月16日 一代宗师
#define ZHU_HUO_LIAO_TIAN     9  // 2012年04月19日 烛火燎天
#define ER_HAI_JING_LONG      10 // 2012年07月12日 洱海惊龙
#define RI_YUE_MING_ZUN       11 // 2012年11月22日 日月明尊
#define AN_SHI_ZHI_LUAN       12 // 2013年06月08日 安史之乱
#define LUAN_SHI_CHANG_AN     13 // 2013年09月16日 乱世长安
#define XUE_ZHAN_TIAN_CE      14 // 2013年11月21日 血战天策
#define ZHU_LU_ZHONG_YUAN     15 // 2014年04月23日 逐鹿中原
#define DIAO_ZHUO_BAN         16 // 2014年07月21日 雕琢版
#define CANG_LONG_XUE_CHENG   17 // 2014年11月24日 苍雪龙城
#define XIA_YI_QIAN_KUN       18 // 2015年04月20日 侠义乾坤
#define JIAN_DAN_QIN_XIN      19 // 2015年10月26日 剑胆琴心
#define ZHUANG_ZHI_LING_YUN   20 // 2016年04月15日 壮志凌云
#define FENG_GU_BA_DAO        21 // 2016年10月17日 风骨霸刀
#define RI_YUE_LING_KONG      22 // 2017年04月20日 日月凌空
#define REMAKE                23 // 2017年12月29日 重制版
#define CHANG_FENG_PO_XIAO    24 // 2018年06月21日 长风破晓
#define SHI_WAI_PENG_LAI      25 // 2018年12月21日 世外蓬莱
#define NU_HAI_ZHENG_FENG     26 // 2019年06月20日 怒海争锋
#define LING_XUE_CANG_FENG    27 // 2019年10月15日 凌雪藏锋
#define JIE_LU_ZAI_JIANG_HU   28 // 2020年05月22日 结庐在江湖
#define TONG_ZHU_SHAN_SHUI_JU 28 // 2020年07月17日 同筑山水居
#define FENG_TIAN_ZHENG_DAO   29 // 2020年10月15日 奉天证道
#define BAI_DI_FENG_YUN       30 // 2021年04月22日 白帝风云
#define BEI_TIAN_YAO_ZONG     31 // 2021年10月28日 北天药宗

/*------------------ JX3DPS - Version ------------------*/

#define AUX_STR(JX3DPS_STR) #JX3DPS_STR
#define STR(JX3DPS_STR)     AUX_STR(JX3DPS_STR)

#define VERSION_MAJOR 3
#define VERSION_MINOR BEI_TIAN_YAO_ZONG

namespace JX3DPS {

const char *const VERSION =
    STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH) "." STR(VERSION_TWEAK);
const char *const BRANCH = STR(VERSION_BRANCH);
const char *const NAME = STR(PROJECT_NAME);


} // namespace JX3DPS

#endif
