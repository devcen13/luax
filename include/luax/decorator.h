#pragma once

#ifndef _LUAX_DECORATOR_H_
#define _LUAX_DECORATOR_H_


namespace luax {


template <typename T>
struct decorator {
  T & obj;

  decorator (T &o) : obj(o) { }

  const T *operator -> () const  { return &obj; }
        T *operator -> ()        { return &obj; }
};


} // namespace luax


/**
  @usage: using DecoratedType = luax_decorate(PlainType);
*/
#define luax_decorate(_BaseT) struct : ::luax::decorator<_BaseT> {}


#endif  // _LUAX_DECORATOR_H_
