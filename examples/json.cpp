#include <iostream>
#include <limits>
#include <cassert>
#include <ostream>

#include "triemap/triemap.h"
#include "triemap/io/json.h"

// Data element
struct Data
{
    Data(char d)
      : value(d)
    {}

    char value;
};

std::ostream&
operator<<(std::ostream& os, const Data& d)
{
    os << d.value;
    return os;
}


template<>
struct O3::io::json::traits<Data>
{
    template<class CharT, class Traits>
    static inline void print(std::basic_ostream<CharT, Traits>& os, const Data& t)
    {
        os << O3::io::json::quoted(t);
    }
    static inline const char* dtag()
    {
        return "data";
    }
};


// Collections of char data elements addressed by string prefixes.
using orepo = O3::collection::otriemap<Data, std::string, std::string>;

int
main(int, char*[])
{
    orepo o;

    std::cout << "\n\nOrdered empty triemap as a JSON-like object.\n" << O3::io::json::like(o) << std::endl;
    std::cout << "\n\nOrdered empty triemap as a proper JSON object.\n" << O3::io::json::proper(o) << std::endl;
    std::cout << "\n\nOrdered empty triemap as a D3 JSON object.\n" << O3::io::json::d3(o) << std::endl;

    o.insert('0');

    //o.insert('A', "a");
    o.insert('B', "b");

    o.insert('C', "a", "c");
    o.insert('D', "a", "d");
    o.insert('E', "b", "e");
    o.insert('F', "b", "f");

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

    std::cout << "\n\nOrdered triemap as a JSON-like object.\n" << O3::io::json::like(o) << std::endl;
    std::cout << "\n\nOrdered triemap as a proper JSON object.\n" << O3::io::json::proper(o) << std::endl;
    std::cout << "\n\nOrdered triemap as a D3 JSON object.\n" << O3::io::json::d3(o) << std::endl;

    std::cout << std::endl;
    return 0;
}
