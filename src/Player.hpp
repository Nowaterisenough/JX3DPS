#ifndef JX3DPS_PLAYER_HPP
#define JX3DPS_PLAYER_HPP

namespace JX3DPS {

template <typename T>
class Player
{

public:
    Player() = default;

    ~Player() = default;

    void AddBuff(jx3id_t id) { }

    bool HasBuff(jx3id_t id) { return false; }

    void CastSkill(jx3id_t id) { }

    T *AsDerived() { return static_cast<T *>(this); }
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_HPP