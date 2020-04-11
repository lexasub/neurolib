#include "neuro.hpp"
template <class T, class W, class SUM, class CNT>
neuro<T, W, SUM, CNT>::neuro(const std::vector<SUM> &input,
                             const std::vector<CNT> &n) {
  srand(time(NULL));
  std::default_random_engine rd;
  std::uniform_real_distribution<W> urd(-1.0, 1.0);
  inp.resize(input.size());
  std::vector<T *> _inp(n[1]), _inp2;
  for (typename std::vector<T *>::iterator it = _inp.begin(); it != _inp.end(); ++it)//for each input
    *it = new T;//create new object
  //можно было конечно юзать стеки, но конструктор вызывается 1 раз))(как-нибудь
  //сделаю потом, если памяти хватать не будет
  {//isolate input_argument_it
    typename std::vector<SUM>::const_iterator input_argument_it = input.begin();
    for (typename std::vector<T *>::iterator it = inp.begin(); it != inp.end();
       ++it) {//for each input
      *it = new T;//create new object
      (*it)->set_sum(*input_argument_it);//copy value
      ++input_argument_it;//inc iterator for input
      for (typename std::vector<T *>::iterator gt = _inp.begin(); gt != _inp.end(); ++gt) //for each _inp
        (*it)->link_out_with(*gt, urd(rd), 0, 0.5, 0); // create link to next neuro and backlink
    }
  }
  //вроде все тут должно быть слинковано(inp и 1-й слой)
  
  for (typename std::vector<CNT>::const_iterator n_it = n.begin() + 2; n_it != n.end(); ++n_it) {//for each n
    _inp2.resize(*n_it);
    for (typename std::vector<T *>::iterator it = _inp2.begin();//for each alloc mem
         it != _inp2.end(); ++it)
	 *it = new T;//create new object
    for (typename std::vector<T *>::iterator it = _inp.begin();//for each create links
         it != _inp.end(); ++it) {
      for (typename std::vector<T *>::iterator gt = _inp2.begin();
           gt != _inp2.end(); ++gt) 
        (*it)->link_out_with(*gt, urd(rd), 0, 0.5, 0); // create link to next neuro and backlink
    }
    _inp.swap(_inp2); //подменяем массивы
  }
  //в итоге получаем в _inp указатели на выходы
  for (typename std::vector<T *>::iterator it = _inp.begin(); it != _inp.end(); ++it)
    output.push_back(*it); // generate output-link array
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::forward() {
  extmap<link<T, W> *, SUM> first_map, second_map;//накопление суммы
  std::unordered_set<T *> first_set, second_set;//куда можно сходить
  first_lay_forward(first_map, first_set);//do forward 0_lay->1_lay
  while (!first_set.empty()) { //пока есть куда сходить
    second_set.clear();//prepare clean containers
    second_map.clear();
    other_lay_forward(second_map, first_set, second_set);//do forward i_lay->(i+1)_lay
    first_set.swap(second_set);//swap
    first_map.swap(second_map);
  }
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::backward(const std::vector<SUM> &real_vect) {
  std::unordered_map<T *, SUM> first_delta, second_delta;//накопление суммы
  std::unordered_set<T *> first_set, second_set;//куда можно сходить
  last_lay_backward(first_delta, first_set, real_vect);//do bacward n_lay->(n-1)_lay
  while (!first_set.empty()) { //пока есть куда сходить
    second_delta.clear();//prepare clean containers
    second_set.clear();
    other_lay_backward(first_delta, second_delta, first_set, second_set);//do bacward i_lay->(i-1)_lay
    first_set.swap(second_set);//swap
    first_delta.swap(second_delta);
  }
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::run(const std::vector<SUM> &real_vect) {
  forward();//run forward, fill sum's
  static CNT ku = 0;
  if (ku < skip_print)//wait for next print
    ku++;
  else {
    SUM sum = 0;
    typename std::vector<SUM>::const_iterator needed_iterator = real_vect.begin();
    for (typename std::vector<T *>::iterator it = output.begin();
         it != output.end(); ++it, ++needed_iterator)
      sum += ((*it)->get_sum() - *needed_iterator) * ((*it)->get_sum() - *needed_iterator);//sum += err*err
    ku = 0;//reset
    sum /= real_vect.size(); //normalize
    std::cout << sum << std::endl;//print
  }
  backward(real_vect);//run forward, fix w
//  serialize("out.dmp");
 //   deserialize("out.dmp");// если дофига раз запустить и не свалиться - можно сказать что нет утечек в этой f()
//  exit(0);
}


template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::first_lay_forward(extmap<link<T, W> *, SUM> &m, std::unordered_set<T *> &out_set){
  for (typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it)//for each input
    for (typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin(); gt != (*it)->out_end(); ++gt) {//for each out
      m[*gt] += (*it)->get_sum() * (*gt)->get_w(); //накопляем сумму
      out_set.insert((*gt)->second); //копим куда должны сходить
    }
  if (debug)
    std::cout << "lays[1].sum:" << std::endl;
  for (typename extmap<link<T, W> *, SUM>::iterator it = m.begin(); it != m.end(); ++it) {//for each link
    it->first->second->set_sum(activate(it->second)); //запихали сумму куда надо
    if (debug)
      std::cout << it->first->second->get_sum();
  }
  if (debug)
    std::cout << std::endl;
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::other_lay_forward(extmap<link<T, W> *, SUM> &m, const std::unordered_set<T *> &in_set, 
																					std::unordered_set<T *> &out_set) const{
 for (typename std::unordered_set<T *>::const_iterator it = in_set.begin(); it != in_set.end(); ++it)//for each in prev_lay.neurons
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin(); gt != (*it)->out_end(); ++gt) {//for each out
        m[*gt] += (*it)->get_sum() * (*gt)->get_w(); //накопляем сумму
        out_set.insert((*gt)->second); //копим куда должны сходить
      }
    if (debug)
      std::cout << "lays[?].sum:" << std::endl;
    for (typename extmap<link<T, W> *, SUM>::iterator it = m.begin(); it != m.end(); ++it) {//for each link
      it->first->second->set_sum(activate(it->second)); //запихали сумму куда надо
      if (debug)
        std::cout << it->first->second->get_sum();
    }
    if (debug)
      std::cout << std::endl;
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::last_lay_backward(std::unordered_map<T *, SUM> &delta_out, std::unordered_set<T *> &out_set,
													 const std::vector<SUM> &real_vect){
  if (debug)
    std::cout << "delta[n - 1]:" << std::endl;
  typename std::vector<SUM>::const_iterator needed_iterator = real_vect.begin(); 
  for (typename std::vector<T *>::iterator it = output.begin(); it != output.end(); ++it, ++needed_iterator) {//for each output
    delta_out.insert(std::pair<T *, SUM>(
        *it, (*needed_iterator - ((*it)->get_sum())) * dactivate((*it)->get_sum()))); //delta_out пустой
    for (typename std::list<link<T, W> *>::iterator gt = (*it)->inp_begin(); gt != (*it)->inp_end(); ++gt)//for each input
      out_set.insert((*gt)->first); //копим куда должны сходить
    if (debug)
      std::cout << (delta_out.find(*it)->second) << ' ';
  }
  if (debug)
    std::cout << std::endl;
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::other_lay_backward(const std::unordered_map<T *, SUM> &delta_in, std::unordered_map<T *, SUM> &delta_out, 
													  std::unordered_set<T *> &in_set, std::unordered_set<T *> &out_set){
    for (typename std::unordered_set<T *>::iterator it = in_set.begin(); it != in_set.end(); ++it) {//for each elem
      SUM sum = 0;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin(); gt != (*it)->out_end(); ++gt)//for each out
        sum += (delta_in.find((*gt)->second)->second) * (*gt)->get_w();//перемножаем delta_in с нейронами которые дальше
      SUM delta = sum * dactivate((*it)->get_sum());//eval delta
      delta_out.insert(std::pair<T *, SUM>(*it, delta)); //тут delta_out пустой
      if (debug)
        std::cout << "delta[?]:" << delta << std::endl << "w, dw:" << std::endl;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin(); gt != (*it)->out_end(); ++gt) {//for each out
        (*gt)->add_w((delta_in.find((*gt)->second)->second) * (*it)->get_sum()); // dw = eps * arg + alpha * dw; w+=dw
        if (debug)
          std::cout << '\t' << (*gt)->get_w() << " : " << (*gt)->get_dw() << ' ';
      }
      if (debug)
        std::cout << std::endl;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->inp_begin(); gt != (*it)->inp_end(); ++gt)//for each input
        out_set.insert((*gt)->first); //копим куда должны сходить
    }
}

template <class T, class W, class SUM, class CNT>
inline SUM neuro<T, W, SUM, CNT>::activate(const SUM &arg) {
  return 1 / (1 + exp(-arg));
//  return sin(arg);
}

template <class T, class W, class SUM, class CNT>
inline SUM neuro<T, W, SUM, CNT>::dactivate(const SUM &arg) {
  return (1 - arg) * arg;
//  return cos(arg); 
}

template <class T, class W, class SUM, class CNT>
std::vector<SUM> neuro<T, W, SUM, CNT>::getout() {
  std::vector<SUM> sum;
  for (typename std::vector<T *>::output::iterator it = output.begin(); it != output.end(); ++it)//for each output
    sum.push_back((*it)->get_sum());
  return sum;
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::setin(const std::vector<SUM> &arg) {
  typename std::vector<T *>::iterator inp_iterator = inp.begin();
  for (typename std::vector<SUM>::const_iterator it = arg.begin(); it != arg.end(); ++it, ++inp_iterator)//for each in
    (*inp_iterator)->set_sum(*it);
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::serialize(std::string filename) {
  std::unordered_set<T *> all, chunk, chunk2;
  for(typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it){//foreach inp
    for(typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin(); //for each inp.out
        gt != (*it)->out_end(); ++gt)
      chunk.insert((*gt)->second);//копим куда должны сходить
    all.insert(*it);//all neurons
  }
  while(!chunk.empty()){
    chunk2.clear();
    for(typename std::unordered_set<T *>::iterator it = chunk.begin();//for each data from prev step
        it != chunk.end(); ++it){
      for(typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin();//for each output of him
          gt != (*it)->out_end(); ++gt)
        chunk2.insert((*gt)->second);//where going?
      all.insert(*it);
    }
    chunk.swap(chunk2);//swap
  }
  //в all все указатели на нейроны
  std::unordered_map<T *, CNT> index_map;
  std::vector<std::vector<serialized_data<W>>> matrix(all.size(), std::vector<serialized_data<W>> (all.size()));
  CNT i = 0;
  for(typename std::unordered_set<T *>::iterator it = all.begin();//for each neuro
      it != all.end(); ++it, ++i)
    index_map.insert(std::pair<T *, CNT>(*it, i));//indexing it's
  for(typename std::unordered_set<T *>::iterator it = all.begin();//for each neuro
      it != all.end(); ++it)
    for(typename std::list<link<T, W> *>::iterator gt = (*it)->out_begin();//for each out
      gt != (*it)->out_end(); ++gt){
      serialized_data<W> &tmp = matrix[index_map.find(*it)->second][index_map.find((*gt)->second)->second];
      tmp.w = (*gt)->get_w();
      tmp.dw = (*gt)->get_dw();
      tmp.eps = (*gt)->get_eps();
      tmp.alpha = (*gt)->get_alpha();
    }
  std::ofstream f(filename);
  f << all.size() << ":" << inp.size() << ":" << output.size() << ":";
  for(typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it)//for each inp
    f << index_map.find(*it)->second << ":";
  f.flush();
  for(typename std::vector<T *>::iterator it = output.begin(); it != output.end(); ++it)//for each out
    f << index_map.find(*it)->second << ":";
  f.flush();
  for(typename std::vector<std::vector<serialized_data<W>>>::iterator it = matrix.begin(); it != matrix.end(); ++it){//for each elem
    for(typename std::vector<serialized_data<W>>::iterator gt = it->begin(); gt != it->end(); ++gt){//for each elem
      f << gt->w << ":" << gt->dw << ":" << gt->eps << ":" << gt->alpha << ":";
    }
    f.flush();
  }
  f.close();
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::deserialize(std::string filename) {
  std::ifstream f(filename, std::ios::binary);
  CNT n, n_in, n_out;
  char ch;
  f >> n >> ch >> n_in >> ch >> n_out;//ch - skip :
  typename std::vector<T *>::iterator it;
  for(it = inp.begin(); it != inp.end(); ++it)
    (*it)->~T();//вызываем деструкторы
  inp.clear();
  output.clear();
  inp.resize(n_in);
  output.resize(n_out);
  std::vector<T *> all_neurons(n);
  CNT tmp;
  for(it = all_neurons.begin(); it != all_neurons.end(); ++it)
    *it = new T;//create nodes(neurons)
  for(it = inp.begin(); it != inp.end(); ++it){//connect input
    f >> tmp >> ch;//skip :
    *it = all_neurons[tmp];
  }
  for(it = output.begin(); it != output.end(); ++it){//connect output
    f >> tmp >> ch;//skip :
    *it = all_neurons[tmp];
  }
  W w = 0, dw = 0, eps = 0, alpha = 0;
  for(typename std::vector<T *>::iterator first_neuron = all_neurons.begin(); first_neuron != all_neurons.end(); ++first_neuron){//foreach neuron
    for(typename std::vector<T *>::iterator second_neuron = all_neurons.begin(); second_neuron != all_neurons.end(); ++second_neuron){//forward neuron(2dim matrix)
      f >> w >> ch >> dw >> ch >> eps >> ch >> alpha >> ch;//skip :
      if(w != 0 || dw != 0 || eps != 0)//если связь существует, то применяем ее
        (*first_neuron)->link_out_with(*second_neuron, w, dw, eps, alpha);//link and backlink
    }
  }
}

template <class T, class W, class SUM, class CNT>
neuro<T, W, SUM, CNT>::~neuro(){
//  for(typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it)
  //  (*it)->~T();//вызываем деструкторы
  inp.clear();
  output.clear();
}
