// RUN: %clang_cc1 -fsyntax-only -verify -fpattern-matching -Wno-string-compare %s

void noParen() {
  inspect 42 { // expected-error {{expected '(' after 'inspect'}}
    __ => {};
  };
}

void noCondition() {
  inspect() { // expected-error {{expected expression}}
    __ => {};
  };
}

void noParenConstExpr() {
  inspect constexpr 42 { // expected-error {{expected '(' after 'constexpr'}}
    __ => {};
  };
}

void wildcardMissingEqualsArrow() {
  inspect(42) {
    __ 7; // expected-error {{expected '=>' after wildcard pattern}}
  }
}

void noSemiColon() {
  inspect(42) {
    __ => {};
  } // expected-error {{expected ';' after expression}}
}

void trailingReturnTypes() {
  int r = 0;
  r = inspect(42) -> int {
    __ => 3;
  };

  r = inspect(42) -> decltype(1) {
    __ => 3;
  };

  inspect(42) -> void {
    __ => {};
  };

  int x;
  r = inspect(42) -> decltype(x) {
    __ => 3;
  };

  int y;
  inspect(42) -> y { // expected-error {{unknown type name 'y'}}
    __ => {};
  };

  inspect(42) -> { // expected-error {{expected a type}}
    __ => {};
  };

  inspect(42) -> foo { // expected-error {{unknown type name 'foo'}}
    __ => {};
  };
}

void returnTypeDeduction() {
  int x = 0;
  x += inspect(42) {
    42 => 42;
    43 => 43;
    __ => !{}; // exclude last from type deduction
  };

  x += inspect(42) {
    42 => 42;
    __ => !{}; // exclude middle from type deduction
    43 => 43;
  };

  x += inspect(42) {
    __ => !{}; // exclude first from type deduction
    42 => 42;
    43 => 43;
  };

  x += inspect(42) {
    42 => 42;
    __ => !throw "whoops"; // expected-error {{expected '{' after '!'}}
  };
}

// Identifier pattern parsing
int id_pat0() {
  int x = 3;
  return inspect(x) {
    y => y++;
    __ => 3;
  };
}

// Expression pattern parsing
int exp_pat0(int x) {
 return inspect (x) {
    1 => 0;
    2 => 1;
    __ => 42;
 };
}

void exp_pat1(const char *s) {
 inspect (s) {
    "foo" => {}
    "bar" => {}
    __ => {}
 };
}

// FIXME: should we support scoped enums?
enum Color { Red, Green, Blue };

void exp_pat2(Color color) {
  inspect (color) {
    Color::Red => {}
    Color::Green => {}
    Color::Blue => {}
  };
}

void exp_pat3(int x) {
  inspect (x) {
    SomeEnum::Red => {} // expected-error {{use of undeclared identifier 'SomeEnum'}}
                        // expected-error@-1 {{expected constant-expression}}
    case RED => {} // expected-error {{use of undeclared identifier 'RED'}}
                   // expected-error@-1 {{expected constant-expression}}
  };
}

void exp_case0(Color color) {
  inspect (color) {
    case Color::Red => {}
    case Color::Green => {}
    case Color::Blue => {}
  };
}

void exp_case1(Color c) {
  inspect (c) {
    case Red => {}
    case Green => {}
    case Blue => {}
  };
}

int exp_case2(int x) {
  int y;
  y = inspect(x) -> int {
    case 0 => 1;
    case 1 => 2;
  };
  return y;
}

void exp_case3(const char *s) {
 inspect (s) {
    case "foo" => {}
    case "bar" => {}
  };
}

void bad_equal_arrow(int x) {
  const int h = 42;
  inspect (x) {
    case h = {} // expected-error {{expected '=>' after constant-expression}}
    y if (h != 0) = > {}; // expected-error {{expected '=>' after if}}
  };
}

void stbind0(int x) {
  struct s {
    int a;
    int b;
  };
  s cond{1,2};
  constexpr int idx = 3;
  inspect (cond) {
    [] =>; // expected-error {{empty structured binding pattern is invalid}}
    [0,] =>; // expected-error {{expected expression}}
    [0,,] =>; // expected-error {{expected expression}}
    [0 0] =>; // expected-error {{expected ',' or ']' in pattern list}}
    [,] =>; // expected-error {{expected expression}}
    [1,2] =>;
    [1,idx] =>;
    id => { id.a++; }
    [1,new_id] => { new_id++; };
    [new_id1, new_id2] =>;
    [__,__] =>;
  };

  struct color_pack {
    Color c1, c2;
  };
  color_pack cp{Red, Blue};
  inspect (cp) {
    [case Red, case Green] =>;
    [Color::Red, Color::Blue] =>;
    [Green, case Color::Red] =>;
  };
}