#include "gtest/gtest.h"
#include "CommandQueue.h"

TEST(CommandQueue, append_remove_cound_clear) {
  ASSERT_EQ(0, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  ASSERT_TRUE(removeCommandFromQueue(1));
  ASSERT_EQ(0, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  clearCommandQueue();
  ASSERT_EQ(0, countCommandQueue());
}

TEST(CommandQueue, multiple) {
  ASSERT_EQ(0, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(2));
  ASSERT_EQ(2, countCommandQueue());
  ASSERT_TRUE(removeCommandFromQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  ASSERT_TRUE(removeCommandFromQueue(2));
  ASSERT_EQ(0, countCommandQueue());
  clearCommandQueue();
}

TEST(CommandQueue, limit) {
  ASSERT_EQ(0, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(2));
  ASSERT_EQ(2, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(3));
  ASSERT_EQ(3, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(4));
  ASSERT_EQ(4, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(5));
  ASSERT_EQ(5, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(6));
  ASSERT_EQ(6, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(7));
  ASSERT_EQ(7, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(8));
  ASSERT_EQ(8, countCommandQueue());
  ASSERT_FALSE(appendCommandToQueue(9));
  clearCommandQueue();
}

uint32_t gEachCommandQueueTestValue = 1;
void CallbackEachTestFunc(void* object, uint32_t sequenceId) {
  ASSERT_EQ(gEachCommandQueueTestValue, sequenceId);
  ASSERT_EQ(NULL, object);
  gEachCommandQueueTestValue++;
}

TEST(CommandQueue, each) {
  ASSERT_EQ(0, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(1));
  ASSERT_EQ(1, countCommandQueue());
  ASSERT_TRUE(appendCommandToQueue(2));

  eachCommandQueue(CallbackEachTestFunc, NULL);
  clearCommandQueue();
}
