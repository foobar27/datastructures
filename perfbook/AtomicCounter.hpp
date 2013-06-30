#pragma once

#include <atomic>

template<typename T>
class AtomicCounter {
  std::atomic<T> m_counter;
public:
  void inc() {
    m_counter++;
  }
  operator T() const {
    return m_counter;
  }
  void registerThread() {}
  void unregisterThread() {}
};
