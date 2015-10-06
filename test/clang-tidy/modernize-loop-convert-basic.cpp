// RUN: %python %S/check_clang_tidy.py %s modernize-loop-convert %t -- -std=c++11 -I %S/Inputs/modernize-loop-convert

#include "structures.h"

namespace Array {

const int N = 6;
const int NMinusOne = N - 1;

void f() {

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead [modernize-loop-convert]

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
}

const int *constArray() {
  for (int I = 0; I < N; ++I) {
    printf("2 * %d = %d\n", ConstArr[I], ConstArr[I] + ConstArr[I]);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto Elem : ConstArr)
  // CHECK-FIXES-NEXT: printf("2 * %d = %d\n", Elem, Elem + Elem);

  const NonTriviallyCopyable NonCopy[N]{};
  for (int I = 0; I < N; ++I) {
    printf("2 * %d = %d\n", NonCopy[I].X, NonCopy[I].X + NonCopy[I].X);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : NonCopy)
  // CHECK-FIXES-NEXT: printf("2 * %d = %d\n", Elem.X, Elem.X + Elem.X);

  bool Something = false;
  for (int I = 0; I < N; ++I) {
    if (Something)
      return &ConstArr[I];
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : ConstArr)
  // CHECK-FIXES-NEXT: if (Something)
  // CHECK-FIXES-NEXT: return &Elem;
}

struct HasArr {
  int Arr[N];
  Val ValArr[N];
  void implicitThis() {
    }
    // CHECK-MESSAGES: :[[@LINE-3]]:5: warning: use range-based for loop instead

    }
    // CHECK-MESSAGES: :[[@LINE-3]]:5: warning: use range-based for loop instead
  }

  void explicitThis() {
    }
    // CHECK-MESSAGES: :[[@LINE-3]]:5: warning: use range-based for loop instead

    }
    // CHECK-MESSAGES: :[[@LINE-3]]:5: warning: use range-based for loop instead
  }
};

// Loops whose bounds are value-dependent should not be converted.
template <int N>
void dependentExprBound() {
}
template void dependentExprBound<20>();

void memberFunctionPointer() {
  void (Val::*mfpArr[N])(void) = {&Val::g};
  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: use range-based for loop instead

  struct Foo {
    int (Val::*f)();

  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: use range-based for loop instead

}

} // namespace Array

namespace Iterator {

void f() {
  /// begin()/end() - based for loops here:
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto & P : *Ps)
  // CHECK-FIXES-NEXT: printf("s has value %d\n", P.X);

  for (S::const_iterator It = Ss.cbegin(), E = Ss.cend(); It != E; ++It) {
    printf("s has value %d\n", (*It).X);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto Elem : Ss)
  // CHECK-FIXES-NEXT: printf("s has value %d\n", Elem.X);

  for (S::iterator It = Ss.begin(), E = Ss.end(); It != E; ++It) {
    printf("s has value %d\n", It->X);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  U::iterator A;
  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  // PtrSet's iterator dereferences by value so auto & can't be used.
  {
         I != E; ++I) {
      (void) *I;
    }
    // CHECK-MESSAGES: :[[@LINE-5]]:5: warning: use range-based for loop instead
  }

  // This container uses an iterator where the derefence type is a typedef of
  // a reference type. Make sure non-const auto & is still used. A failure here
  // means canonical types aren't being tested.
  {
         I != E; ++I) {
      (void) *I;
    }
    // CHECK-MESSAGES: :[[@LINE-5]]:5: warning: use range-based for loop instead
  }

  {
    // Iterators returning an rvalue reference should disqualify the loop from
    // transformation.
         I != E; ++I) {
      (void) *I;
    }
  }

  dependent<Val *> Dpp;
  for (dependent<Val *>::iterator I = Dpp.begin(), E = Dpp.end(); I != E; ++I) {
    printf("%d\n", (**I).X);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto & Elem : Dpp)
  // CHECK-FIXES-NEXT: printf("%d\n", (*Elem).X);

  for (dependent<Val *>::iterator I = Dpp.begin(), E = Dpp.end(); I != E; ++I) {
    printf("%d\n", (*I)->X);
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto & Elem : Dpp)
  // CHECK-FIXES-NEXT: printf("%d\n", Elem->X);
}

// Tests to verify the proper use of auto where the init variable type and the
// initializer type differ or are mostly the same except for const qualifiers.
void different_type() {
  // differs from const_iterator only on the const qualification.
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  // different from const_iterator, disqualifies these loops from
  // transformation.
  }

  }
}

// Tests to ensure that an implicit 'this' is picked up as the container.
// If member calls are made to 'this' within the loop, the transform becomes
// risky as these calls may affect state that affects the loop.
class C {
public:
  typedef MutableVal *iterator;
  typedef const MutableVal *const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  void doSomething();
  void doSomething() const;

  void doLoop() {
    for (iterator I = begin(), E = end(); I != E; ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead

    for (iterator I = C::begin(), E = C::end(); I != E; ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead

    for (iterator I = begin(), E = end(); I != E; ++I) {
      (void) *I;
      doSomething();
    }

    for (iterator I = begin(); I != end(); ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead

    for (iterator I = begin(); I != end(); ++I) {
      (void) *I;
      doSomething();
    }
  }

  void doLoop() const {
    for (const_iterator I = begin(), E = end(); I != E; ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead

    for (const_iterator I = C::begin(), E = C::end(); I != E; ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead

    for (const_iterator I = begin(), E = end(); I != E; ++I) {
      (void) *I;
      doSomething();
    }
  }
};

class C2 {
public:
  typedef MutableVal *iterator;

  iterator begin() const;
  iterator end() const;

  void doLoop() {
    // The implicit 'this' will have an Implicit cast to const C2* wrapped
    // around it. Make sure the replacement still happens.
    for (iterator I = begin(), E = end(); I != E; ++I)
      (void) *I;
    // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: use range-based for loop instead
  }
};

} // namespace Iterator

namespace PseudoArray {

const int N = 6;
dependent<int> V;
dependent<int> *Pv;
const dependent<NonTriviallyCopyable> Constv;
const dependent<NonTriviallyCopyable> *Pconstv;

transparent<dependent<int>> Cv;

void f() {
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  // This test will fail if size() isn't called repeatedly, since it
  // returns unsigned int, and 0 is deduced to be signed int.
  // FIXME: Insert the necessary explicit conversion, or write out the types
  // explicitly.
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
}

// Ensure that 'const auto &' is used with containers of non-trivial types.
void constness() {
  int Sum = 0;
  for (int I = 0, E = Constv.size(); I < E; ++I) {
    printf("Fibonacci number is %d\n", Constv[I].X);
    Sum += Constv[I].X + 2;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : Constv)
  // CHECK-FIXES-NEXT: printf("Fibonacci number is %d\n", Elem.X);
  // CHECK-FIXES-NEXT: Sum += Elem.X + 2;

  for (int I = 0, E = Constv.size(); I < E; ++I) {
    printf("Fibonacci number is %d\n", Constv.at(I).X);
    Sum += Constv.at(I).X + 2;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : Constv)
  // CHECK-FIXES-NEXT: printf("Fibonacci number is %d\n", Elem.X);
  // CHECK-FIXES-NEXT: Sum += Elem.X + 2;

  for (int I = 0, E = Pconstv->size(); I < E; ++I) {
    printf("Fibonacci number is %d\n", Pconstv->at(I).X);
    Sum += Pconstv->at(I).X + 2;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : *Pconstv)
  // CHECK-FIXES-NEXT: printf("Fibonacci number is %d\n", Elem.X);
  // CHECK-FIXES-NEXT: Sum += Elem.X + 2;

  // This test will fail if size() isn't called repeatedly, since it
  // returns unsigned int, and 0 is deduced to be signed int.
  // FIXME: Insert the necessary explicit conversion, or write out the types
  // explicitly.
  for (int I = 0; I < Pconstv->size(); ++I) {
    printf("Fibonacci number is %d\n", (*Pconstv).at(I).X);
    Sum += (*Pconstv)[I].X + 2;
  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (const auto & Elem : *Pconstv)
  // CHECK-FIXES-NEXT: printf("Fibonacci number is %d\n", Elem.X);
  // CHECK-FIXES-NEXT: Sum += Elem.X + 2;
}

void constRef(const dependent<int>& ConstVRef) {
  int sum = 0;
  // FIXME: This does not work with size_t (probably due to the implementation
  // of dependent); make dependent work exactly like a std container type.
  for (int i = 0; i < ConstVRef.size(); ++i) {
    sum += ConstVRef[i];
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto Elem : ConstVRef)
  // CHECK-FIXES-NEXT: sum += Elem;

  for (auto I = ConstVRef.begin(), E = ConstVRef.end(); I != E; ++I) {
    sum += *I;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead
  // CHECK-FIXES: for (auto Elem : ConstVRef)
  // CHECK-FIXES-NEXT: sum += Elem;
}

// Check for loops that don't mention containers.
void noContainer() {
  }

    ;
}

struct NoBeginEnd {
  unsigned size() const;
  unsigned& operator[](int);
  const unsigned& operator[](int) const;
};

struct NoConstBeginEnd {
  NoConstBeginEnd();
  unsigned size() const;
  unsigned* begin();
  unsigned* end();
  unsigned& operator[](int);
  const unsigned& operator[](int) const;
};

struct ConstBeginEnd {
  ConstBeginEnd();
  unsigned size() const;
  unsigned* begin() const;
  unsigned* end() const;
  unsigned& operator[](int);
  const unsigned& operator[](int) const;
};

// Shouldn't transform pseudo-array uses if the container doesn't provide
// begin() and end() of the right const-ness.
void NoBeginEndTest() {
  NoBeginEnd NBE;


  ConstBeginEnd CBE;
  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: use range-based for loop instead

  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: use range-based for loop instead
}

struct DerefByValue {
  DerefByValue();
  struct iter { unsigned operator*(); };
  unsigned size() const;
  iter begin();
  iter end();
  unsigned operator[](int);
};

void derefByValueTest() {
  DerefByValue DBV;
  }
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: use range-based for loop instead

  }
  // CHECK-MESSAGES: :[[@LINE-4]]:3: warning: use range-based for loop instead
}

} // namespace PseudoArray
