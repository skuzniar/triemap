/*
 Copyright (c) 2022, Slawomir Kuzniar.
 Distributed under the MIT License (http://opensource.org/licenses/MIT).
*/

#ifndef O3_IO_JSON_DOT_H
#define O3_IO_JSON_DOT_H

#include <ostream>
#include <iomanip>
#include <utility>
#include <typeinfo>

#include "triemap/triemap.h"

namespace O3 {
namespace io {
namespace json {

namespace detail {

// Helper functions to manage formatting
inline int
indidx()
{
    static int i = std::ios_base::xalloc();
    return i;
}

inline int
fmtidx()
{
    static int i = std::ios_base::xalloc();
    return i;
}

} // namespace detail

// Output new line, then output indentation
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
ind(std::basic_ostream<CharT, Traits>& os)
{
    os << '\n';
    for (int i = 0; i < os.iword(detail::indidx()); ++i)
        os << ' ';
    return os;
}

// Output new line, then increase and output indentation
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
inc(std::basic_ostream<CharT, Traits>& os)
{
    os.iword(detail::indidx()) += 2;
    return os << ind;
}

// Output new line, then decrease and output indentation
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
dec(std::basic_ostream<CharT, Traits>& os)
{
    os.iword(detail::indidx()) -= 2;
    return os << ind;
}

// Clear indentation
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
clr(std::basic_ostream<CharT, Traits>& os)
{
    os.iword(detail::indidx()) = 0;
    return os;
}

// Conditionally print comma followed by new line and indentation, then reset flag to false
class cif
{
    bool& m_flag;

public:
    explicit cif(bool& flag)
      : m_flag(flag)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const cif& x)
    {
        if (x.m_flag)
            os << ',' << ind;
        x.m_flag = false;
        return os;
    }
};

// Unconditionally print comma followed by new line and indentation
class cin
{
public:
    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const cin& x)
    {
        os << ',' << ind;
        return os;
    }
};

enum class kind
{
    none,
    like = none,
    proper,
    d3
};

// Format flag access
template<class CharT, class Traits>
kind
fmt(std::basic_ostream<CharT, Traits>& os)
{
    return static_cast<kind>(os.iword(detail::fmtidx()));
}

template<class CharT, class Traits>
void
fmt(std::basic_ostream<CharT, Traits>& os, kind k)
{
    os.iword(detail::fmtidx()) = static_cast<int>(k);
}

namespace detail {

// Print element with enclosing quotes
template<typename E>
class quoted_wrapper
{
    const E& m_e;

    template<class CharT, class Traits>
    void print(std::basic_ostream<CharT, Traits>& os) const
    {
        os << '"' << m_e << '"';
    }

public:
    explicit quoted_wrapper(const E& e)
      : m_e(e)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quoted_wrapper& x)
    {
        x.print(os);
        return os;
    }
};

} // namespace detail

template<typename E>
inline detail::quoted_wrapper<E>
quoted(const E& e)
{
    return detail::quoted_wrapper<E>(e);
}

// Data type traits
template<typename T>
struct traits
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, const T& t)
    {
        os << t;
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

// Data type traits specialization for types that must be quoted
template<>
struct traits<std::string>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, const std::string& t)
    {
        os << quoted(t);
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

template<>
struct traits<char>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, char t)
    {
        os << quoted(t);
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

template<>
struct traits<const char*>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, const char* t)
    {
        os << quoted(t);
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

template<std::size_t N>
struct traits<char[N]>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, const char (&t)[N])
    {
        os << __LINE__ << ' ' << "Specialized t=" << typeid(t).name() << '<' << t << '>';
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

// Data type traits specialization for bool
template<>
struct traits<bool>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, bool t)
    {
        os << (t ? "true" : "false");
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

// JSON format flavours

namespace detail {

// Print triemap collection as a JSON-like object
template<typename TM>
class json_like
{
    const TM& m_tmap;

    template<class CharT, class Traits>
    void print(std::basic_ostream<CharT, Traits>& os) const
    {
        bool comma = false;

        m_tmap.traverse_dfs(
            [&](const auto& n, auto&&... qs) {
                os << cif(comma);
                if constexpr (sizeof...(qs) > 0) {
                    (os << ... << qs) << ':';
                }

                os << '{' << inc;

                if (n) {
                    os << traits<typename TM::data_type>::dtag() << ':';
                    traits<typename TM::data_type>::print(os, *n);
                    comma = true;
                } else {
                    comma = false;
                }

                return true;
            },
            [&](const auto&, auto&&...) {
                os << dec << '}';
                comma = true;
                return true;
            });
    }

public:
    explicit json_like(const TM& tmap)
      : m_tmap(tmap)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const json_like& x)
    {
        fmt(os, kind::like);
        x.print(os);
        return os;
    }
};

// Print triemap collection as a proper JSON object
template<typename TM>
class json_proper
{
    const TM& m_tmap;

    template<class CharT, class Traits>
    void print(std::basic_ostream<CharT, Traits>& os) const
    {
        bool comma = false;

        m_tmap.traverse_dfs(
            [&](const auto& n, auto&&... qs) {
                os << cif(comma);
                if constexpr (sizeof...(qs) > 0) {
                    os << quoted(qs...) << ':';
                }

                os << '{' << inc;

                if (n) {
                    os << quoted(traits<typename TM::data_type>::dtag()) << ':';
                    traits<typename TM::data_type>::print(os, *n);
                    comma = true;
                } else {
                    comma = false;
                }

                return true;
            },
            [&](const auto&, auto&&...) {
                os << dec << '}';
                comma = true;
                return true;
            });
    }

public:
    explicit json_proper(const TM& tmap)
      : m_tmap(tmap)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const json_proper& x)
    {
        fmt(os, kind::proper);
        x.print(os);
        return os;
    }
};

// Print triemap collection as a proper JSON object suitable for D3 visualization
template<typename TM>
class json_d3
{
    const TM& m_tmap;

    template<class CharT, class Traits, typename Node, typename Prefix>
    void print(std::basic_ostream<CharT, Traits>& os, const Node& n, const Prefix& p, bool first = true) const
    {
        bool comma = !first;

        os << cif(comma) << '{' << inc << quoted("type") << ':' << quoted(typeid(p).name()) << cin() << quoted("name")
           << ':' << quoted(p);
        comma = true;

        if (n) {
            os << cif(comma) << quoted(traits<typename TM::data_type>::dtag()) << ':';
            traits<typename TM::data_type>::print(os, *n);
            comma = !n.leaf();
        }

        if (!n.leaf()) {
            os << cif(comma) << quoted("children") << ':' << '[' << ind;

            first = true;
            n.traverse_level([&](const auto& sn, const auto& sp) {
                print(os, sn, sp, first);
                first = false;
                return true;
            });
            os << ind << ']';
        }
        os << dec << '}';
    }

    template<class CharT, class Traits, typename Node>
    void print(std::basic_ostream<CharT, Traits>& os, const Node& n) const
    {
        os << '{' << inc;

        bool comma = false;
        if (n) {
            os << cif(comma) << quoted(traits<typename TM::data_type>::dtag()) << ':';
            traits<typename TM::data_type>::print(os, *n);
            comma = !n.leaf();
        }

        if (!n.leaf()) {
            os << cif(comma) << quoted("children") << ':' << '[' << ind;

            bool first = true;
            n.traverse_level([&](const auto& sn, const auto& sp) {
                print(os, sn, sp, first);
                first = false;
                return true;
            });
            os << ind << ']';
        }
        os << dec << '}';
    }

public:
    explicit json_d3(const TM& tmap)
      : m_tmap(tmap)
    {}

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const json_d3& x)
    {
        fmt(os, kind::d3);
        x.print(os, x.m_tmap);
        return os;
    }
};

} // namespace detail

// Triemap collection as a JSON-like object manipulator
template<typename TM>
inline detail::json_like<TM>
like(const TM& tm)
{
    return detail::json_like<TM>(tm);
}

// Triemap collection as proper JSON object manipulator
template<typename TM>
inline detail::json_proper<TM>
proper(const TM& tm)
{
    return detail::json_proper<TM>(tm);
}

// Triemap collection as a proper JSON object manipulator suitable for D3 visualization
template<typename TM>
inline detail::json_d3<TM>
d3(const TM& tm)
{
    return detail::json_d3<TM>(tm);
}

// Data type traits specialization for ordered and unordered triemap
template<typename DATA, typename PFIX, typename... PFIXS>
struct traits<O3::collection::otriemap<DATA, PFIX, PFIXS...>>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>&                    os,
                             const O3::collection::otriemap<DATA, PFIX, PFIXS...>& t)
    {
        switch (fmt(os)) {
            case kind::like:
                os << like(t);
                break;
            case kind::proper:
                os << proper(t);
                break;
            case kind::d3:
                os << d3(t);
                break;
        };
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

template<typename DATA, typename PFIX, typename... PFIXS>
struct traits<O3::collection::utriemap<DATA, PFIX, PFIXS...>>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>&                    os,
                             const O3::collection::utriemap<DATA, PFIX, PFIXS...>& t)
    {
        switch (fmt(os)) {
            case kind::like:
                os << like(t);
                break;
            case kind::proper:
                os << proper(t);
                break;
            case kind::d3:
                os << d3(t);
                break;
        };
    }
    static inline const char* dtag()
    {
        return "data";
    }
};

} // namespace json
} // namespace io
} // namespace O3

// Ordered and unordered triemap output operator
template<typename CharT, typename Traits, typename DATA, typename PFIX, typename... PFIXS>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const O3::collection::otriemap<DATA, PFIX, PFIXS...>& t)
{
    os << O3::io::json::like(t);
    return os;
}

template<typename CharT, typename Traits, typename DATA, typename PFIX, typename... PFIXS>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const O3::collection::utriemap<DATA, PFIX, PFIXS...>& t)
{
    os << O3::io::json::like(t);
    return os;
}

#endif
