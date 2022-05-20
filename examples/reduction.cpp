#include <iostream>
#include <limits>
#include <cassert>
#include <ostream>
#include <random>
#include <chrono>
#include <utility>
#include <functional>

#include "triemap.h"

// User will have their own configuration represented as a number. Different users may share the same configuration.
struct Configuration
{
    int  config;

    bool operator==(const Configuration& oth) const
    {
        return config == oth.config;
    }
    bool operator<(const Configuration& oth) const
    {
        return config < oth.config;
    }
};

std::ostream&
operator<<(std::ostream& os, const Configuration& c)
{
    os << "config=" << c.config;
    return os;
}

// Users are assigned to departments which are part of divisions. All three use a single letter identifier.
using Division   = char;
using Department = char;
using User       = char;

// We use flat map of user configurations where <Division, Department, User> is a unique key.
using Key        = std::tuple<Division, Department, User>;
using FlatMap    = std::map<Key, Configuration>;

// We will store the same configuration using ordered - to better see the changes - triemap.
using TrieMap    = O3::collection::otriemap<Configuration, Division, Department, User>;

// Fill the flat map and the trie map with the same random and non unique user configurations.
void
fill(FlatMap& fm, TrieMap& tm, bool verbose)
{
    // In verbose mode we print the collection so we want it small
    Division                           div_limit = verbose ? 'B' : 'Z';
    Department                         dep_limit = verbose ? 'B' : 'Z';
    User                               usr_limit = verbose ? 'e' : 'z';
    int                                cfg_limit = verbose ? 2 : 4;

    std::default_random_engine         r_eng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> r_dis(1, cfg_limit);

    for (Division div = 'A'; div <= div_limit; ++div) {
        for (Department dep = 'A'; dep <= dep_limit; ++dep) {
            for (User usr = 'a'; usr <= usr_limit; ++usr) {
                Configuration config = { r_dis(r_eng) };
                fm.emplace(Key{ div, dep, usr }, config);
                tm.insert(config, div, dep, usr);
            }
        }
    }
}

// Verify that the flat map and the trie map return identical results for the same key.
void
verify(const FlatMap& fm, const TrieMap& tm)
{
    for (const auto& pair : fm) {
        assert(pair.second == *tm.match(std::get<0>(pair.first), std::get<1>(pair.first), std::get<2>(pair.first)));
    }
}

// Remove leaves that share largest value count and set their parent to that value.
template <typename TM>
void
reduce(TM& tm)
{
    tm.traverse_post([&](auto& n, auto&&...) {
        if (!n) {
            // 1. Find the most common value among siblings
            using data = typename TM::data_type;
            std::map<std::reference_wrapper<const data>, int, std::less<const data>> counts;

            n.traverse_level([&](const auto& s, auto&&...) {
                if (s) {
                    counts[std::cref(*s)]++;
                }
                return true;
            });
            if (!counts.empty()) {
                auto top = std::max_element(counts.begin(), counts.end(), [](const auto& l, const auto& r) {
                               return l.second < r.second;
                           })->first;
                // 2 Set the parent node to the most common value
                n.insert(top);

                // 3 Erase child nodes that share the most common value
                n.traverse_level([&](auto& s, auto&&... ss) {
                    if (s && *s == *n) {
                        n.erase(ss...);
                    }
                    return true;
                });
            }
        }
        return true;
    });
}

// Global instances for simplicity
FlatMap FM;
TrieMap TM;

int
main(int argc, char* argv[])
{
    bool verbose = argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v';

    // Initialize both collections
    fill(FM, TM, verbose);
    std::cout << "Flat map size=" << FM.size() << ' ' << "Trie map size=" << TM.size() << std::endl;

    if (verbose) {
        std::cout << "Before reduction:\n" << TM << std::endl;
    }

    // Verify that both collections have the same data
    verify(FM, TM);

    // Reduce trie map
    reduce(TM);
    std::cout << "Reduced trie map size=" << TM.size() << std::endl;

    if (verbose) {
        std::cout << "After reduction:\n" << TM << std::endl;
    }

    verify(FM, TM);

    std::cout << "All good." << std::endl;
    return 0;
}
