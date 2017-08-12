''' TODO(joe): 
     - Display account in the browser
     - Adding Transactions 
     - Categories
     - Multiple Accounts
     - Transfer transactions
'''

class Transaction:
    def __init__(self, date, payee, note, debit, credit, cleared):
        self.date = date
        self.payee = payee
        self.note = note
        self.debit = debit
        self.credit = credit
        self.cleared = cleared

class Account:
    def __init__(self, name, starting_balance):
        self.name = name
        self.starting_balance = starting_balance
        self.transactions = []

    def add_transaction(self, transaction):
        self.transactions.append(transaction)

    def calc_balance(self):
        cleared_balance = self.starting_balance
        pending_balance = 0.0

        for transaction in self.transactions:
            if transaction.cleared:
                cleared_balance += transaction.credit
                cleared_balance -= transaction.debit
            else:
                pending_balance += transaction.credit
                pending_balance -= transaction.debit

        return {
            "cleared_balance": cleared_balance,
            "pending_balance": pending_balance,
            "working_balance": cleared_balance + pending_balance,
        }

    def print_account(self):
        print("--------------------------------------------------------------")
        print("Account: %s" % self.name)
        print("Cleared Balance + Pending Balance = Working Balance")
        balance = self.calc_balance()
        print("{0} + {1} = {2}".format(balance["cleared_balance"],
                                       balance["pending_balance"],
                                       balance["working_balance"]))
        print("Date | Payee | Note | Debit | Credit | Cleared") 
        print("---- | ----- | ---- | ----- | ------ | -------") 
        for transaction in self.transactions:
            print("{} | {} | {} | {} | {} | {}".format(transaction.date,
                                                       transaction.payee,
                                                       transaction.note,
                                                       transaction.debit,
                                                       transaction.credit,
                                                       transaction.cleared))
        print("\n")
