#include <vector>

/**
 * Removes elements from an std::vector by swapping the element to remove with
 * the last element and then popping back, to void paying the cost of shifting
 * all the elements back one position. Use this if the order isn't important.
 */
template <typename T>
void removeBySwapAndShrink(std::vector<T>& vec, typename std::vector<T>::iterator it) {
    std::iter_swap(it, vec.end() - 1);
    vec.pop_back();
}

/*
 * Removes elements from an std::vector by swapping the element to remove with
 * the last element and then popping back, to void paying the cost of shifting
 * all the elements back one position. Use this if the order isn't important.
 * Returns true if the element was found and removed, false otherwise.
 */
template <typename T>
bool removeBySwapAndShrink(std::vector<T>& vec, const T& val) {
    auto it = std::find(vec.begin(), vec.end(), val);
    if (it != vec.end()) {
        std::iter_swap(it, vec.end() - 1);
        vec.pop_back();
        return true;
    }
    return false;
}
