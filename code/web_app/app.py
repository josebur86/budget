from flask import Flask, render_template
from budget import *

app = Flask(__name__)

@app.route("/")
def hello():
    account = create_test_account();
    return render_template("account.html",
                           account_name=account.name,
                           balance=account.calc_balance(),
                           transactions=account.transactions)
