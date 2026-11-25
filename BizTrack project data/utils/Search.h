#pragma once
// ---------------------------------------------------------------
// Searching algorithms (generic).
//  linearSearch : checks elements one by one       -> O(n), works on UNSORTED data
//  binarySearch : halves the range each step       -> O(log n), needs SORTED data
// Both return the index of the match, or -1. The optional 'comparisons'
// pointer counts how many elements were examined (used in the Lab).
// ---------------------------------------------------------------
#include "structures/DynamicArray.h"

template <typename T, typename Pred>
int linearSearch(const DynamicArray<T>& a, Pred matches, long* comparisons = nullptr) {
    for (int i = 0; i < a.size(); i++) {
        if (comparisons) (*comparisons)++;
        if (matches(a[i])) return i;
    }
    return -1;
}

// getKey(element) must return the field the array is sorted by (ascending).
template <typename T, typename K, typename KeyFn>
int binarySearch(const DynamicArray<T>& a, const K& key, KeyFn getKey, long* comparisons = nullptr) {
    int lo = 0, hi = a.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (comparisons) (*comparisons)++;
        const auto& k = getKey(a[mid]);
        if (key == k) return mid;
        if (key < k) hi = mid - 1;
        else lo = mid + 1;
    }
    return -1;
}
