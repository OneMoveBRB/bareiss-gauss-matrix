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
  explicit ContiguousStorage(std::size_t length)
      : length_(length), data_(length ? new T[length * length]() : nullptr) {}

  ContiguousStorage(const ContiguousStorage& other)
      : ContiguousStorage(other.length_) {
    std::copy(other.data_, other.data_ + length_ * length_, data_);
  }

  ContiguousStorage(ContiguousStorage&& other) noexcept
      : length_(other.length_), data_(other.data_) {
    other.length_ = 0;
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

  std::size_t size() const { return length_; }
  std::size_t length() const { return length_; }

  T* row(std::size_t idx) { return data_ + length_ * idx; }
  const T* row(std::size_t idx) const { return data_ + length_ * idx; }

  RowView<T> operator[](std::size_t row_idx) {
    return RowView<T>(row(row_idx));
  }
  RowView<const T> operator[](std::size_t row_idx) const {
    return RowView<const T>(row(row_idx));
  }

  void swap_rows(std::size_t lhs_idx, std::size_t rhs_idx) {
    std::swap_ranges(row(lhs_idx), row(lhs_idx) + length_, row(rhs_idx));
  }

  void swap(ContiguousStorage& other) noexcept {
    std::swap(length_, other.length_);
    std::swap(data_, other.data_);
  }

 private:
  std::size_t length_;
  T* data_;
};

template <typename T>
class JaggedStorage {
 public:
  explicit JaggedStorage(std::size_t length)
      : length_(length), rows_(length ? new T*[length]() : nullptr) {
    try {
      for (std::size_t i = 0; i < length_; ++i) {
        rows_[i] = new T[length_]();
      }
    } catch (...) {
      release();
      throw;
    }
  }

  JaggedStorage(const JaggedStorage& other) : JaggedStorage(other.length_) {
    for (std::size_t i = 0; i < length_; ++i) {
      std::copy(other.rows_[i], other.rows_[i] + length_, rows_[i]);
    }
  }

  JaggedStorage(JaggedStorage&& other) noexcept
      : length_(other.length_), rows_(other.rows_) {
    other.length_ = 0;
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

  std::size_t size() const { return length_; }
  std::size_t length() const { return length_; }

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
    std::swap(length_, other.length_);
    std::swap(rows_, other.rows_);
  }

 private:
  void release() noexcept {
    if (rows_ == nullptr) {
      return;
    }

    for (std::size_t i = 0; i < length_; ++i) {
      delete[] rows_[i];
    }

    delete[] rows_;
  }

  std::size_t length_;
  T** rows_;
};

} // namespace memory
