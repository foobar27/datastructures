#pragma once

#include <urcu.h>

template<typename T>
class UrcuAtomicCounter {
  T m_counter;
public:
  void inc() {
    uatomic_inc(&m_counter);
  }
  operator T() const {
    return uatomic_read(&m_counter);
  }
  void registerThread() {}
  void unregisterThread() {}
};
