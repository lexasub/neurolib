#pragma once
#include <unordered_map>
#include <unordered_set>
template <class T, class U> class extmap : public std::unordered_map<T, U> {
  std::unordered_set<T> used_ind;

public:
  extmap() : std::unordered_map<T, U>(){};
  U &operator[](const T &arg);
  virtual ~extmap() {used_ind.clear();}
};
