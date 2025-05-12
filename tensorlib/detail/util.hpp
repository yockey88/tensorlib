/**
 * \file detail/util.hpp
 **/
#ifndef TENSORLIB_DETAIL_UTIL_HPP
#define TENSORLIB_DETAIL_UTIL_HPP

#include <concepts>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

namespace tensor {
  namespace detail {

    template <class F, class T, class I, class U>
    concept IndirectlyBinaryLeftFoldableImpl =
      std::movable<T> && std::movable<U> && std::convertible_to<T, U> &&
      std::invocable<F&, U, std::iter_reference_t<I>> && std::assignable_from<U&, std::invoke_result_t<F&, U, std::iter_reference_t<I>>>;

    template <class F, class T, class I>
    concept IndirectlyBinaryLeftFoldable =
      std::copy_constructible<F> && std::indirectly_readable<I> &&
      std::invocable<F&, T, std::iter_reference_t<I>> &&
      std::convertible_to<std::invoke_result_t<F&, T, std::iter_reference_t<I>>, std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>> &&
      IndirectlyBinaryLeftFoldableImpl<F, T, I, std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>>;

    struct fold_left_fn {
      template <std::input_iterator I, std::sentinel_for<I> S, class T = std::iter_value_t<I>, IndirectlyBinaryLeftFoldable<T, I> F>
      constexpr auto operator()(I first, S last, T init, F f) const {
        using U = std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
        if (first == last) {
          return U(std::move(init));
        }

        U accum = std::invoke(f, std::move(init), *first);
        for (++first; first != last; ++first) {
          accum = std::invoke(f, std::move(accum), *first);
        }
        return std::move(accum);
      }

      template <std::ranges::input_range R, class T = std::ranges::range_value_t<R>, IndirectlyBinaryLeftFoldable<T, std::ranges::iterator_t<R>> F>
      constexpr auto operator()(R&& r, T init, F f) const {
        return (*this)(std::ranges::begin(r), std::ranges::end(r), std::move(init), std::ref(f));
      }
    };

    constexpr inline fold_left_fn fold_left;

    std::vector<uint8_t> read_to_bytes(const std::string& filename);

  }  // namespace detail
}  // namespace tensor

#endif  // TENSORLIB_DETAIL_UTIL_HPP