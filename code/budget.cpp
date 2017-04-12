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

#include <wx/wx.h>
#include <wx/grid.h>

#include <sqlite3.h>

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
    return CopyString(Source);
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

//
// Events
//

enum
{
    ID_HELLO = 1,
};

//
// BudgetFrame
//

class BudgetFrame : public wxFrame
{
public:
    BudgetFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

};

BudgetFrame::BudgetFrame()
: wxFrame(0, wxID_ANY, "Budget", wxPoint(10, 20), wxSize(1000, 1000))
{
    wxMenu *MenuFile = new wxMenu;
    MenuFile->Append(ID_HELLO, "&Hello...\tCtrl-H", 
            "Help string shown in status bar for this menu item.");
    MenuFile->AppendSeparator();
    MenuFile->Append(wxID_EXIT);

    wxMenu *MenuHelp = new wxMenu;
    MenuHelp->Append(wxID_ABOUT);

    wxMenuBar *MenuBar = new wxMenuBar;
    MenuBar->Append(MenuFile, "&File");
    MenuBar->Append(MenuHelp, "&Help");

    SetMenuBar(MenuBar);

    Bind(wxEVT_MENU, &BudgetFrame::OnHello, this, ID_HELLO);
    Bind(wxEVT_MENU, &BudgetFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &BudgetFrame::OnExit, this, wxID_EXIT);

    bool AccountLoaded = LoadTransactionsIntoAccount(&GlobalAccount, 0.00f);
    //Assert(AccountLoaded);

    // Create Grid
    wxGrid *Grid = new wxGrid(this, -1, wxPoint(0, 0), wxSize(1000, 540));
    if (AccountLoaded)
    {
        // TODO(joe): Load the cells with the data.
        //
        // Then we call CreateGrid to set the dimensions of the grid
        Grid->CreateGrid( GlobalAccount.TransactionCount, Transaction_ColumnCount );

        // We can set the sizes of individual rows and columns  in pixels
        Grid->HideRowLabels();
        Grid->SetRowSize( 0, 60 );

        for (int ColIndex = 0; ColIndex < Transaction_ColumnCount; ++ColIndex)
        {
            Grid->SetColLabelValue(ColIndex, GetLabelForColumn((transaction_columns)ColIndex));
        }

        for (int RowIndex = 0; RowIndex < GlobalAccount.TransactionCount; ++RowIndex)
        {
            transaction *Transaction = GlobalAccount.Transactions + RowIndex;
            for (int ColIndex = 0; ColIndex < Transaction_ColumnCount; ++ColIndex)
            {
                Grid->SetCellValue( RowIndex, ColIndex, Transaction->Values[ColIndex]);
            }
        }
        Grid->AutoSizeColumns(true);
    }

    wxStaticText *BalanceText = new wxStaticText(this, -1, "Current Balance: ");

    wxBoxSizer *Sizer = new wxBoxSizer( wxVERTICAL );
    Sizer->Add(BalanceText);
    Sizer->Add(Grid, 1, wxEXPAND);

    SetSizerAndFit( Sizer );
}

void BudgetFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets.");
}

void BudgetFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void BudgetFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World sample", 
            "About Hello World", wxOK | wxICON_INFORMATION);
}

//
// BudgetApp
//

class BudgetApp : public wxApp
{
public:
    virtual bool OnInit();
};

bool BudgetApp::OnInit()
{
    BudgetFrame *Frame = new BudgetFrame();
    Frame->Show(true);

    return true; // Successful init
}

wxIMPLEMENT_APP(BudgetApp);
