#pragma once
#include <cstdint>
#include <functional>
#include <numeric>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace kodanuki
{

/**
 * An implemenation of the Van Emde Boas Tree.
 * 
 * This is a data structure that implements associative array methods, as well
 * as in order element search. This includes the following functions:
 *     - get_min() / get_max()
 *     - get_next() / get_prev()
 *     - insert() / remove()
 *     - contains()
 * 
 * The vebtree has good performance on all operations but requires linear
 * space. It can store any type which can be mapped uniquely to integer types.
 * Ensure that the size of the vebtree is not too large. The required storage
 * is given by O(sizeof(T) * size).
 * 
 * Note: Using not integer types requires changing the default parameters.
 * 
 * @param T The type of values that this tree stores.
 * @param size The maximum size of the universe.
 * @param order The total ordering of the unmapped values.
 */
template <typename T, uint64_t size, auto order = [](T x){ return static_cast<uint64_t>(x); }>
class Vebtree
{
private:
    template <typename U, uint64_t S, auto O>
    friend class Vebtree;

    static constexpr uint64_t full_size = std::bit_ceil(size);
    static constexpr uint64_t high_bits = std::countr_zero(full_size) / 2;
    static constexpr uint64_t sqrt_size = 1 << high_bits;
    static constexpr uint64_t low_mask  = sqrt_size - 1;
    static constexpr uint64_t high_mask = full_size - low_mask - 1;
    static constexpr bool is_leaf       = full_size <= 1;

    using ClusterType = std::vector<Vebtree<T, sqrt_size, order>>;
    using SummaryType = Vebtree<T, sqrt_size, order>;

public:
    /**
     * Constructs a new vebtree with zero elements.
     */
    Vebtree()
    {
        if constexpr (!is_leaf) {
            /*
            TODO: figure out the correct index

            So the +1 is normally not needed, but in some cases the tree
            segfaults inside the destructor. I think it has something to
            do with the different memory for trees of size 1 and n > 1 and
            the memory alignment. However, its been hours and I have no
            idea how to fix that, so i'll keep this hack for now :D
             */
            cluster.resize(sqrt_size + 1);
        }  
    }

    /**
     * Returns the minimum value of the vebtree.
     *
     * The complexity is O(1) each vebtree stores the minimum value
     * directly. The minimum value depends on the given total order.
     *
     * @return The minimum value of the vebtree.
     */
    std::optional<T> get_min() const
    {
        return min_value;
    }

    /**
     * Returns the maximum value of the vebtree.
     *
     * The complexity is O(1) each vebtree stores the maximum value
     * directly. The maximum value depends on the given total order.
     *
     * @return The maximum value of the vebtree.
     */
    std::optional<T> get_max() const
    {
        return max_value;
    }

    /**
     * Returns the next value that is inside the vebtree.
     *
     * The given value must not be inside the tree. The next value might
     * include the given value. The complexity is O(log(log(size))) since
     * we search one subtree. The succussor value depends on the given
     * total order.
     *
     * @param value The value from which to find the next value.
     * @return The next value that is inside the vebtree.
     */
    std::optional<T> get_next(T value) const
    {
        return get_next_impl(order(value)).first;
    }
    
    /**
     * Returns the previous value that is inside the vebtree.
     *
     * The given value must not be inside the tree. The prev value might
     * include the given value. The complexity is O(log(log(size))) since
     * we search one subtree. The precursor value depends on the given
     * total order.
     *
     * @param value The value from which to find the prev value.
     * @return The previous value that is inside the vebtree.
     */
    std::optional<T> get_prev(T value) const
    {
        return get_prev_impl(order(value)).first;
    }
    
    /**
     * Inserts the value inside the vebtree.
     *
     * Does nothing if the value is already inside the vebtree. The
     * complexity is O(log(log(size))) since we search one subtree.
     *
     * @param value The value which to insert.
     */
    void insert(T value)
    {
        insert_impl(value, order(value));
    }
    
    /**
     * Removes the value inside the vebtree.
     *
     * Does nothing if the value is not inside the vebtree. The
     * complexity is O(log(log(size))) since we search one subtree.
     *
     * @param value The value which to remove.
     */
    void remove(T value)
    {
        remove_impl(order(value));
    }
    
    /**
     * Returns true iff the value is inside the vebtree.
     *
     * @param value The value for which to check.
     * @return Is the value inside the vebtree?
     */
    bool contains(T value) const
    {
        return contains_impl(order(value));
    }

private:
    /**
     * Returns true iff the vebtree contains zero elements.
     *
     * @return Is the vebtree empty?
     */
    bool empty() const
    {
        return !min_value.has_value();
    }

    /**
     * Splits the bits of the given index into two parts.
     *
     * hi = floor(position / sqrt(full_size))
     * lo = x mod sqrt(full_size)
     *
     * @param index The index for which to split the bits.
     * @return The pair containing the high and low bits.
     */
    static std::pair<uint64_t, uint64_t> split_bits(uint64_t index)
    {
        uint64_t hi = (high_mask & index) >> high_bits;
        uint64_t lo = low_mask & index;
        return {hi, lo};
    }

    /**
     * Concatenates the bits from the two part into one index.
     * 
     * index = sqrt(full_size) * hi + lo
     * 
     * This essentially is the reverse operation of split_bits().
     * 
     * @param hi The higher bits of the index.
     * @param lo The lower bits of the index.
     * @return The single index containing the bits of hi and lo.
     */
    static uint64_t concat_bits(uint64_t hi, uint64_t lo)
    {
        return (hi << high_bits) + lo;
    }

private:
    /**
     * Returns the next value and order inside the subtree.
     *
     * To find the next value we split the current position into the high
     * and low bits. We need to binary search the cluster for the current
     * position, then the summary, and then the cluster for the index found
     * inside the summary. Because we keep track of the min and max values
     * we have to binary search at most one of these.
     *
     * The time complexity is O(log(log(full_size))).
     *
     * @param position The position for the given value inside the tree.
     * @return The pair containing the next value and their order.
     */
    std::pair<std::optional<T>, uint64_t> get_next_impl(uint64_t position) const
    {
        if (empty()) {
            return {std::nullopt, size};
        }
        if (position < min_order) {
            return {min_value, min_order};
        }
        if (position >= max_order) {
            return {std::nullopt, size};
        }
        if constexpr (!is_leaf) {
            auto[hi, lo] = split_bits(position);
            if (lo < cluster[hi].max_order && !cluster[hi].empty()) {
                auto[next_value, next_order] = cluster[hi].get_next_impl(lo);
                return {next_value, concat_bits(hi, next_order)};
            } else {
                auto[_, next_order] = summary.get_next_impl(hi);
                return {cluster[next_order].min_value, concat_bits(next_order, cluster[next_order].min_order)};
            }
        }
        return {min_value, min_order};
    }

    /**
     * Returns the prev value and order inside the subtree.
     *
     * To find the prev value we split the current position into the high
     * and low bits. We need to binary search the cluster for the current
     * position, then the summary, and then the cluster for the index found
     * inside the summary. Because we keep track of the min and max values
     * we have to binary search at most one of these.
     *
     * This version is a little more complicated than get_prev_impl() since
     * the max value can either be inside a subtree or the current one. We
     * also have to make sure that we never use the min_order and max_order
     * if they are invalid. This is because the algorithm is supposed to use
     * signed ints, but we choose unsigned because it made bitshifts easier.
     *
     * The time complexity is O(log(log(full_size))).
     *
     * @param position The position for the given value inside the tree.
     * @return The pair containing the prev value and their order.
     */
    std::pair<std::optional<T>, uint64_t> get_prev_impl(uint64_t position) const
    {
        if (empty()) {
            return {std::nullopt, -1}; // -1 as dummy, can never be used!
        }
        if (position > max_order) {
            return {max_value, max_order};
        }
        if (position <= min_order) {
            return {std::nullopt, -1};
        }
        if constexpr (!is_leaf) {
            auto[hi, lo] = split_bits(position);
            if (lo > cluster[hi].min_order && !cluster[hi].empty()) {
                auto[prev_value, prev_order] = cluster[hi].get_prev_impl(lo);
                if (!prev_value.has_value()) {
                    return {min_value, min_order};
                }
                return {prev_value, concat_bits(hi, prev_order)};
            } else {
                auto[prev_value, prev_order] = summary.get_prev_impl(hi);
                if (!prev_value.has_value()) {
                    return {min_value, min_order};
                }
                return {cluster[prev_order].max_value, concat_bits(prev_order, cluster[prev_order].max_order)};
            }
        }
        return {max_value, max_order};
    }

    /**
     * Inserts the value inside the correct subtree.
     * 
     * To find the correct place we split the current position into the high
     * and low bits. We recursively go into the correct subtree and insert
     * the value there. If the subtree was previously empty, we also insert
     * the value inside the summary. We also update the min and max value.
     *
     * The tree stores unique values, inserting twice does nothing.
     *
     * The time complexity is O(log(log(full_size))).
     *
     * @param value The value which to insert.
     * @param position The position for the given value inside the tree.
     */
    void insert_impl(std::optional<T> value, uint64_t position)
    {
        if (empty()) {
            min_order = max_order = position;
            min_value = max_value = value;
            return;
        }
        if (position < min_order) {
            std::swap(position, min_order);
            std::swap(value, min_value);
        }
        if (position > max_order) {
            max_order = position;
            max_value = value;
        }
        if constexpr (!is_leaf) {
            auto[hi, lo] = split_bits(position);
            bool was_empty = cluster[hi].empty();
            cluster[hi].insert_impl(value, lo);
            if (was_empty) {
                summary.insert_impl(value, hi);
            }
        }
    }

    /**
     * Deletes the value inside the correct subtree.
     * 
     * To find the correct place we split the current position into the high
     * and low bits. We recursively go into the correct subtree and delete
     * the value there. If the subtree is now empty, we remove the value from
     * the summary too. We also update the min and max value.
     *
     * Removing non-existing elements does nothing.
     *
     * The time complexity is O(log(log(full_size))).
     *
     * @param position The position for the given value inside the tree.
     */
    void remove_impl(uint64_t position)
    {
        if (empty()) {
            return;
        }
        if (position == min_order && position == max_order) {
            min_order = size;
            max_order = -1;
            min_value = {};
            max_value = {};
            return;
        }
        if constexpr (!is_leaf) {
            if (position == min_order) {
                uint64_t lo = cluster[summary.min_order].min_order;
                min_value = summary.min_value;
                min_order = position = concat_bits(summary.min_order, lo);
            }
            auto[hi, lo] = split_bits(position);
            cluster[hi].remove_impl(lo);
            if (cluster[hi].empty()) {
                summary.remove_impl(hi);
            }
            if (position == max_order) {
                if (summary.empty()) {
                    max_order = min_order;
                    max_value = min_value;
                } else {
                    uint64_t prev_hi = summary.max_order;
                    max_order = concat_bits(prev_hi, cluster[prev_hi].max_order);
                    max_value = cluster[prev_hi].max_value;
                }
            }
        }
    }

    /**
     * Returns true iff the value is inside the vebtree.
     *
     * To find the correct place we split the current position into the high
     * and low bits. We recursively go into the correct subtree and check if
     * the value is inside the subtree. We also have some base cases where
     * we know when the value is in the tree or not.
     *
     * The time complexity is O(log(log(full_size))).
     *
     * @param position The position for the given value inside the tree.
     * @return Is the value inside the vebtree?
     */
    bool contains_impl(uint64_t position) const
    {
        if (empty()) {
            return false;
        }
        if (position < min_order || position > max_order) {
            return false;
        }
        if (position == min_order || position == max_order) {
            return true;
        }
        if constexpr (is_leaf) {
            return false;
        } else {
            auto[hi, lo] = split_bits(position);
            return cluster[hi].contains_impl(lo);
        }
    }

private:
    // The order inside the tree which the min value has.
    uint64_t min_order;

    // The order inside the tree which the max value has.
    uint64_t max_order;

    // The min value of the current tree node.
    std::optional<T> min_value;

    // The max value of the current tree node.
    std::optional<T> max_value;

    // The subtree cluster of smaller sizes (lo-index).
    std::conditional_t<is_leaf, bool, ClusterType> cluster;

    // The summary of the cluster subtrees (hi-index).
    std::conditional_t<is_leaf, bool, SummaryType> summary;
};

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
