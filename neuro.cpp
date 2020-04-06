#include "neuro.hpp"
template <class T, class W, class SUM, class CNT>
neuro<T, W, SUM, CNT>::neuro(const std::vector<SUM> &input,
                             const std::vector<CNT> &n) {
  std::default_random_engine rd;
  std::uniform_real_distribution<W> urd(-1.0, 1.0);
  inp.resize(input.size());
  std::vector<T *> _inp(n[1], new T), _inp2;
  //можно было конечно юзать стеки, но конструктор вызывается 1 раз))(как-нибудь
  //сделаю потом, если памяти хватать не будет
  CNT i = 0;
  for (typename std::vector<T *>::iterator it = inp.begin(); it != inp.end();
       ++it) {
    *it = new T;
    (*it)->sum = input[i++];
    for (CNT j = 0; j < n[1]; ++j) {
      (*it)->out.push_back(
          new link<T, W>(*it, _inp[j]));            // create link to next neuro
      _inp[j]->inp.push_back(*(*it)->out.rbegin()); // backlink
      (*(*it)->out.rbegin())->set_w(urd(rd));
    }
  } //вроде все тут должно быть слинковано(inp и 1-й слой)
  for (i = 2; i < n.size(); ++i) {
    _inp2.resize(n[i]);
    for (typename std::vector<T *>::iterator it = _inp2.begin();
         it != _inp2.end(); ++it)
      *it = new T;
    for (typename std::vector<T *>::iterator it = _inp.begin();
         it != _inp.end(); ++it) {
      for (typename std::vector<T *>::iterator gt = _inp2.begin();
           gt != _inp2.end(); ++gt) {
        (*it)->out.push_back(
            new link<T, W>(*it, *gt));              // create link to next neuro
        (*gt)->inp.push_back(*(*it)->out.rbegin()); // backlink
        (*(*it)->out.rbegin())->set_w(urd(rd));
      }
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
  first_lay_forward(first_map, first_set);
  while (!first_set.empty()) { //пока есть куда сходить
    second_set.clear();
    second_map.clear();
   	other_lay_forward(second_map, first_set, second_set);
    first_set.swap(second_set);
    first_map.swap(second_map);
  }
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::backward(const std::vector<SUM> &real_vect) {
  std::unordered_map<T *, SUM> first_delta, second_delta;//накопление суммы
  std::unordered_set<T *> first_set, second_set;//куда можно сходить
  last_lay_backward(first_delta, first_set, real_vect);
  while (!first_set.empty()) { //пока есть куда сходить
    second_delta.clear();
    second_set.clear();
    other_lay_backward(first_delta, second_delta, first_set, second_set);
    first_set.swap(second_set);
    first_delta.swap(second_delta);
  }
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::run(const std::vector<SUM> &real_vect) {
  forward();
  std::cout.precision(10);
  static long long ku = 0;
  if (ku < 100)
    ku++;
  else {
    SUM sum = 0;
    CNT i = 0;
    for (typename std::vector<T *>::iterator it = output.begin();
         it != output.end(); ++it, ++i)
      sum += ((*it)->sum - real_vect[i]) * ((*it)->sum - real_vect[i]);
    ku = 0;
    std::cout << (sum / real_vect.size()) << std::endl;
  }
  backward(real_vect);
//  serialize("out.dmp");
//  exit(0);
}


template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::first_lay_forward(extmap<link<T, W> *, SUM> &m, std::unordered_set<T *> &out_set){
  for (typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it)
    for (typename std::list<link<T, W> *>::iterator ig = (*it)->out.begin(); ig != (*it)->out.end(); ++ig) {
      m[*ig] += (*it)->sum * (*ig)->get_w(); //накопляем сумму
      out_set.insert((*ig)->second); //копим куда должны сходить
    }
  if (debug)
    std::cout << "lays[1].sum:" << std::endl;
  for (typename extmap<link<T, W> *, SUM>::iterator it = m.begin();
       it != m.end(); ++it) {
    it->first->second->sum = activate(it->second); //запихали сумму куда надо
    if (debug)
      std::cout << it->first->second->sum;
  }
  if (debug)
    std::cout << std::endl;
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::other_lay_forward(extmap<link<T, W> *, SUM> &m, const std::unordered_set<T *> &in_set, 
																					std::unordered_set<T *> &out_set){
 for (typename std::unordered_set<T *>::const_iterator it = in_set.begin(); it != in_set.end(); ++it)
      for (typename std::list<link<T, W> *>::iterator ig = (*it)->out.begin(); ig != (*it)->out.end(); ++ig) {
        m[*ig] += (*it)->sum * (*ig)->get_w(); //накопляем сумму
        out_set.insert((*ig)->second); //копим куда должны сходить
      }
    if (debug)
      std::cout << "lays[?].sum:" << std::endl;
    for (typename extmap<link<T, W> *, SUM>::iterator it = m.begin();
         it != m.end(); ++it) {
      it->first->second->sum = activate(it->second); //запихали сумму куда надо
      if (debug)
        std::cout << it->first->second->sum;
    }
    if (debug)
      std::cout << std::endl;
}

template <class T, class W, class SUM, class CNT>
inline void neuro<T, W, SUM, CNT>::last_lay_backward(std::unordered_map<T *, SUM> &delta_out, std::unordered_set<T *> &out_set,
													 const std::vector<SUM> &real_vect){
  if (debug)
    std::cout << "delta[n - 1]:" << std::endl;
  CNT i = 0;
  for (typename std::vector<T *>::iterator it = output.begin(); it != output.end(); ++it, ++i) {
    delta_out.insert(std::pair<T *, SUM>(
        *it, (real_vect[i] - ((*it)->sum)) * dactivate((*it)->sum))); //тута delta_out пустой
    for (typename std::list<link<T, W> *>::iterator gt = (*it)->inp.begin(); gt != (*it)->inp.end(); ++gt)
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
	for (typename std::unordered_set<T *>::iterator it = in_set.begin(); it != in_set.end(); ++it) {
      SUM sum = 0;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->out.begin(); gt != (*it)->out.end(); ++gt)
        sum += (delta_in.find((*gt)->second)->second) * (*gt)->get_w();//перемножаем delta_in с нейронами которые дальше
      SUM delta = sum * dactivate((*it)->sum);
      delta_out.insert(std::pair<T *, SUM>(*it, delta)); //тута delta_out пустой
      if (debug)
        std::cout << "delta[?]:" << delta << std::endl << "w, dw:" << std::endl;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->out.begin(); gt != (*it)->out.end(); ++gt) {
        (*gt)->add_w((delta_in.find((*gt)->second)->second) * (*it)->sum); // dw = eps * arg + alpha * dw; w+=dw
        if (debug)
          std::cout << '\t' << (*gt)->get_w() << " : " << (*gt)->get_dw() << ' ';
      }
      if (debug)
        std::cout << std::endl;
      for (typename std::list<link<T, W> *>::iterator gt = (*it)->inp.begin();
           gt != (*it)->inp.end(); ++gt)
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
  std::vector<SUM> tmp;
  for (typename std::vector<T *>::output::iterator it = output.begin();
       it != output.end(); ++it)
    tmp.push_back((*it)->sum);
  return tmp;
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::setin(const std::vector<SUM> &arg) {
  CNT i = 0;
  for (typename std::vector<SUM>::const_iterator it = arg.begin();
       it != arg.end(); ++it, ++i)
    inp[i]->sum = *it;
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::serialize(std::string filename) {
  std::ofstream f(filename, std::ios::binary);
  std::unordered_set<T *> all, chunk, chunk2;
  for(typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it){
    for(typename std::list<link<T, W> *>::iterator gt = (*it)->out.begin(); 
        gt != (*it)->out.end(); ++gt)
      chunk.insert((*gt)->second);//копим куда должны сходить
    all.insert(*it);
  }
  while(!chunk.empty()){
    chunk2.clear();
    for(typename std::unordered_set<T *>::iterator it = chunk.begin(); 
        it != chunk.end(); ++it){
      for(typename std::list<link<T, W> *>::iterator gt = (*it)->out.begin();
          gt != (*it)->out.end(); ++gt)
        chunk2.insert((*gt)->second);
      all.insert(*it);
    }
    chunk.swap(chunk2);
  }
  //в all все указатели на нейроны
  std::unordered_map<T *, CNT> index_map;
  std::vector<std::vector<serialized_data<W>>> matrix(all.size(), std::vector<serialized_data<W>> (all.size()));
  CNT i = 0;
  for(typename std::unordered_set<T *>::iterator it = all.begin();
      it != all.end(); ++it, ++i)
    index_map.insert(std::pair<T *, CNT>(*it, i));
  for(typename std::unordered_set<T *>::iterator it = all.begin();
      it != all.end(); ++it)
    for(typename std::list<link<T, W> *>::iterator gt = (*it)->out.begin();
      gt != (*it)->out.end(); ++gt){
      serialized_data<W> &tmp = matrix[index_map.find(*it)->second][index_map.find((*gt)->second)->second];
      tmp.w = (*gt)->get_w();
      tmp.dw = (*gt)->get_dw();
      tmp.eps = (*gt)->get_eps();
      tmp.alpha = (*gt)->get_alpha();
    }
  f << ((CNT) all.size()) << ((CNT) inp.size()) << ((CNT) output.size());
  for(typename std::vector<T *>::iterator it = inp.begin(); it != inp.end(); ++it)
    f << ((CNT) index_map.find(*it)->second);
  f.flush();
  for(typename std::vector<T *>::iterator it = output.begin(); it != output.end(); ++it)
    f << ((CNT) index_map.find(*it)->second);
  f.flush();
  for(typename std::vector<std::vector<serialized_data<W>>>::iterator it = matrix.begin(); it != matrix.end(); ++it){
    for(typename std::vector<serialized_data<W>>::iterator gt = it->begin(); gt != it->end(); ++gt){
      f.write(reinterpret_cast<char*>(&gt->w), sizeof(&gt->w));
      f.write(reinterpret_cast<char*>(&gt->dw), sizeof(&gt->dw));
      f.write(reinterpret_cast<char*>(&gt->eps), sizeof(&gt->eps));
      f.write(reinterpret_cast<char*>(&gt->alpha), sizeof(&gt->alpha));
    }
    //f << ((W)2);//если буду переходить к целым - надо будет поправить
    f.flush();
  }
  f.close();
}

template <class T, class W, class SUM, class CNT>
void neuro<T, W, SUM, CNT>::deserialize(std::string filename) {
  std::ifstream f(filename, std::ios::binary);
  CNT n, n_in, n_out;
  f >> n >> n_in >> n_out;//get size
  inp.clear();
  inp.resize(n_in);
  output.clear();
  output.resize(n_out);
 //тут наверное создаем map 
}

template <class T, class W, class SUM, class CNT>
neuro<T, W, SUM, CNT>::~neuro(){
  inp.clear();
  output.clear();
}
