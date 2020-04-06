#pragma once
template <class T, class U> class link {
  U w, dw;
  U eps = 0.7, alpha = 0;//spd, moment
public:
  T *first, *second;
  link(T *a, T *b);
  T *get_sibling(T *a);
  U get_w() const{return w;}
  U get_dw() const{return dw;}
  U get_eps() const{return eps;}
  U get_alpha() const{return alpha;}
  U add_w(U a); // dw=..; w+=dw;
  void set_w(U a){w = a;}; //!юзать это только в конструкторе графа!
  ~link();
};
