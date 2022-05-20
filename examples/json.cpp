#include <iostream>
#include <limits>
#include <cassert>
#include <ostream>

#include "triemap.h"

// Helper functions to manage indentation
namespace io {
namespace detail {
inline int
geti()
{
    static int i = std::ios_base::xalloc();
    return i;
}
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
}

// Decrease indentation
inline std::ostream&
dec(std::ostream& os)
{
    os.iword(detail::geti()) -= 2;
    return os;
}

}

// Print collection as a JSON object
template<typename R>
void
print(const R& r, std::ostream& os)
{
    bool closed = false;

    r.traverse_dfs(
        [&](const auto& n, auto&&... qs) {
            if (closed) {
                os << ',' << '\n' << io::ind;
            }
            if constexpr (sizeof...(qs) > 0) {
                (os << ... << qs) << ':';
            }

            os << '{' << '\n' << io::inc << io::ind;

            if (n) {
                os << "Data" << ':' << *n;
                closed = true;
            } else {
                closed = false;
            }

            return true;
        },
        [&](const auto& , auto&&...) {
            os << '\n' << io::dec << io::ind << '}';
            closed = true;
            return true;
        });
}

// Collections of char data elements addressed by string prefixes.
using orepo = O3::collection::otriemap<char, std::string, std::string>;
using urepo = O3::collection::utriemap<char, std::string, std::string>;

int
main(int, char*[])
{
    orepo o;
    urepo u;

    o.insert('0');
    u.insert('0');
    o.insert('A', "a");
    u.insert('A', "a");

    o.insert('B', "b");
    u.insert('B', "b");
    o.insert('C', "a", "c");
    u.insert('C', "a", "c");
    o.insert('D', "a", "d");
    u.insert('D', "a", "d");
    o.insert('E', "b", "e");
    u.insert('E', "b", "e");
    o.insert('F', "b", "f");
    u.insert('F', "b", "f");

    /* Tree has the following structure
     *
     *        0
     *      a/ \b
     *      /   \
     *     A     B
     *   c/ \d e/ \f
     *   /   \ /   \
     *  C    D E    F
     *
     */

    std::cout << "\n\nOrdered triemap.\n";
    print(o, std::cout);

    std::cout << "\n\nUnordered triemap.\n";
    print(u, std::cout);

    std::cout << std::endl;
    return 0;
}
