#include "link.hpp"

template <class T, class U>
link<T, U>::link(T *a, T *b) : w(0), dw(0), first(a), second(b) {}

template <class T, class U> T *link<T, U>::get_sibling(T *a) {
  return (first == a) ? second : first;
}

template <class T, class U> U link<T, U>::add_w(U a) {
  w += dw = eps * a + alpha * dw ;
  return w;
}

template <class T, class U> link<T, U>::~link() { 
  try{delete second;}//вызываем деструктор
  catch(...){}//вдруг память уже освобождена
}


