/**
 * \file math/n_choose_k.cpp
 **/
#include "math/n_choose_k.hpp"

#include <vector>

namespace tensor {
  namespace math {

    natural_t n_choose_k(natural_t n, natural_t k) {
      if (k > n) {
        return 0;
      }
      if (k == 0 || k == n) {
        return 1;
      }
      if (k > n / 2) {
        k = n - k;
      }
      static std::vector<std::vector<tensor::natural_t>> binomials = {
        { 1 },
        { 1, 1 },
        { 1, 2, 1 },
        { 1, 3, 3, 1 },
        { 1, 4, 6, 4, 1 },
        { 1, 5, 10, 10, 5, 1 },
        { 1, 6, 15, 20, 15, 6, 1 },
        { 1, 7, 21, 35, 35, 21, 7, 1 },
        { 1, 8, 28, 56, 70, 56, 28, 8, 1 },
      };

      while (n >= binomials.size()) {
        tensor::natural_t s = binomials.size();

        std::vector<tensor::natural_t> next_row;
        next_row.resize(s + 1);

        next_row[0] = 1;
        for (tensor::natural_t i = 1; i < s; ++i) {
          next_row[i] = binomials[s - 1][i - 1] + binomials[s - 1][i];
        }
        next_row[s] = 1;

        binomials.push_back(next_row);
      }
      return binomials[n][k];
    }

  }  // namespace math
}  // namespace tensor