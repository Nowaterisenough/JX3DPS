/**
 * Project: 
 * File: Version.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-26 17:47:56
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_VERSION_H
#define JX3DPS_VERSION_H

namespace JX3DPS {

/*------------------ 剑网3 - 历代资料片 ------------------*/

/* 2009年08月28日 公测 */
#define OPEN_BATE             0

/* 2009年10月22日 破晓之路 */
#define PO_XIAO_ZHI_LU        1

/* 2010年02月08日 撼世禅劫 */
#define HAN_SHI_CHAN_JIE      2

/* 2010年05月26日 藏剑山庄 */
#define CANG_JIAN_SHAN_ZHUANG 3

/* 2010年10月28日 战魂劫 */
#define ZHAN_HUN_JIE          4

/* 2011年01月20日 龙争虎斗 */
#define LONG_ZHENG_HU_DOU     5

/* 2011年04月28日 血龙出渊 */
#define XUE_LONG_CHU_YUAN     6

/* 2011年05月19日 巴蜀风云 */
#define BA_SHU_FENG_YUN       7

/* 2011年11月16日 一代宗师 */
#define YI_DAI_ZONG_SHI       8

/* 2012年04月19日 烛火燎天 */
#define ZHU_HUO_LIAO_TIAN     9

/* 2012年07月12日 洱海惊龙 */
#define ER_HAI_JING_LONG      10

/* 2012年11月22日 日月明尊 */
#define RI_YUE_MING_ZUN       11

/* 2013年06月08日 安史之乱 */
#define AN_SHI_ZHI_LUAN       12

/* 2013年09月16日 乱世长安 */
#define LUAN_SHI_CHANG_AN     13

/* 2013年11月21日 血战天策 */
#define XUE_ZHAN_TIAN_CE      14

/* 2014年04月23日 逐鹿中原 */
#define ZHU_LU_ZHONG_YUAN     15

/* 2014年07月21日 雕琢版 */
#define DIAO_ZHUO_BAN         16

/* 2014年11月24日 苍雪龙城 */
#define CANG_LONG_XUE_CHENG   17

/* 2015年04月20日 侠义乾坤 */
#define XIA_YI_QIAN_KUN       18

/* 2015年10月26日 剑胆琴心 */
#define JIAN_DAN_QIN_XIN      19

/* 2016年04月15日 壮志凌云 */
#define ZHUANG_ZHI_LING_YUN   20

/* 2016年07月14日 百家争鸣 */
#define BAI_JIA_ZHENG_MING    21

/* 2016年10月17日 风骨霸刀 */
#define FENG_GU_BA_DAO        22

/* 2017年04月20日 日月凌空 */
#define RI_YUE_LING_KONG      23

/* 2017年12月29日 重制版 */
#define REMAKE                24

/* 2018年06月21日 长风破晓 */
#define CHANG_FENG_PO_XIAO    25

/* 2018年12月21日 世外蓬莱 */
#define SHI_WAI_PENG_LAI      26

/* 2019年06月20日 怒海争锋 */
#define NU_HAI_ZHENG_FENG     27

/* 2019年10月15日 凌雪藏锋 */
#define LING_XUE_CANG_FENG    28

/* 2020年05月22日 结庐在江湖 */
#define JIE_LU_ZAI_JIANG_HU   29

/* 2020年07月17日 同筑山水居 */
#define TONG_ZHU_SHAN_SHUI_JU 30

/* 2020年10月15日 奉天证道 */
#define FENG_TIAN_ZHENG_DAO   31

/* 2020年11月30日 月满归乡 */
#define YUE_MAN_GUI_XIANG     32

/* 2021年04月22日 白帝风云 */
#define BAI_DI_FENG_YUN       33

/* 2021年10月28日 北天药宗 */
#define BEI_TIAN_YAO_ZONG     34

/* 2022年04月28日 江湖无限 */
#define JIANG_HU_WU_XIAN      35

/* 2022年10月27日 横刀断浪 */
#define HENG_DAO_DUAN_LANG    36

/* 2023年04月27日 群侠万变 */
#define QUN_XIA_WAN_BIAN      37

/*------------------ 版本号 ------------------*/

/* 剑网3 - 等级 */
#define JX3_LEVEL_VERSION     120

/* 剑网3 - 资料片 */
#define JX3_VERSION           QUN_XIA_WAN_BIAN

/* JX3DPS - 主版本号 */
#define VERSION_MAJOR         0

/* JX3DPS - 次版本号 */
#define VERSION_MINOR         JX3_VERSION

#define AUX_STR(_STR)         #_STR
#define STR(_STR)             AUX_STR(_STR)

const char *const VERSION =
    STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH) "." STR(VERSION_TWEAK);
const char *const BRANCH = STR(VERSION_BRANCH);
const char *const NAME   = STR(PROJECT_NAME);

} // namespace JX3DPS

#endif // JX3DPS_VERSION_H