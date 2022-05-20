#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include "triemap.h"

//-------------------------------------------------------------------------------------------------
// Return list of data elements collected by using immediate children of a given node
//-------------------------------------------------------------------------------------------------
template<typename REPO, typename... QS>
std::string
level_order_traversal(const REPO& r, QS&&... qs)
{
    std::string result;
    r.jump([&](const auto& n) {
        n.traverse_level([&](const auto& nn, auto&&...) {
            if (nn) {
                result += *nn;
            }
            return true;
        });
    }, std::forward<QS>(qs)...);
    return result;
}

//-------------------------------------------------------------------------------------------------
// Return list of data elements collected by using pre-order traversal starting from a given node
//-------------------------------------------------------------------------------------------------
template<typename REPO, typename... QS>
std::string
pre_order_traversal(const REPO& r, QS&&... qs)
{
    std::string result;
    r.jump([&](const auto& n) {
        n.traverse_pre([&](const auto& nn, auto&&...) {
            if (nn) {
                result += *nn;
            }
            return true;
        });
    }, std::forward<QS>(qs)...);
    return result;
}

//-------------------------------------------------------------------------------------------------
// Return list of data elements collected by using post-order traversal starting from a given node
//-------------------------------------------------------------------------------------------------
template<typename REPO, typename... QS>
std::string
post_order_traversal(const REPO& r, QS&&... qs)
{
    std::string result;
    r.jump([&](const auto& n) {
        n.traverse_post([&](const auto& nn, auto&&...) {
            if (nn) {
                result += *nn;
            }
            return true;
        });
    }, std::forward<QS>(qs)...);
    return result;
}

//-------------------------------------------------------------------------------------------------
// Return list of data elements collected by using pre-order climb along the path
//-------------------------------------------------------------------------------------------------
template<typename REPO, typename... QS>
std::string
pre_order_climb(const REPO& r, QS&&... qs)
{
    std::string result;
    r.climb_pre ([&](const auto& n, auto&&...) {
        if (n) {
            result += *n;
        }
        return true;
    }, std::forward<QS>(qs)...);
    return result;
}

//-------------------------------------------------------------------------------------------------
// Return list of data elements collected by using post-order climb along the path
//-------------------------------------------------------------------------------------------------
template<typename REPO, typename... QS>
std::string
post_order_climb(const REPO& r, QS&&... qs)
{
    std::string result;
    r.climb_post ([&](const auto& n, auto&&...) {
        if (n) {
            result += *n;
        }
        return true;
    }, std::forward<QS>(qs)...);
    return result;
}

// Collections of char data elements addressed by string prefixes.
using orepo = O3::collection::otriemap<char, std::string, std::string>;
using urepo = O3::collection::utriemap<char, std::string, std::string>;

// Check if two strings contain the same letters - rather unorthodox use of operator overloading.
bool operator &= (const std::string& l, const std::string& r)
{
    return l.size() == r.size() && std::is_permutation(l.begin(), l.end(), r.begin());
}

int
main(int, char* [])
{
    orepo o;
    urepo u;

    o.insert('0');              u.insert('0');
    o.insert('A', "a");         u.insert('A', "a");

    o.insert('B', "b");         u.insert('B', "b");
    o.insert('C', "a", "c");    u.insert('C', "a", "c");
    o.insert('D', "a", "d");    u.insert('D', "a", "d");
    o.insert('E', "b", "e");    u.insert('E', "b", "e");
    o.insert('F', "b", "f");    u.insert('F', "b", "f");

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
    assert(level_order_traversal(o) == "AB");
    assert(level_order_traversal(u) &= "AB");

    assert(level_order_traversal(o, "a") == "CD");
    assert(level_order_traversal(u, "a") &= "CD");

    assert(level_order_traversal(o, "b") == "EF");
    assert(level_order_traversal(u, "b") &= "EF");

    assert(level_order_traversal(o, "a", "c") == "");
    assert(level_order_traversal(u, "a", "c") == "");
    assert(level_order_traversal(o, "a", "d") == "");
    assert(level_order_traversal(u, "a", "d") == "");

    assert(level_order_traversal(o, "b", "e") == "");
    assert(level_order_traversal(u, "b", "e") == "");
    assert(level_order_traversal(o, "b", "f") == "");
    assert(level_order_traversal(u, "b", "f") == "");

    assert( pre_order_traversal(o) == "0ACDBEF");
    assert(post_order_traversal(o) == "CDAEFB0");

    assert( pre_order_traversal(u) &= "0ACDBEF");
    assert(post_order_traversal(u) &= "CDAEFB0");

    assert( pre_order_traversal(o, "a") == "ACD");
    assert(post_order_traversal(o, "a") == "CDA");
    assert( pre_order_traversal(o, "b") == "BEF");
    assert(post_order_traversal(o, "b") == "EFB");

    assert( pre_order_traversal(u, "a") &= "ACD");
    assert(post_order_traversal(u, "a") &= "CDA");
    assert( pre_order_traversal(u, "b") &= "BEF");
    assert(post_order_traversal(u, "b") &= "EFB");

    assert( pre_order_traversal(o, "a", "c") == "C");
    assert( pre_order_traversal(o, "a", "c") == "C");
    assert(post_order_traversal(o, "a", "d") == "D");
    assert(post_order_traversal(o, "a", "d") == "D");

    assert( pre_order_traversal(u, "a", "c") == "C");
    assert( pre_order_traversal(u, "a", "c") == "C");
    assert(post_order_traversal(u, "a", "d") == "D");
    assert(post_order_traversal(u, "a", "d") == "D");

    assert( pre_order_traversal(o, "b", "e") == "E");
    assert( pre_order_traversal(o, "b", "e") == "E");
    assert(post_order_traversal(o, "b", "f") == "F");
    assert(post_order_traversal(o, "b", "f") == "F");

    assert( pre_order_traversal(u, "b", "e") == "E");
    assert( pre_order_traversal(u, "b", "e") == "E");
    assert(post_order_traversal(u, "b", "f") == "F");
    assert(post_order_traversal(u, "b", "f") == "F");

    assert( pre_order_climb(o)           == "0");
    assert( pre_order_climb(o, "a")      == "0A");
    assert( pre_order_climb(o, "a", "c") == "0AC");
    assert( pre_order_climb(o, "a", "d") == "0AD");
    assert( pre_order_climb(o, "b")      == "0B");
    assert( pre_order_climb(o, "b", "e") == "0BE");
    assert( pre_order_climb(o, "b", "f") == "0BF");

    assert(post_order_climb(o)           == "0");
    assert(post_order_climb(o, "a")      == "A0");
    assert(post_order_climb(o, "a", "c") == "CA0");
    assert(post_order_climb(o, "a", "d") == "DA0");
    assert(post_order_climb(o, "b")      == "B0");
    assert(post_order_climb(o, "b", "e") == "EB0");
    assert(post_order_climb(o, "b", "f") == "FB0");

    assert( pre_order_climb(o, "x")      == "0");
    assert( pre_order_climb(o, "a", "x") == "0A");
    assert( pre_order_climb(o, "b", "x") == "0B");

    assert(post_order_climb(o, "x")      == "0");
    assert(post_order_climb(o, "a", "x") == "A0");
    assert(post_order_climb(o, "b", "x") == "B0");

    // Climbing ordered and unordered collections is equivalent
    assert( pre_order_climb(o)           ==  pre_order_climb(u));
    assert( pre_order_climb(o, "a")      ==  pre_order_climb(u, "a"));
    assert( pre_order_climb(o, "a", "c") ==  pre_order_climb(u, "a", "c"));
    assert( pre_order_climb(o, "a", "d") ==  pre_order_climb(u, "a", "d"));
    assert( pre_order_climb(o, "b")      ==  pre_order_climb(u, "b"));
    assert( pre_order_climb(o, "b", "e") ==  pre_order_climb(u, "b", "e"));
    assert( pre_order_climb(o, "b", "f") ==  pre_order_climb(u, "b", "f"));

    assert(post_order_climb(o)           == post_order_climb(u));
    assert(post_order_climb(o, "a")      == post_order_climb(u, "a"));
    assert(post_order_climb(o, "a", "c") == post_order_climb(u, "a", "c"));
    assert(post_order_climb(o, "a", "d") == post_order_climb(u, "a", "d"));
    assert(post_order_climb(o, "b")      == post_order_climb(u, "b"));
    assert(post_order_climb(o, "b", "e") == post_order_climb(u, "b", "e"));
    assert(post_order_climb(o, "b", "f") == post_order_climb(u, "b", "f"));

    std::cout << "All traversal tests passed." << std::endl;

    return 0;
}
