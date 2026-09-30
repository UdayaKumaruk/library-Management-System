#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX 100
#define FINE_PER_DAY 5

/* ---------- STRUCTURES ---------- */

struct Book
{
    int id;
    char title[50];
    char author[50];
    int quantity;
};

struct Issue
{
    int issueId;
    int bookId;
    int userId;
    char userName[50];

    char issueDate[11];
    char dueDate[11];
    char returnDate[11];

    int fine;
    int returned;
};

/* ---------- GLOBAL DATA ---------- */

struct Book books[MAX];
struct Issue issues[MAX];

int bookCount = 0;
int issueCount = 0;


/* ---------- FIND BOOK ---------- */

int findBook(int id)
{
    int i;

    for(i = 0; i < bookCount; i++)
    {
        if(books[i].id == id)
            return i;
    }

    return -1;
}


/* ---------- FIND ACTIVE ISSUE ---------- */

int findIssue(int bookId, int userId)
{
    int i;

    for(i = 0; i < issueCount; i++)
    {
        if(issues[i].bookId == bookId &&
           issues[i].userId == userId &&
           issues[i].returned == 0)
        {
            return i;
        }
    }

    return -1;
}


/* ---------- GET DATE ---------- */

void getDate(char date[], int days)
{
    time_t t;
    struct tm *p;

    t = time(NULL);
    p = localtime(&t);

    p->tm_mday += days;
    mktime(p);

    sprintf(date, "%02d-%02d-%04d",
            p->tm_mday,
            p->tm_mon + 1,
            p->tm_year + 1900);
}


/* ---------- CONVERT DATE ---------- */

time_t convertDate(char date[])
{
    int d, m, y;
    struct tm t = {0};

    sscanf(date, "%d-%d-%d", &d, &m, &y);

    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;

    return mktime(&t);
}


/* ---------- CALCULATE LATE DAYS ---------- */

int lateDays(char dueDate[], char returnDate[])
{
    time_t due;
    time_t ret;

    due = convertDate(dueDate);
    ret = convertDate(returnDate);

    if(ret <= due)
        return 0;

    return (int)((ret - due) / (60 * 60 * 24));
}


/* ---------- SAVE DATA ---------- */

void saveData()
{
    FILE *fp;

    fp = fopen("books.dat", "wb");

    if(fp == NULL)
    {
        printf("\nError saving books.\n");
        return;
    }

    fwrite(&bookCount, sizeof(int), 1, fp);
    fwrite(books, sizeof(struct Book), bookCount, fp);

    fclose(fp);


    fp = fopen("issues.dat", "wb");

    if(fp == NULL)
    {
        printf("\nError saving issues.\n");
        return;
    }

    fwrite(&issueCount, sizeof(int), 1, fp);
    fwrite(issues, sizeof(struct Issue), issueCount, fp);

    fclose(fp);

    printf("\nData saved successfully.\n");
}


/* ---------- LOAD DATA ---------- */

void loadData()
{
    FILE *fp;

    fp = fopen("books.dat", "rb");

    if(fp != NULL)
    {
        fread(&bookCount, sizeof(int), 1, fp);
        fread(books, sizeof(struct Book), bookCount, fp);
        fclose(fp);
    }


    fp = fopen("issues.dat", "rb");

    if(fp != NULL)
    {
        fread(&issueCount, sizeof(int), 1, fp);
        fread(issues, sizeof(struct Issue), issueCount, fp);
        fclose(fp);
    }
}


/* ---------- ADD BOOK ---------- */

void addBook()
{
    int id;

    if(bookCount >= MAX)
    {
        printf("\nBook limit reached.\n");
        return;
    }

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    if(findBook(id) != -1)
    {
        printf("\nBook ID already exists.\n");
        return;
    }

    books[bookCount].id = id;

    printf("Enter Book Title: ");
    scanf(" %[^\n]", books[bookCount].title);

    printf("Enter Author Name: ");
    scanf(" %[^\n]", books[bookCount].author);

    printf("Enter Quantity: ");
    scanf("%d", &books[bookCount].quantity);

    if(books[bookCount].quantity < 0)
    {
        printf("\nInvalid quantity.\n");
        return;
    }

    bookCount++;

    printf("\nBook added successfully.\n");
}


/* ---------- DISPLAY ONE BOOK ---------- */

void displayBook(struct Book b)
{
    printf("\nID       : %d", b.id);
    printf("\nTitle    : %s", b.title);
    printf("\nAuthor   : %s", b.author);
    printf("\nQuantity : %d\n", b.quantity);
}


/* ---------- VIEW BOOKS ---------- */

void viewBooks()
{
    int i;

    if(bookCount == 0)
    {
        printf("\nNo books available.\n");
        return;
    }

    printf("\n========== ALL BOOKS ==========\n");

    for(i = 0; i < bookCount; i++)
        displayBook(books[i]);
}


/* ---------- SEARCH BOOK ---------- */

void searchBook()
{
    int choice, id, i, found = 0;
    char text[50];

    printf("\n========== SEARCH ==========");
    printf("\n1. By Book ID");
    printf("\n2. By Book Name");
    printf("\n3. By Author");
    printf("\nEnter choice: ");
    scanf("%d", &choice);

    if(choice == 1)
    {
        printf("Enter Book ID: ");
        scanf("%d", &id);

        i = findBook(id);

        if(i != -1)
            displayBook(books[i]);
        else
            printf("\nBook not found.\n");
    }

    else if(choice == 2)
    {
        printf("Enter Book Name: ");
        scanf(" %[^\n]", text);

        for(i = 0; i < bookCount; i++)
        {
            if(strcmp(books[i].title, text) == 0)
            {
                displayBook(books[i]);
                found = 1;
            }
        }

        if(!found)
            printf("\nBook not found.\n");
    }

    else if(choice == 3)
    {
        printf("Enter Author Name: ");
        scanf(" %[^\n]", text);

        for(i = 0; i < bookCount; i++)
        {
            if(strcmp(books[i].author, text) == 0)
            {
                displayBook(books[i]);
                found = 1;
            }
        }

        if(!found)
            printf("\nBook not found.\n");
    }

    else
    {
        printf("\nInvalid choice.\n");
    }
}


/* ---------- UPDATE BOOK ---------- */

void updateBook()
{
    int id, i, choice;

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    i = findBook(id);

    if(i == -1)
    {
        printf("\nBook not found.\n");
        return;
    }

    printf("\n1. Update Title");
    printf("\n2. Update Author");
    printf("\n3. Update Quantity");
    printf("\nEnter choice: ");
    scanf("%d", &choice);

    if(choice == 1)
    {
        printf("Enter new title: ");
        scanf(" %[^\n]", books[i].title);
    }

    else if(choice == 2)
    {
        printf("Enter new author: ");
        scanf(" %[^\n]", books[i].author);
    }

    else if(choice == 3)
    {
        printf("Enter new quantity: ");
        scanf("%d", &books[i].quantity);

        if(books[i].quantity < 0)
        {
            printf("\nInvalid quantity.\n");
            return;
        }
    }

    else
    {
        printf("\nInvalid choice.\n");
        return;
    }

    printf("\nBook updated successfully.\n");
}


/* ---------- REMOVE BOOK ---------- */

void removeBook()
{
    int id, i, j;

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    i = findBook(id);

    if(i == -1)
    {
        printf("\nBook not found.\n");
        return;
    }

    /* Do not delete currently issued book */

    for(j = 0; j < issueCount; j++)
    {
        if(issues[j].bookId == id &&
           issues[j].returned == 0)
        {
            printf("\nBook is currently issued.\n");
            return;
        }
    }

    for(j = i; j < bookCount - 1; j++)
        books[j] = books[j + 1];

    bookCount--;

    printf("\nBook removed successfully.\n");
}


/* ---------- ISSUE BOOK ---------- */

void issueBook()
{
    int id, i;

    if(issueCount >= MAX)
    {
        printf("\nIssue limit reached.\n");
        return;
    }

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    i = findBook(id);

    if(i == -1)
    {
        printf("\nBook not found.\n");
        return;
    }

    if(books[i].quantity <= 0)
    {
        printf("\nBook is not available.\n");
        return;
    }

    issues[issueCount].issueId = issueCount + 1;
    issues[issueCount].bookId = id;

    printf("Enter User ID: ");
    scanf("%d", &issues[issueCount].userId);

    printf("Enter User Name: ");
    scanf(" %[^\n]", issues[issueCount].userName);

    getDate(issues[issueCount].issueDate, 0);
    getDate(issues[issueCount].dueDate, 7);

    strcpy(issues[issueCount].returnDate, "Not Returned");

    issues[issueCount].fine = 0;
    issues[issueCount].returned = 0;

    books[i].quantity--;
    issueCount++;

    printf("\nBook issued successfully.");
    printf("\nIssue Date : %s", issues[issueCount - 1].issueDate);
    printf("\nDue Date   : %s\n", issues[issueCount - 1].dueDate);
}


/* ---------- RETURN BOOK ---------- */

void returnBook()
{
    int bookId, userId;
    int i, bookIndex;
    int days;

    char today[11];

    printf("\nEnter Book ID: ");
    scanf("%d", &bookId);

    printf("Enter User ID: ");
    scanf("%d", &userId);

    i = findIssue(bookId, userId);

    if(i == -1)
    {
        printf("\nActive issue record not found.\n");
        return;
    }

    getDate(today, 0);

    strcpy(issues[i].returnDate, today);

    days = lateDays(issues[i].dueDate,
                     issues[i].returnDate);

    issues[i].fine = days * FINE_PER_DAY;
    issues[i].returned = 1;

    bookIndex = findBook(bookId);

    if(bookIndex != -1)
        books[bookIndex].quantity++;

    printf("\nBook returned successfully.");
    printf("\nReturn Date : %s", issues[i].returnDate);
    printf("\nLate Days   : %d", days);
    printf("\nFine        : Rs.%d\n", issues[i].fine);
}


/* ---------- LIST ISSUED BOOKS ---------- */

void listIssuedBooks()
{
    int i, bookIndex;

    if(issueCount == 0)
    {
        printf("\nNo issue records.\n");
        return;
    }

    printf("\n========== ISSUE RECORDS ==========\n");

    for(i = 0; i < issueCount; i++)
    {
        bookIndex = findBook(issues[i].bookId);

        printf("\nIssue ID   : %d", issues[i].issueId);
        printf("\nBook ID    : %d", issues[i].bookId);

        if(bookIndex != -1)
            printf("\nBook Title : %s", books[bookIndex].title);

        printf("\nUser ID    : %d", issues[i].userId);
        printf("\nUser Name  : %s", issues[i].userName);
        printf("\nIssue Date : %s", issues[i].issueDate);
        printf("\nDue Date   : %s", issues[i].dueDate);
        printf("\nReturn Date: %s", issues[i].returnDate);
        printf("\nFine       : Rs.%d", issues[i].fine);

        if(issues[i].returned)
            printf("\nStatus     : Returned\n");
        else
            printf("\nStatus     : Issued\n");
    }
}


/* ---------- MAIN ---------- */

int main()
{
    int choice;

    loadData();

    while(1)
    {
        printf("\n\n================================");
        printf("\n     LIBRARY MANAGEMENT SYSTEM");
        printf("\n================================");

        printf("\n1. Add New Book");
        printf("\n2. Update Book Details");
        printf("\n3. Remove Book");
        printf("\n4. Search Book");
        printf("\n5. View All Books");
        printf("\n6. Issue Book");
        printf("\n7. Return Book");
        printf("\n8. List Issued Books");
        printf("\n9. Save");
        printf("\n10. Exit");

        printf("\n\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                addBook();
                break;

            case 2:
                updateBook();
                break;

            case 3:
                removeBook();
                break;

            case 4:
                searchBook();
                break;

            case 5:
                viewBooks();
                break;

            case 6:
                issueBook();
                break;

            case 7:
                returnBook();
                break;

            case 8:
                listIssuedBooks();
                break;

            case 9:
                saveData();
                break;

            case 10:
                saveData();
                printf("\nProgram closed.\n");
                return 0;

            default:
                printf("\nInvalid choice.\n");
        }
    }
}