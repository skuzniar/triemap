#include <iostream>
#include <limits>
#include <cassert>
#include <ostream>

#include "triemap.h"

struct Person
{
    std::string id;
    std::string first;
    std::string last;
    std::string division;
    std::string department;
};

std::ostream&
operator<<(std::ostream& os, const Person& p)
{
    os << p.first << ' ' << p.last << " (" << p.id << ')';
    return os;
}

// Users will acquire and relese some kind of resource
struct Resource
{
    size_t value;
};

std::ostream&
operator<<(std::ostream& os, const Resource& r)
{
    os << "value=" << r.value;
    return os;
}

// We will to keep track of resource utilization
struct Limit
{
    size_t threshold;
    size_t utilization;

    Limit(size_t threshold = 0, size_t utilization = 0)
      : threshold(threshold)
      , utilization(utilization)
    {}

    Limit& operator+=(const Resource& delta)
    {
        utilization += delta.value;
        return *this;
    }

    Limit& operator-=(const Resource& delta)
    {
        utilization -= delta.value;
        return *this;
    }
};

std::ostream&
operator<<(std::ostream& os, const Limit& l)
{
    os << "threshold=" << l.threshold << ' ' << "utilization=" << l.utilization;
    return os;
}

// We will keep track of resource utilization at the division, department and user level.
using Division   = std::string;
using Department = std::string;
using Id         = std::string;

// To achieve this we will store Limits at division, department and user level.
using Limits     = O3::collection::utriemap<Limit, Division, Department, Id>;

// Global instance for simplicity
Limits GL;

void
acquire(const Person& person, const Resource& resource)
{
    GL.climb_pre(
        [&](auto& n, auto&&...) {
            if (n) {
                *n += resource;
            }
            return true;
        },
        person.division,
        person.department,
        person.id);
}

void
release(const Person& person, const Resource& resource)
{
    GL.climb_pre(
        [&](auto& n, auto&&...) {
            if (n) {
                *n -= resource;
            }
            return true;
        },
        person.division,
        person.department,
        person.id);
}

int
main(int argc, char* argv[])
{
    bool verbose = argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v';

    Person people[] = {
        { "001", "Mary", "Moe", "Sales", "Retail" },
        { "002", "John", "Doe", "Services", "Support" },
        { "003", "Jill", "Noe", "Services", "Support" },
        { "004", "Jane", "Poe", "Services", "Consulting" },
    };

    // Set user, department and division level limits
    for (const auto& person : people) {
        GL.insert(Limit(1000), person.division, person.department, person.id);
        GL.insert(Limit(100 * 1000), person.division, person.department);
        GL.insert(Limit(100 * 100 * 1000), person.division);
    }

    // For good measure add global limit
    GL.insert(Limit(10 * 100 * 100 * 1000));

    if (verbose) {
        std::cout << "Initial:\n" << GL << std::endl;
    }

    // Two nodes for person, three for department and two for division, plus a global one
    assert(GL.size() == 4 + 3 + 2 + 1);

    // Everybody acquires some amount of resource.
    for (const auto& person : people) {
        acquire(person, { 100 });
    }

    // Verify that utilization lavels have been properly updated
    assert(GL.find("Sales", "Retail", "001")->utilization == 100);
    assert(GL.find("Sales", "Retail")->utilization == 100);
    assert(GL.find("Sales")->utilization == 100);

    assert(GL.find("Services", "Support", "002")->utilization == 100);
    assert(GL.find("Services", "Support", "003")->utilization == 100);
    assert(GL.find("Services", "Consulting", "004")->utilization == 100);
    assert(GL.find("Services", "Support")->utilization == 200);
    assert(GL.find("Services", "Consulting")->utilization == 100);
    assert(GL.find("Services")->utilization == 300);

    assert(GL.find()->utilization == 400);

    if (verbose) {
        std::cout << "After acquire:\n" << GL << std::endl;
    }

    // Everybody releases half of the amount they acquired.
    for (const auto& person : people) {
        release(person, { 50 });
    }

    assert(GL.find("Sales", "Retail", "001")->utilization == 50);
    assert(GL.find("Sales", "Retail")->utilization == 50);
    assert(GL.find("Sales")->utilization == 50);

    assert(GL.find("Services", "Support", "002")->utilization == 50);
    assert(GL.find("Services", "Support", "003")->utilization == 50);
    assert(GL.find("Services", "Consulting", "004")->utilization == 50);
    assert(GL.find("Services", "Support")->utilization == 100);
    assert(GL.find("Services", "Consulting")->utilization == 50);
    assert(GL.find("Services")->utilization == 150);

    assert(GL.find()->utilization == 200);

    if (verbose) {
        std::cout << "After first release:\n" << GL << std::endl;
    }

    // Everybody releases the remaining amount.
    for (const auto& person : people) {
        release(person, { 50 });
    }

    // Verify that utilization goes to zero everywhere.
    assert(GL.find("Sales", "Retail", "001")->utilization == 0);
    assert(GL.find("Sales", "Retail")->utilization == 0);
    assert(GL.find("Sales")->utilization == 0);

    assert(GL.find("Services", "Support", "002")->utilization == 0);
    assert(GL.find("Services", "Support", "003")->utilization == 0);
    assert(GL.find("Services", "Consulting", "004")->utilization == 0);
    assert(GL.find("Services", "Support")->utilization == 0);
    assert(GL.find("Services", "Consulting")->utilization == 0);
    assert(GL.find("Services")->utilization == 0);

    assert(GL.find()->utilization == 0);

    if (verbose) {
        std::cout << "After second release:\n" << GL << std::endl;
    }

    std::cout << "All good." << std::endl;
    return 0;
}
