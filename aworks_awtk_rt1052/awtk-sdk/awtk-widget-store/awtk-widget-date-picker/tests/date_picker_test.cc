#include "date_picker/date_picker.h"
#include "gtest/gtest.h"

TEST(date_picker, basic) {
  value_t v;
  widget_t* w = date_picker_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}
