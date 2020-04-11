#include "link.hpp"
template <class T, class U>
link<T, U>::link(T *a, T *b, U _w, U _dw, U _eps, U _alpha) : w(_w), dw(_dw), eps(_eps),
                                                              alpha(_alpha), first(a), second(b) {}
							      //full constructor
template <class T, class U> T *link<T, U>::get_sibling(T *a) {
  return (first == a) ? second : first;//get sibling, "friend"
}

template <class T, class U> U link<T, U>::add_w(U a) {
  w += dw = eps * a + alpha * dw ;//eval dw && add to w
  return w;
}

template <class T, class U> link<T, U>::~link() { 
  try{delete second;}//вызываем деструктор
  catch(...){}//вдруг память уже освобождена
}


