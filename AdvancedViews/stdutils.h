 #pragma once

#include <functional>

namespace stdutils {

auto reduce(const auto& container, const auto& callable, const auto initialValue) {
    auto result = initialValue;
    for (const auto& element : container)
        result += std::invoke(callable, element);
    return result;
}

void remove_if(auto& container, const auto& callable) {
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

}
