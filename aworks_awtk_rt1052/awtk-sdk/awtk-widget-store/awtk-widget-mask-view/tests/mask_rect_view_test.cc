#include "mask_view/mask_view.h"
#include "gtest/gtest.h"

TEST(mask_view, basic) {
  value_t v;
  widget_t* w = mask_view_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}
