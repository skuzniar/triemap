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

Person people[] = {
    { "001", "Mary", "Moe", "Sales", "Retail" },
    { "002", "John", "Doe", "Services", "Support" },
    { "003", "Jill", "Noe", "Services", "Support" },
    { "004", "Jane", "Poe", "Services", "Consulting" },
};

// We want to have a list of named features and be able to control them at the division, department and user level.
using Feature      = std::string;

using Division     = std::string;
using Department   = std::string;
using Id           = std::string;

// To achieve this we will store boolean flags using <Feature, Division, Department, Id> key.
using FeatureFlags = O3::collection::utriemap<bool, Feature, Division, Department, Id>;

void
checkFeature(const Feature& f, const FeatureFlags& ff)
{
    for (const auto& person : people) {
        const auto* data    = ff.match(f, person.division, person.department, person.id);
        bool        enabled = (data != nullptr) && *data;
        std::cout << person << (enabled ? " can " : " can't ") << "use " << f << std::endl;
    }
    std::cout << std::endl;
}

int
main(int argc, char* argv[])
{
    bool verbose = argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v';

    FeatureFlags ff;

    Feature      feature = "Text-Notification";

    // Initially only enable the feature for one member of the Support department
    ff.insert(true, feature, "Services", "Support", "003");
    checkFeature(feature, ff);

    // Then enable it for all members of the Services division
    ff.insert(true, feature, "Services");
    checkFeature(feature, ff);

    // Then enable it for everyone
    ff.insert(true, feature);
    checkFeature(feature, ff);

    // Until people in Consulting complain and want it removed
    ff.insert(false, feature, "Services", "Consulting");
    checkFeature(feature, ff);

    if (verbose) {
        std::cout << "Feature flags:\n" << ff << std::endl;
    }

    return 0;
}
