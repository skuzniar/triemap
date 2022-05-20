# Examples

This directory contains some examples of how triemap can be used.

## json.cpp

This program shows how to print triemap collections using a JSON-like format. The output is not a valid JSON syntax. For clarity, we stripped quotes from string elements.

## feature-flags.cpp
A feature flag is hardly a novel idea. Switches to enable specific functionality exist in every system. They are however often implemented as all-in/all-out toggles. The use of a hierarchical data structure allows us to gradually enable new functionality.

## aggregation.cpp

Rolling up values from children into parents is one of the things hierarchical data structures are good at. This program shows how to tally up numbers at various levels of hierarchy.

## reduction.cpp

This program shows how to start with a flat collection of records, put them into a hierarchical structure and then reduce the number of records by removing some duplicates. We then show that the flat and hierarchical structures are equivalent when looking up elements. 
