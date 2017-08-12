from datetime import date
from ledger import Transaction, Account

credit_account = Account("Credit Account", -100.0)
checking_account = Account("Checking Account", 0.0)

credit_account.print_account()
checking_account.print_account()

checking_account.add_transaction(Transaction(date.today(), "Paycheck", "", 0.0, 500.0, True))

credit_account.add_transaction(Transaction(date.today(), "ACME", "This is a note", 100.0, 0.0, False))
credit_account.add_transaction(Transaction(date.today(), "ACME", "This is a note", 100.0, 0.0, False))

credit_account.print_account()
checking_account.print_account()
