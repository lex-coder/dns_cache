#include "dns_cache.h"

DNSCache::DNSCache(size_t max_size)
  : capacity_(max_size)
  , index_(capacity_)
{}

// // Вариант оптимизированный по доступу к unordered_map.
// void DNSCache::update(const std::string& name, const std::string& ip) {
//   std::unique_lock<shared_mutex_t> lock(mutex_);
//   auto [it, done] = index_.try_emplace(name, cache_element_t{});
//   if (done) {   // Добавление нового элемента в кэш.
//     if (list_.size() >= capacity) {  // Чаще будет происходить ситуация, когда кэш заполнен.
//       list_.splice(list_.begin(), list_, std::next(list_.rbegin()).base()); // Перемещение последнего элемента списка в начало.
//       key_type tmp_key = std::move(list_.begin()->key);  // Сохранение ключа вытесняемого элемента, для последующего удаления из индекса.
//       it->second = list_.begin();                        // Заполнение индекса.
//       it->second->set(name, ip);                     
//       index_.erase(tmp_key);                             // Удаление вытесненного элемента из индекса .
//     }
//     else {  // Ситуация, когда в кэшэ еще есть место.
//       list_.emplace_front(name, ip); // Создание элемента списка.
//       it->second = list_.begin();    // Создание элемента списка.
//     }
//   }
//   else {  // Обновление существующего элемента к кэше.
//     it->second->value = ip;                       // Обновление значения элемента
//     list_.splice(list_.begin(), list_, it->second);  // Перемещение элемента в начало списка.
//   }
// }

// Вариант оптимизированный аллокации памяти.
void DNSCache::update(const std::string& name, const std::string& ip) {
  std::unique_lock<shared_mutex_t> lock(mutex_);
  if (list_.size() >= capacity_) { // Кэш заполнен, поэтому произойдет либо обновление записи, либо вытеснение.
    if (auto it = index_.find(name); it == index_.end()) {
      list_.splice(list_.begin(), list_, std::next(list_.rbegin()).base()); // Премещение последнего элемента в начало.
      auto index_node = index_.extract(list_.begin()->key);               // Извлечение элемента из индекса.
      index_node.key() = name;                                          
      index_.insert(std::move(index_node));                              // Вставка элемента в индекс с новым ключом.
      list_.begin()->set(name, ip);                                      // Обновление элемента.
    }
    else {
      it->second->value = ip; // Обновление значения элемента
      list_.splice(list_.begin(), list_, it->second);  // Перемещение элемента в начало списка.
    }
  }
  else {  // Кэш не заполнен, поэтому скорее всего придется добавлять новый элемент.
    auto [it, done] = index_.try_emplace(name, cache_element_t{});
    if (done) {
      list_.emplace_front(name, ip);
      it->second = list_.begin();
    }
    else {
      it->second->value = ip; // Обновление значения элемента
      list_.splice(list_.begin(), list_, it->second);  // Перемещение элемента в начало списка.
    }
  }
}


std::string DNSCache::resolve(const std::string& name) {
  std::unique_lock<shared_mutex_t> lock(mutex_);
  if (auto it = index_.find(name); it != index_.end()) {
    list_.splice(list_.begin(), list_, it->second); // Премещение последнего элемента в начало.
    return it->second->value;
  }
  return {};
}