 #pragma once

#include <functional>

namespace stdutils {

template<typename Container, typename Callable, typename Value>
auto reduce(const Container& container, const Callable& callable, const Value& initialValue) {
    auto result = initialValue;
    for (const auto& element : container)
        result += std::invoke(callable, element);
    return result;
}

template<typename Container, typename Callable>
void remove_if(Container& container, const Callable& callable) {
    auto first = std::begin(container);
    auto last = std::end(container);
    for (; first != last; ++first)
        if (std::invoke(callable, *first))
            break;
    if (first == container.end())
        return;
    for (auto it = std::next(first); it != last; ++it) {
        if (!std::invoke(callable, *it)) {
            std::iter_swap(it, first);
            ++first;
        }
    }
    container.erase(first, std::end(container));
}

template<typename Container, typename Callable>
void for_each(const Container& container, const Callable& callable) {
    for (const auto& element : container)
        std::invoke(callable, element);
}

}
