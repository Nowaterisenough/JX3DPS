// #include <jx3dps.h>

// int main(int argc, char *argv[])
// {
//     char out[1024];
//     jx3dps_simulate("test", out);
//     return 0;
// }

// #include <iostream>
// #include <memory>
// #include <typeinfo>

// #include "proxy.h"

// template <class F>
// concept ReflectionApplicable = requires(pro::proxy<F> p) {
//     {
//         p.reflect()
//     };
// };

// class RttiReflection
// {
// public:
//     template <class P>
//     constexpr explicit RttiReflection(std::in_place_type_t<P>) : type_(typeid(P))
//     {
//     }

// const char *GetName() const noexcept { return type_.name(); }

// private:
//     const std::type_info &type_;
// };

// struct TraitsReflection
// {
// public:
//     template <class P>
//     constexpr explicit TraitsReflection(std::in_place_type_t<P>) :
//         is_default_constructible_(std::is_default_constructible_v<P>),
//         is_copy_constructible_(std::is_copy_constructible_v<P>),
//         is_nothrow_move_constructible_(std::is_nothrow_move_constructible_v<P>),
//         is_nothrow_destructible_(std::is_nothrow_destructible_v<P>),
//         is_trivial_(std::is_trivial_v<P>)
//     {
//     }

// bool is_default_constructible_;
// bool is_copy_constructible_;
// bool is_nothrow_move_constructible_;
// bool is_nothrow_destructible_;
// bool is_trivial_;
// };

// PRO_DEF_MEMBER_DISPATCH(Run, void());

// PRO_DEF_FACADE(DefaultFacade);
// static_assert(!ReflectionApplicable<DefaultFacade>);

// PRO_DEF_FACADE(TestRttiFacade, PRO_MAKE_DISPATCH_PACK(Run), pro::trivial_ptr_constraints,
// RttiReflection); static_assert(ReflectionApplicable<TestRttiFacade>);

// PRO_DEF_FACADE(TestTraitsFacade, PRO_MAKE_DISPATCH_PACK(), pro::relocatable_ptr_constraints,
// TraitsReflection); static_assert(ReflectionApplicable<TestTraitsFacade>);

// class Skills
// {
// public:
//     Skills()                   = default;
//     Skills(const Skills &)     = default;
//     Skills(Skills &&) noexcept = default;
//     ~Skills() noexcept         = default;
// };

// class 无我无剑
// {
// public:
//     void Run() {}
// };

// class 八荒归元
// {
// public:
//     void Run() {}
// };

// int main(int argc, char *argv[])
// {
    // int                        foo = 123;
    // pro::proxy<TestRttiFacade> p   = &foo;
    // std::cout << p.reflect().GetName() << " " << typeid(int *).name() << std::endl;

    // 无我无剑          s;
    // 八荒归元          s2;
    // pro::proxy<TestRttiFacade> p2(&s);
    // pro::proxy<TestRttiFacade> p3(&s2);

    // // pro::proxy<TestRttiFacade> p2 = Skills();
    // std::cout << p3.reflect().GetName() << sizeof("无我无剑");

    // std::cout << " " << typeid(s).name() << std::endl;
// }

#include <iostream>

#include "concepts.h"
#include "jx3dps.h"

class A
{
public:
    int physics_ap = 0;
};

class B
{
public:
    thread_local inline constexpr static int physics_ap[5] = { 0 };
};

class C
{
public:
    thread_local inline constexpr static int physics_ap = 0;
};

template <typename T>
void Fun(T t) noexcept
{
    if constexpr (JX3DPS::IsArrayAP<T>) {
        std::cout << "MultiCof" << std::endl;
    } else {
        std::cout << "Not MultiCof" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::cout << jx3dps_jx3_version() << std::endl;
    Fun(A());
    Fun(B());
    Fun(C());

    return 0;
}