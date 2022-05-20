/*
 Copyright (c) 2022, Slawomir Kuzniar.
 Distributed under the MIT License (http://opensource.org/licenses/MIT).
*/

#ifndef O3_ALGO_REDUCE_DOT_H
#define O3_ALGO_REDUCE_DOT_H

#include <map>
#include <functional>
#include <algorithm>

namespace O3 {
namespace algo {

// Remove leaves that share largest value count and set their parent to that value.
template<typename TM>
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
}
}

#endif
