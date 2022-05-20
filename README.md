# Triemap

Triemap is an associative container that combines a trie, also called a prefix tree and a map. It is a tree data structure that stores key-value pairs with unique keys. Key is an ordered list of underlying types. The number of elements in the key and their types is fixed at compile time. To access an element one must use a key with zero or more elements. Zero-length key points to the root of the tree. Elements of the key work as links between the parent node and its children. Every node in the tree, not just the leaf node can store data.

Triemap is similar to std::map with a tuple as a key. The standard map, like all containers in the standard library, is fundamentally flat. One can express the parent-child relationship using a map with a key tuple but doing even a simple traversal becomes problematic.

## Motivation

Many problems that programmers deal with have a hierarchical structure. Things are part of a group, which is part of a larger group, and so on. Employees are part of a team, which is part of a department or division, which is part of a larger organization. Triemap lets us express these relationships better than traditional flat containers.

Hierarchical configuration and aggregation are two examples where triemap is very useful. In the hierarchical configuration, we use the parent-child relationship to define default settings at the parent and override or refine them at the child level. In hierarchical aggregation, we roll up child node values to the parent. 

## Installation

Triemap is a header-only library with no dependencies other than the standard library. It requires a C++17 compiler.

## Usage

The tests and examples directories contain simple programs that show how to use triemap. Please refer to the readme-files in those directories for more information.

## Implementation

The code snippet below shows the gist of the implementation. Triemap is a recursive structure where each node has a map of children nodes and storage for optional data.

```cpp
template<template<typename K, typename T> class MAP, typename DATA, typename PFIX, typename... PFIXS>
class triemap<MAP, DATA, PFIX, PFIXS...>
{
public:
    using this_type = triemap<MAP, DATA, PFIX, PFIXS...>;
    using data_type = std::optional<DATA>;
    using repo_type = MAP<PFIX, triemap<MAP, DATA, PFIXS...>>;
...
private:
    data_type m_data;
    repo_type m_repo;
};
```

## License

[MIT](LICENSE)
