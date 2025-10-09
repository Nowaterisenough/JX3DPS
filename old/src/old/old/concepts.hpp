#ifndef JX3DPS_CONCEPTS_HPP
#define JX3DPS_CONCEPTS_HPP

#include <concepts>

template <typename T>
concept HasFixedDamage = requires {
    {
        T::fixed_damage
    } -> std::same_as<int &>;
};

template <typename T>
concept HasPhysicsAP = requires {
    {
        T::physics_ap
    } -> std::same_as<int &>;
};

template <typename T>
concept HasNeutralAP = requires {
    {
        T::neutral_ap
    } -> std::same_as<int &>;
};

template <typename T>
concept HasSolarAP = requires {
    {
        T::solar_ap
    } -> std::same_as<int &>;
};

template <typename T>
concept HasLunarAP = requires {
    {
        T::lunar_ap
    } -> std::same_as<int &>;
};

template <typename T>
concept HasPoisonAP = requires {
    {
        T::poison_ap
    } -> std::same_as<int &>;
};

template <typename T>
concept HasMagicAP = requires {
    {
        HasPoisonAP<T> | HasLunarAP<T> | HasSolarAP<T> | HasNeutralAP<T>
    };
};

template <typename T>
concept HasWeaponDamage = requires {
    {
        T::weapon_coffecient
    } -> std::same_as<int &>;
};

template <typename T>
concept HasDamage = requires {
    {
        HasFixedDamage<T> | HasPhysicsAP<T> | HasMagicAP<T> | HasWeaponDamage<T>
    };
};

template <typename T>
concept IsAoe = requires {
    {
        T::limit
    } -> std::same_as<int &>;
    requires T::limit > 1;
};

template <typename T>
concept IsDebuff = requires {
    {
        T::is_debuff
    } -> std::convertible_to<const bool>;
} && T::is_debuff;

template <typename T>
concept HasInterval = requires {
    {
        T::interval
    } -> std::same_as<int &>;
};

template <typename T>
concept IsHasted = requires {
    {
        T::interval
    } -> std::same_as<int &>;
};

template <typename T>
concept HasPrepare = requires {
    {
        T::prepare
    } -> std::same_as<int &>;
};

template <typename T>
concept HasCasting = requires {
    {
        T::casting
    } -> std::convertible_to<const bool>;
} && T::casting;

#endif // JX3DPS_CONCEPTS_HPP