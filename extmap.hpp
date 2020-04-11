#pragma once
#include <unordered_map>
#include <unordered_set>
template <class T, class U> class extmap : public std::unordered_map<T, U> {
  std::unordered_set<T> used_ind;//set for used indexes

public:
  extmap() : std::unordered_map<T, U>(){};//empty constructor
  U &operator[](const T &arg);//index operator
  virtual ~extmap() {used_ind.clear();}//destructor
};
