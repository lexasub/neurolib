#pragma once
#include "link.hpp"
#include <list>

template <class SUM, class W> class neuron {
  std::list<link<neuron<SUM, W>, W> *> inp, out;//in, out siblings
  SUM sum = 0;//result of neuron
  public:
  void link_out_with(neuron<SUM, W> *next_neuron, W w, W dw, W eps, W alpha);
  SUM get_sum() const{return sum;}
  void set_sum(const SUM arg){sum = arg;}
  typename std::list<link<neuron<SUM, W>, W> *>::iterator out_begin(){return out.begin();}
  typename std::list<link<neuron<SUM, W>, W> *>::iterator out_end(){return out.end();}
  typename std::list<link<neuron<SUM, W>, W> *>::iterator inp_begin(){return inp.begin();}
  typename std::list<link<neuron<SUM, W>, W> *>::iterator inp_end(){return inp.end();}
};
