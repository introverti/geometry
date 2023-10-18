/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/pre_define.h
 */
#pragma once

// noreturn c++11
#if __cplusplus >= 201103L
#define NORETURN [[noreturn]]
#else
#define NORETURN
#endif

// nodiscard c++17
#if __cplusplus >= 201703L
#define NODISCARD [[nodiscard]]
// GCC/Clang
#elif defined(__GNUC__) || defined(__clang__)
#define NODISCARD __attribute__((warn_unused_result))
// MSVC
#elif defined(_MSC_VER)
#define NODISCARD _Check_return_
#else
#define NODISCARD
#endif

// likely c++20
#if __cplusplus >= 202002L
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]
#else
#define LIKELY
#define UNLIKELY
#endif
