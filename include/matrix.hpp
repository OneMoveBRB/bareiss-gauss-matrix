#pragma once

#include "storage.hpp"

namespace linear_algebra {

template <typename T,
          template <typename> class Storage = memory::ContiguousStorage>
class SquareMatrix {
 public:
  explicit SquareMatrix(std::size_t length) : storage_(length) {}

  SquareMatrix(std::size_t length, const T* contiguous_ptr) : storage_(length) {
    for (std::size_t i = 0; i < length; i++) {
      std::copy(contiguous_ptr + i * length, contiguous_ptr + (i + 1) * length,
                row(i));
    }
  }

  std::size_t size() const { return storage_.length(); }
  std::size_t length() const { return storage_.length(); }

  T* row(std::size_t idx) { return storage_.row(idx); }
  const T* row(std::size_t idx) const { return storage_.row(idx); }

  memory::RowView<T> operator[](std::size_t row_idx) {
    return storage_[row_idx];
  }
  memory::RowView<const T> operator[](std::size_t row_idx) const {
    return storage_[row_idx];
  }

  void swap_rows(std::size_t lhs_idx, std::size_t rhs_idx) {
    return storage_.swap_rows(lhs_idx, rhs_idx);
  }

 private:
  Storage<T> storage_;
};

}  // namespace linear_algebra
