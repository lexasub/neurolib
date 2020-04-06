#pragma once
#include "link.hpp"
#include <list>

template <class SUM, class W> struct neuron {
  std::list<link<neuron<SUM, W>, W> *> inp, out;
  SUM sum = 0;
};
