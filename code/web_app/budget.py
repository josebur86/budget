from datetime import date
from ledger import Transaction, Account

def create_test_account():
    credit_account = Account("Credit Account", -100.0)
    credit_account.add_transaction(Transaction(date.today(), "ACME", "This is a note", 100.0, 0.0, False))
    credit_account.add_transaction(Transaction(date.today(), "ACME", "", 100.0, 0.0, True))

    return credit_account

