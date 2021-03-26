#include "template/template.h"
#include "gtest/gtest.h"

TEST(template, basic) {
  value_t v;
  widget_t* w = template_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}
