#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <utility>

namespace memory {

template <typename T>
class RowView {
 public:
  RowView(T* data) noexcept : ptr_(data) {}

  T& operator[](std::size_t j) { return ptr_[j]; }
  const T& operator[](std::size_t j) const { return ptr_[j]; }

 private:
  T* ptr_;
};

template <typename T>
class ContiguousStorage {
 public:
  explicit ContiguousStorage(std::size_t size)
      : size_(size), data_(size ? new T[size * size]() : nullptr) {}

  ContiguousStorage(const ContiguousStorage& other)
      : ContiguousStorage(other.size_) {
    std::copy(other.data_, other.data_ + size_ * size_, data_);
  }

  ContiguousStorage(ContiguousStorage&& other) noexcept
      : size_(other.size_), data_(other.data_) {
    other.size_ = 0;
    other.data_ = nullptr;
  }

  ~ContiguousStorage() { delete[] data_; }

  ContiguousStorage& operator=(const ContiguousStorage& other) {
    ContiguousStorage temp_storage(other);
    swap(temp_storage);
    return *this;
  }

  ContiguousStorage& operator=(ContiguousStorage&& other) noexcept {
    ContiguousStorage temp_storage(std::move(other));
    swap(temp_storage);
    return *this;
  }

  std::size_t size() const { return size_; }

  T* row(std::size_t idx) { return data_ + size_ * idx; }
  const T* row(std::size_t idx) const { return data_ + size_ * idx; }

  RowView<T> operator[](std::size_t row_idx) {
    return RowView<T>(row(row_idx));
  }
  RowView<const T> operator[](std::size_t row_idx) const {
    return RowView<const T>(row(row_idx));
  }

  void swap_rows(std::size_t lhs_idx, std::size_t rhs_idx) {
    std::swap_ranges(row(lhs_idx), row(lhs_idx) + size_, row(rhs_idx));
  }

  void swap(ContiguousStorage& other) noexcept {
    std::swap(size_, other.size_);
    std::swap(data_, other.data_);
  }

 private:
  std::size_t size_;
  T* data_;
};

template <typename T>
class JaggedStorage {
 public:
  explicit JaggedStorage(std::size_t size)
      : size_(size), rows_(size ? new T*[size]() : nullptr) {
    try {
      for (std::size_t i = 0; i < size_; ++i) {
        rows_[i] = new T[size_]();
      }
    } catch (...) {
      release();
      throw;
    }
  }

  JaggedStorage(const JaggedStorage& other) : JaggedStorage(other.size_) {
    for (std::size_t i = 0; i < size_; ++i) {
      std::copy(other.rows_[i], other.rows_[i] + size_, rows_[i]);
    }
  }

  JaggedStorage(JaggedStorage&& other) noexcept
      : size_(other.size_), rows_(other.rows_) {
    other.size_ = 0;
    other.rows_ = nullptr;
  }

  ~JaggedStorage() { release(); }

  JaggedStorage& operator=(const JaggedStorage& other) {
    JaggedStorage temp_storage(other);
    swap(temp_storage);
    return *this;
  }

  JaggedStorage& operator=(JaggedStorage&& other) noexcept {
    JaggedStorage temp_storage(std::move(other));
    swap(temp_storage);
    return *this;
  }

  std::size_t size() const { return size_; }

  T* row(std::size_t idx) { return rows_[idx]; }
  const T* row(std::size_t idx) const { return rows_[idx]; }

  RowView<T> operator[](std::size_t row_idx) {
    return RowView<T>(row(row_idx));
  }
  RowView<const T> operator[](std::size_t row_idx) const {
    return RowView<const T>(row(row_idx));
  }

  void swap_rows(std::size_t lhs_idx, std::size_t rhs_idx) {
    std::swap(rows_[lhs_idx], rows_[rhs_idx]);
  }

  void swap(JaggedStorage& other) noexcept {
    std::swap(size_, other.size_);
    std::swap(rows_, other.rows_);
  }

 private:
  void release() noexcept {
    if (rows_ == nullptr) {
      return;
    }

    for (std::size_t i = 0; i < size_; ++i) {
      delete[] rows_[i];
    }

    delete[] rows_;
  }

  std::size_t size_;
  T** rows_;
};

} // namespace memory
