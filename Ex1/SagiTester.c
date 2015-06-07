/**
 * Sagi Dayan 2014 (c) - Use At Your Own Risk! v1.0
 *
 * For Ex1 - In "Computer Communication Applications" Course
 *
 *
 * If This Tester Is Giving You Compiling Errors For Functions That I Used
 * And Are Not Defined In The Blue Print Header File - Let Me Know!
 * I Wrote It (the Tester) For My Own Use.
 *
 *
 * sagidayan@gmail.com
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "GenericHashTable.h"

#define _info(msg) printf("[ INFO ]\t%s\n", msg)
#define _fail(msg) printf("[\x1B[31m ERROR \x1B[0m]\t%s\n", msg)
#define _pass(msg) printf("[\x1B[32m *PASSED* \x1B[0m]\t%s\n", msg)
#define _title(msg) printf("\t*******************\n");\
    printf("\t  %s\n", msg);\
    printf("\t*******************\n\n");


void intTest();
void strTest();
void pointerTest();
void exTest();
void holdOn(char *);




int clean_stdin()
{
    while (getchar() != '\n');
    return 1;
}

int main(int argc, char const *argv[])
{
    //_debug("Starting...");

    int input;

menu:
    printf("\x1B[32m _____         _      _           _   \n");
    printf("|  |  |___ ___| |_   | |_ ___ ___| |_ \n");
    printf("|     | .'|_ -|   |  |  _| -_|_ -|  _|\n");
    printf("|__|__|__,|___|_|_|  |_| |___|___|_| \x1B[0m\n");

    printf("\n\n%s\n", "Please select your test of choice: ");
    printf("\t1. Integer Table Test\n");
    printf("\t%s\n", "2. String Table Test");
    printf("\t%s\n", "3. Pointer Validation Test (encapsulation test)");
    printf("\t%s\n", "4. Extreme Case Test" );
    printf("\t%s\n", "5. Exit" );

    while ((scanf("%d", &input) != 1 && clean_stdin()) || input > 5 || input < 1 )
    {
        printf("%s\n", "Invalid menu item. please pick again.");
    }


    switch (input)
    {
    case 1:
        intTest();
        break;
    case 2:
        strTest();
        break;
    case 3:
        pointerTest();
        break;
    case 4:
        exTest();
        break;
    default:
        printf("\t\t \x1b[34mThank You For Using FuckingTester (c) 2014 Sagi Dayan\x1B[0m\n\n\n\n");
        return 0;
    }

    printf("%s\n", "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    goto menu;
    return 0;
}


/**
 * Tests all integet Shit...
 * it will add data, remove data - multiply the array - and check your hashing.
 */
void intTest()
{
    _title("Integer Table Test");
    int arr[20];


    Table *tbl = createTable(2, INT_TYPE, 2);
    int i;
    for (i = 0; i < 20; i++)
        arr[i] = i;

    printTable(tbl);

    _info("Addint one int");
    add(tbl, &arr[0]);
    _info("Added\n");
    printTable(tbl);
    if (search(tbl, &arr[0]) == NULL)
        _fail("integer is not beeing plased in the right place!");
    else
        _pass("One element is right");

    _info("Atempting to remove the one object within the table...");
    if (removeObj(tbl, &arr[0]) != 0)
        _fail("Not right index return from removeObjet() function");
    else
        _pass("removeObject() func returns the right index");




    holdOn("Press Any Key To Continue...");

    _info("Filling table for multiply... 4 items\n");

    for (i = 0; i < 4; ++i)
        add(tbl, &arr[i]);

    printTable(tbl);

    holdOn("Continue ? ...");
    _info("Adding alot  more: ");

    for (; i < 20; ++i)
        add(tbl, &arr[i]);

    printTable(tbl);
    _info("Now Cheacking If Data Is In The Right Place");

    if (removeObj(tbl, &arr[0]) == 0 && removeObj(tbl, &arr[19]) == 9 && removeObj(tbl, &arr[8]) == 2)
        _pass("Cheacked 3 Out Of 20 Numbers - And They Are In The Right Place!");
    else
        _fail("Cheacked 3 Out Of 20 Numbers - And They Are NOT The Right Place!");

    _info("Checking if removed the objects correctly");

    if (removeObj(tbl, &arr[0]) == -1 && removeObj(tbl, &arr[19]) == -1 && removeObj(tbl, &arr[8]) == -1)
        _pass("Removed Successfully");
    else
        _fail("Something is wrong with removeObj() function");


    holdOn("End Of Test");


    freeTable(tbl);
    tbl = NULL;


}

void strTest()
{
    Table *tbl = createTable(3, STR_TYPE, 3);
    int i;
    if (tbl == NULL)
    {
        _fail("Table Was Not Created Successfully...");
        _info("Cant continue The Test.. Aborting..");
        holdOn("Press Any Key To Go To Menu");

    }
    _title("String Table Test");
    holdOn("Press Any Key To Continue");
    _info("Created a string table of size 3 and list length of 3");
    printTable(tbl);
    char *arr[5] = {"One", "Tow", "Tree", "pinnes", "boobs"};

    /**
     * First Test
     */
    _info("Trying to add the string \"One\" to the array... needs to go to index 2");
    add(tbl, arr[0]);
    printTable(tbl);
    if (removeObj(tbl, arr[0]) != 2)
        _fail("Index Is not right - check removeObj() OR add()");
    else
        _pass("Nice Job");
    holdOn("Press Any Key To Continue");

    /**
     * Second test
     */
    _info("Adding to table 5 strings...");
    for ( i = 0; i < 5; ++i)
        add(tbl, arr[i]);
    _info("Added. Your Array looks like this: ");
    printTable(tbl);
    _info("The String \"boobs\" should be in index 5");
    if (removeObj(tbl, arr[4]) != 5)
        _fail("Index Is not right - check removeObj() OR add()");
    else
        _pass("Nice Job");

    holdOn("Press Any Key To Continue");




    freeTable(tbl);
    holdOn("Done with String Test...");
}

void pointerTest()
{
    _title("Pointer Test");
    _title("INTEGER:");
    _info("creating table:");
    Table *tbl = createTable(2, INT_TYPE, 1);
    _info("done.");
    int integer = 5, testInt = 5;
    int *integerPtr = &integer;
    int *testPtr = &testInt;
    char *str = "test";
    printTable(tbl);
    _info("Addint the number 5 to table.");
    add(tbl, integerPtr);
    printTable(tbl);
    _info("Changing value of that integer from the main.");
    *integerPtr = 6;
    printTable(tbl);
    if (search(tbl, testPtr) == NULL)
        _fail("User can change values of your objects from the main");
    else
        _pass("Nice worck with integer encapusalation!");

    _info("Atempting to free the table");
    freeTable(tbl);
    tbl = NULL;
    _info("done!");

    holdOn("Press Any Key To Continue To char* Test");

    _title("STRING: ");

    _info("creating table");
    tbl = createTable(2, STR_TYPE, 1);
    _info("done.");
    printTable(tbl);
    _info("Adding string to table");
    add(tbl, str);
    printTable(tbl);

    _info("Changing the string from the main.");

    str = "test2";

    printTable(tbl);

    if (search(tbl, str) != NULL)
        _fail("User can change values of your objects from the main");
    else
        _pass("Nice worck with String encapusalation!");

    _info("Atempting to free the table");

    _info("Now Cheacking If Data Is In The Right Place");



    freeTable(tbl);
    tbl = NULL;

    _info("done!");

    holdOn("Press Any key To Continue");
}

void exTest()
{
    _title("Extreme Test");
    _info("Trying to create a table with 0 as the list size...");
    Table *tbl = createTable(1, INT_TYPE, 0);
    Table *tbl2 = createTable(3, STR_TYPE, 2);
    int answer;
    char *arr[5] = {"One", "Tow", "Tree", "pinnes", "boobs"};
    int arrInt[5];
    int i;
    for ( i = 0; i < 5; ++i)
        arrInt[i] = i;
    if (tbl != NULL)
        _fail("You cannot create a table with listLength of 0.");
    else
        _pass("Good job, you made sure that user cannot do that!");
    holdOn("To Continue press [Enter]");

    _info("Trying to add a string to an Integer table: ");
    tbl = createTable(2, INT_TYPE, 2);
    add(tbl, arr[0]);
    printTable(tbl);
    printf("[ ?? ]Answer This: Look at the table, is the only element %d, and it is stored in %d? [y/n]\n", (int)(*(int *)arr[0]), removeObj(tbl, arr[0]));
    answer = getchar();
    if (answer == 'y' || answer == 'Y')
        _pass("Nice Job handeling Strings in integer table!");
    else
        _fail("Somethig went wrong...");
    holdOn("Coninue? ");

    _info("Trying to add int to a string table");
    add(tbl2, &arrInt[3]);
    printTable(tbl2);
    printf("[ ?? ]Answer This: Look at the table, is the only element %s, and it is stored in %d? [y/n]\n", (char *)&arrInt[3], removeObj(tbl2, &arrInt[3]));
    answer = getchar();
    if (answer == 'y' || answer == 'Y')
        _pass("Nice Job handeling Strings in integer table!");
    else
        _fail("Something went wrong...");


    freeTable(tbl2);

    printf("\n\n\n");
    _info("Trying to add NULL to integer table");
    if (add(tbl, NULL) != -1)
        _fail("You need to return -1 (from add() ) if there is a null pointer");
    else
        _pass("Nice job! - add returns -1 as expected!");

    holdOn("Coninue? ");
    freeTable(tbl);

    holdOn("Done!");
}


void holdOn(char *msg)
{
    printf("\n\n*** %s ***\n", msg);
    clean_stdin();
    getchar();
}