#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"
#include <stdexcept>

using ::testing::Return;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

TEST(AccountTest, Initialization) {
    Account wallet(10, 500);
    EXPECT_EQ(wallet.id(), 10);
    EXPECT_EQ(wallet.GetBalance(), 500);
}

TEST(AccountTest, LockUnlockOperations) {
    Account wallet(1, 0);
    EXPECT_NO_THROW(wallet.Lock());
    EXPECT_THROW(wallet.Lock(), std::runtime_error);
    EXPECT_NO_THROW(wallet.Unlock());
}

TEST(AccountTest, BalanceModification) {
    Account wallet(2, 100);
    EXPECT_THROW(wallet.ChangeBalance(20), std::runtime_error);
    wallet.Lock();
    wallet.ChangeBalance(30);
    EXPECT_EQ(wallet.GetBalance(), 130);
}

TEST(TransactionTest, FeeConfiguration) {
    Transaction tx;
    EXPECT_EQ(tx.fee(), 1);
    tx.set_fee(5);
    EXPECT_EQ(tx.fee(), 5);
}

TEST(TransactionTest, ValidationErrors) {
    MockTransaction tx;
    MockAccount src(1, 1000), dst(2, 1000);

    EXPECT_THROW(tx.Make(src, src, 200), std::logic_error);
    EXPECT_THROW(tx.Make(src, dst, -10), std::invalid_argument);
    EXPECT_THROW(tx.Make(src, dst, 50), std::logic_error);

    tx.set_fee(60);
    EXPECT_FALSE(tx.Make(src, dst, 100));
}

TEST(TransactionTest, SuccessfulTransfer) {
    MockTransaction tx;
    MockAccount src(1, 5000), dst(2, 5000);
    tx.set_fee(1);

    EXPECT_CALL(src, Lock());
    EXPECT_CALL(dst, Lock());
    EXPECT_CALL(dst, ChangeBalance(100));
    EXPECT_CALL(dst, GetBalance()).WillOnce(Return(2000));
    EXPECT_CALL(dst, ChangeBalance(-101));
    EXPECT_CALL(tx, SaveToDataBase(_, _, 100));
    EXPECT_CALL(dst, Unlock());
    EXPECT_CALL(src, Unlock());

    EXPECT_TRUE(tx.Make(src, dst, 100));
}

TEST(TransactionTest, FailedDebitRollback) {
    MockTransaction tx;
    MockAccount src(1, 5000), dst(2, 5000);
    tx.set_fee(1);

    EXPECT_CALL(src, Lock());
    EXPECT_CALL(dst, Lock());
    EXPECT_CALL(dst, ChangeBalance(100));
    EXPECT_CALL(dst, GetBalance()).WillOnce(Return(50));
    EXPECT_CALL(dst, ChangeBalance(-100));
    EXPECT_CALL(tx, SaveToDataBase(_, _, 100));
    EXPECT_CALL(dst, Unlock());
    EXPECT_CALL(src, Unlock());

    EXPECT_FALSE(tx.Make(src, dst, 100));
}

TEST(TransactionTest, DestructorCoverage) {
    Transaction* tx = new Transaction();
    delete tx;
    
    Account* acc = new Account(1, 100);
    delete acc;
}

TEST(TransactionTest, RealExecutionPath) {
    Transaction tx;
    Account from(1, 1000);
    Account to(2, 1000);

    EXPECT_TRUE(tx.Make(from, to, 100));
}
