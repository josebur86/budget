import sqlalchemy

from sqlalchemy import create_engine
from sqlalchemy import Column, Date, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

engine = create_engine('sqlite:///test.db', echo=True)
Session = sessionmaker(bind=engine)

Base = declarative_base()

class Transaction(Base):
    __tablename__ = 'transactions'

    id = Column(Integer, primary_key=True)
    stage = Column(String)
    transaction_date = Column(Date)
    posted_date = Column(Date)
    card = Column(String)
    description = Column(String)
    note = Column(String)
    debit = Column(String)
    credit = Column(String)

    def __repr__(self):
        return "<Transaction(card='%s', transaction_date='%s', description='%s', debit='%s', credit='%s')>" % (
                self.card, self.transaction_date, self.description, self.debit, self.credit)

session = Session()

for row in session.query(Transaction).all():
    print (row)
    
