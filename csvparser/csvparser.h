#pragma once

#include <istream>
#include <stdexcept>
#include <charconv>
#include <type_traits>
#include <algorithm>
#include <locale>
#include <ciso646>
#include <memory>
#include <utility>

namespace csvparser
{

struct empty_translator
{
    void operator()(std::string& from, std::string& to) const
        noexcept(std::is_nothrow_swappable_v<std::string>)
    {
        swap(from, to);
    }

    std::locale loc;
};

struct default_translator : empty_translator
{
    using empty_translator::operator();

    template<class T>
    auto operator()(std::string const& from, T& to) const
        -> std::enable_if_t<std::is_arithmetic_v<T>>
    {
        auto not_ws = [this](char ch) { return not std::isspace(ch, loc); };
        auto first = std::find_if(begin(from), end(from), not_ws);
        if (first == end(from))
            throw std::invalid_argument{ "no convertible content" };

        auto ed = std::to_address(end(from));
        if (auto [ptr, ec] = std::from_chars(std::to_address(first), ed, to);
            ec != std::errc())
            throw std::invalid_argument{ "failed to convert" };
        else if (std::any_of(ptr, ed, not_ws))
            throw std::invalid_argument{ "trailing unconvertible content" };
    }
};

namespace detail
{

template<class Cvt>
class parser
{
    template<char Delim>
    static void parse_field(std::istream& in, std::string& content)
    {
        content.clear();
        switch (in.get())
        {
        case Delim: break;
        case '"': return quoted_field<Delim>(in, content);
        default: return bare_field<Delim>(in.unget(), content);
        }
    }

    template<char Delim>
    static void quoted_field(std::istream& in, std::string& content)
    {
        bool next_to_dq = false;
        for (char ch;;)
        {
            if (!in.get(ch))
                throw std::invalid_argument{ "unexpected EOF" };
            if (std::exchange(next_to_dq, false))
            {
                if (ch == '"')
                    content.push_back(ch);
                else if (ch == Delim)
                    break;
                else
                    throw std::invalid_argument{ "invalid quoted field" };
            }
            else
            {
                if (ch == '"')
                    next_to_dq = true;
                else
                    content.push_back(ch);
            }
        }
    }

    template<char Delim>
    static void bare_field(std::istream& in, std::string& content)
    {
        for (char ch;;)
        {
            if (!in.get(ch))
                throw std::invalid_argument{ "unexpected EOF" };
            if (ch == Delim)
                break;
            content.push_back(ch);
        }
    }

    template<class... T, size_t... I>
    static void getrecord(std::index_sequence<I...>, std::istream& in,
                          T&... arg)
    {
        using traits = std::istream::traits_type;

        Cvt tr{ in.getloc() };
        std::string buffer;
        ((parse_field<(I + 1 != sizeof...(I) ? ',' : '\n')>(in, buffer),
          tr(buffer, arg)),
         ...);
        if (traits::not_eof(in.get()))
            in.unget();
    }

  public:
    template<class... T>
    static void getrecord(std::istream& in, T&... arg)
    {
        getrecord(std::make_index_sequence<sizeof...(T)>(), in, arg...);
    }
};

}

template<class Cvt = default_translator, class... T>
inline auto getrecord(std::istream& in, T&... arg) -> std::istream&
{
    detail::parser<Cvt>::getrecord(in, arg...);
    return in;
}

}
