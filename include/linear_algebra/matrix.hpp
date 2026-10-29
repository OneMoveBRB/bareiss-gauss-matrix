#pragma once

#include <utility>

#include "storage.hpp"

namespace linear_algebra {

template <typename T,
          template <typename> class Storage = memory::ContiguousStorage>
class SquareMatrix {
 public:
  explicit SquareMatrix(std::size_t size) : storage_(size) {}

  SquareMatrix(std::size_t size, const T* contiguous_ptr) : storage_(size) {
    for (std::size_t i = 0; i < size; i++) {
      std::copy(contiguous_ptr + i * size, contiguous_ptr + (i + 1) * size,
                row(i));
    }
  }

  SquareMatrix(const SquareMatrix& other) : storage_(other.storage_) {}
  SquareMatrix(SquareMatrix&& other) noexcept
      : storage_(std::move(other.storage_)) {}

  ~SquareMatrix() = default;

  SquareMatrix& operator=(const SquareMatrix& other) {
    SquareMatrix temp_matrix = other;
    swap(temp_matrix);
    return *this;
  }

  SquareMatrix& operator=(SquareMatrix&& other) noexcept {
    swap(other);
    return *this;
  }

  std::size_t size() const { return storage_.size(); }

  T* row(std::size_t idx) { return storage_.row(idx); }
  const T* row(std::size_t idx) const { return storage_.row(idx); }

  memory::RowView<T> operator[](std::size_t row_idx) {
    return storage_[row_idx];
  }
  memory::RowView<const T> operator[](std::size_t row_idx) const {
    return storage_[row_idx];
  }

  void swap_rows(std::size_t lhs_idx, std::size_t rhs_idx) {
    storage_.swap_rows(lhs_idx, rhs_idx);
  }

  void swap(SquareMatrix& other) noexcept {
    std::swap(storage_, other.storage_);
  }

 private:
  Storage<T> storage_;
};

}  // namespace linear_algebra
