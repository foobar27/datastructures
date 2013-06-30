#pragma once

#include <boost/smart_ptr/detail/spinlock.hpp>
#include <list>
#include <mutex>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_list.hpp>

namespace {
  template<typename T>
  class ThreadLocal {
  public:// TODO remove!
    T m_counter;
    typename boost::ptr_list<T>::iterator m_counterPosition;
    //  friend class ::ThreadLocalCounter;
  };
}

template<typename T>
class ThreadLocalCounter {

  typedef boost::detail::spinlock lock_type;
  typedef std::lock_guard<lock_type> spinlock_guard;

  mutable lock_type m_writeLock;

  boost::ptr_list<T> m_counters;
  T m_finalCount = 0;
  boost::thread_specific_ptr<ThreadLocal<T>> tls;
public:

  void inc() {
    tls->m_counter++;
  }
  operator T() const {
    spinlock_guard lock(m_writeLock);
    //    spinlock_guard lock(m_writeLock);
    T sum = m_finalCount;
    for (auto c : m_counters)
      sum += c;
    return sum;
  }

  void registerThread() {
    spinlock_guard lock(m_writeLock);
    tls.reset(new ThreadLocal<T>());
    m_counters.push_front(&tls->m_counter);
    tls->m_counterPosition = m_counters.begin();
  }

  void unregisterThread() {
    spinlock_guard lock(m_writeLock);
    m_finalCount += tls->m_counter;
    m_counters.erase(tls->m_counterPosition);
  }

};
