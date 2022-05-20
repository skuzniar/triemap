/*
 Copyright (c) 2022, Slawomir Kuzniar.
 Distributed under the MIT License (http://opensource.org/licenses/MIT).
*/

#ifndef O3_IO_JSON_DOT_H
#define O3_IO_JSON_DOT_H

#include <ostream>
#include <iomanip>

// Helper functions to manage indentation
namespace O3 {
namespace io {
namespace detail {
inline int
geti()
{
    static int i = std::ios_base::xalloc();
    return i;
}

// Output indentation
inline std::ostream&
ind(std::ostream& os)
{
    for (int i = 0; i < os.iword(detail::geti()); ++i)
        os << ' ';
    return os;
}

// Increase indentation
inline std::ostream&
inc(std::ostream& os)
{
    os.iword(detail::geti()) += 2;
    return os;

    ///
    os << std::setw(2);
}

// Decrease indentation
inline std::ostream&
dec(std::ostream& os)
{
    os.iword(detail::geti()) -= 2;
    return os;
}

// Print triemap collection as a json-like object
template<typename TM>
class json_like
{
    const TM& m_tm;

    template<class CharT, class Traits>
    void print(std::basic_ostream<CharT, Traits>& os) const
    {
        bool closed = false;

        m_tm.traverse_dfs(
            [&](const auto& n, auto&&... qs) {
                if (closed) {
                    os << ',' << '\n' << io::detail::ind;
                }
                if constexpr (sizeof...(qs) > 0) {
                    (os << ... << qs) << ':';
                }

                os << '{' << '\n' << io::detail::inc << io::detail::ind;

                if (n) {
                    os << "Data" << ':' << *n;
                    closed = true;
                } else {
                    closed = false;
                }

                return true;
            },
            [&](const auto&, auto&&...) {
                os << '\n' << io::detail::dec << io::detail::ind << '}';
                closed = true;
                return true;
            });
    }

public:
    explicit json_like(const TM& tm)
      : m_tm(tm)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const json_like& x)
    {
        x.print(os);
        return os;
    }
};

}

namespace json {

// Triemap collection as a json-like object manipulator
template<typename TM>
inline detail::json_like<TM>
like(const TM& tm)
{
    return detail::json_like<TM>(tm);
}

}

}
}

#endif
