#include "neuron.hpp"

template <class SUM, class W> void neuron<SUM, W>::link_out_with(neuron<SUM, W> *next_neuron, W w, W dw, W eps, W alpha){//make link
  out.push_back(new link<neuron<SUM, W>, W>(this, next_neuron, w, dw, eps, alpha));//create link
  next_neuron->inp.push_back(*out.rbegin());//create backlink
}
