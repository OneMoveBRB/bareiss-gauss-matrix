#pragma once

#include <utility>

#include "matrix.hpp"

namespace linear_algebra::details {

class GaussSolver {
 public:
  template <typename T, template <typename> class Storage>
  static T solve(SquareMatrix<T, Storage> matrix) {
    static_assert(std::is_floating_point_v<T>,
                  "GaussSolver requires floating-point type");

    const std::size_t size = matrix.size();
    if (size == 0) {
      return T{1};
    }

    T det = T{1};
    bool negate = false;

    for (std::size_t i = 0; i < size; ++i) {
      // Partial pivoting: find the element
      // with the largest absolute value in column i
      std::size_t pivot = i;
      for (std::size_t j = i + 1; j < size; ++j) {
        if (std::abs(matrix[j][i]) > std::abs(matrix[pivot][i])) {
          pivot = j;
        }
      }

      // Zero column — the matrix is singular
      if (std::abs(matrix[pivot][i]) < 1e-12) {
        return T{0};
      }

      // Swapping rows changes the sign of the determinant
      if (pivot != i) {
        matrix.swap_rows(i, pivot);
        negate = !negate;
      }

      const T diag = matrix[i][i];
      det *= diag;

      // Forward pass: zero out the elements below the diagonal
      for (std::size_t j = i + 1; j < size; ++j) {
        const T factor = matrix[j][i] / diag;
        for (std::size_t k = i + 1; k < size; ++k) {
          matrix[j][k] -= factor * matrix[i][k];
        }
      }
    }

    return negate ? -det : det;
  }
};

class BareissSolver {
 public:
  template <typename T, template <typename> class Storage>
  static T solve(SquareMatrix<T, Storage> matrix) {
    static_assert(std::is_integral_v<T>,
                  "BareissSolver requires integral type");

    const std::size_t size = matrix.size();
    if (size == 0) {
      return T{1};
    }

    if (size == 1) {
      return matrix[0][0];
    }

    T det = T{1};
    bool negate = false;
    T prev_pivot = T{1};
    T zero_elem = T{0};

    for (std::size_t k = 0; k + 1 < size; ++k) {
      // Partial pivoting: searching for a non-zero pivot element
      if (matrix[k][k] == zero_elem) {
        std::size_t pivot = k + 1;
        while (pivot < size && matrix[pivot][k] == zero_elem) {
          ++pivot;
        }

        if (pivot == size) {
          return zero_elem;
        }

        matrix.swap_rows(k, pivot);
        negate = !negate;
      }

      const T pivot = matrix[k][k];

      // Bareiss formula:
      // a[i][j] = (a[i][j] * pivot - a[i][k] * a[k][j]) / prev_pivot
      for (std::size_t i = k + 1; i < size; ++i) {
        for (std::size_t j = k + 1; j < size; ++j) {
          const T numerator =
              matrix[i][j] * pivot - matrix[i][k] * matrix[k][j];
          matrix[i][j] = numerator / prev_pivot;
        }
      }

      prev_pivot = pivot;
    }

    det = matrix[size - 1][size - 1];
    return negate ? -det : det;
  }
};

}  // namespace linear_algebra::details

namespace linear_algebra {

template <typename T>
using DefaultDetMethod =
    std::conditional_t<std::is_integral_v<T>, details::BareissSolver,
                       std::conditional_t<std::is_floating_point_v<T>,
                                          details::GaussSolver, void>>;

template <typename T, template <typename> class Storage,
          typename Method = DefaultDetMethod<T>>
T determinant(SquareMatrix<T, Storage> matrix) {
  static_assert(!std::is_void_v<Method>,
                "determinant: unsupported type T "
                "(expected integral or floating-point)");
  return Method::solve(std::move(matrix));
}

template <typename T, template <typename> class Storage>
T det(SquareMatrix<T, Storage> matrix) {
  return determinant(std::move(matrix));
}

}  // namespace linear_algebra
