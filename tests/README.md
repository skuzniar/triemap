# Tests

This directory contains simple tests that show the basic functionality of the triemap.

## basics.cpp
The basic test demonstrates how to insert, remove and lookup elements in an ordered and unordered triemap.

## traversal.cpp
The traversal test shows how to perform triemap traversals. All traversal tests visit triemap nodes and return the string that is a concatenation of characters stored in them.

Pre-order and post-order traversal can start at an arbitrary node and continue to the leaf nodes. The list of prefixes given to the traversal function determines the starting node. 

For the unordered triemap, the order in which child nodes are visited is non-deterministic. We know which nodes will be visited but we do not know in which order.

Climb traversal always starts at the root node and visits every node following the path given by the key. The traversal will stop if some part of the key can not be found.
