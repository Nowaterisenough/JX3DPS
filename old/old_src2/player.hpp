#ifndef JX3DPS_PLAYER_HPP
#define JX3DPS_PLAYER_HPP

#include "common.h"

namespace JX3DPS {

template <typename DerivedPlayer>
class Player
{

public:
    Player() = default;

    ~Player() = default;

    void AddBuff(int id) { }

    bool HasBuff(int id) { return false; }

    void CastSkill(int id, int level) { }

    DerivedPlayer *AsDerived() { return static_cast<DerivedPlayer *>(this); }
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_HPP