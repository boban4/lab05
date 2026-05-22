#include "Account.h"
#include "Transaction.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class AccountMock : public Account {
public:
	AccountMock(int id, int balance) : Account(id, balance) {}
	MOCK_CONST_METHOD0(GetBalance, int());
	MOCK_METHOD1(ChangeBalance, void(int diff));
	MOCK_METHOD0(Lock, void());
	MOCK_METHOD0(Unlock, void());
};

class TransactionMock : public Transaction {
public:
	MOCK_METHOD3(SaveToDataBase, void(Account& from, Account& to, int sum));
};

TEST(account, test) {
	Account a(1, 1000);
	EXPECT_EQ(a.id(), 1);
	EXPECT_EQ(a.GetBalance(), 1000);
	EXPECT_THROW(a.ChangeBalance(1000), std::runtime_error);
	EXPECT_NO_THROW(a.Lock());
	a.ChangeBalance(1000);
	EXPECT_EQ(a.GetBalance(), 2000);
	EXPECT_THROW(a.Lock(), std::runtime_error);
	a.Unlock();
	EXPECT_NO_THROW(a.Lock()); 
}

TEST(Transaction, mock) {
    TransactionMock tr;
    AccountMock a1(1, 1000); 
    AccountMock a2(2, 2000);
    tr.set_fee(250);
    EXPECT_CALL(tr, SaveToDataBase(testing::_, testing::_, 500)).Times(1);
    EXPECT_CALL(a1, Lock()).Times(1);
    EXPECT_CALL(a2, Lock()).Times(1);
    EXPECT_CALL(a1, GetBalance()).WillOnce(testing::Return(1000));
    EXPECT_CALL(a1, ChangeBalance(-750)).Times(1);
    EXPECT_CALL(a2, ChangeBalance(500)).Times(1);
    EXPECT_CALL(a2, Unlock()).Times(1);
    EXPECT_CALL(a1, Unlock()).Times(1);
    EXPECT_FALSE(tr.Make(a1, a2, 499));
    EXPECT_TRUE(tr.Make(a1, a2, 500));
    EXPECT_THROW(tr.Make(a1, a2, 99), std::logic_error);
    EXPECT_THROW(tr.Make(a1, a2, -500), std::invalid_argument);
}


TEST(Transaction, test) {
    Transaction tr;
    Account a1(1, 1000);
    Account a2(2, 2000);
    tr.set_fee(250);
    EXPECT_FALSE(tr.Make(a1, a2, 499)); 
    EXPECT_EQ(a1.GetBalance(), 1000); 
    EXPECT_TRUE(tr.Make(a1, a2, 500)); 
    EXPECT_EQ(a1.GetBalance(), 250);   
    EXPECT_EQ(a2.GetBalance(), 2500);  
    EXPECT_NO_THROW(a1.Lock()); 
    EXPECT_NO_THROW(a2.Lock());
    a1.Unlock();
    a2.Unlock();
    EXPECT_THROW(tr.Make(a1, a1, 500), std::logic_error);
    EXPECT_THROW(tr.Make(a1, a2, 99), std::logic_error);
    EXPECT_THROW(tr.Make(a1, a2, -500), std::invalid_argument);
    Account a3(3, 100); 
    Account a4(4, 2000);
    tr.set_fee(100);    
    EXPECT_FALSE(tr.Make(a3, a4, 300)); 
    EXPECT_EQ(a3.GetBalance(), 100);    
    EXPECT_EQ(a4.GetBalance(), 2000);   
    Account a5(5, 1000);
    Account a6(6, 2000);
    tr.set_fee(1);
    EXPECT_TRUE(tr.Make(a5, a6, 100));
}
