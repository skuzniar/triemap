/*
 Copyright (c) 2022, Slawomir Kuzniar.
 Distributed under the MIT License (http://opensource.org/licenses/MIT).
*/

#ifndef O3_COLLECTION_TRIEMAP_DOT_H
#define O3_COLLECTION_TRIEMAP_DOT_H

#include <optional>
#include <numeric>
#include <map>
#include <unordered_map>

#include "io/json.h"

namespace O3 {
namespace collection {

namespace details {

//----------------------------------------------------------------------------------------------------------------------
// Trie-map collection base case.
//----------------------------------------------------------------------------------------------------------------------
template<template<typename K, typename T> class MAP, typename DATA, typename... PFIXS>
class triemap
{
public:
    using this_type = triemap<MAP, DATA>;
    using data_type = DATA;

    //------------------------------------------------------------------------------------------------------------------
    // Check if node holds data
    //------------------------------------------------------------------------------------------------------------------
    explicit operator bool() const
    {
        return m_data.has_value();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Dereference data in the node
    //------------------------------------------------------------------------------------------------------------------
    const DATA& operator*() const
    {
        return *m_data;
    }
    DATA& operator*()
    {
        return *m_data;
    }

    const DATA* operator->() const
    {
        return &*m_data;
    }
    DATA* operator->()
    {
        return &*m_data;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Insert or return existing data
    //------------------------------------------------------------------------------------------------------------------
    template<class D>
    auto insert(D&& data)
    {
        bool exists = m_data.has_value();
        if (!exists) {
            m_data = std::forward<D>(data);
        }
        return std::make_pair(&*m_data, !exists);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Erase data
    //------------------------------------------------------------------------------------------------------------------
    size_t erase()
    {
        size_t count = m_data ? 1 : 0;
        m_data.reset();
        return count;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Clear all data
    //------------------------------------------------------------------------------------------------------------------
    void clear()
    {
        m_data.reset();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return true if the node has no children
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool leaf() const
    {
        return true;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return true if there is no data at the current and all children nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool empty() const
    {
        return !m_data.has_value();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return number of data elements
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t size() const
    {
        return m_data ? 1 : 0;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return number of (possibly empty) nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t count() const
    {
        return 1;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return height of the tree - maximum distance between current node and leaf nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t height() const
    {
        return 0;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Find the data given the list of prefixes
    //------------------------------------------------------------------------------------------------------------------
    const DATA* find() const
    {
        return m_data ? &*m_data : nullptr;
    }

    DATA* find()
    {
        return m_data ? &*m_data : nullptr;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Find the data element as far as possible along the list of prefixes
    //------------------------------------------------------------------------------------------------------------------
    const DATA* match() const
    {
        return m_data ? &*m_data : nullptr;
    }

    DATA* match()
    {
        return m_data ? &*m_data : nullptr;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Visit specific node and apply given operation
    //------------------------------------------------------------------------------------------------------------------
    template<typename F>
    void jump(F&& f) const
    {
        f(*this);
    }
    template<typename F>
    void jump(F&& f)
    {
        f(*this);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Visit all nodes as far as possible along the list of prefixes performing pre and post order operations
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF, typename POSF>
    void climb(PREF&& pref, POSF&& posf) const
    {
        pref(*this);
        posf(*this);
    }

    template<typename PREF, typename POSF>
    void climb(PREF&& pref, POSF&& posf)
    {
        pref(*this);
        posf(*this);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Pre order climb
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF>
    void climb_pre(PREF&& pref) const
    {
        climb(std::forward<PREF>(pref), [](const auto&...) {});
    }

    template<typename PREF>
    void climb_pre(PREF&& pref)
    {
        climb(std::forward<PREF>(pref), [](const auto&...) {});
    }

    //------------------------------------------------------------------------------------------------------------------
    // Post order climb
    //-----------------------------------------------------------------------------------------------------------------
    template<typename POSF>
    void climb_post(POSF&& posf) const
    {
        climb([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    template<typename POSF>
    void climb_post(POSF&& posf)
    {
        climb([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    //------------------------------------------------------------------------------------------------------------------
    // Level order traversal. It is a no-op for leaf node.
    //------------------------------------------------------------------------------------------------------------------
    template<typename LEVF>
    void traverse_level(LEVF&& levf) const
    {}

    template<typename LEVF>
    void traverse_level(LEVF&& levf)
    {}

    //------------------------------------------------------------------------------------------------------------------
    // Depth first search traversal with early termination that combines pre and post order variants.
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF, typename POSF, typename... PS>
    void traverse_dfs(PREF&& pref, POSF&& posf, PS&&... ps) const
    {
        pref(*this, std::forward<PS>(ps)...);
        posf(*this, std::forward<PS>(ps)...);
    }

    template<typename PREF, typename POSF, typename... PS>
    void traverse_dfs(PREF&& pref, POSF&& posf, PS&&... ps)
    {
        pref(*this, std::forward<PS>(ps)...);
        posf(*this, std::forward<PS>(ps)...);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Pre order traversal
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF>
    void traverse_pre(PREF&& pref) const
    {
        traverse_dfs(std::forward<PREF>(pref), [](const auto&...) {});
    }

    template<typename PREF>
    void traverse_pre(PREF&& pref)
    {
        traverse_dfs(std::forward<PREF>(pref), [](const auto&...) {});
    }

    //------------------------------------------------------------------------------------------------------------------
    // Post order traversal
    //------------------------------------------------------------------------------------------------------------------
    template<typename POSF>
    void traverse_post(POSF&& posf) const
    {
        traverse_dfs([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    template<typename POSF>
    void traverse_post(POSF&& posf)
    {
        traverse_dfs([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    //------------------------------------------------------------------------------------------------------------------
    // Trie-map equality
    //------------------------------------------------------------------------------------------------------------------
    bool operator==(const this_type& oth) const
    {
        return m_data == oth.m_data;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Trie-map ordering
    //------------------------------------------------------------------------------------------------------------------
    bool operator<(const this_type& oth) const
    {
        return m_data < oth.m_data;
    }

private:
    friend std::ostream& operator<<(std::ostream& os, const this_type& tm) {
        return os << O3::io::json::like(tm);
    }

    std::optional<data_type> m_data;
};

//----------------------------------------------------------------------------------------------------------------------
// Trie-map. A collection of elements indexed by list of prefixes.
//----------------------------------------------------------------------------------------------------------------------
template<template<typename K, typename T> class MAP, typename DATA, typename PFIX, typename... PFIXS>
class triemap<MAP, DATA, PFIX, PFIXS...>
{
public:
    using this_type = triemap<MAP, DATA, PFIX, PFIXS...>;
    using data_type = DATA;
    using repo_type = MAP<PFIX, triemap<MAP, DATA, PFIXS...>>;

    //------------------------------------------------------------------------------------------------------------------
    // Check if node holds data
    //------------------------------------------------------------------------------------------------------------------
    explicit operator bool() const
    {
        return m_data.has_value();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Dereference data in the node
    //------------------------------------------------------------------------------------------------------------------
    const DATA& operator*() const
    {
        return *m_data;
    }
    DATA& operator*()
    {
        return *m_data;
    }

    const DATA* operator->() const
    {
        return &*m_data;
    }
    DATA* operator->()
    {
        return &*m_data;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Insert or return existing data
    //------------------------------------------------------------------------------------------------------------------
    template<class D>
    auto insert(D&& data)
    {
        bool exists = m_data.has_value();
        if (!exists) {
            m_data = std::forward<D>(data);
        }
        return std::make_pair(&*m_data, !exists);
    }

    template<class D, typename P, typename... PS>
    auto insert(D&& data, P&& p, PS&&... ps)
    {
        return m_repo[p].insert(std::forward<D>(data), std::forward<PS>(ps)...);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Erase data
    //------------------------------------------------------------------------------------------------------------------
    size_t erase()
    {
        size_t count = m_data ? 1 : 0;
        m_data.reset();
        return count;
    }

    template<typename P, typename... PS>
    size_t erase(P&& p, PS&&... ps)
    {
        size_t count = 0;
        auto   itr   = m_repo.find(std::forward<P>(p));
        if (itr != m_repo.end()) {
            count = itr->second.erase(std::forward<PS>(ps)...);
            if (itr->second.empty()) {
                m_repo.erase(itr);
            }
        }
        return count;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Clear all data
    //------------------------------------------------------------------------------------------------------------------
    void clear()
    {
        m_data.reset();
        m_repo.clear();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return true if the node has no children
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool leaf() const
    {
        return m_repo.empty();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return true if there is no data at the current and all children nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] bool empty() const
    {
        return !m_data.has_value() &&
               std::all_of(m_repo.begin(), m_repo.end(), [](const typename repo_type::value_type& r) {
                   return r.second.empty();
               });
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return number of data elements
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t size() const
    {
        return std::accumulate(m_repo.begin(),
                               m_repo.end(),
                               m_data ? 1 : 0,
                               [](size_t s, const typename repo_type::value_type& r) { return s + r.second.size(); });
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return number of (possibly empty) nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t count() const
    {
        return std::accumulate(m_repo.begin(), m_repo.end(), 1, [](size_t c, const typename repo_type::value_type& r) {
            return c + r.second.count();
        });
    }

    //------------------------------------------------------------------------------------------------------------------
    // Return height of the tree - maximum distance between current node and leaf nodes
    //------------------------------------------------------------------------------------------------------------------
    [[nodiscard]] size_t height() const
    {
        return m_repo.empty() ? 0
                              : 1 + std::accumulate(m_repo.begin(),
                                                    m_repo.end(),
                                                    0,
                                                    [](std::size_t h, const typename repo_type::value_type& r) {
                                                        return std::max(h, r.second.height());
                                                    });
    }

    //------------------------------------------------------------------------------------------------------------------
    // Find the data given the list of prefixes
    //------------------------------------------------------------------------------------------------------------------
    const DATA* find() const
    {
        return m_data ? &*m_data : nullptr;
    }
    template<typename P, typename... PS>
    const DATA* find(P&& p, PS&&... ps) const
    {
        auto itr = m_repo.find(std::forward<P>(p));
        return itr != m_repo.end() ? itr->second.find(std::forward<PS>(ps)...) : nullptr;
    }

    DATA* find()
    {
        return m_data ? &*m_data : nullptr;
    }
    template<typename P, typename... PS>
    DATA* find(P&& p, PS&&... ps)
    {
        auto itr = m_repo.find(std::forward<P>(p));
        return itr != m_repo.end() ? itr->second.find(std::forward<PS>(ps)...) : nullptr;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Find the data element as far as possible along the list of prefixes
    //------------------------------------------------------------------------------------------------------------------
    const DATA* match() const
    {
        return m_data ? &*m_data : nullptr;
    }
    template<typename P, typename... PS>
    const DATA* match(P&& p, PS&&... ps) const
    {
        auto itr = m_repo.find(std::forward<P>(p));
        auto rv  = itr != m_repo.end() ? itr->second.match(std::forward<PS>(ps)...) : nullptr;
        return rv ? rv : match();
    }

    DATA* match()
    {
        return m_data ? &*m_data : nullptr;
    }
    template<typename P, typename... PS>
    DATA* match(P&& p, PS&&... ps)
    {
        auto itr = m_repo.find(std::forward<P>(p));
        auto rv  = itr != m_repo.end() ? itr->second.match(std::forward<PS>(ps)...) : nullptr;
        return rv ? rv : match();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Visit specific node and apply given operation
    //------------------------------------------------------------------------------------------------------------------
    template<typename F>
    void jump(F&& f) const
    {
        f(*this);
    }
    template<typename F, typename P, typename... PS>
    void jump(F&& f, P&& p, PS&&... ps) const
    {
        auto itr = m_repo.find(std::forward<P>(p));
        if (itr != m_repo.end()) {
            itr->second.jump(std::forward<F>(f), std::forward<PS>(ps)...);
        }
    }

    template<typename F>
    void jump(F&& f)
    {
        f(*this);
    }
    template<typename F, typename P, typename... PS>
    void jump(F&& f, P&& p, PS&&... ps)
    {
        auto itr = m_repo.find(std::forward<P>(p));
        if (itr != m_repo.end()) {
            itr->second.jump(std::forward<F>(f), std::forward<PS>(ps)...);
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    // Visit all nodes as far as possible along the list of prefixes performing pre and post order operations
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF, typename POSF>
    void climb(PREF&& pref, POSF&& posf) const
    {
        pref(*this);
        posf(*this);
    }
    template<typename PREF, typename POSF, typename P, typename... PS>
    void climb(PREF&& pref, POSF&& posf, P&& p, PS&&... ps) const
    {
        if (pref(*this)) {
            auto itr = m_repo.find(std::forward<P>(p));
            if (itr != m_repo.end()) {
                itr->second.climb(std::forward<PREF>(pref), std::forward<POSF>(posf), std::forward<PS>(ps)...);
            }
        }
        posf(*this);
    }

    template<typename PREF, typename POSF>
    void climb(PREF&& pref, POSF&& posf)
    {
        pref(*this);
        posf(*this);
    }
    template<typename PREF, typename POSF, typename P, typename... PS>
    void climb(PREF&& pref, POSF&& posf, P&& p, PS&&... ps)
    {
        if (pref(m_data)) {
            auto itr = m_repo.find(std::forward<P>(p));
            if (itr != m_repo.end()) {
                itr->second.climb(std::forward<PREF>(pref), std::forward<POSF>(posf), std::forward<PS>(ps)...);
            }
        }
        posf(m_data);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Pre order climb
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF>
    void climb_pre(PREF&& pref) const
    {
        climb(std::forward<PREF>(pref), [](const auto&...) {});
    }
    template<typename PREF, typename P, typename... PS>
    void climb_pre(PREF&& pref, P&& p, PS&&... ps) const
    {
        climb(
            std::forward<PREF>(pref), [](const auto&...) {}, std::forward<P>(p), std::forward<PS>(ps)...);
    }

    template<typename PREF>
    void climb_pre(PREF&& pref)
    {
        climb(std::forward<PREF>(pref), [](const auto&...) {});
    }
    template<typename PREF, typename P, typename... PS>
    void climb_pre(PREF&& pref, P&& p, PS&&... ps)
    {
        climb(
            std::forward<PREF>(pref), [](const auto&...) {}, std::forward<P>(p), std::forward<PS>(ps)...);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Post order climb
    //-----------------------------------------------------------------------------------------------------------------
    template<typename POSF>
    void climb_post(POSF&& posf) const
    {
        climb([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }
    template<typename POSF, typename P, typename... PS>
    void climb_post(POSF&& posf, P&& p, PS&&... ps) const
    {
        climb(
            [](const auto&...) { return true; }, std::forward<POSF>(posf), std::forward<P>(p), std::forward<PS>(ps)...);
    }

    template<typename POSF>
    void climb_post(POSF&& posf)
    {
        climb([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }
    template<typename POSF, typename P, typename... PS>
    void climb_post(POSF&& posf, P&& p, PS&&... ps)
    {
        climb(
            [](const auto&...) { return true; }, std::forward<POSF>(posf), std::forward<P>(p), std::forward<PS>(ps)...);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Level order traversal with early termination. Visit immediate children.
    //------------------------------------------------------------------------------------------------------------------
    template<typename LEVF>
    void traverse_level(LEVF&& levf) const
    {
        for (auto itr = m_repo.begin(); itr != m_repo.end();) {
            auto cur = itr++;
            if (!levf(cur->second, cur->first))
                break;
        }
    }

    template<typename LEVF>
    void traverse_level(LEVF&& levf)
    {
        for (auto itr = m_repo.begin(); itr != m_repo.end();) {
            auto cur = itr++;
            if (!levf(cur->second, cur->first))
                break;
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    // Depth first search traversal with early termination that combines pre and post order variants.
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF, typename POSF, typename... PS>
    void traverse_dfs(PREF&& pref, POSF&& posf, PS&&... ps) const
    {
        if (pref(*this, std::forward<PS>(ps)...)) {
            for (auto itr = m_repo.begin(); itr != m_repo.end();) {
                auto cur = itr++;
                cur->second.traverse_dfs(std::forward<PREF>(pref), std::forward<POSF>(posf), cur->first);
            }
        }
        posf(*this, std::forward<PS>(ps)...);
    }

    template<typename PREF, typename POSF, typename... PS>
    void traverse_dfs(PREF&& pref, POSF&& posf, PS&&... ps)
    {
        if (pref(*this, std::forward<PS>(ps)...)) {
            for (auto itr = m_repo.begin(); itr != m_repo.end();) {
                auto cur = itr++;
                cur->second.traverse_dfs(std::forward<PREF>(pref), std::forward<POSF>(posf), cur->first);
            }
        }
        posf(*this, std::forward<PS>(ps)...);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Pre order traversal
    //------------------------------------------------------------------------------------------------------------------
    template<typename PREF>
    void traverse_pre(PREF&& pref) const
    {
        traverse_dfs(std::forward<PREF>(pref), [](const auto&...) {});
    }

    template<typename PREF>
    void traverse_pre(PREF&& pref)
    {
        traverse_dfs(std::forward<PREF>(pref), [](const auto&...) {});
    }

    //------------------------------------------------------------------------------------------------------------------
    // Post order traversal
    //------------------------------------------------------------------------------------------------------------------
    template<typename POSF>
    void traverse_post(POSF&& posf) const
    {
        traverse_dfs([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    template<typename POSF>
    void traverse_post(POSF&& posf)
    {
        traverse_dfs([](const auto&...) { return true; }, std::forward<POSF>(posf));
    }

    //------------------------------------------------------------------------------------------------------------------
    // Trie-map equality
    //------------------------------------------------------------------------------------------------------------------
    bool operator==(const this_type& oth) const
    {
        return m_data == oth.m_data && m_repo == oth.m_repo;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Trie-map ordering
    //------------------------------------------------------------------------------------------------------------------
    bool operator<(const this_type& oth) const
    {
        if (!(m_data == oth.m_data)) {
            return m_data < oth.m_data;
        }

        if (m_repo.size() != oth.m_repo.size()) {
            return m_repo.size() < oth.m_repo.size();
        }

        auto l = m_repo.begin();
        auto r = oth.m_repo.begin();

        for (; l != m_repo.end(); ++l, ++r) {
            if (*l != *r) {
                return *l < *r;
            }
        }

        return false;
    }

private:
    friend std::ostream& operator<<(std::ostream& os, const this_type& tm) {
        return os << O3::io::json::like(tm);
    }

    std::optional<data_type> m_data;
    repo_type                m_repo;
};

} // namespace details

//----------------------------------------------------------------------------------------------------------------------
// Ordered trie-map collection.
//----------------------------------------------------------------------------------------------------------------------
template<typename K, typename T>
using omap = std::map<K, T, std::less<>>;

template<typename DATA, typename PFIX, typename... PFIXS>
using otriemap = details::triemap<omap, DATA, PFIX, PFIXS...>;

//----------------------------------------------------------------------------------------------------------------------
// Unordered trie-map collection.
//----------------------------------------------------------------------------------------------------------------------
template<typename K, typename T>
using umap = std::unordered_map<K, T>;

template<typename DATA, typename PFIX, typename... PFIXS>
using utriemap = details::triemap<umap, DATA, PFIX, PFIXS...>;

} // namespace collection
} // namespace O3

#endif
