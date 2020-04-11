#pragma once
template <class T, class U> class link {
  U w, dw;//weight && last diff of weight
  U eps = 0.5, alpha = 0;//spd, moment
public:
  T *first, *second;//what we need connect?
  link(T *a, T *b, U _w, U _dw, U _eps, U _alpha);//full constructor
  T *get_sibling(T *a);//get sibling
  U get_w() const{return w;}//get w
  U get_dw() const{return dw;}//get dw
  U get_eps() const{return eps;}//get eps
  U get_alpha() const{return alpha;}//get alpha
  U add_w(U a); // dw=..; w+=dw;
  ~link();//destructor
};
