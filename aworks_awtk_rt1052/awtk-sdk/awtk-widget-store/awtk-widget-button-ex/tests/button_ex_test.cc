#include "button_ex/button_ex.h"
#include "gtest/gtest.h"

TEST(button_ex, basic) {
  value_t v;
  widget_t* w = button_ex_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}
