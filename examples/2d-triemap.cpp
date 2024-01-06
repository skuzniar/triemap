#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <type_traits>
#include <utility>

#include "triemap/triemap.h"
#include "triemap/io/json.h"

// Data element
struct Data
{
    char value;

    explicit Data(char d)
      : value(d)
    {}

    bool operator == (const Data& oth) const
    {
        return value == oth.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Data& d)
    {
        os << d.value;
        return os;
    }
};

// Geographical dimansion
struct Continent
{
    std::string name;

    explicit Continent(std::string  name)
      : name(std::move(name))
    {}

    bool operator<(const Continent& oth) const
    {
        return name < oth.name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Continent& c)
    {
        os << c.name;
        return os;
    }
};

struct Country
{
    std::string name;

    explicit Country(std::string  name)
      : name(std::move(name))
    {}

    bool operator<(const Country& oth) const
    {
        return name < oth.name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Country& c)
    {
        os << c.name;
        return os;
    }
};

// Organizational dimention
struct Division
{
    std::string name;

    explicit Division(std::string  name)
      : name(std::move(name))
    {}

    bool operator<(const Division& oth) const
    {
        return name < oth.name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Division& d)
    {
        os << d.name;
        return os;
    }
};

struct Department
{
    std::string name;

    explicit Department(std::string  name)
      : name(std::move(name))
    {}

    bool operator<(const Department& oth) const
    {
        return name < oth.name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Department& d)
    {
        os << d.name;
        return os;
    }
};


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

// Trie map of <Geo, Org> domain key into Data
using OrgTrieMap    = O3::collection::otriemap<Data, Division, Department>;
using GeoOrgTrieMap = O3::collection::otriemap<OrgTrieMap, Continent, Country>;

// Global instances for simplicity
GeoOrgTrieMap GOTM;

int
main(int argc, char* argv[])
{
    bool verbose = argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v';


    GOTM.insert(OrgTrieMap(), Continent("Europe"), Country("Ukraine")).first->insert(Data('A'), Division("Sales"), Department("Retail"));
    GOTM.insert(OrgTrieMap(), Continent("Europe"), Country("Germany")).first->insert(Data('B'), Division("Services"), Department("Support"));
    GOTM.insert(OrgTrieMap(), Continent("Europe"), Country("Germany")).first->insert(Data('C'), Division("Services"), Department("Consulting"));

    std::cout << "\n\n2D ordered triemap as a JSON-like object.\n" << O3::io::json::like(GOTM) << std::endl;
    std::cout << "\n\n2D ordered triemap as a proper JSON object.\n" << O3::io::json::proper(GOTM) << std::endl;
    std::cout << "\n\n2D ordered triemap as a D3 JSON object.\n" << O3::io::json::d3(GOTM) << std::endl;

    return 0;
}
