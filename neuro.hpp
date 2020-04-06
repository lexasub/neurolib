#pragma once
#include "extmap.cpp"
#include "link.cpp"
#include "neuron.cpp"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <stack>
#include <unordered_set>
#include <vector>
#include <fstream>

template <class T> struct serialized_data{
  T w = 0, dw = 0, eps = 0, alpha = 0;
};

template <class T, class W, class SUM, class CNT> class neuro {
  bool debug = false;
  std::vector<T *> inp, output;

  void forward();
  void backward(const std::vector<SUM> &real_vect);
  
  void first_lay_forward(extmap<link<T, W> *, SUM> &m, 
                         std::unordered_set<T *> &out_set);
  void other_lay_forward(extmap<link<T, W> *, SUM> &m, 
                         const std::unordered_set<T *> &in_set,
                         std::unordered_set<T *> &out_set);
  void last_lay_backward(std::unordered_map<T *, SUM> &delta_out, 
                         std::unordered_set<T *> &out_set,
                         const std::vector<SUM> &real_vect);
  void other_lay_backward(const std::unordered_map<T *, SUM> &delta_in,
                          std::unordered_map<T *, SUM> &delta_out,
                          std::unordered_set<T *> &in_set, 
                          std::unordered_set<T *> &out_set);
  SUM activate(const SUM &arg);
  SUM dactivate(const SUM &arg);
  public:
  neuro(const std::vector<SUM> &input, const std::vector<CNT> &n);
  void run(const std::vector<SUM> &real_vect); // forward then backward
  std::vector<SUM> getout();
  void setin(const std::vector<SUM> &arg);
  void serialize(std::string filename);
  void deserialize(std::string filename);
  ~neuro();
};
