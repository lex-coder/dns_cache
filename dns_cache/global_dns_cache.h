// По заданию было необходимо сделать класс DNSCache синглтоном, но для упрощения 
// тестирования было решено сделать синглтон - обертку global::DNSCache.
// Однако данный подход будет не удобен, если интерфейс DNSCache будет модифицироваться.

#pragma once

#include "dns_cache.h"

namespace global {
  struct DNSCache {
    using base_t = ::DNSCache;

    static base_t& instance(size_t max_size = 0) {
      static base_t inst(max_size);
      return inst;
    }

    static base_t& init(size_t max_size) {
      return instance(max_size);
    }

    void update(const std::string& name, const std::string& ip) {
      return instance().update(name, ip);
    }

    std::string resolve(const std::string& name) {
      return instance().resolve(name);
    }
  };
}
