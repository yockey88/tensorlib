/**
 * \file test_suites/memory_tests/arena_tests.cpp
 **/
#include <gtest/gtest.h>

#include "core/arena.hpp"
#include "core/arena_allocator.hpp"

#include "tensorlib_testing.hpp"

namespace tmem = tensor::memory;

class arena_tests : public ::testing::Test {
 protected:
  tmem::arena* arena;

  size_t bytes_allocated = 0;
  size_t bytes_freed = 0;

  void SetUp() override {
    arena = tensor::testing::get_test_environment()->get_memory_arena();
  }

  void TearDown() override {
    arena = nullptr;
  }
};

TEST_F(arena_tests, main_arena_tests) {
  {
    ASSERT_EQ(arena->total_allocations, 1);  /// the first page
    ASSERT_EQ(arena->bytes_freed, 0);
    ASSERT_EQ(arena->bytes_allocated, tmem::arena::kPageSize);
    ASSERT_EQ(arena->living_memory(), tmem::arena::kPageSize);

    /// TODO: custom alignment
    void* memory = tmem::allocate_in_arena(arena, sizeof(int));
    new (memory) int(10);
    ASSERT_NE(memory, nullptr);
    EXPECT_EQ(*static_cast<int*>(memory), 10);

    EXPECT_EQ(arena->total_allocations, 2);  /// first page + 1 allocation
    EXPECT_EQ(arena->bytes_freed, 0);
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize + sizeof(int));

    tmem::deallocate_from_arena(arena, memory, sizeof(int));

    EXPECT_EQ(arena->total_allocations, 1);
    EXPECT_EQ(arena->bytes_freed, sizeof(int));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize);

    /// now test the arena allocator
    tmem::arena_allocator<int> allocator{ arena };
    int* ptr = allocator.allocate(10);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 10);

    EXPECT_EQ(arena->total_allocations, 2);
    EXPECT_EQ(arena->bytes_freed, sizeof(int));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize + sizeof(int));

    allocator.free(ptr);

    EXPECT_EQ(arena->total_allocations, 1);
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize);
  }

  /// complex objects
  {
    struct ComplexObject {
      int a = 42;
      double b = 3.14;
      char c[10];
    };

    tmem::arena_allocator<ComplexObject> allocator{ arena };
    ComplexObject* obj = allocator.allocate();
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(obj->a, 42);
    EXPECT_EQ(obj->b, 3.14);
    std::strcpy(obj->c, "Hello");

    EXPECT_EQ(arena->total_allocations, 2);  /// first page + 1 allocation
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize + sizeof(ComplexObject));

    allocator.free(obj);

    EXPECT_EQ(arena->total_allocations, 1);
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int) + sizeof(ComplexObject));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize);

    struct ReallyComplexObject {
      int a = 42;
      double b = 3.14;
      char c[10] = { 0 };
      std::array<int, 100> d;

      ComplexObject object;
      std::vector<ComplexObject> other_objects;

      ReallyComplexObject() {
        std::strcpy(c, "Hello");
        for (size_t i = 0; i < 100; ++i) {
          d[i] = i;
        }

        for (size_t i = 0; i < 10; ++i) {
          auto& obj = other_objects.emplace_back();
          obj.a = i;
          obj.b = i * 1.1;
        }
      }

      ReallyComplexObject(int a, double b, const char* c, const std::array<int, 100>& d, const ComplexObject& object, const std::vector<ComplexObject>& other_objects)
          : a(a), b(b), d(d), object(object), other_objects(other_objects) {
        std::strcpy(this->c, c);
      }
    };

    tmem::arena_allocator<ReallyComplexObject> allocator2{ arena };
    ReallyComplexObject* obj2 = allocator2.allocate();
    EXPECT_NE(obj2, nullptr);
    obj2->a = 42;
    obj2->b = 3.14;
    ASSERT_STREQ(obj2->c, "Hello");
    for (size_t i = 0; i < 100; ++i) {
      EXPECT_EQ(obj2->d[i], i);
    }
    EXPECT_EQ(obj2->other_objects.size(), 10);
    for (size_t i = 0; i < 10; ++i) {
      EXPECT_EQ(obj2->other_objects[i].a, i);
      EXPECT_EQ(obj2->other_objects[i].b, i * 1.1);
    }
    EXPECT_EQ(arena->total_allocations, 2);  /// first page + 1 allocation + 1 allocation for ReallyComplexObject
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int) + sizeof(ComplexObject));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject) + sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize + sizeof(ReallyComplexObject));

    allocator2.free(obj2);

    EXPECT_EQ(arena->total_allocations, 1);
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int) + sizeof(ComplexObject) + sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject) + sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize);

    int a = 42;
    double b = 3.14;
    std::array<int, 100> d = { 0 };
    for (size_t i = 0; i < 100; ++i) {
      d[i] = i;
    }
    ComplexObject object = { 42, 3.14, "Hello" };
    std::vector<ComplexObject> other_objects = { { 1, 2.0, "World" }, { 3, 4.0, "!" } };

    ReallyComplexObject* obj3 = allocator2.allocate(a, b, "Hello", d, object, other_objects);
    EXPECT_NE(obj3, nullptr);
    EXPECT_EQ(obj3->a, 42);
    EXPECT_EQ(obj3->b, 3.14);
    ASSERT_STREQ(obj3->c, "Hello");
    for (size_t i = 0; i < 100; ++i) {
      EXPECT_EQ(obj3->d[i], i);
    }
    EXPECT_EQ(obj3->other_objects.size(), 2);
    EXPECT_EQ(obj3->other_objects[0].a, 1);
    EXPECT_EQ(obj3->other_objects[0].b, 2.0);
    ASSERT_STREQ(obj3->other_objects[0].c, "World");
    EXPECT_EQ(obj3->other_objects[1].a, 3);
    EXPECT_EQ(obj3->other_objects[1].b, 4.0);
    ASSERT_STREQ(obj3->other_objects[1].c, "!");

    EXPECT_EQ(arena->total_allocations, 2);  /// first page + 1 allocation + 2 allocations for ReallyComplexObject
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int) + sizeof(ComplexObject) + sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject) + 2 * sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize + sizeof(ReallyComplexObject));

    allocator2.free(obj3);

    EXPECT_EQ(arena->total_allocations, 1);
    EXPECT_EQ(arena->bytes_freed, sizeof(int) + sizeof(int) + sizeof(ComplexObject) + 2 * sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->bytes_allocated, tmem::arena::kPageSize + sizeof(int) + sizeof(int) + sizeof(ComplexObject) + 2 * sizeof(ReallyComplexObject));
    EXPECT_EQ(arena->living_memory(), tmem::arena::kPageSize);
  }
}