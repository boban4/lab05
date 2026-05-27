#include <gtest/gtest.h>
#include <stdexcept>
#include "Account.h"

TEST(Account, ConstructorStoresIdAndBalance) {
    Account account(7, 100);

    EXPECT_EQ(account.id(), 7);
    EXPECT_EQ(account.GetBalance(), 100);
}

TEST(Account, ChangeBalanceThrowsIfNotLocked) {
    Account account(1, 100);

    EXPECT_THROW(account.ChangeBalance(50), std::runtime_error);
    EXPECT_EQ(account.GetBalance(), 100);
}

TEST(Account, LockAllowsBalanceChange) {
    Account account(1, 100);

    account.Lock();
    account.ChangeBalance(50);

    EXPECT_EQ(account.GetBalance(), 150);
}

TEST(Account, DoubleLockThrowsException) {
    Account account(1, 100);

    account.Lock();
    EXPECT_THROW(account.Lock(), std::runtime_error);
}

TEST(Account, UnlockDisablesBalanceChange) {
    Account account(1, 100);

    account.Lock();
    account.ChangeBalance(25);
    EXPECT_EQ(account.GetBalance(), 125);

    account.Unlock();
    EXPECT_THROW(account.ChangeBalance(10), std::runtime_error);
    EXPECT_EQ(account.GetBalance(), 125);
}
