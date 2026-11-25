#pragma once
// ---------------------------------------------------------------
// Sorting algorithms (generic, take a comparator: less(a, b) == "a goes before b").
//  insertionSort : O(n^2) worst/average, simple, fast on tiny/nearly-sorted data
//  mergeSort     : O(n log n) always, RECURSIVE divide and conquer, needs O(n) extra space
// ---------------------------------------------------------------
#include "structures/DynamicArray.h"

template <typename T, typename Less>
void insertionSort(DynamicArray<T>& a, Less less) {
    for (int i = 1; i < a.size(); i++) {
        T key = a[i];
        int j = i - 1;
        while (j >= 0 && less(key, a[j])) {      // shift larger elements one step right
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

template <typename T, typename Less>
void mergeSortRec(DynamicArray<T>& a, DynamicArray<T>& tmp, int lo, int hi, Less less) {
    if (lo >= hi) return;                         // base case: 0 or 1 element
    int mid = lo + (hi - lo) / 2;
    mergeSortRec(a, tmp, lo, mid, less);          // sort left half
    mergeSortRec(a, tmp, mid + 1, hi, less);      // sort right half
    int i = lo, j = mid + 1, k = lo;              // merge the two sorted halves
    while (i <= mid && j <= hi) {
        if (!less(a[j], a[i])) tmp[k++] = a[i++]; // <= keeps the sort stable
        else                   tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi)  tmp[k++] = a[j++];
    for (int x = lo; x <= hi; x++) a[x] = tmp[x];
}

template <typename T, typename Less>
void mergeSort(DynamicArray<T>& a, Less less) {
    if (a.size() < 2) return;
    DynamicArray<T> tmp(a.size());
    for (int i = 0; i < a.size(); i++) tmp.push_back(a[i]);   // tmp needs the same length as a
    mergeSortRec(a, tmp, 0, a.size() - 1, less);
}
