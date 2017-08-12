from flask import Flask, render_template, request
from datetime import date
from budget import *

app = Flask(__name__)

account = create_test_account();

@app.route("/")
def hello():
    return render_template("account.html",
                           account_name=account.name,
                           balance=account.calc_balance(),
                           transactions=account.transactions)

@app.route("/add_transaction", methods=["POST"])
def add_transaction():

    debit = float(request.form['debit']) if request.form['debit'] else 0.0
    credit = float(request.form['credit']) if request.form['credit'] else 0.0

    tx = Transaction(date.today(),
                     request.form['payee'],
                     request.form['note'],
                     debit,
                     credit, 
                     False)
    account.add_transaction(tx)

    return render_template("account.html",
                           account_name=account.name,
                           balance=account.calc_balance(),
                           transactions=account.transactions)

