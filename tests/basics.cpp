#include <iostream>
#include <limits>
#include <cassert>

#include "triemap.h"

//-------------------------------------------------------------------------------------------------
// Collections of char data elements addressed by string prefixes.
//-------------------------------------------------------------------------------------------------
using orepo = O3::collection::otriemap<char, std::string, std::string>;
using urepo = O3::collection::utriemap<char, std::string, std::string>;

//-------------------------------------------------------------------------------------------------
// Test insertion
//-------------------------------------------------------------------------------------------------
template<typename REPO>
void
test_insertion()
{
    REPO r;

    // Even empty collection has root node
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);

    assert(r.insert('0').second == true);
    assert(!r.empty() && r.size() == 1 && r.count() == 1 && r.height() == 0);
    assert(*r.find() == '0');

    assert(r.insert('X').second == false);
    assert(*r.find() == '0');

    assert(r.insert('A', "a").second == true);
    assert(!r.empty() && r.size() == 2 && r.count() == 2 && r.height() == 1);
    assert(*r.find() == '0' && *r.find("a") == 'A');

    assert(r.insert('B', "b").second == true);
    assert(!r.empty() && r.size() == 3 && r.count() == 3 && r.height() == 1);
    assert(*r.find() == '0' && *r.find("a") == 'A' && *r.find("b") == 'B');

    assert(r.insert('X', "b").second == false);
    assert(!r.empty() && r.size() == 3 && r.count() == 3 && r.height() == 1);
}

//-------------------------------------------------------------------------------------------------
// Test removal
//-------------------------------------------------------------------------------------------------
template<typename REPO>
void
test_removal()
{
    REPO r;

    // Even empty collection has root node
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);

    assert(r.erase() == 0);

    assert(r.insert('0').second == true);
    assert(!r.empty() && r.size() == 1 && r.count() == 1 && r.height() == 0);

    assert(r.erase() == 1);
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);

    assert(r.insert('A', "a").second == true);
    assert(!r.empty() && r.size() == 1 && r.count() == 2 && r.height() == 1);

    assert(r.erase("x") == 0);
    assert(!r.empty() && r.size() == 1 && r.count() == 2 && r.height() == 1);

    assert(r.erase("a") == 1);
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);

    assert(r.insert('B', "b").second == true);
    assert(!r.empty() && r.size() == 1 && r.count() == 2 && r.height() == 1);

    assert(r.erase("b") == 1);
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);

    r.insert('0');
    r.insert('A', "a");
    r.insert('B', "b");
    assert(!r.empty() && r.size() == 3 && r.count() == 3 && r.height() == 1);

    r.clear();
    assert(r.empty() && r.size() == 0 && r.count() == 1 && r.height() == 0);
}

//-------------------------------------------------------------------------------------------------
// Test lookup
//-------------------------------------------------------------------------------------------------
template<typename REPO>
void
test_lookup()
{
    REPO r;

    r.insert('0');
    r.insert('A', "a");
    r.insert('B', "b");
    r.insert('C', "a", "c");
    r.insert('D', "a", "d");
    r.insert('E', "b", "e");
    r.insert('F', "b", "f");

    assert(!r.empty() && r.size() == 7 && r.count() == 7 && r.height() == 2);

    assert(*r.find() == '0');
    assert(*r.match() == '0');

    assert(*r.find("a") == 'A');
    assert(*r.match("a") == 'A');

    assert(*r.find("b") == 'B');
    assert(*r.match("b") == 'B');

    assert(*r.find("a", "c") == 'C');
    assert(*r.match("a", "c") == 'C');

    assert(*r.find("a", "d") == 'D');
    assert(*r.match("a", "d") == 'D');

    assert(*r.find("b", "e") == 'E');
    assert(*r.match("b", "e") == 'E');

    assert(*r.find("b", "f") == 'F');
    assert(*r.match("b", "f") == 'F');

    assert(r.find("x") == nullptr);
    assert(*r.match("x") == '0');

    assert(r.find("a", "x") == nullptr);
    assert(*r.match("a", "x") == 'A');

    assert(r.find("b", "x") == nullptr);
    assert(*r.match("b", "x") == 'B');
}

int
main(int argc, char* argv[])
{
    test_insertion<orepo>();
    test_removal<orepo>();
    test_lookup<orepo>();

    test_insertion<urepo>();
    test_removal<urepo>();
    test_lookup<urepo>();

    std::cout << "All basic tests pased." << std::endl;

    return 0;
}
