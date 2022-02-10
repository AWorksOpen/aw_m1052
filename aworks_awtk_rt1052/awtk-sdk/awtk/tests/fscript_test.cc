﻿#include "tkc/utf8.h"
#include "tkc/fscript.h"
#include "tkc/object_default.h"
#include "gtest/gtest.h"

TEST(FScript, basic0) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "1", &v);
  ASSERT_EQ(1, value_int(&v));
  value_reset(&v);

  fscript_eval(obj, "-1", &v);
  ASSERT_EQ(-1, value_int(&v));
  value_reset(&v);

  fscript_eval(obj, "+1", &v);
  ASSERT_EQ(1, value_int(&v));
  value_reset(&v);

  fscript_eval(obj, "true", &v);
  ASSERT_EQ(TRUE, value_bool(&v));
  value_reset(&v);

  fscript_eval(obj, "false", &v);
  ASSERT_EQ(FALSE, value_bool(&v));
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic1) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2)", &v);
  ASSERT_EQ(3, value_int(&v));
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic2) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2);sum(1, 2, 3, 4)", &v);
  ASSERT_EQ(10, value_int(&v));
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic3) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "sum(1, 2)\nsum(1, 2, 3, 4)\nsum(123, 321)", &v);
  ASSERT_EQ(444, value_int(&v));
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic4) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "print(sum(1, 2))", &v);
  ASSERT_EQ(true, value_bool(&v));
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic5) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "sum \n(1, 2)", &v);
  ASSERT_EQ(3, value_int(&v));
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic6) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "unset(a)", &v);
  ASSERT_EQ(TRUE, value_bool(&v));
  value_reset(&v);

  fscript_eval(obj, "unset(abc)", &v);
  ASSERT_EQ(TRUE, value_bool(&v));
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic7) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sum(1, 2)//line comments\n", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "//comment\r\nsum(1, 2)//line comments\n", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "/*comment*//**/\r\nsum(1, 2)//line comments\n", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic8) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "\"hello\"", &v);
  ASSERT_STREQ(value_str(&v), "hello");
  value_reset(&v);

  fscript_eval(obj, "\"he\\\"llo\"", &v);
  ASSERT_STREQ(value_str(&v), "he\"llo");
  value_reset(&v);

  fscript_eval(obj, "'he\\\"llo'", &v);
  ASSERT_STREQ(value_str(&v), "he\"llo");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, basic9) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "0xf0112233", &v);
  ASSERT_EQ(value_uint32(&v), 0xf0112233u);
  value_reset(&v);

  fscript_eval(obj, "0b1100", &v);
  ASSERT_EQ(value_uint32(&v), 0b1100u);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, number) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "0x1122334455667788", &v);
  ASSERT_EQ(value_int64(&v), 0x1122334455667788);
  value_reset(&v);

  fscript_eval(obj, "-0x1122334455667788", &v);
  ASSERT_EQ(value_int64(&v), -0x1122334455667788);
  value_reset(&v);

  fscript_eval(obj, "0x8822334455667788", &v);
  ASSERT_EQ(value_uint64(&v), 0x8822334455667788u);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, one_of) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "one_of(\"aa;bb;cc\", 0)", &v);
  ASSERT_STREQ("aa", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa;bb;cc\", 1)", &v);
  ASSERT_STREQ("bb", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa;bb;cc\", 2)", &v);
  ASSERT_STREQ("cc", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa;bb;cc\", 3)", &v);
  ASSERT_STREQ("", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa.bb.cc\", 0,\".\")", &v);
  ASSERT_STREQ("aa", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa.bb.cc\", 1,\".\")", &v);
  ASSERT_STREQ("bb", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa.bb.cc\", 2,\".\")", &v);
  ASSERT_STREQ("cc", value_str(&v));
  value_reset(&v);

  fscript_eval(obj, "one_of(\"aa.bb.cc\", 3,\".\")", &v);
  ASSERT_STREQ("", value_str(&v));
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, if1) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "if(1, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "hello");
  value_reset(&v);

  fscript_eval(obj, "if(0, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "world");
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, while1) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "set(a,0);set(b,0);while(<=(a,100), set(b, +(b, a)),set(a, +(a,1)));int(b)",
               &v);
  ASSERT_EQ(value_int(&v), 5050);
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, if2) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "if(true, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "hello");
  value_reset(&v);

  fscript_eval(obj, "if(false, \"hello\", \"world\")", &v);
  ASSERT_STREQ(value_str(&v), "world");
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, if3) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "if(true, sum(1, 2), sum(1, 2, 3))", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "if(false, sum(1, 2), sum(1, 2, 3))", &v);
  ASSERT_EQ(value_int(&v), 6);
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, convert) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "int(\"3\")", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "float(\"3.5\")", &v);
  ASSERT_EQ(value_float(&v), 3.5);
  value_reset(&v);

  fscript_eval(obj, "str(123)", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  fscript_eval(obj, "str(int(123))", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  const char* str = "abc123";
  tk_object_set_prop_pointer(obj, "abc", (void*)str);

  fscript_eval(obj, "str(abc,true)", &v);
  ASSERT_STREQ(value_str(&v), "abc123");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, join) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "join(\",\",\"a\", \"b\", \"c\")", &v);
  ASSERT_STREQ(value_str(&v), "a,b,c");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, set) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "set(a, 123)sum(a, 321)", &v);
  ASSERT_EQ(value_int(&v), 444);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, iformat) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "iformat(\"hello:%d\", 123)", &v);
  ASSERT_STREQ(value_str(&v), "hello:123");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, fformat) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "fformat(\"hello:%lf\", 123)", &v);
  ASSERT_STREQ(value_str(&v), "hello:123.000000");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, mod) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_eval(obj, "%(23, 7)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, and) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "and(true, true)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "and(true, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "and(false, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, or) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "or(true, true)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "||(true, false)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "||(false, false)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, not ) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "not(true)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "!(false)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, bit_and) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "&(1, 1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "&(1, 2)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, bit_or) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "|(1, 1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "|(1, 2)", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, bit_not) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "~(1)", &v);
  ASSERT_EQ(value_int(&v), ~1);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, less) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "<(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "<(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, le) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "<=(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "<=(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, great) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, ">(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, ">(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, ge) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "ge(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "ge(2, 1)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, eq) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "eq(1, 2)", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "eq(2, 2)", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "eq(\"2\", \"2\")", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);
  TK_OBJECT_UNREF(obj);
}

TEST(FScript, trim) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "trim(\" aaa \")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  fscript_eval(obj, "trim(\"aaa \")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, len) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "len(\"aaa\")", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "len(\"\")", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, toupper) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "toupper(\"aaa\")", &v);
  ASSERT_STREQ(value_str(&v), "AAA");
  value_reset(&v);

  fscript_eval(obj, "trim(\"AAA\")", &v);
  ASSERT_STREQ(value_str(&v), "AAA");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, tolower) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "tolower(\"aaa\")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  fscript_eval(obj, "tolower(\"AAA\")", &v);
  ASSERT_STREQ(value_str(&v), "aaa");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, replace) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "replace(\"aaa\", \"a\", \"B\")", &v);
  ASSERT_STREQ(value_str(&v), "BBB");
  value_reset(&v);

  fscript_eval(obj, "replace(\"aaa123\", \"a\", \"\")", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, contains) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "contains(\"aaa\", \"a\")", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "contains(\"aaa\", \"A\")", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, substr) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "substr(\"1234567\", 1, 3)", &v);
  ASSERT_STREQ(value_str(&v), "234");
  value_reset(&v);

  fscript_eval(obj, "substr(\"1234567\", 0, 3)", &v);
  ASSERT_STREQ(value_str(&v), "123");
  value_reset(&v);

  fscript_eval(obj, "substr(\"1234567\", 0, 300)", &v);
  ASSERT_STREQ(value_str(&v), "1234567");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, min) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "min(100, 50)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  fscript_eval(obj, "min(50, 100)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, max) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "max(100, 50)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  fscript_eval(obj, "max(50, 100)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, clamp) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "clamp(100, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  fscript_eval(obj, "clamp(10, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 50);
  value_reset(&v);

  fscript_eval(obj, "clamp(1000, 50, 200)", &v);
  ASSERT_EQ(value_int(&v), 200);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, pow) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "pow(3, 2)", &v);
  ASSERT_EQ(value_int(&v), 9);
  value_reset(&v);

  fscript_eval(obj, "pow(3, 0)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, sqrt) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sqrt(4)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, complex) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sub(sum(sqrt(4), sqrt(9)), 1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "if(true, 123, sub(sum(sqrt(4), sqrt(9)), 1))", &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  fscript_eval(obj, "if(false, 123, sub(sum(sqrt(4), sqrt(9)), 1))", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "sqrt(if(<(3, 1), 123, sub(sum(sqrt(4), sqrt(9)), 1)))", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

static ret_t func_foo(fscript_t* fscript, fscript_args_t* args, value_t* v) {
  value_set_int(v, 123);

  return RET_OK;
}

TEST(FScript, func) {
  value_t v;
  tk_object_t* obj = object_default_create();
  tk_object_set_prop_pointer(obj, "function.foo", (void*)func_foo);

  fscript_eval(obj, "foo()", &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, term1) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "2", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, term2) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "(2)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "( 2 + 2 )", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "( 2+2 )", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, term3) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sqrt(4)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "sum(4,4)", &v);
  ASSERT_EQ(value_int(&v), 8);
  value_reset(&v);

  fscript_eval(obj, "sum( 4 , 2, 2)", &v);
  ASSERT_EQ(value_int(&v), 8);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, sum) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1+1", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "1+(1)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "(1)+(1)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "(1+1)+(1+1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "sum(1+1)+sum(1+1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "1-1", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "1-(1)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "(1)-(1)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "(1+1)-(1+1)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "sum(1+1)-sum(1+1)", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, product) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1*1", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "1*(1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "(1)*(1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "(1+1)*(1+1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "sum(1+1)*sum(1+1)", &v);
  ASSERT_EQ(value_int(&v), 4);
  value_reset(&v);

  fscript_eval(obj, "1/1", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "1/(1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "(1)/(1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "(1+1)/(1+1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "sum(1+1)/sum(1+1)", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, or) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "true||true", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "true||false", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "true||0", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "false||false", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, and) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "true && true", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "true&&false", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "true && 0", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "false && false", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, bit_or) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1|2", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  fscript_eval(obj, "1|1", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "1|0", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "3|0", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, bit_and) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1&2", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "1&1", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "1&0", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  fscript_eval(obj, "3 & 0", &v);
  ASSERT_EQ(value_int(&v), 0);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, if) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1<2?1:2", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "1>2?1:2", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  fscript_eval(obj, "(1<2)?1:2", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  fscript_eval(obj, "(1>2)?(1):(2)", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, unary) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "!true", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  fscript_eval(obj, "!!true", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  fscript_eval(obj, "!!!true", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  fscript_eval(obj, "~1", &v);
  ASSERT_EQ(value_int32(&v), -2);
  value_reset(&v);

  fscript_eval(obj, "~~1", &v);
  ASSERT_EQ(value_uint32(&v), 1u);
  value_reset(&v);

  fscript_eval(obj, "~~~1", &v);
  ASSERT_EQ(value_uint32(&v), (uint32_t)-2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, var) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "set(a,100);(a+100)*2", &v);
  ASSERT_EQ(value_int(&v), 400);
  value_reset(&v);

  fscript_eval(obj, "set(a,100);set(a, (a+100)*2);a+100", &v);
  ASSERT_EQ(value_int(&v), 500);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, var_name) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "set(a.b.c,100);(a.b.c+100)*2", &v);
  ASSERT_EQ(value_int(&v), 400);
  value_reset(&v);

  fscript_eval(obj, "set(a.b.c#size,100);(a.b.c#size+100)*2", &v);
  ASSERT_EQ(value_int(&v), 400);
  value_reset(&v);

  fscript_eval(obj, "set(a.b.c[0]#size,100);(a.b.c[0]#size+100)*2", &v);
  ASSERT_EQ(value_int(&v), 400);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExpr, not_eq) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "1 != 2", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "2!=2", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "1.0 != 2.0", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  fscript_eval(obj, "2.0!=2.0", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "\"a\" != \"a\"", &v);
  ASSERT_EQ(value_bool(&v), false);
  value_reset(&v);

  fscript_eval(obj, "\"a\" != \"b\"", &v);
  ASSERT_EQ(value_bool(&v), true);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, global_func) {
  value_t v;
  tk_object_t* obj = object_default_create();
  fscript_register_func("foo", func_foo);

  fscript_eval(obj, "foo()", &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, var_dollar) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "aaa+\"b\"", &v);
  ASSERT_STREQ(value_str(&v), "aaab");
  value_reset(&v);

  fscript_eval(obj, "$aaa+\"b\"", &v);
  ASSERT_STREQ(value_str(&v), "b");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, error) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sin();has_error()", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  fscript_eval(obj, "sin(1);has_error()", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, clear_error) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "sin();clear_error();has_error()", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, if_statement1) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "if(1) {1} else {2}", &v);
  ASSERT_EQ(value_int(&v), 1);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, if_statement2) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "if(false) {1} else {2}", &v);
  ASSERT_EQ(value_int(&v), 2);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, while_statement1) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "abc=0;while(abc<100) {abc = abc+1};abc", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, while_statement2) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "abc=0;while(abc<100) {abc = abc+1;if(abc > 50) {break}};abc", &v);
  ASSERT_EQ(value_int(&v), 51);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, while_statement3) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "a=0;b=0;while(a<100) {a=a+1;if((a > 20) && (a<=50)) {continue};b=b+1};b", &v);
  ASSERT_EQ(value_int(&v), 70);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, syntax_check) {
  tk_object_t* obj = object_default_create();
  fscript_parser_error_t error;
  fscript_syntax_check(obj, "1", &error);
  ASSERT_EQ(error.message, (const char*)NULL);
  fscript_parser_error_deinit(&error);

  fscript_syntax_check(obj, "(1+1", &error);
  ASSERT_STREQ(error.message, "expect )");
  ASSERT_EQ(error.row, 0);
  ASSERT_EQ(error.col, 4);
  ASSERT_EQ(error.offset, 4);
  fscript_parser_error_deinit(&error);

  fscript_syntax_check(obj, "1+1)", &error);
  ASSERT_STREQ(error.token, ")");
  ASSERT_STREQ(error.message, "unexpected token\n");
  ASSERT_EQ(error.row, 0);
  ASSERT_EQ(error.col, 4);
  ASSERT_EQ(error.offset, 4);

  fscript_parser_error_deinit(&error);

  fscript_syntax_check(obj, "create_array().size", &error);
  ASSERT_STREQ(error.message, "var can't begin with '.'");

  fscript_parser_error_deinit(&error);

  TK_OBJECT_UNREF(obj);
}

#ifdef FSCRIPT_WITH_WIDGET
#include "widgets/button.h"
TEST(FExr, widget) {
  value_t v;
  tk_object_t* obj = object_default_create();
  widget_t* button = button_create(NULL, 10, 20, 80, 30);
  tk_object_set_prop_pointer(obj, STR_PROP_SELF, button);
  widget_set_text_utf8(button, "hello");

  fscript_eval(obj, "widget_get(\"text\")==\"hello\"", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  fscript_eval(obj, "widget_get(\"text\")==\"Hello\"", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  fscript_eval(obj, "widget_get(\"x\")==10", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  fscript_eval(obj, "widget_get(\"y\")==20", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  widget_destroy(button);
  TK_OBJECT_UNREF(obj);
}
#endif /*FSCRIPT_WITH_WIDGET*/

TEST(FExr, minus) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "a=100;a=-a;a", &v);
  ASSERT_EQ(value_int(&v), -100);
  value_reset(&v);

  fscript_eval(obj, "a=100;a=-  a;a", &v);
  ASSERT_EQ(value_int(&v), -100);
  value_reset(&v);

  fscript_eval(obj, "a=100;a=- /*hello*/ a;a", &v);
  ASSERT_EQ(value_int(&v), -100);
  value_reset(&v);

  fscript_eval(obj, "a=100;a=0-a;a", &v);
  ASSERT_EQ(value_int(&v), -100);
  value_reset(&v);

  fscript_eval(obj, "a=100;a=---a;a", &v);
  ASSERT_EQ(value_int(&v), -100);
  value_reset(&v);

  fscript_eval(obj, "a=100;a=--a;a", &v);
  ASSERT_EQ(value_int(&v), 100);
  value_reset(&v);

  fscript_eval(obj, "a=-(100-300);a", &v);
  ASSERT_EQ(value_int(&v), 200);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, get) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "value_is_valid(get(abc))", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  fscript_eval(obj, "abc=123;value_is_valid(get(abc))", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, dollor) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "value_is_valid($abc)", &v);
  ASSERT_EQ(value_bool(&v), FALSE);
  value_reset(&v);

  fscript_eval(obj, "abc=123;value_is_valid(abc)", &v);
  ASSERT_EQ(value_bool(&v), TRUE);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, get_last_error) {
  value_t v;
  tk_object_t* obj = object_default_create();
  tk_object_t* error = NULL;
  fscript_eval(obj, "print(123);\nprint(len());\nget_last_error()", &v);
  error = value_object(&v);
  ASSERT_EQ(tk_object_get_prop_int(error, "code", 0), 16);
  ASSERT_EQ(tk_object_get_prop_int(error, "line", 0), 1);
  ASSERT_EQ(tk_object_get_prop_int(error, "col", 0), 10);
  ASSERT_STREQ(tk_object_get_prop_str(error, "message"), "args->size == 1 not satisfied.");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, chinese_var) {
  value_t v;
  char code[128];
  const wchar_t* wcode = L"set(变量,100);(变量+100)*2";
  tk_object_t* obj = object_default_create();

  tk_utf8_from_utf16(wcode, code, sizeof(code));
  fscript_eval(obj, code, &v);

  ASSERT_EQ(value_int(&v), 400);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, chinese_func) {
  value_t v;
  char code[128];
  tk_object_t* obj = object_default_create();
  const wchar_t* wcode = L"function 求和(数1,数2) {return 数1+数2;}; 求和(100, 23)";

  tk_utf8_from_utf16(wcode, code, sizeof(code));
  fscript_eval(obj, code, &v);

  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, global) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "global.count=400;global.count", &v);
  ASSERT_EQ(value_int(&v), 400);
  fscript_eval(obj, "global.count=global.count+400;global.count", &v);
  ASSERT_EQ(value_int(&v), 800);
  fscript_eval(obj, "global.count=global.count+400;global.count", &v);
  ASSERT_EQ(value_int(&v), 1200);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, text_count) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "text_count(\"abc abc\", \"abc\")", &v);
  ASSERT_EQ(value_int(&v), 2);
  fscript_eval(obj, "text_count(\"abc abc abc\", \"abc\")", &v);
  ASSERT_EQ(value_int(&v), 3);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, text_reverse) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "text_reverse(\"abc\")", &v);
  ASSERT_STREQ(value_str(&v), "cba");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, list_reverse) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(
      obj, "a=array_create_with_str(\"1,2,3\", \",\", \"i\");array_reverse(a);array_join(a,\",\")",
      &v);
  ASSERT_STREQ(value_str(&v), "3,2,1");
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FExr, unset) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(
      obj, "a=\"abc\";aaa=123;unset(aaa);a",
      &v);
  ASSERT_STREQ(value_str(&v), "abc");
  value_reset(&v);
  
  fscript_eval(
      obj, "a=\"abc\";aaa=123;unset(a);aaa",
      &v);
  ASSERT_EQ(value_int(&v), 123);
  value_reset(&v);

  TK_OBJECT_UNREF(obj);
}

TEST(FScript, convert1) {
  value_t v;
  tk_object_t* obj = object_default_create();

  fscript_eval(obj, "i8(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_INT8, true);
  ASSERT_EQ(123, value_int8(&v));
  
  fscript_eval(obj, "u8(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_UINT8, true);
  ASSERT_EQ(123, value_uint8(&v));
  
  fscript_eval(obj, "i16(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_INT16, true);
  ASSERT_EQ(123, value_int16(&v));
  
  fscript_eval(obj, "u16(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_UINT16, true);
  ASSERT_EQ(123, value_uint16(&v));
  
  fscript_eval(obj, "i32(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_INT32, true);
  ASSERT_EQ(123, value_int32(&v));
  
  fscript_eval(obj, "u32(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_UINT32, true);
  ASSERT_EQ(123, value_uint32(&v));
  
  fscript_eval(obj, "i64(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_INT64, true);
  ASSERT_EQ(123, value_int64(&v));
  
  fscript_eval(obj, "u64(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_UINT64, true);
  ASSERT_EQ(123, value_uint64(&v));
  
  fscript_eval(obj, "f32(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_FLOAT32, true);
  ASSERT_EQ(123, value_float32(&v));
  
  fscript_eval(obj, "float(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_FLOAT32, true);
  ASSERT_EQ(123, value_float32(&v));
  
  fscript_eval(obj, "f64(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_DOUBLE, true);
  ASSERT_EQ(123, value_double(&v));
  
  fscript_eval(obj, "double(\"123\")", &v);
  ASSERT_EQ(v.type == VALUE_TYPE_DOUBLE, true);
  ASSERT_EQ(123, value_double(&v));

  value_reset(&v);

}

