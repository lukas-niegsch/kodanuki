#pragma once
#include "engine/nekolib/container/vebtree.h"
#include <optional>
#include <vector>

namespace kodanuki
{

/**
 * Sorts the given items by the given order.
 *
 * This generates the vebtree from the items and then iterates over the
 * vebtree by getting the minimum item and then repeatedly getting the
 * successor item.
 *
 * Since each tree operation is O(log(log(size))) it gives us complexity
 * of O(n * log(log(size))) where n is number of items.
 *
 * @param T The type of values that the tree stores.
 * @param size The maximum size of the universe.
 * @param order The total ordering of the unmapped values.
 * @param items The items that should be sorted.
 */
template <uint64_t size, typename T, auto order = [](T x){ return static_cast<uint64_t>(x); }>
void vebsort(std::vector<T>& items)
{
    if (items.empty()) {
        return;
    }
    Vebtree<T, size, order> tree;
    for (auto& item : items) {
        tree.insert(item);
    }
    std::optional<T> next_item = tree.get_min();
    items[0] = next_item.value();
    for (int i = 1; i < (int) items.size(); i++) {
        next_item = tree.get_next(items[i - 1]);
        items[i] = next_item.value();
    }
}

}
