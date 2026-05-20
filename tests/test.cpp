
#include "Account.h"
#include "Transaction.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

TEST(Account, SimpleTest) {
    Account acc(1, 100);
    EXPECT_EQ(acc.id(), 1);
    EXPECT_EQ(acc.GetBalance(), 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
    EXPECT_NO_THROW(acc.Lock());
    acc.ChangeBalance(50);
    EXPECT_EQ(acc.GetBalance(), 150);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
    acc.Unlock();
    EXPECT_NO_THROW(acc.Lock());
}

TEST(Transaction, MockTest) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction tx;
    tx.set_fee(100);

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    
    EXPECT_CALL(to, ChangeBalance(500)).Times(1);
    
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(to, ChangeBalance(-600)).Times(1);
    
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_TRUE(tx.Make(from, to, 500));
}

TEST(Transaction, SimpleTest) {
    Transaction tx;
    Account from(1, 1000);
    Account to(2, 500);
    tx.set_fee(100);

    EXPECT_TRUE(tx.Make(from, to, 500));
    EXPECT_EQ(from.GetBalance(), 1000);
    EXPECT_EQ(to.GetBalance(), 400);
}
