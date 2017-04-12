#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite/sqlite3.h"

static char CreateTableSQL[] = 
"create table transactions (id integer primary key asc, "
                           "stage varchar(64), "
                           "transaction_date text, "
                           "posted_date text, "
                           "card varchar(64), "
                           "description varchar(64), "
                           "note varchar(64), "
                           "debit varchar(64), "
                           "credit varchar(64)"
                           ");";
static char CheckTableSQL[] = 
"select name from sqlite_master where type='table' AND name='transactions'";

static char InsertSQL[] = "insert into transactions (stage, "
                                                    "transaction_date, "
                                                    "posted_date, "
                                                    "card, "
                                                    "description, "
                                                    "note, "
                                                    "debit, "
                                                    "credit"
                                                    ")" 
                          " values (?, ?, ?, ?, ?, ?, ?, ?);";

struct transaction
{
    char *Stage;
    char *TransactionDate;
    char *PostedDate;
    char *Card;
    char *Description;
    char *Note;
    char *Debit;
    char *Credit;
};

static char * CopyStringUntilComma(char *Source, char *Dest)
{
    while (Source && (*Source != ',' && *Source != '\0'))
    {
        *Dest++ = *Source++;
    }
    *Dest = '\0';

    return ++Source;
}

static bool CreateTransactionTable(sqlite3 *Database)
{
    bool TableCreated = true;

    sqlite3_stmt *Statement;
    int Result = sqlite3_prepare_v2(Database, CreateTableSQL, sizeof(CreateTableSQL), &Statement, 0);
    if (Result == SQLITE_OK)
    {
        Result = sqlite3_step(Statement);
        if (Result == SQLITE_DONE)
        {
            // NOTE(joe): Success!
        }
        else
        {
            TableCreated = false;
        }
    }
    else
    {
        TableCreated = false;
    }
    sqlite3_finalize(Statement);

    return TableCreated;
}

#define Assert(Statement) if (!(Statement)) { *(int *)0 = 0; }
#define BIND(Index, Value) sqlite3_bind_text(Statement, (Index), (Value), strlen(Value), 0)

static bool DoesTransactionTableExist(sqlite3 *Database)
{
    bool TableExists = false;

    sqlite3_stmt *Statement;
    int Result = sqlite3_prepare_v2(Database, CheckTableSQL, sizeof(CheckTableSQL), &Statement, 0);
    if (Result == SQLITE_OK)
    {
        Result = sqlite3_step(Statement);
        if (Result == SQLITE_ROW)
        {
            TableExists = true;
        }
    }
    sqlite3_finalize(Statement);

    return TableExists;
}

void InsertTransaction(sqlite3 *Database, transaction Transaction)
{

    sqlite3_stmt *Statement;
    int Result = sqlite3_prepare_v2(Database, InsertSQL, sizeof(InsertSQL), &Statement, 0);
    if (Result == SQLITE_OK)
    {
        Result = BIND(1, Transaction.Stage);
        Assert(Result == SQLITE_OK);
        Result = BIND(2, Transaction.TransactionDate);
        Assert(Result == SQLITE_OK);
        Result = BIND(3, Transaction.PostedDate);
        Assert(Result == SQLITE_OK);
        Result = BIND(4, Transaction.Card);
        Assert(Result == SQLITE_OK);
        Result = BIND(5, Transaction.Description);
        Assert(Result == SQLITE_OK);
        Result = BIND(6, Transaction.Note);
        Assert(Result == SQLITE_OK);
        Result = BIND(7, Transaction.Debit);
        Assert(Result == SQLITE_OK);
        Result = BIND(8, Transaction.Credit);
        Assert(Result == SQLITE_OK);

        Result = sqlite3_step(Statement);
        if (Result == SQLITE_DONE)
        {
            // NOTE(joe): Success!
        }
        else
        {
            printf("Could not insert the transaction: %s.\n", sqlite3_errmsg(Database));
        }
    }
    else
    {
        printf("Could not insert the transaction: %s.\n", sqlite3_errmsg(Database));
    }
    sqlite3_finalize(Statement);
}

// NOTE(joe): Convert the Capital One date format (MM/DD/YYYY) into SQLite
// format (YYYY-MM-DD).
static void FormatDate(char *Date)
{
    char Month[3] = {0};
    char Day[3] = {0};
    char Year[5] = {0};

    char *Source = Date;

    // NOTE(joe): Grab the month
    char *Dest = Month;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest = '\0';
    ++Source; // NOTE(joe): /

    // NOTE(joe): Grab the day
    Dest = Day;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest = '\0';
    ++Source; // NOTE(joe): /

    // NOTE(joe): Grab the day
    Dest = Year;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest = '\0';

    Dest = Date;
    Source = Year;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest++ = *Source++;
    *Dest++ = *Source;
    *Dest++ = '-';

    Source = Month;
    *Dest++ = *Source++;
    *Dest++ = *Source;
    *Dest++ = '-';
    
    Source = Day;
    *Dest++ = *Source++;
    *Dest++ = *Source;
}

static void InsertTransactionsFromFile(sqlite3 *Database, char *FileName)
{
    FILE *File = fopen(FileName, "r");
    if (File)
    {
        bool HeaderRead = false;
        char Buffer[512];
        while (fgets(Buffer, 512, File))
        {
            if (HeaderRead)
            {
                transaction Transaction = {};
                Transaction.Stage = (char *)calloc(64, sizeof(char));
                Transaction.TransactionDate = (char *)calloc(64, sizeof(char));
                Transaction.PostedDate = (char *)calloc(64, sizeof(char));
                Transaction.Card = (char *)calloc(64, sizeof(char));
                Transaction.Description = (char *)calloc(64, sizeof(char));
                Transaction.Note = (char *)calloc(64, sizeof(char));
                Transaction.Debit = (char *)calloc(64, sizeof(char));
                Transaction.Credit = (char *)calloc(64, sizeof(char));

                char *Source = Buffer;
                Source = CopyStringUntilComma(Source, Transaction.Stage);
                Source = CopyStringUntilComma(Source, Transaction.TransactionDate);
                Source = CopyStringUntilComma(Source, Transaction.PostedDate);
                Source = CopyStringUntilComma(Source, Transaction.Card);
                Source = CopyStringUntilComma(Source, Transaction.Description);
                Source = CopyStringUntilComma(Source, Transaction.Note);
                Source = CopyStringUntilComma(Source, Transaction.Debit);
                Source = CopyStringUntilComma(Source, Transaction.Credit);

                FormatDate(Transaction.TransactionDate);
                FormatDate(Transaction.PostedDate);
                
                // TODO(joe): Batch the insertions?
                InsertTransaction(Database, Transaction);
            }
            else
            {
                HeaderRead = true;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("Usage: import_trans <file>.csv\n\n");
        return 1;
    }

    // TODO(joe): Handle if the file is not a CSV file.

    sqlite3 *Database = 0;
    int Result = sqlite3_open("test.db", &Database);
    if (Result == SQLITE_OK)
    {
        if (DoesTransactionTableExist(Database) || CreateTransactionTable(Database))
        {
            char * FileName = argv[1];
            InsertTransactionsFromFile(Database, FileName);
        }
        else
        {
            printf("Unable to create the transactions table: %s\n", sqlite3_errmsg(Database));
            sqlite3_close(Database);
        }
    }
    else
    {
        printf("Unable to open/create the database: %s\n", sqlite3_errstr(Result));
        return 1;
    }
    sqlite3_close(Database);

    return 0;
}
