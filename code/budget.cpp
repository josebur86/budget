/* TODO(joe):
 *  - Starting Balance + Current Balance
 *  - Render dollar amounts correctly ($1,234.56)
 *  - Implement register functionality.
 *      + Date
 *      + Payee
 *      + Category
 *      + Memo
 *      + Inflow / Outflow
 *      + Is Cleared
 *  - Port transaction importing into the main application.
 *  - Reconcile with imported transactions.
 *  - Platform services
 *      + Memory management
 *      + Treading
 *  + Implement Budget View
 */

#include <stdio.h>
#include <stdlib.h>

#include "sqlite/sqlite3.h"

#define Assert(Statement) if (!(Statement)) { *(int *)0 = 0; }

//
// SQL / Accounts 
//

static char SelectTransactionsSQL[] = 
"select  stage, "
        "transaction_date, "
        "posted_date, "
        "card, "
        "description, "
        "note, "
        "debit, "
        "credit from transactions order by transaction_date desc;";

enum transaction_columns
{
    Transaction_Stage,
    Transaction_TransactionDate,
    Transaction_PostedDate,
    Transaction_Card,
    Transaction_Description,
    Transaction_Note,
    Transaction_Debit,
    Transaction_Credit,
    
    Transaction_ColumnCount
};

static char *GetLabelForColumn(transaction_columns Column)
{
    switch (Column)
    {
        case Transaction_Stage:
        {
            return "Stage"; 
        } break;
        case Transaction_TransactionDate:
        {
            return "Transaction Date";
        } break;
        case Transaction_PostedDate:
        {
            return "Posted Date";
        } break;
        case Transaction_Card:
        {
            return "Card";
        } break;
        case Transaction_Description:
        {
            return "Description";
        } break;
        case Transaction_Note:
        {
            return "Note";
        } break;
        case Transaction_Debit:
        {
            return "Debit";
        } break;
        case Transaction_Credit:
        {
            return "Credit";
        } break;
        default:
        {
            Assert(false);
            return "UNKNOWN!";
        };
    }
}

union transaction
{
    struct
    {
        unsigned char *Stage;
        unsigned char *TransactionDate;
        unsigned char *PostedDate;
        unsigned char *Card;
        unsigned char *Description;
        unsigned char *Note;
        unsigned char *Debit;
        unsigned char *Credit;
    };
    unsigned char *Values[Transaction_ColumnCount];
};

struct account
{
#define MAX_TRANSACTION_COUNT 1024
    transaction Transactions[MAX_TRANSACTION_COUNT];
    int TransactionCount;

    float CurrentBalance;
    float StartingBalance;
};
static account GlobalAccount = {};

static void AddTransactionToAccount(account *Account, transaction *Transaction)
{
    Assert(Account->TransactionCount+1 <= MAX_TRANSACTION_COUNT);
    Account->Transactions[Account->TransactionCount++] = *Transaction;
}

static unsigned char * CopyString(const unsigned char *Source)
{
    int Length = 0;
    while (Source[Length] != '\0')
    {
        ++Length;
    }  
    ++Length;
    
    // TODO(joe): Allocate this out of a stack that is managed by us
    // instead operating system.
    unsigned char *Result = (unsigned char *)calloc(Length, sizeof(unsigned char));

    unsigned char *Dest = Result;
    for (int Index = 0; Index < Length; ++Index)
    {
        *Dest++ = *Source++;
    }

    return Result;
}

static unsigned char * CopyString(char *Source)
{
    return CopyString((const unsigned char *)Source);
}

static bool LoadTransactionsIntoAccount(account *Account, float StartingBalance)
{
    bool Success = false;

    transaction StartingTransaction = {};
    StartingTransaction.Stage = CopyString("POSTED");
    StartingTransaction.TransactionDate = CopyString("2017-01-01");
    StartingTransaction.PostedDate = CopyString("2017-01-01");
    StartingTransaction.Description = CopyString("Starting Balance");
    StartingTransaction.Debit = CopyString("0.0");

    AddTransactionToAccount(Account, &StartingTransaction);


    sqlite3 *Database = 0;
    int Result = sqlite3_open("test.db", &Database);
    if (Result == SQLITE_OK)
    {
        sqlite3_stmt *Statement;
        int Result = sqlite3_prepare_v2(Database, SelectTransactionsSQL, sizeof(SelectTransactionsSQL), &Statement, 0);
        if (Result == SQLITE_OK)
        {
            Result = sqlite3_step(Statement);
            while (Result == SQLITE_ROW)
            {
                transaction Transaction = {};
                Transaction.Stage = CopyString(sqlite3_column_text(Statement, 0));
                Transaction.TransactionDate = CopyString(sqlite3_column_text(Statement, 1));
                Transaction.PostedDate = CopyString(sqlite3_column_text(Statement, 2));
                Transaction.Card = CopyString(sqlite3_column_text(Statement, 3));
                Transaction.Description = CopyString(sqlite3_column_text(Statement, 4));
                Transaction.Note = CopyString(sqlite3_column_text(Statement, 5));
                Transaction.Debit = CopyString(sqlite3_column_text(Statement, 6));
                Transaction.Credit = CopyString(sqlite3_column_text(Statement, 7));

                AddTransactionToAccount(Account, &Transaction);

                Result = sqlite3_step(Statement);
            }    

            Success = Result == SQLITE_DONE;
        }
        else
        {
            printf("Unable to load transactions: %s\n", sqlite3_errmsg(Database));
        }

        sqlite3_finalize(Statement);
    }

    return Success;
}

int main(int argc, char **argv)
{
    printf("Hello, Budget!\n");
}
