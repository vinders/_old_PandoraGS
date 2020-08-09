#include <gtest/gtest.h>
#include <memory/circular_queue.h>

using namespace pandora::memory;

class CircularQueueTest : public testing::Test {
public:
protected:
  //static void SetUpTestCase() {}
  //static void TearDownTestCase() {}
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(CircularQueueTest, dummy) {}
/*
// -- fake item objects --

#define __FAKE_ITEM_CLASS(className, copiable, movable) \
        class className { \
        public: \
          className () = default; \
          className (int val) : _val(val) {} \
          className (const className &) = copiable ; \
          className (className &&) = movable ; \
          className & operator=(const className &) = copiable ; \
          className & operator=(className &&) = movable ; \
          int value() const { return _val; } \
        private: \
          int _val = 0; \
        }

__FAKE_ITEM_CLASS(CopyObject, default, delete);
__FAKE_ITEM_CLASS(MoveObject, delete, default);
__FAKE_ITEM_CLASS(CopyMoveObject, default, default);
__FAKE_ITEM_CLASS(UniqueObject, delete, delete);
#undef __FAKE_ITEM_CLASS

// -- constructors/accessors --

TEST_F(CircularQueueTest, templateArgs) {
  CircularQueue<int, 20> baseType;
  CircularQueue<CopyObject, 20> copyType;
  CircularQueue<MoveObject, 20> moveType;
  CircularQueue<CopyMoveObject, 20> copyMoveType;
  CircularQueue<UniqueObject, 20> noCopyNoMoveType;

  EXPECT_EQ(size_t{0u}, baseType.size());
  EXPECT_EQ(size_t{0u}, copyType.size());
  EXPECT_EQ(size_t{0u}, moveType.size());
  EXPECT_EQ(size_t{0u}, copyMoveType.size());
  EXPECT_EQ(size_t{0u}, noCopyNoMoveType.size());
  EXPECT_EQ(size_t{20u}, baseType.capacity());
  EXPECT_EQ(size_t{20u}, copyType.capacity());
  EXPECT_EQ(size_t{20u}, moveType.capacity());
  EXPECT_EQ(size_t{20u}, copyMoveType.capacity());
  EXPECT_EQ(size_t{20u}, noCopyNoMoveType.capacity());
  EXPECT_TRUE(baseType.empty());
  EXPECT_TRUE(copyType.empty());
  EXPECT_TRUE(moveType.empty());
  EXPECT_TRUE(copyMoveType.empty());
  EXPECT_TRUE(noCopyNoMoveType.empty());
  EXPECT_FALSE(baseType.full());
  EXPECT_FALSE(copyType.full());
  EXPECT_FALSE(moveType.full());
  EXPECT_FALSE(copyMoveType.full());
  EXPECT_FALSE(noCopyNoMoveType.full());
}

TEST_F(CircularQueueTest, copyEmptyQueue) {
  CircularQueue<int, 20> baseType;
  CircularQueue<CopyObject, 20> copyType;
  CircularQueue<CopyMoveObject, 20> copyMoveType;

  CircularQueue<int, 20> baseTypeCopy(baseType);
  EXPECT_EQ(baseType.size(), baseTypeCopy.size());
  EXPECT_TRUE(baseTypeCopy.empty());
  CircularQueue<CopyObject, 20> copyTypeCopy(copyType);
  EXPECT_EQ(copyType.size(), copyTypeCopy.size());
  EXPECT_TRUE(copyTypeCopy.empty());
  CircularQueue<CopyMoveObject, 20> copyMoveTypeCopy(copyMoveType);
  EXPECT_EQ(copyMoveType.size(), copyMoveTypeCopy.size());
  EXPECT_TRUE(copyMoveTypeCopy.empty());

  baseType = baseTypeCopy;
  EXPECT_EQ(baseTypeCopy.size(), baseType.size());
  EXPECT_TRUE(baseType.empty());
  copyType = copyTypeCopy;
  EXPECT_EQ(copyTypeCopy.size(), copyType.size());
  EXPECT_TRUE(copyType.empty());
  copyMoveType = copyMoveTypeCopy;
  EXPECT_EQ(copyMoveTypeCopy.size(), copyMoveType.size());
  EXPECT_TRUE(copyMoveType.empty());
}

TEST_F(CircularQueueTest, copyQueue) {
  CircularQueue<int, 20> baseType;
  baseType.push_back(5);
  baseType.push_back(7);
  CircularQueue<CopyObject, 20> copyType;
  copyType.push_back(CopyObject(5));
  copyType.push_back(CopyObject(7));
  CircularQueue<CopyMoveObject, 20> copyMoveType;
  copyMoveType.push_back(CopyMoveObject(5));
  copyMoveType.push_back(CopyMoveObject(7));

  CircularQueue<int, 20> baseTypeCopy(baseType);
  EXPECT_EQ(baseType.size(), baseTypeCopy.size());
  EXPECT_EQ(baseType.front(), baseTypeCopy.front());
  EXPECT_EQ(baseType.back(), baseTypeCopy.back());
  CircularQueue<CopyObject, 20> copyTypeCopy(copyType);
  EXPECT_EQ(copyType.size(), copyTypeCopy.size());
  EXPECT_EQ(copyType.front().value(), copyTypeCopy.front().value());
  EXPECT_EQ(copyType.back().value(), copyTypeCopy.back().value());
  CircularQueue<CopyMoveObject, 20> copyMoveTypeCopy(copyMoveType);
  EXPECT_EQ(copyMoveType.size(), copyMoveTypeCopy.size());
  EXPECT_EQ(copyMoveType.front().value(), copyMoveTypeCopy.front().value());
  EXPECT_EQ(copyMoveType.back().value(), copyMoveTypeCopy.back().value());

  baseType.clear();
  baseType = baseTypeCopy;
  EXPECT_EQ(baseTypeCopy.size(), baseType.size());
  EXPECT_EQ(baseTypeCopy.front(), baseType.front());
  EXPECT_EQ(baseTypeCopy.back(), baseType.back());
  copyType.clear();
  copyType = copyTypeCopy;
  EXPECT_EQ(copyTypeCopy.size(), copyType.size());
  EXPECT_EQ(copyTypeCopy.front().value(), copyType.front().value());
  EXPECT_EQ(copyTypeCopy.back().value(), copyType.back().value());
  copyMoveType.clear();
  copyMoveType = copyMoveTypeCopy;
  EXPECT_EQ(copyMoveTypeCopy.size(), copyMoveType.size());
  EXPECT_EQ(copyMoveTypeCopy.front().value(), copyMoveType.front().value());
  EXPECT_EQ(copyMoveTypeCopy.back().value(), copyMoveType.back().value());
}

TEST_F(CircularQueueTest, moveEmptyQueue) {
  CircularQueue<MoveObject, 20> moveType;
  CircularQueue<CopyMoveObject, 20> copyMoveType;

  CircularQueue<MoveObject, 20> moveTypeCopy(std::move(moveType));
  EXPECT_TRUE(moveType.empty());
  EXPECT_TRUE(moveTypeCopy.empty());
  CircularQueue<CopyMoveObject, 20> copyMoveTypeCopy(std::move(copyMoveType));
  EXPECT_TRUE(copyMoveType.empty());
  EXPECT_TRUE(copyMoveTypeCopy.empty());

  moveType.clear();
  moveType = std::move(moveTypeCopy);
  EXPECT_TRUE(moveType.empty());
  EXPECT_TRUE(moveTypeCopy.empty());
  copyMoveType.clear();
  copyMoveType = std::move(copyMoveTypeCopy);
  EXPECT_TRUE(copyMoveType.empty());
  EXPECT_TRUE(copyMoveTypeCopy.empty());
}

TEST_F(CircularQueueTest, moveQueue) {
  CircularQueue<MoveObject, 20> moveType;
  moveType.push_back(MoveObject(5));
  moveType.push_back(MoveObject(7));
  CircularQueue<CopyMoveObject, 20> copyMoveType;
  copyMoveType.push_back(CopyMoveObject(5));
  copyMoveType.push_back(CopyMoveObject(7));

  CircularQueue<MoveObject, 20> moveTypeMoved(std::move(moveType));
  EXPECT_TRUE(moveType.empty());
  EXPECT_EQ(2, moveTypeMoved.size());
  EXPECT_EQ(5, moveTypeMoved.front().value());
  EXPECT_EQ(7, moveTypeMoved.back().value());
  CircularQueue<CopyMoveObject, 20> copyMoveTypeMoved(std::move(copyMoveType));
  EXPECT_TRUE(copyMoveTypeMoved.empty());
  EXPECT_EQ(2, copyMoveTypeMoved.size());
  EXPECT_EQ(5, copyMoveTypeMoved.front().value());
  EXPECT_EQ(7, copyMoveTypeMoved.back().value());

  moveType = std::move(moveTypeMoved);
  EXPECT_TRUE(moveTypeMoved.empty());
  EXPECT_EQ(2, moveType.size());
  EXPECT_EQ(5, moveType.front().value());
  EXPECT_EQ(7, moveType.back().value());
  copyMoveType = std::move(copyMoveTypeMoved);
  EXPECT_TRUE(copyMoveTypeMoved.empty());
  EXPECT_EQ(2, copyMoveType.size());
  EXPECT_EQ(5, copyMoveType.front().value());
  EXPECT_EQ(7, copyMoveType.back().value());
}

TEST_F(CircularQueueTest, swapQueues) {
  CircularQueue<int, 20> baseType;
  CircularQueue<int, 20> baseType2;
  baseType.push_back(5);
  baseType.push_back(7);
  baseType2.push_back(9);
  CircularQueue<CopyObject, 20> copyType;
  CircularQueue<CopyObject, 20> copyType2;
  copyType.push_back(CopyObject(5));
  copyType.push_back(CopyObject(7));
  CircularQueue<MoveObject, 20> moveType;
  CircularQueue<MoveObject, 20> moveType2;
  moveType.push_back(MoveObject(5));
  moveType.push_back(MoveObject(7));
  moveType2.push_back(MoveObject(9));
  CircularQueue<CopyMoveObject, 20> copyMoveType;
  CircularQueue<CopyMoveObject, 20> copyMoveType2;
  copyMoveType.push_back(CopyMoveObject(5));
  copyMoveType.push_back(CopyMoveObject(7));

  std::swap(baseType, baseType2);
  EXPECT_EQ(1, baseType.size());
  EXPECT_EQ(9, baseType.front());
  EXPECT_EQ(2, baseType2.size());
  EXPECT_EQ(5, baseType2.front());
  EXPECT_EQ(7, baseType2.back());
  baseType2.swap(baseType);
  EXPECT_EQ(2, baseType.size());
  EXPECT_EQ(5, baseType.front());
  EXPECT_EQ(7, baseType.back());
  EXPECT_EQ(1, baseType2.size());
  EXPECT_EQ(9, baseType2.front());

  std::swap(copyType, copyType2);
  EXPECT_EQ(0, copyType.size());
  EXPECT_EQ(2, copyType2.size());
  EXPECT_EQ(5, copyType2.front().value());
  EXPECT_EQ(7, copyType2.back().value());
  copyType2.swap(copyType);
  EXPECT_EQ(2, copyType.size());
  EXPECT_EQ(5, copyType.front().value());
  EXPECT_EQ(7, copyType.back().value());
  EXPECT_EQ(0, copyType2.size());

  std::swap(moveType, moveType2);
  EXPECT_EQ(1, moveType.size());
  EXPECT_EQ(9, moveType.front().value());
  EXPECT_EQ(2, moveType2.size());
  EXPECT_EQ(5, moveType2.front().value());
  EXPECT_EQ(7, moveType2.back().value());
  moveType2.swap(moveType);
  EXPECT_EQ(2, moveType.size());
  EXPECT_EQ(5, moveType.front().value());
  EXPECT_EQ(7, moveType.back().value());
  EXPECT_EQ(1, moveType2.size());
  EXPECT_EQ(9, moveType2.front().value());

  std::swap(copyMoveType, copyMoveType2);
  EXPECT_EQ(0, copyMoveType.size());
  EXPECT_EQ(2, copyMoveType2.size());
  EXPECT_EQ(5, copyMoveType2.front().value());
  EXPECT_EQ(7, copyMoveType2.back().value());
  copyMoveType2.swap(copyMoveType);
  EXPECT_EQ(2, copyMoveType.size());
  EXPECT_EQ(5, copyMoveType.front().value());
  EXPECT_EQ(7, copyMoveType.back().value());
  EXPECT_EQ(0, copyMoveType2.size());
}

// -- operations --

TEST_F(CircularQueueTest, pushPopOne) {
  CircularQueue<int, 1> baseType1;
  CircularQueue<int, 2> baseType2;
  CircularQueue<int, 20> baseType20;
  CircularQueue<CopyObject, 1> copyType1;
  CircularQueue<CopyObject, 20> copyType20;
  CircularQueue<MoveObject, 20> moveType20;
  CircularQueue<CopyMoveObject, 20> copyMoveType20;

  EXPECT_TRUE(baseType1.push_back(5));
  EXPECT_EQ(size_t{1u}, baseType1.size());
  EXPECT_EQ(size_t{1u}, baseType1.capacity());
  EXPECT_EQ(5, baseType1.front());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->front());
  EXPECT_EQ(5, baseType1.back());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->back());
  EXPECT_FALSE(baseType1.empty());
  EXPECT_TRUE(baseType1.full());
  EXPECT_TRUE(baseType2.push(5));
  EXPECT_EQ(size_t{1u}, baseType2.size());
  EXPECT_EQ(size_t{2u}, baseType2.capacity());
  EXPECT_EQ(5, baseType2.front());
  EXPECT_EQ(5, ((const CircularQueue<int,2>*)&baseType2)->front());
  EXPECT_EQ(5, baseType2.back());
  EXPECT_EQ(5, ((const CircularQueue<int,2>*)&baseType2)->back());
  EXPECT_FALSE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  EXPECT_TRUE(baseType20.push_back(5));
  EXPECT_EQ(size_t{1u}, baseType20.size());
  EXPECT_EQ(size_t{20u}, baseType20.capacity());
  EXPECT_EQ(5, baseType20.front());
  EXPECT_EQ(5, baseType20.back());
  EXPECT_FALSE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType1.push(CopyObject(5)));
  EXPECT_EQ(size_t{1u}, copyType1.size());
  EXPECT_EQ(size_t{1u}, copyType1.capacity());
  EXPECT_EQ(5, copyType1.front().value());
  EXPECT_EQ(5, copyType1.back().value());
  EXPECT_FALSE(copyType1.empty());
  EXPECT_TRUE(copyType1.full());
  EXPECT_TRUE(copyType20.push_back(CopyObject(5)));
  EXPECT_EQ(size_t{1u}, copyType20.size());
  EXPECT_EQ(size_t{20u}, copyType20.capacity());
  EXPECT_EQ(5, copyType20.front().value());
  EXPECT_EQ(5, copyType20.back().value());
  EXPECT_FALSE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.push(MoveObject(5)));
  EXPECT_EQ(size_t{1u}, moveType20.size());
  EXPECT_EQ(size_t{20u}, moveType20.capacity());
  EXPECT_EQ(5, moveType20.front().value());
  EXPECT_EQ(5, moveType20.back().value());
  EXPECT_FALSE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.push_back(CopyMoveObject(5)));
  EXPECT_EQ(size_t{1u}, copyMoveType20.size());
  EXPECT_EQ(size_t{20u}, copyMoveType20.capacity());
  EXPECT_EQ(5, copyMoveType20.front().value());
  EXPECT_EQ(5, copyMoveType20.back().value());
  EXPECT_FALSE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());

  EXPECT_TRUE(baseType1.pop_front());
  EXPECT_EQ(size_t{0u}, baseType1.size());
  EXPECT_TRUE(baseType1.empty());
  EXPECT_FALSE(baseType1.full());
  EXPECT_TRUE(baseType2.pop());
  EXPECT_EQ(size_t{0u}, baseType2.size());
  EXPECT_TRUE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  EXPECT_TRUE(baseType20.pop_front());
  EXPECT_EQ(size_t{0u}, baseType20.size());
  EXPECT_TRUE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType1.pop());
  EXPECT_EQ(size_t{0u}, copyType1.size());
  EXPECT_TRUE(copyType1.empty());
  EXPECT_FALSE(copyType1.full());
  EXPECT_TRUE(copyType20.pop_front());
  EXPECT_EQ(size_t{0u}, copyType20.size());
  EXPECT_TRUE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.pop());
  EXPECT_EQ(size_t{0u}, moveType20.size());
  EXPECT_TRUE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.pop_front());
  EXPECT_EQ(size_t{0u}, copyMoveType20.size());
  EXPECT_TRUE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());
}

TEST_F(CircularQueueTest, pushPopClear) {
  CircularQueue<int, 1> baseType1;
  CircularQueue<int, 2> baseType2;
  CircularQueue<int, 20> baseType20;
  CircularQueue<CopyObject, 20> copyType20;
  CircularQueue<MoveObject, 20> moveType20;
  CircularQueue<CopyMoveObject, 20> copyMoveType20;

  EXPECT_TRUE(baseType1.push_back(5));
  EXPECT_FALSE(baseType1.push_back(7));
  EXPECT_EQ(size_t{1u}, baseType1.size());
  EXPECT_EQ(size_t{1u}, baseType1.capacity());
  EXPECT_EQ(5, baseType1.front());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->front());
  EXPECT_EQ(5, baseType1.back());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->back());
  EXPECT_FALSE(baseType1.empty());
  EXPECT_TRUE(baseType1.full());
  EXPECT_TRUE(baseType2.push_back(5));
  EXPECT_TRUE(baseType2.push(7));
  EXPECT_EQ(size_t{2u}, baseType2.size());
  EXPECT_EQ(size_t{2u}, baseType2.capacity());
  EXPECT_EQ(5, baseType2.front());
  EXPECT_EQ(5, ((const CircularQueue<int,2>*)&baseType2)->front());
  EXPECT_EQ(7, baseType2.back());
  EXPECT_EQ(7, ((const CircularQueue<int,2>*)&baseType2)->back());
  EXPECT_FALSE(baseType2.empty());
  EXPECT_TRUE(baseType2.full());
  EXPECT_TRUE(baseType20.push_back(5));
  EXPECT_TRUE(baseType20.push(7));
  EXPECT_EQ(size_t{2u}, baseType20.size());
  EXPECT_EQ(size_t{20u}, baseType20.capacity());
  EXPECT_EQ(5, baseType20.front());
  EXPECT_EQ(7, baseType20.back());
  EXPECT_FALSE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType20.push_back(CopyObject(5)));
  EXPECT_TRUE(copyType20.push(CopyObject(7)));
  EXPECT_EQ(size_t{2u}, copyType20.size());
  EXPECT_EQ(size_t{20u}, copyType20.capacity());
  EXPECT_EQ(5, copyType20.front().value());
  EXPECT_EQ(7, copyType20.back().value());
  EXPECT_FALSE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.push_back(MoveObject(5)));
  EXPECT_TRUE(moveType20.push(MoveObject(7)));
  EXPECT_EQ(size_t{2u}, moveType20.size());
  EXPECT_EQ(size_t{20u}, moveType20.capacity());
  EXPECT_EQ(5, moveType20.front().value());
  EXPECT_EQ(7, moveType20.back().value());
  EXPECT_FALSE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.push_back(CopyMoveObject(5)));
  EXPECT_TRUE(copyMoveType20.push(CopyMoveObject(7)));
  EXPECT_EQ(size_t{2u}, copyMoveType20.size());
  EXPECT_EQ(size_t{20u}, copyMoveType20.capacity());
  EXPECT_EQ(5, copyMoveType20.front().value());
  EXPECT_EQ(7, copyMoveType20.back().value());
  EXPECT_FALSE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());

  EXPECT_TRUE(baseType2.pop());
  EXPECT_EQ(size_t{1u}, baseType2.size());
  EXPECT_EQ(size_t{2u}, baseType2.capacity());
  EXPECT_EQ(7, baseType2.front().value());
  EXPECT_EQ(7, baseType2.back().value());
  EXPECT_FALSE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  EXPECT_TRUE(baseType20.pop_front());
  EXPECT_EQ(size_t{1u}, baseType20.size());
  EXPECT_EQ(size_t{20u}, baseType20.capacity());
  EXPECT_EQ(7, baseType20.front().value());
  EXPECT_EQ(7, baseType20.back().value());
  EXPECT_FALSE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType20.pop_front());
  EXPECT_EQ(size_t{1u}, copyType20.size());
  EXPECT_EQ(size_t{20u}, copyType20.capacity());
  EXPECT_EQ(7, copyType20.front().value());
  EXPECT_EQ(7, copyType20.back().value());
  EXPECT_FALSE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.pop());
  EXPECT_EQ(size_t{1u}, moveType20.size());
  EXPECT_EQ(size_t{20u}, moveType20.capacity());
  EXPECT_EQ(7, moveType20.front().value());
  EXPECT_EQ(7, moveType20.back().value());
  EXPECT_FALSE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.pop_front());
  EXPECT_EQ(size_t{1u}, copyMoveType20.size());
  EXPECT_EQ(size_t{20u}, copyMoveType20.capacity());
  EXPECT_EQ(7, copyMoveType20.front().value());
  EXPECT_EQ(7, copyMoveType20.back().value());
  EXPECT_FALSE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());

  baseType1.clear();
  EXPECT_EQ(size_t{0u}, baseType1.size());
  EXPECT_TRUE(baseType1.empty());
  EXPECT_FALSE(baseType1.full());
  baseType2.clear();
  EXPECT_EQ(size_t{0u}, baseType2.size());
  EXPECT_TRUE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  baseType20.clear();
  EXPECT_EQ(size_t{0u}, baseType20.size());
  EXPECT_TRUE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  copyType20.clear();
  EXPECT_EQ(size_t{0u}, copyType20.size());
  EXPECT_TRUE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  moveType20.clear();
  EXPECT_EQ(size_t{0u}, moveType20.size());
  EXPECT_TRUE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  copyMoveType20.clear();
  EXPECT_EQ(size_t{0u}, copyMoveType20.size());
  EXPECT_TRUE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());

  baseType1.clear(); // re-clear -> no effect
  EXPECT_EQ(size_t{0u}, baseType1.size());
  EXPECT_TRUE(baseType1.empty());
  EXPECT_FALSE(baseType1.full());
  baseType2.clear();
  EXPECT_EQ(size_t{0u}, baseType2.size());
  EXPECT_TRUE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
}

TEST_F(CircularQueueTest, emplacePopOne) {
  CircularQueue<int, 1> baseType1;
  CircularQueue<int, 2> baseType2;
  CircularQueue<int, 20> baseType20;
  CircularQueue<CopyObject, 20> copyType20;
  CircularQueue<MoveObject, 20> moveType20;
  CircularQueue<CopyMoveObject, 20> copyMoveType20;
  CircularQueue<UniqueObject, 20> noCopyNoMoveType20;

  EXPECT_TRUE(baseType1.emplace(5));
  EXPECT_EQ(size_t{1u}, baseType1.size());
  EXPECT_EQ(size_t{1u}, baseType1.capacity());
  EXPECT_EQ(5, baseType1.front());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->front());
  EXPECT_EQ(5, baseType1.back());
  EXPECT_EQ(5, ((const CircularQueue<int,1>*)&baseType1)->back());
  EXPECT_FALSE(baseType1.empty());
  EXPECT_TRUE(baseType1.full());
  EXPECT_TRUE(baseType2.emplace_back(5));
  EXPECT_EQ(size_t{1u}, baseType2.size());
  EXPECT_EQ(size_t{2u}, baseType2.capacity());
  EXPECT_EQ(5, baseType2.front());
  EXPECT_EQ(5, ((const CircularQueue<int,2>*)&baseType2)->front());
  EXPECT_EQ(5, baseType2.back());
  EXPECT_EQ(5, ((const CircularQueue<int,2>*)&baseType2)->back());
  EXPECT_FALSE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  EXPECT_TRUE(baseType20.emplace(5));
  EXPECT_EQ(size_t{1u}, baseType20.size());
  EXPECT_EQ(size_t{20u}, baseType20.capacity());
  EXPECT_EQ(5, baseType20.front());
  EXPECT_EQ(5, baseType20.back());
  EXPECT_FALSE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType20.emplace_back(5));
  EXPECT_EQ(size_t{1u}, copyType20.size());
  EXPECT_EQ(size_t{20u}, copyType20.capacity());
  EXPECT_EQ(5, copyType20.front().value());
  EXPECT_EQ(5, copyType20.back().value());
  EXPECT_FALSE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.emplace(5));
  EXPECT_EQ(size_t{1u}, moveType20.size());
  EXPECT_EQ(size_t{20u}, moveType20.capacity());
  EXPECT_EQ(5, moveType20.front().value());
  EXPECT_EQ(5, moveType20.back().value());
  EXPECT_FALSE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.emplace_back(5));
  EXPECT_EQ(size_t{1u}, copyMoveType20.size());
  EXPECT_EQ(size_t{20u}, copyMoveType20.capacity());
  EXPECT_EQ(5, copyMoveType20.front().value());
  EXPECT_EQ(5, copyMoveType20.back().value());
  EXPECT_FALSE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());
  EXPECT_TRUE(noCopyNoMoveType20.emplace(5));
  EXPECT_EQ(size_t{1u}, noCopyNoMoveType20.size());
  EXPECT_EQ(size_t{20u}, noCopyNoMoveType20.capacity());
  EXPECT_EQ(5, noCopyNoMoveType20.front().value());
  EXPECT_EQ(5, noCopyNoMoveType20.back().value());
  EXPECT_FALSE(noCopyNoMoveType20.empty());
  EXPECT_FALSE(noCopyNoMoveType20.full());

  EXPECT_TRUE(baseType1.pop_front());
  EXPECT_EQ(size_t{0u}, baseType1.size());
  EXPECT_TRUE(baseType1.empty());
  EXPECT_FALSE(baseType1.full());
  EXPECT_TRUE(baseType2.pop());
  EXPECT_EQ(size_t{0u}, baseType2.size());
  EXPECT_TRUE(baseType2.empty());
  EXPECT_FALSE(baseType2.full());
  EXPECT_TRUE(baseType20.pop_front());
  EXPECT_EQ(size_t{0u}, baseType20.size());
  EXPECT_TRUE(baseType20.empty());
  EXPECT_FALSE(baseType20.full());
  EXPECT_TRUE(copyType20.pop_front());
  EXPECT_EQ(size_t{0u}, copyType20.size());
  EXPECT_TRUE(copyType20.empty());
  EXPECT_FALSE(copyType20.full());
  EXPECT_TRUE(moveType20.pop());
  EXPECT_EQ(size_t{0u}, moveType20.size());
  EXPECT_TRUE(moveType20.empty());
  EXPECT_FALSE(moveType20.full());
  EXPECT_TRUE(copyMoveType20.pop_front());
  EXPECT_EQ(size_t{0u}, copyMoveType20.size());
  EXPECT_TRUE(copyMoveType20.empty());
  EXPECT_FALSE(copyMoveType20.full());
  EXPECT_TRUE(noCopyNoMoveType20.pop_front());
  EXPECT_EQ(size_t{0u}, noCopyNoMoveType20.size());
  EXPECT_TRUE(noCopyNoMoveType20.empty());
  EXPECT_FALSE(noCopyNoMoveType20.full());
}

TEST_F(CircularQueueTest, popEmpty) {
  CircularQueue<int, 1> baseType1;
  CircularQueue<int, 20> baseType20;
  CircularQueue<CopyObject, 20> copyType20;
  CircularQueue<MoveObject, 20> moveType20;
  CircularQueue<CopyMoveObject, 20> copyMoveType20;
  CircularQueue<UniqueObject, 20> noCopyNoMoveType20;
  EXPECT_FALSE(baseType1.pop_front());
  EXPECT_FALSE(baseType20.pop_front());
  EXPECT_FALSE(copyType20.pop_front());
  EXPECT_FALSE(moveType20.pop_front());
  EXPECT_FALSE(copyMoveType20.pop_front());
  EXPECT_FALSE(noCopyNoMoveType20.pop_front());
  EXPECT_TRUE(baseType1.empty());
  EXPECT_TRUE(baseType20.empty());
  EXPECT_TRUE(copyType20.empty());
  EXPECT_TRUE(moveType20.empty());
  EXPECT_TRUE(copyMoveType20.empty());
  EXPECT_TRUE(noCopyNoMoveType20.empty());
}

TEST_F(CircularQueueTest, pushPopClearMultiple) {
  CircularQueue<int, 20> baseType20;
  CircularQueue<CopyObject, 20> copyType20;
  CircularQueue<MoveObject, 20> moveType20;
  CircularQueue<CopyMoveObject, 20> copyMoveType20;
  CircularQueue<UniqueObject, 20> noCopyNoMoveType20;
}
//push+emplace multiple + front/back/size/empty/full + pop multiple + push + clear

TEST_F(CircularQueueTest, fillPopAll) {

}

TEST_F(CircularQueueTest, fillClear) {

}
//push(fill) + front/back/size/empty/full + push-fail + emplace-fail

TEST_F(CircularQueueTest, mixedOperations) {

}
//push X -> pop 1 -> push X -> pop all -> push(fill) -> pop all + pop-fail

// -- iteration --

TEST_F(CircularQueueTest, iterateEmpty) {

}
//empty -> it + c-it

TEST_F(CircularQueueTest, iterateOne) {

}
//push 1 -> it + c-it -> pop 1 -> it + c-it

TEST_F(CircularQueueTest, iterateMultiple) {

}
//push X -> it + c-it -> pop 1 -> it + c-it -> pop all -> it + c-it

TEST_F(CircularQueueTest, iterateFull) {

}
//push(fill) -> it + c-it -> clear -> it + c-it
*/
