#include "extmap.hpp"

template <class T, class U> U &extmap<T, U>::operator[](const T &arg) {
  if (used_ind.find(arg) != used_ind.end()) //elem exist
    return this->find(arg)->second; //return him
  this->insert(std::pair<T, U>(arg, 0));//else insert him
  used_ind.insert(arg);//add to used index
  return this->find(arg)->second;//return him
}
