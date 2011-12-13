<?php

class C1 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
    echo "\n";
  }
}
// call_user_func
call_user_func(array('C1', "__callStatic"), "a", "b", "c", "d");
call_user_func(array('C1', "foo"), "a", "b", "c", "d");

// call_user_func_array
call_user_func_array(array('C1', "__callStatic"), array("a", "b", "c", "d"));
call_user_func_array(array('C1', "foo"), array("a", "b", "c", "d"));



class C2 {
  public static function __callStatic($fn, $args) {
    echo "C2::__callStatic\n";
    var_dump($fn, $args);
    echo "\n";
  }
  public function test() {
    call_user_func(array('C2', '__callStatic'), "a", "b", "c", "d");
    call_user_func(array('C2', 'foo'), "a", "b", "c", "d");
    call_user_func(array('self', '__callStatic'), "a", "b", "c", "d");
    call_user_func(array('self', 'foo'), "a", "b", "c", "d");
  }
}
$obj = new C2;
$obj->test();



class C3 {
  public static function __callStatic($fn, $args) {
    echo "C3::__callStatic\n";
    var_dump($fn, $args);
    echo "\n";
  }
  public static function test() {
    // FPushClsMethodD
    call_user_func(array('C3', 'foo'), "a", "b", "c", "d");
    // FPushClsMethodF
    call_user_func(array('self', 'foo'), "a", "b", "c", "d");
  }
}
$obj = new C3;
$obj->test();



class B4 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
    echo "\n";
  }
}
class C4 extends B4 {
}
$obj = new C4;

call_user_func(array('C4', 'foo'), "a", "b", "c", "d");



class A5 {
  public function foo($w, $x, $y, $z) {
    echo "A5::foo\n";
  }
}
class B5 extends A5 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
    echo "\n";
  }
}
class C5 extends B5 {
}
$obj = new C5;
call_user_func(array('C5', 'foo'), "a", "b", "c", "d");



class A6 {
}
class B6 extends A6 {
  public function test() {
    call_user_func('A6::foo', 1, 2, 3);
    call_user_func(array('A6','foo'), 1, 2, 3);
  }
}
class C6 extends B6 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
  }
}

$obj = new C6;
$obj->test();

$obj = new B6;
$obj->test();



class A7 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
  }
}
class B7 extends A7 {
  public function test() {
    call_user_func('A7::foo', 1, 2, 3);
    call_user_func(array('A7', 'foo'), 1, 2, 3);
  }
}
class C7 extends B7 {
}

$obj = new C7;
$obj->test();

$obj = new B7;
$obj->test();



class A8 {
  public static function __callStatic($fn, $args) {
    var_dump($fn, $args);
  }
}
class B8 {
  public function test() {
    call_user_func('A8::foo', 1, 2, 3);
    call_user_func(array('A8', 'foo'), 1, 2, 3);
  }
}
class C8 extends B8 {
}

$obj = new C8;
$obj->test();



class C9 {
}
class D9 extends C9 {
  public static function __callStatic($fn, $args) {
    echo "D9::__callStatic\n";
  }
}
class E9 extends D9 {
  public static function __callStatic($fn, $args) {
    echo "E9::__callStatic\n";
  }
  public function test() {
    call_user_func(array('self', 'foo'));
    call_user_func(array('D9', 'foo'));
    call_user_func(array('E9', 'foo'));
  }
}
class F9 extends D9 {
  public static function __callStatic($fn, $args) {
    echo "F9::__callStatic\n";
  }
}
$obj = new E9;
$obj->test();

call_user_func(array('E9', 'foo'));
call_user_func(array('D9', 'foo'));
