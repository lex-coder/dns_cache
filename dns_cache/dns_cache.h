#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>

class DNSCache {
public:
  using key_type = std::string;
  using value_type = std::string;

  struct element_t {
    key_type key;
    value_type value;
    element_t(const key_type& k, const value_type& v)
      : key(k), value(v)
    {}
    void set(const key_type& k, const value_type& v) {
      key = k;
      value = v;
    };
  };

  using list_t = std::list<element_t>;
  using cache_element_t = typename list_t::iterator;
  using index_t = std::unordered_map<key_type, cache_element_t>;

  using mutex_t = std::mutex;

  explicit DNSCache(size_t max_size);

  void update(const std::string& name, const std::string& ip);
  std::string resolve(const std::string& name);

  size_t size() const {
    return list_.size();
  }

  size_t capacity() const {
    return capacity_;
  }

protected:
  size_t capacity_;
  list_t list_;
  index_t index_;
  mutex_t mutex_;
};