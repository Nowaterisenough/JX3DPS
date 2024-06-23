```cpp
#include <iostream>

template <typename T>
concept IsDynamic = requires(T t) {
    {
        t.damage_add
    } -> std::convertible_to<int>;
};

constexpr int DAMAGE = 1024;

template <typename T>
struct Coefficient
{
    constexpr static int value(const T &t)
    {
        if constexpr (IsDynamic<T>) {
            return std::max(T::value * DAMAGE * t.damage_add, DAMAGE * 2);
        }
        return std::max(T::value * DAMAGE, DAMAGE * 2);
    }
};

class SkillA
{
public:
    inline constexpr static int value = 3;
};

class SkillB
{
public:
    inline constexpr static int value      = 5;
    int                         damage_add = 11;
};

int main(int argc, char **argv)
{
    SkillA a;
    SkillB b;
    b.damage_add = 4;
    std::cout << Coefficient<SkillA>::value(a) << std::endl;
    std::cout << Coefficient<SkillB>::value(b) << std::endl;

    return 0;
}
```

```assembly
// x86-64 clang 18.1.0 -O3 -std=c++20
main:                                   # @main
        push    r14
        push    rbx
        push    rax
        mov     rdi, qword ptr [rip + std::cout@GOTPCREL]
        mov     esi, 3072
        call    std::ostream::operator<<(int)@PLT
        mov     rcx, qword ptr [rax]
        mov     rcx, qword ptr [rcx - 24]
        mov     rbx, qword ptr [rax + rcx + 240]
        test    rbx, rbx
        je      .LBB0_9
        cmp     byte ptr [rbx + 56], 0
        je      .LBB0_3
        movzx   ecx, byte ptr [rbx + 67]
        jmp     .LBB0_4
.LBB0_3:
        mov     rdi, rbx
        mov     r14, rax
        call    std::ctype<char>::_M_widen_init() const@PLT
        mov     rax, qword ptr [rbx]
        mov     rdi, rbx
        mov     esi, 10
        call    qword ptr [rax + 48]
        mov     ecx, eax
        mov     rax, r14
.LBB0_4:
        movsx   esi, cl
        mov     rdi, rax
        call    std::ostream::put(char)@PLT
        mov     rdi, rax
        call    std::ostream::flush()@PLT
        mov     rdi, qword ptr [rip + std::cout@GOTPCREL]
        mov     esi, 20480
        call    std::ostream::operator<<(int)@PLT
        mov     rcx, qword ptr [rax]
        mov     rcx, qword ptr [rcx - 24]
        mov     rbx, qword ptr [rax + rcx + 240]
        test    rbx, rbx
        je      .LBB0_9
        cmp     byte ptr [rbx + 56], 0
        je      .LBB0_7
        movzx   ecx, byte ptr [rbx + 67]
        jmp     .LBB0_8
.LBB0_7:
        mov     rdi, rbx
        mov     r14, rax
        call    std::ctype<char>::_M_widen_init() const@PLT
        mov     rax, qword ptr [rbx]
        mov     rdi, rbx
        mov     esi, 10
        call    qword ptr [rax + 48]
        mov     ecx, eax
        mov     rax, r14
.LBB0_8:
        movsx   esi, cl
        mov     rdi, rax
        call    std::ostream::put(char)@PLT
        mov     rdi, rax
        call    std::ostream::flush()@PLT
        xor     eax, eax
        add     rsp, 8
        pop     rbx
        pop     r14
        ret
.LBB0_9:
        call    std::__throw_bad_cast()@PLT
```

```assembly
// x86-64 gcc 14.1 -O3 -std=c++20
std::basic_ostream<char, std::char_traits<char> >& std::endl<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&) [clone .isra.0] [clone .cold]:
main:
        sub     rsp, 8
        mov     esi, 3072
        mov     edi, OFFSET FLAT:std::cout
        call    std::basic_ostream<char, std::char_traits<char> >::operator<<(int)
        mov     rdi, rax
        call    std::basic_ostream<char, std::char_traits<char> >& std::endl<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&) [clone .isra.0]
        mov     esi, 20480
        mov     edi, OFFSET FLAT:std::cout
        call    std::basic_ostream<char, std::char_traits<char> >::operator<<(int)
        mov     rdi, rax
        call    std::basic_ostream<char, std::char_traits<char> >& std::endl<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&) [clone .isra.0]
        xor     eax, eax
        add     rsp, 8
        ret
```
