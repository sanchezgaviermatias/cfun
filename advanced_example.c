// Fix the handleAddBook function
void handleAddBook()
{
    char title[MAX_TITLE], author[MAX_AUTHOR],
        isbn[MAX_ISBN], category[MAX_CATEGORY];
    int quantity;
    char input[10];

    printf("\n=== Add New Book ===\n");

    clearInputBuffer(); // Clear any leftover input

    printf("Enter ISBN: ");
    if (fgets(isbn, MAX_ISBN, stdin) != NULL)
    {
        isbn[strcspn(isbn, "\n")] = 0; // Remove newline
    }

    printf("Enter Title: ");
    if (fgets(title, MAX_TITLE, stdin) != NULL)
    {
        title[strcspn(title, "\n")] = 0;
    }

    printf("Enter Author: ");
    if (fgets(author, MAX_AUTHOR, stdin) != NULL)
    {
        author[strcspn(author, "\n")] = 0;
    }

    printf("Enter Category: ");
    if (fgets(category, MAX_CATEGORY, stdin) != NULL)
    {
        category[strcspn(category, "\n")] = 0;
    }

    printf("Enter Quantity: ");
    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        quantity = atoi(input);
    }
    else
    {
        quantity = 0;
    }

    // Validate inputs
    if (strlen(isbn) == 0 || strlen(title) == 0 ||
        strlen(author) == 0 || strlen(category) == 0 || quantity <= 0)
    {
        printf("Error: All fields are required and quantity must be positive.\n");
        stats.failedTransactions++;
        return;
    }

    Book *newBook = createBook(isbn, title, author, category, quantity);
    if (insertBook(newBook))
    {
        printf("Book added successfully.\n");
        stats.successfulTransactions++;
    }
    else
    {
        printf("Error: Book with ISBN already exists.\n");
        free(newBook);
        stats.failedTransactions++;
    }
    stats.totalTransactions++;
}

// Fix the main menu input handling
int main()
{
    initializeLibrary();
    stats.startTime = time(NULL);
    loadFromFile();

    char input[10];
    int choice;
    bool running = true;

    while (running)
    {
        printMainMenu();

        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            choice = atoi(input);
        }
        else
        {
            choice = 0;
        }

        switch (choice)
        {
        case 1:
            handleAddBook();
            break;
        case 2:
            handleSearchBook();
            break;
        case 3:
            handleBorrowBook();
            break;
        case 4:
            handleReturnBook();
            break;
        case 5:
            handleDeleteBook();
            break;
        case 6:
            generateDetailedReport();
            break;
        case 7:
            exportToCsv();
            break;
        case 8:
            saveToFile();
            break;
        case 9:
            running = false;
            printf("Saving and exiting...\n");
            break;
        default:
            printf("Invalid choice. Please enter a number between 1 and 9.\n");
        }
    }

    // Cleanup before exit
    saveToFile();
    // Free all allocated memory
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Book *current = library->table[i];
        while (current != NULL)
        {
            Book *temp = current;
            current = current->next;
            free(temp);
        }
    }
    deleteBST(titleIndex);
    free(library);
    pthread_mutex_destroy(&transaction_mutex);

    return 0;
}

// Improve the clearInputBuffer function
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Update the handleSearchBook function
void handleSearchBook()
{
    char query[MAX_BUFFER];
    char input[10];
    int searchType;

    printf("\n=== Search Books ===\n");
    printf("1. Title\n");
    printf("2. Author\n");
    printf("3. Category\n");
    printf("4. ISBN\n");
    printf("Enter choice: ");

    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        searchType = atoi(input);
    }
    else
    {
        searchType = 0;
    }

    if (searchType < 1 || searchType > 4)
    {
        printf("Invalid search type. Please try again.\n");
        return;
    }

    printf("Enter search term: ");
    if (fgets(query, MAX_BUFFER, stdin) != NULL)
    {
        query[strcspn(query, "\n")] = 0; // Remove newline
        searchBooks(query, searchType);
    }
}