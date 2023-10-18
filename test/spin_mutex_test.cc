/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/test/spin_mutex_test.cc
 */
#include "spin_mutex.h"

#include <gtest/gtest.h>

#include "thread"
using innovusion::spin_mutex;

TEST(SpinMutexTest, Initialization) {
  spin_mutex mutex;
  mutex.lock();
  mutex.unlock();
}

// Tests that multiple threads can acquire and release the lock without any
// conflicts.
TEST(SpinMutexTest, MultipleThreads) {
  spin_mutex mutex;
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&mutex]() {
      mutex.lock();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mutex.unlock();
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

// Tests that the lock and unlock functions can handle multiple threads trying
// to acquire and release the lock simultaneously.
TEST(SpinMutexTest, MultipleThreadsContention) {
  spin_mutex mutex;
  std::atomic<int> counter = 0;
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&mutex, &counter]() {
      for (int j = 0; j < 100; ++j) {
        mutex.lock();
        counter++;
        mutex.unlock();
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
  EXPECT_EQ(counter, 1000);
}

// Tests that the lock and unlock functions can handle multiple threads, some of
// which are blocked, without any issues.
TEST(SpinMutexTest, MultipleThreadsBlocked) {
  spin_mutex mutex;
  std::atomic<int> counter = 0;
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&mutex, &counter]() {
      if (counter < 5) {
        mutex.lock();
        counter++;
        mutex.unlock();
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
  EXPECT_EQ(counter, 5);
}

// Tests that the lock and unlock functions work correctly with different memory
// orders.
TEST(SpinMutexTest, MemoryOrder) {
  spin_mutex mutex;
  std::atomic<int> counter = 0;
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&mutex, &counter]() {
      for (int j = 0; j < 100; ++j) {
        mutex.lock();
        counter.fetch_add(1, std::memory_order_relaxed);
        mutex.unlock();
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
  EXPECT_EQ(counter, 1000);
}
