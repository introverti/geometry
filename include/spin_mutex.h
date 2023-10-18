/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/spin_mutex.h
 */
#pragma once
#include <atomic>

namespace innovusion {

class spin_mutex {
 private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

 public:
  // lock once created
  spin_mutex() = default;
  virtual ~spin_mutex(){};
  // delete default constructor
  explicit spin_mutex(bool) = delete;
  spin_mutex(const spin_mutex&) = delete;
  // delete move constructor
  spin_mutex(spin_mutex&&) = delete;
  // delete copy assignment
  spin_mutex& operator=(const spin_mutex&) = delete;
  // delete move assignment
  spin_mutex& operator=(spin_mutex&&) = delete;

  void lock() noexcept;
  void unlock() noexcept;

#if __cplusplus >= 202002L
  bool is_locked() const noexcept;
#endif  // c++20
};

};  // namespace innovusion
