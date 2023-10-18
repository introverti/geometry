/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/src/spin_mutex.cc
 */
#include "spin_mutex.h"

namespace innovusion {

void spin_mutex::lock() noexcept {
  while (flag_.test_and_set(std::memory_order_acquire))
    ;
}

void spin_mutex::unlock() noexcept { flag_.clear(std::memory_order_release); }

#if __cplusplus >= 202002L
bool spin_mutex::is_locked() const noexcept {
  return flag_.test(std::memory_order_acquire);
}
#endif  // c++20

}  // namespace innovusion
