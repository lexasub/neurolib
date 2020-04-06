#include "extmap.hpp"

template <class T, class U> U &extmap<T, U>::operator[](const T &arg) {
  if (used_ind.find(arg) != used_ind.end())
    return this->find(arg)->second;
  this->insert(std::pair<T, U>(arg, 0));
  used_ind.insert(arg);
  return this->find(arg)->second;
}
