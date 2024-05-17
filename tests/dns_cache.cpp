#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <dns_cache.h>
#include <future>
#include <array>
#include <random>
#include <ctime>

using input_container = std::vector<std::pair<std::string, std::string>>;

struct generator_t {
  std::mt19937 random;
  generator_t() {
    random.seed(static_cast<uint32_t>(std::time(nullptr)));
  }

  input_container operator()(const std::string prefix, size_t count) {
    input_container values;
    for (size_t i = 0; i < count; i++) {
      values.emplace_back(name(prefix), value());
    }
    return values;
  }

  std::pair<std::string, std::string> operator()(const std::string prefix) {
    return {name(prefix), value()};
  }

private:
  std::string name(const std::string prefix) {
    return prefix + "." + std::to_string(random()%256) + ".com";
  }

  std::string value() {
    return std::to_string(random());
  }
};

TEST_CASE("dns_cache") {
  constexpr size_t capacity = 10;
  DNSCache cache(capacity);
  
  SECTION("simple") {
    input_container input = {
      {"example1.com", "1.1.1.1"},
      {"example2.com", "1.1.1.2"},
      {"example3.com", "1.1.1.3"},
      {"example4.com", "1.1.1.4"},
      {"example5.com", "1.1.1.5"},
      {"example1.com", "1.1.1.6"},
      {"example6.com", "1.1.1.6"},
      {"example7.com", "1.1.1.7"},
      {"example8.com", "1.1.1.8"},
      {"example9.com", "1.1.1.9"},
      {"example10.com", "1.1.1.10"},
      {"example11.com", "1.1.1.11"},
      {"example12.com", "1.1.1.12"},
      {"example13.com", "1.1.1.13"},
      {"example10.com", "1.1.1.14"},
    };

    for (const auto& i : input) {
        cache.update(i.first, i.second);
        auto address = cache.resolve(i.first);
        REQUIRE(cache.size() <= capacity);
        REQUIRE(address == i.second);
    }
  }

  SECTION("multithreaded") {
    using result_t = std::vector<bool>;

    std::array<input_container, 2> input = {
      input_container {
        {"thread1.1.com", "1.1.1.1"},
        {"thread1.2.com", "1.1.1.2"},
        {"thread1.3.com", "1.1.1.3"},
        {"thread1.4.com", "1.1.1.4"},
        {"thread1.5.com", "1.1.1.5"},
        {"thread1.1.com", "1.1.1.6"},
        {"thread1.6.com", "1.1.1.7"},
        {"thread1.7.com", "1.1.1.8"},
        {"thread1.8.com", "1.1.1.9"},
        {"thread1.9.com", "1.1.1.10"},
        {"thread1.10.com", "1.1.1.11"},
        {"thread1.11.com", "1.1.1.12"},
        {"thread1.12.com", "1.1.1.13"},
        {"thread1.13.com", "1.1.1.14"},
        {"thread1.10.com", "1.1.1.15"},
      },
      input_container {
        {"thread2.1.com", "1.1.1.1"},
        {"thread2.2.com", "1.1.1.2"},
        {"thread2.3.com", "1.1.1.3"},
        {"thread2.4.com", "1.1.1.4"},
        {"thread2.5.com", "1.1.1.5"},
        {"thread2.1.com", "1.1.1.6"},
        {"thread2.6.com", "1.1.1.7"},
        {"thread2.7.com", "1.1.1.8"},
        {"thread2.8.com", "1.1.1.9"},
        {"thread2.9.com", "1.1.1.10"},
        {"thread2.10.com", "1.1.1.11"},
        {"thread2.11.com", "1.1.1.12"},
        {"thread2.12.com", "1.1.1.13"},
        {"thread2.13.com", "1.1.1.14"},
        {"thread2.10.com", "1.1.1.15"},
      }
    };

    auto multithreaded_check = [&cache](const input_container& input) {
      result_t results;
      for (const auto& i : input) {
        cache.update(i.first, i.second);
        auto address = cache.resolve(i.first);
        results.emplace_back(address == i.second);
      }
      return results;
    };

    std::vector<std::future<result_t>> features;
    for (const auto& set: input) {
      features.emplace_back(std::async(std::launch::async, multithreaded_check, set));
    }
    
    for (const auto& f : features) f.wait();

    for (auto& f : features) {
      for (bool r : f.get()) {
        REQUIRE(r == true);
      }
    }

  }
}

TEST_CASE("benchmark") {
  generator_t generator;

  DNSCache cache(100);
  std::array<input_container, 2> input = {
    generator("thread1", 10000),
    generator("thread2", 10000),
  };

  std::atomic_bool start = false;
  auto update_task = [&cache, &start](const input_container& input) {
    while (!start);
    for (const auto& i : input) {
      cache.update(i.first, i.second);
    }
  };

  BENCHMARK("update cache") {
    std::vector<std::future<void>> features;
    for (const auto& set: input) {
      features.emplace_back(std::async(std::launch::async, update_task, set));
    }
    start = true;
    for (const auto& f : features) f.wait();
  };
}