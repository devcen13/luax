#pragma once

#ifndef _LUAX_VM_H_
#define _LUAX_VM_H_


#include <stack>

#include <luax\base.h>


namespace luax {

struct vm {
  static ::luax::VM current () {
    return _curr;
  }

  static bool is_same (::luax::VM vm) {
    return vm == current();
  }

  static void push (::luax::VM vm) {
    _prev.push(_curr);
    _curr = vm;
  }

  static void pop () {
    _curr = _prev.top();
    _prev.pop();
  }


  LUAX_NODISCARD struct handler final {
    handler (::luax::VM vm)
      : _pushed(!::luax::vm::is_same(vm)) {
      if (_pushed) {
        ::luax::vm::push(vm);
      }
    }

    handler (const handler &) = delete;
    handler &operator = (const handler &) = delete;

    handler (handler &&other)
      : _pushed(other._pushed) {
      other._pushed = false;
    }
    handler &operator = (handler &&other) {
      _pushed = other._pushed;
      other._pushed = false;      
    }

    ~handler () {
      if (_pushed) {
        ::luax::vm::pop();
      }
    }

    bool     valid () const { return true; }
    operator bool ()  const { return valid(); }

  private:
    bool _pushed;
  };


private:
  static ::luax::VM             _curr;
  static std::stack<::luax::VM> _prev;
};

} // namespace luax


#define luax_use_vm(_Vm)    luax_use   (::luax::vm::handler(_Vm))
#define luax_using_vm(_Vm)  luax_using (::luax::vm::handler(_Vm))
#define luax_current_vm     ::luax::vm::current()


#endif  // _LUAX_VM_H_
