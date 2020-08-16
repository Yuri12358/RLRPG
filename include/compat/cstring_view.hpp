#ifndef RLPRG_COMPAT_CSTRING_VIEW_HPP
#define RLPRG_COMPAT_CSTRING_VIEW_HPP

#include<stdexcept>
#include<string>
#include<string_view>

// A string_view variant that maintains an additional invariant that the
// referenced character sequence is null-terminated.
// Inspired by p1402r0.

namespace compat {
    static constexpr struct null_terminated_t {} null_terminated {};

    template <class charT, class traits = std::char_traits<charT>>
    class basic_cstring_view : public std::basic_string_view<charT, traits>
    {
    public:
        using string_view_type       = std::basic_string_view<charT, traits>;
        using traits_type            = typename string_view_type::traits_type;
        using value_type             = typename string_view_type::value_type;
        using pointer                = typename string_view_type::pointer;
        using const_pointer          = typename string_view_type::const_pointer;
        using reference              = typename string_view_type::reference;
        using const_reference        = typename string_view_type::const_reference;
        using const_iterator         = typename string_view_type::const_iterator;
        using iterator               = typename string_view_type::iterator;
        using const_reverse_iterator = typename string_view_type::const_reverse_iterator;
        using reverse_iterator       = typename string_view_type::reverse_iterator;
        using size_type              = typename string_view_type::size_type;
        using difference_type        = typename string_view_type::difference_type;
        using string_view_type::npos;

        constexpr basic_cstring_view() noexcept
            : string_view_type(&null, 0)
        {}
        constexpr basic_cstring_view(const basic_cstring_view&) noexcept = default;
        basic_cstring_view& operator=(const basic_cstring_view&) noexcept = default;
        // cppcheck-suppress noExplicitConstructor
        constexpr basic_cstring_view(const charT* str)
            : string_view_type(str)
        {}
        constexpr basic_cstring_view(null_terminated_t, const charT* str, size_type len)
            : string_view_type(str, len)
        {}
        constexpr basic_cstring_view(null_terminated_t, string_view_type sv)
            : string_view_type(sv.data(), sv.size())
        {}

        void remove_suffix(size_type n) = delete;

        constexpr void swap(basic_cstring_view& s) noexcept
        {
            string_view_type::swap(*this, s);
        }

        using string_view_type::substr;
        constexpr basic_cstring_view substr(size_type pos = 0) const
        {
            return pos <= string_view_type::size()
                ? basic_cstring_view(null_terminated, string_view_type::data() + pos, string_view_type::size() - pos)
                : throw std::out_of_range("");
        }

        constexpr const charT* c_str() const noexcept
        {
            return string_view_type::data();
        }

        // Non-standard, workaround for missing support in std::string
        template <class Allocator>
        // cppcheck-suppress noExplicitConstructor
        basic_cstring_view(const std::basic_string<charT, traits, Allocator>& str) noexcept
            : string_view_type(str.data(), str.size())
        {}

    private:
        static const charT null { 0 };
    };

    template <class charT, class traits>
    const charT basic_cstring_view<charT, traits>::null;

    typedef basic_cstring_view<char> cstring_view;
    typedef basic_cstring_view<char16_t> u16cstring_view;
    typedef basic_cstring_view<char32_t> u32cstring_view;
    typedef basic_cstring_view<wchar_t> wcstring_view;

    inline namespace literals {
        inline namespace cstring_view_literals {
            constexpr cstring_view operator "" _csv(const char* str, std::size_t len) noexcept {
                return cstring_view{ null_terminated, str, len };
            }
        }
    }
}

#endif // RLPRG_COMPAT_CSTRING_VIEW_HPP
