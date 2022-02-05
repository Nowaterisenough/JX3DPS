/**
 * @Description : 太虚剑意
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 13:17:36
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\TaiXuJianYi.h
 */

#ifndef TAIXUJIANYI_H
#define TAIXUJIANYI_H

#include "Common/ConstVal.h"
#include "Core/Player.h"

namespace JX3DPS {

/* Buff - 太虚剑意 */
constexpr Id_t BUF_ZI_QI_DONG_LAI     = 2101;
constexpr Id_t BUF_XUAN_MEN           = 2102;
constexpr Id_t BUF_SUI_XING_CHEN      = 2103;
constexpr Id_t BUF_SUI_XING_CHEN_STD  = 2104;
constexpr Id_t BUF_SUI_XING_CHEN_STD2 = 2105;
constexpr Id_t BUF_SHENG_TAI_JI_STD   = 2106;
constexpr Id_t BUF_TUN_RI_YUE_STD     = 2107;
constexpr Id_t BUF_YUN_ZHONG_JIAN_C   = 2108;
constexpr Id_t BUF_YUN_ZHONG_JIAN_J   = 2109;
constexpr Id_t BUF_YUN_ZHONG_JIAN_Y   = 2110;
constexpr Id_t BUF_CHI_YING           = 2111;
constexpr Id_t BUF_QI_SHENG           = 2112;
constexpr Id_t BUF_FENG_SHI           = 2113;

/* TBuff - 太虚剑意 */
constexpr Id_t TBUF_DIE_REN        = 2201;
constexpr Id_t TBUF_REN_JIAN_HE_YI = 2202;

/* 技能 - 太虚剑意 */
constexpr Id_t SKI_WU_WO_WU_JIAN     = 2301;
constexpr Id_t SKI_BA_HUANG_GUI_YUAN = 2302;
constexpr Id_t SKI_SAN_HUAN_TAO_YUE  = 2303;
constexpr Id_t SKI_REN_JIAN_HE_YI    = 2304;
constexpr Id_t SKI_SUI_XING_CHEN     = 2305;
constexpr Id_t SKI_SHENG_TAI_JI      = 2306;
constexpr Id_t SKI_TUN_RI_YUE        = 2307;
constexpr Id_t SKI_ZI_QI_DONG_LAI    = 2308;
constexpr Id_t SKI_SAN_CHAI_JIAN_FA  = 2309;

/* 奇穴 - 太虚剑意 */
constexpr Id_t TAL_CUO_RUI        = 2401;
constexpr Id_t TAL_XIN_GU         = 2402;
constexpr Id_t TAL_TONG_GEN       = 2403;
constexpr Id_t TAL_SHEN_MAI       = 2404;
constexpr Id_t TAL_KUN_WU         = 2405;
constexpr Id_t TAL_BAI_HONG       = 2406;
constexpr Id_t TAL_HUA_SAN_QING   = 2407;
constexpr Id_t TAL_YUN_ZHONG_JIAN = 2408;
constexpr Id_t TAL_WU_YI          = 2409;
constexpr Id_t TAL_FENG_SHI       = 2410;
constexpr Id_t TAL_KUANG_GE       = 2411;
constexpr Id_t TAL_DIE_REN        = 2412;
constexpr Id_t TAL_QIE_YU         = 2413;
constexpr Id_t TAL_CHANG_SHENG    = 2414;
constexpr Id_t TAL_FU_YIN         = 2415;
constexpr Id_t TAL_HE_GUANG       = 2416;
constexpr Id_t TAL_SUI_WU         = 2417;
constexpr Id_t TAL_QI_SHENG       = 2418;
constexpr Id_t TAL_WU_YU          = 2419;
constexpr Id_t TAL_XUAN_MEN       = 2420;

namespace TaiXuJianYi {

class TaiXuJianYi : public Player
{
public:
    TaiXuJianYi();
    TaiXuJianYi(const TaiXuJianYi &player);
    ~TaiXuJianYi();
    TaiXuJianYi *Clone();
    TaiXuJianYi &operator=(const TaiXuJianYi &player);

    /* 橙武效果 */
    void SetGearSetCW(bool gearSetCW) override;

    /* 门派套装技能效果 */
    void SetGearSetClass(bool gearSetClass) override;

    /* 门派套装攻击效果 */
    void SetGearSetAttack(bool gearSetAttack) override;

    /* 气点 */
    void UpdateQidian(int qidian = 0);
    int GetQidian();

private:
    /* 初始化 */
    void InitClass();

    int m_qidian = 10;
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // TAIXUJIANYI_H
