#include "label_rotate/label_rotate.h"
#include "gtest/gtest.h"

TEST(label_rotate, basic) {
  value_t v;
  widget_t* w = label_rotate_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(label_rotate_set_length(w, 100), RET_OK);
  ASSERT_EQ(LABEL_ROTATE(w)->length, 100);

  ASSERT_EQ(label_rotate_set_orientation(w, LABEL_ROTATE_ORIENTATION_90), RET_OK);
  ASSERT_EQ(LABEL_ROTATE(w)->orientation, LABEL_ROTATE_ORIENTATION_90);

  widget_destroy(w);
}
