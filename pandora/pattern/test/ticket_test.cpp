#include <gtest/gtest.h>
#include <memory>
#include <pattern/ticket.h>

using namespace pandora::pattern;

class TicketTest : public testing::Test {
public:
protected:
  //static void SetUpTestCase() {}
  //static void TearDownTestCase() {}

  void SetUp() override {}
  void TearDown() override {}
};


// -- ticket validation --

TEST_F(TicketTest, booleanTicket) {
  Ticket<bool,bool,false> emptyTicket;
  EXPECT_TRUE(emptyTicket.id() == 1uLL);
  EXPECT_TRUE(emptyTicket.data() == false);
  EXPECT_FALSE(emptyTicket.isValid());
  EXPECT_FALSE(emptyTicket);
  EXPECT_TRUE(emptyTicket() == 1uLL);
  EXPECT_TRUE((uint64_t)emptyTicket == 1uLL);
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<bool,bool,false> invalidTicket(false);
  EXPECT_TRUE(invalidTicket.id() == 2uLL);
  EXPECT_TRUE(invalidTicket.data() == false);
  EXPECT_FALSE(invalidTicket.isValid());
  EXPECT_FALSE(invalidTicket);
  EXPECT_TRUE(invalidTicket() == 2uLL);
  EXPECT_TRUE((uint64_t)invalidTicket == 2uLL);
  invalidTicket.invalidate();
  EXPECT_FALSE(invalidTicket.isValid());

  Ticket<bool,bool,false> ticket1(true);
  EXPECT_TRUE(ticket1.id() == 3uLL);
  EXPECT_TRUE(ticket1.data() == true);
  EXPECT_TRUE(ticket1.isValid());
  EXPECT_TRUE(ticket1);
  EXPECT_TRUE(ticket1() == 3uLL);
  EXPECT_TRUE((uint64_t)ticket1 == 3uLL);

  Ticket<bool,bool,false> ticket2(true);
  EXPECT_TRUE(ticket2.id() == 4uLL);
  EXPECT_TRUE(ticket2.data() == true);
  EXPECT_TRUE(ticket2.isValid());
  EXPECT_TRUE(ticket2);
  EXPECT_TRUE(ticket2() == 4uLL);
  EXPECT_TRUE((uint64_t)ticket2 == 4uLL);

  EXPECT_TRUE(ticket1 == ticket1);
  EXPECT_FALSE(ticket1 == ticket2);
  EXPECT_FALSE(ticket1 != ticket1);
  EXPECT_TRUE(ticket1 != ticket2);
  EXPECT_TRUE(ticket1 < ticket2);
  EXPECT_FALSE(ticket2 < ticket1);
  EXPECT_TRUE(ticket1 <= ticket2);
  EXPECT_FALSE(ticket2 <= ticket1);
  EXPECT_FALSE(ticket1 > ticket2);
  EXPECT_TRUE(ticket2 > ticket1);
  EXPECT_FALSE(ticket1 >= ticket2);
  EXPECT_TRUE(ticket2 >= ticket1);

  emptyTicket = std::move(ticket1);
  EXPECT_TRUE(emptyTicket.id() == 3uLL);
  EXPECT_TRUE(emptyTicket.isValid());
  EXPECT_FALSE(ticket1.isValid());
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<bool,bool,false> copyTicket2(ticket2);
  EXPECT_TRUE(copyTicket2.id() == 4uLL);
  EXPECT_TRUE(copyTicket2.isValid());
  Ticket<bool,bool,false> newTicket2(std::move(ticket2));
  EXPECT_TRUE(newTicket2.id() == 4uLL);
  EXPECT_TRUE(newTicket2.isValid());
  EXPECT_FALSE(ticket2.isValid());
  newTicket2.invalidate();
  EXPECT_FALSE(newTicket2.isValid());
}

TEST_F(TicketTest, intTicket) {
  Ticket<int,int,1> emptyTicket;
  EXPECT_TRUE(emptyTicket.id() == 1uLL);
  EXPECT_TRUE(emptyTicket.data() == 1);
  EXPECT_FALSE(emptyTicket.isValid());
  EXPECT_FALSE(emptyTicket);
  EXPECT_TRUE(emptyTicket() == 1uLL);
  EXPECT_TRUE((uint64_t)emptyTicket == 1uLL);
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<int,int,1> invalidTicket(1);
  EXPECT_TRUE(invalidTicket.id() == 2uLL);
  EXPECT_TRUE(invalidTicket.data() == 1);
  EXPECT_FALSE(invalidTicket.isValid());
  EXPECT_FALSE(invalidTicket);
  EXPECT_TRUE(invalidTicket() == 2uLL);
  EXPECT_TRUE((uint64_t)invalidTicket == 2uLL);
  invalidTicket.invalidate();
  EXPECT_FALSE(invalidTicket.isValid());

  Ticket<int,int,1> ticket1(5);
  EXPECT_TRUE(ticket1.id() == 3uLL);
  EXPECT_TRUE(ticket1.data() == 5);
  EXPECT_TRUE(ticket1.isValid());
  EXPECT_TRUE(ticket1);
  EXPECT_TRUE(ticket1() == 3uLL);
  EXPECT_TRUE((uint64_t)ticket1 == 3uLL);

  Ticket<int,int,1> ticket2(6);
  EXPECT_TRUE(ticket2.id() == 4uLL);
  EXPECT_TRUE(ticket2.data() == 6);
  EXPECT_TRUE(ticket2.isValid());
  EXPECT_TRUE(ticket2);
  EXPECT_TRUE(ticket2() == 4uLL);
  EXPECT_TRUE((uint64_t)ticket2 == 4uLL);

  EXPECT_TRUE(ticket1 == ticket1);
  EXPECT_FALSE(ticket1 == ticket2);
  EXPECT_FALSE(ticket1 != ticket1);
  EXPECT_TRUE(ticket1 != ticket2);
  EXPECT_TRUE(ticket1 < ticket2);
  EXPECT_FALSE(ticket2 < ticket1);
  EXPECT_TRUE(ticket1 <= ticket2);
  EXPECT_FALSE(ticket2 <= ticket1);
  EXPECT_FALSE(ticket1 > ticket2);
  EXPECT_TRUE(ticket2 > ticket1);
  EXPECT_FALSE(ticket1 >= ticket2);
  EXPECT_TRUE(ticket2 >= ticket1);

  emptyTicket = std::move(ticket1);
  EXPECT_TRUE(emptyTicket.id() == 3uLL);
  EXPECT_TRUE(emptyTicket.data() == 5);
  EXPECT_TRUE(emptyTicket.isValid());
  EXPECT_FALSE(ticket1.isValid());
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<int,int,1> copyTicket2(ticket2);
  EXPECT_TRUE(copyTicket2.id() == 4uLL);
  EXPECT_TRUE(copyTicket2.data() == 6);
  EXPECT_TRUE(copyTicket2.isValid());
  Ticket<int,int,1> newTicket2(std::move(ticket2));
  EXPECT_TRUE(newTicket2.id() == 4uLL);
  EXPECT_TRUE(newTicket2.data() == 6);
  EXPECT_TRUE(newTicket2.isValid());
  EXPECT_FALSE(ticket2.isValid());
  newTicket2.invalidate();
  EXPECT_FALSE(newTicket2.isValid());
}

TEST_F(TicketTest, pointerTicket) {
  Ticket<int*,int*,nullptr> emptyTicket;
  EXPECT_TRUE(emptyTicket.id() == 1uLL);
  EXPECT_TRUE(emptyTicket.data() == nullptr);
  EXPECT_FALSE(emptyTicket.isValid());
  EXPECT_FALSE(emptyTicket);
  EXPECT_TRUE(emptyTicket() == 1uLL);
  EXPECT_TRUE((uint64_t)emptyTicket == 1uLL);
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<int*,int*,nullptr> invalidTicket(nullptr);
  EXPECT_TRUE(invalidTicket.id() == 2uLL);
  EXPECT_TRUE(invalidTicket.data() == nullptr);
  EXPECT_FALSE(invalidTicket.isValid());
  EXPECT_FALSE(invalidTicket);
  EXPECT_TRUE(invalidTicket() == 2uLL);
  EXPECT_TRUE((uint64_t)invalidTicket == 2uLL);
  invalidTicket.invalidate();
  EXPECT_FALSE(invalidTicket.isValid());

  int dummy = 5;
  Ticket<int*,int*,nullptr> ticket1(&dummy);
  EXPECT_TRUE(ticket1.id() == 3uLL);
  ASSERT_TRUE(ticket1.data() != nullptr);
  EXPECT_TRUE(*(ticket1.data()) == 5);
  EXPECT_TRUE(ticket1.isValid());
  EXPECT_TRUE(ticket1);
  EXPECT_TRUE(ticket1() == 3uLL);
  EXPECT_TRUE((uint64_t)ticket1 == 3uLL);

  int dummy2 = 6;
  Ticket<int*,int*,nullptr> ticket2(&dummy2);
  EXPECT_TRUE(ticket2.id() == 4uLL);
  ASSERT_TRUE(ticket2.data() != nullptr);
  EXPECT_TRUE(*(ticket2.data()) == 6);
  EXPECT_TRUE(ticket2.isValid());
  EXPECT_TRUE(ticket2);
  EXPECT_TRUE(ticket2() == 4uLL);
  EXPECT_TRUE((uint64_t)ticket2 == 4uLL);

  EXPECT_TRUE(ticket1 == ticket1);
  EXPECT_FALSE(ticket1 == ticket2);
  EXPECT_FALSE(ticket1 != ticket1);
  EXPECT_TRUE(ticket1 != ticket2);
  EXPECT_TRUE(ticket1 < ticket2);
  EXPECT_FALSE(ticket2 < ticket1);
  EXPECT_TRUE(ticket1 <= ticket2);
  EXPECT_FALSE(ticket2 <= ticket1);
  EXPECT_FALSE(ticket1 > ticket2);
  EXPECT_TRUE(ticket2 > ticket1);
  EXPECT_FALSE(ticket1 >= ticket2);
  EXPECT_TRUE(ticket2 >= ticket1);

  emptyTicket = std::move(ticket1);
  EXPECT_TRUE(emptyTicket.id() == 3uLL);
  ASSERT_TRUE(emptyTicket.data() != nullptr);
  EXPECT_TRUE(*(emptyTicket.data()) == 5);
  EXPECT_TRUE(emptyTicket.isValid());
  EXPECT_FALSE(ticket1.isValid());
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<int*,int*,nullptr> copyTicket2(ticket2);
  EXPECT_TRUE(copyTicket2.id() == 4uLL);
  ASSERT_TRUE(copyTicket2.data() != nullptr);
  EXPECT_TRUE(*(copyTicket2.data()) == 6);
  EXPECT_TRUE(copyTicket2.isValid());
  Ticket<int*,int*,nullptr> newTicket2(std::move(ticket2));
  EXPECT_TRUE(newTicket2.id() == 4uLL);
  ASSERT_TRUE(newTicket2.data() != nullptr);
  EXPECT_TRUE(*(newTicket2.data()) == 6);
  EXPECT_TRUE(newTicket2.isValid());
  EXPECT_FALSE(ticket2.isValid());
  newTicket2.invalidate();
  EXPECT_FALSE(newTicket2.isValid());
}

TEST_F(TicketTest, smartPointerTicket) {
  Ticket<std::shared_ptr<int>,int*,nullptr> emptyTicket;
  EXPECT_TRUE(emptyTicket.id() == 1uLL);
  EXPECT_TRUE(emptyTicket.data() == nullptr);
  EXPECT_FALSE(emptyTicket.isValid());
  EXPECT_FALSE(emptyTicket);
  EXPECT_TRUE(emptyTicket() == 1uLL);
  EXPECT_TRUE((uint64_t)emptyTicket == 1uLL);
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<std::shared_ptr<int>,int*,nullptr> invalidTicket(nullptr);
  EXPECT_TRUE(invalidTicket.id() == 2uLL);
  EXPECT_TRUE(invalidTicket.data() == nullptr);
  EXPECT_FALSE(invalidTicket.isValid());
  EXPECT_FALSE(invalidTicket);
  EXPECT_TRUE(invalidTicket() == 2uLL);
  EXPECT_TRUE((uint64_t)invalidTicket == 2uLL);
  invalidTicket.invalidate();
  EXPECT_FALSE(invalidTicket.isValid());

  std::shared_ptr<int> dummy = std::make_shared<int>(5);
  Ticket<std::shared_ptr<int>,int*,nullptr> ticket1(std::move(dummy));
  EXPECT_TRUE(ticket1.id() == 3uLL);
  ASSERT_TRUE(ticket1.data() != nullptr);
  EXPECT_TRUE(*(ticket1.data()) == 5);
  EXPECT_TRUE(ticket1.isValid());
  EXPECT_TRUE(ticket1);
  EXPECT_TRUE(ticket1() == 3uLL);
  EXPECT_TRUE((uint64_t)ticket1 == 3uLL);

  std::shared_ptr<int> dummy2 = std::make_shared<int>(6);
  Ticket<std::shared_ptr<int>,int*,nullptr> ticket2(std::move(dummy2));
  EXPECT_TRUE(ticket2.id() == 4uLL);
  ASSERT_TRUE(ticket2.data() != nullptr);
  EXPECT_TRUE(*(ticket2.data()) == 6);
  EXPECT_TRUE(ticket2.isValid());
  EXPECT_TRUE(ticket2);
  EXPECT_TRUE(ticket2() == 4uLL);
  EXPECT_TRUE((uint64_t)ticket2 == 4uLL);

  EXPECT_TRUE(ticket1 == ticket1);
  EXPECT_FALSE(ticket1 == ticket2);
  EXPECT_FALSE(ticket1 != ticket1);
  EXPECT_TRUE(ticket1 != ticket2);
  EXPECT_TRUE(ticket1 < ticket2);
  EXPECT_FALSE(ticket2 < ticket1);
  EXPECT_TRUE(ticket1 <= ticket2);
  EXPECT_FALSE(ticket2 <= ticket1);
  EXPECT_FALSE(ticket1 > ticket2);
  EXPECT_TRUE(ticket2 > ticket1);
  EXPECT_FALSE(ticket1 >= ticket2);
  EXPECT_TRUE(ticket2 >= ticket1);

  emptyTicket = std::move(ticket1);
  EXPECT_TRUE(emptyTicket.id() == 3uLL);
  ASSERT_TRUE(emptyTicket.data() != nullptr);
  EXPECT_TRUE(*(emptyTicket.data()) == 5);
  EXPECT_TRUE(emptyTicket.isValid());
  EXPECT_FALSE(ticket1.isValid());
  emptyTicket.invalidate();
  EXPECT_FALSE(emptyTicket.isValid());

  Ticket<std::shared_ptr<int>,int*,nullptr> copyTicket2(ticket2);
  EXPECT_TRUE(copyTicket2.id() == 4uLL);
  ASSERT_TRUE(copyTicket2.data() != nullptr);
  EXPECT_TRUE(*(copyTicket2.data()) == 6);
  EXPECT_TRUE(copyTicket2.isValid());
  Ticket<std::shared_ptr<int>,int*,nullptr> newTicket2(std::move(ticket2));
  EXPECT_TRUE(newTicket2.id() == 4uLL);
  ASSERT_TRUE(newTicket2.data() != nullptr);
  EXPECT_TRUE(*(newTicket2.data()) == 6);
  EXPECT_TRUE(newTicket2.isValid());
  EXPECT_FALSE(ticket2.isValid());
  newTicket2.invalidate();
  EXPECT_FALSE(newTicket2.isValid());
}
