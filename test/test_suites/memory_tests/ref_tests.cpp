/**
 * \file test_suites/memory_tests/ref_tests.cpp
 **/
#include <gtest/gtest.h>

#include "core/ref.hpp"

#include "tensorlib_testing.hpp"

namespace tmem = tensor::memory;

class ref_tests : public ::testing::Test {
 protected:
  tmem::arena* arena;

  void SetUp() override {
    arena = tensor::testing::get_test_environment()->get_memory_arena();
  }

  void TearDown() override {
    arena = nullptr;
  }
};

class test_object : public tensor::ref_counted {
 public:
  test_object() = default;
  ~test_object() override = default;
};

TEST_F(ref_tests, basic_ref_counted) {
  tensor::ref<test_object> ref1 = tensor::make_ref<test_object>();

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);
  {
    tensor::ref<test_object> ref2 = ref1;
    EXPECT_EQ(ref1->count(), 2);
    EXPECT_EQ(ref2->count(), 2);
    EXPECT_EQ(ref1->view_count(), 0);
    EXPECT_EQ(ref2->view_count(), 0);
  }

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);
}

TEST_F(ref_tests, pass_by_value) {
  tensor::ref<test_object> ref1 = tensor::make_ref<test_object>();

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);

  auto pass_by_ref = [](tensor::ref<test_object> r) -> bool {
    return r->count() == 2 && r->view_count() == 0;
  };

  EXPECT_TRUE(pass_by_ref(ref1));

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);
}

TEST_F(ref_tests, move_ref) {
  tensor::ref<test_object> ref1 = tensor::make_ref<test_object>();

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);

  tensor::ref<test_object> ref2 = std::move(ref1);

  EXPECT_EQ(ref2->count(), 1);
  EXPECT_EQ(ref2->view_count(), 0);
  EXPECT_EQ(ref1, nullptr);
}

/// should not change ref count or view count
TEST_F(ref_tests, pass_by_reference) {
  tensor::ref<test_object> ref1 = tensor::make_ref<test_object>();

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);

  auto pass_by_ref = [](const tensor::ref<test_object>& r) -> bool {
    return r->count() == 1 && r->view_count() == 0;
  };

  EXPECT_TRUE(pass_by_ref(ref1));

  EXPECT_EQ(ref1->count(), 1);
  EXPECT_EQ(ref1->view_count(), 0);
}