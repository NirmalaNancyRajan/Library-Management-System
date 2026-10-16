#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define MAX 100

struct Book {
    int id;
    char title[50];
    char author[50];
    int quantity;
    int issued;
};

struct Book books[MAX];
int count = 0;

/* ---------- Safe input helpers ---------- */

/* Reads one line into buf (newline stripped). Returns 0 on EOF/error. */
int readLine(char *buf, size_t size) {
    size_t len;
    int ch;

    if (fgets(buf, (int)size, stdin) == NULL)
        return 0;

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        /* Line was longer than the buffer: discard the rest */
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }
    return 1;
}

/* Prompts until a valid int is entered. Returns 0 only on EOF. */
int readInt(const char *prompt, int *out) {
    char line[64];
    char *end;
    long val;

    while (1) {
        printf("%s", prompt);
        if (!readLine(line, sizeof line))
            return 0;

        errno = 0;
        val = strtol(line, &end, 10);

        if (end == line || errno == ERANGE || val > INT_MAX || val < INT_MIN) {
            printf("Invalid number, please try again.\n");
            continue;
        }
        while (*end == ' ' || *end == '\t')
            end++;
        if (*end != '\0') {
            printf("Invalid number, please try again.\n");
            continue;
        }

        *out = (int)val;
        return 1;
    }
}

/* Prompts until a non-empty string is entered. Returns 0 only on EOF. */
int readText(const char *prompt, char *buf, size_t size) {
    while (1) {
        printf("%s", prompt);
        if (!readLine(buf, size))
            return 0;
        if (buf[0] != '\0')
            return 1;
        printf("Input cannot be empty.\n");
    }
}

/* ---------- Library functions ---------- */

// Find book using ID
int findBook(int id) {
    int i;

    for (i = 0; i < count; i++) {
        if (books[i].id == id)
            return i;
    }

    return -1;
}

// Add a new book
void addBook() {
    struct Book b;

    if (count >= MAX) {
        printf("\nLibrary storage is full!\n");
        return;
    }

    if (!readInt("\nEnter Book ID: ", &b.id)) return;

    if (findBook(b.id) != -1) {
        printf("Book ID already exists!\n");
        return;
    }

    if (!readText("Enter Book Title: ", b.title, sizeof b.title)) return;
    if (!readText("Enter Author Name: ", b.author, sizeof b.author)) return;
    if (!readInt("Enter Quantity: ", &b.quantity)) return;

    if (b.quantity < 0) {
        printf("Invalid quantity!\n");
        return;
    }

    b.issued = 0;
    books[count++] = b;   /* only stored after all input is validated */

    printf("\nBook added successfully!\n");
}

// Display all books
void displayBooks() {
    int i;

    if (count == 0) {
        printf("\nNo books available in the library.\n");
        return;
    }

    printf("\n========== BOOK DETAILS ==========\n");
    printf("%-8s %-25s %-20s %-10s %-10s\n",
           "ID", "Title", "Author", "Available", "Issued");

    for (i = 0; i < count; i++) {
        printf("%-8d %-25.25s %-20.20s %-10d %-10d\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].quantity - books[i].issued,
               books[i].issued);
    }
}

// Search for a book
void searchBook() {
    int id, index;

    if (!readInt("\nEnter Book ID to search: ", &id)) return;

    index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
    } else {
        printf("\nBook ID: %d\n", books[index].id);
        printf("Title: %s\n", books[index].title);
        printf("Author: %s\n", books[index].author);
        printf("Available Copies: %d\n",
               books[index].quantity - books[index].issued);
        printf("Issued Copies: %d\n", books[index].issued);
    }
}

// Issue a book
void issueBook() {
    int id, index;

    if (!readInt("\nEnter Book ID to issue: ", &id)) return;

    index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
    } else if (books[index].issued >= books[index].quantity) {
        printf("No copies available for issue!\n");
    } else {
        books[index].issued++;
        printf("Book issued successfully!\n");
    }
}

// Return a book
void returnBook() {
    int id, index;

    if (!readInt("\nEnter Book ID to return: ", &id)) return;

    index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
    } else if (books[index].issued == 0) {
        printf("No copies of this book are currently issued!\n");
    } else {
        books[index].issued--;
        printf("Book returned successfully!\n");
    }
}

// Update book quantity
void updateBook() {
    int id, index, quantity;

    if (!readInt("\nEnter Book ID to update: ", &id)) return;

    index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
        return;
    }

    printf("Current total quantity: %d\n", books[index].quantity);

    if (!readInt("Enter new total quantity: ", &quantity)) return;

    /* Check negative first so the error message is accurate */
    if (quantity < 0) {
        printf("Invalid quantity!\n");
        return;
    }

    if (quantity < books[index].issued) {
        printf("Quantity cannot be less than issued copies!\n");
        return;
    }

    books[index].quantity = quantity;
    printf("Book quantity updated successfully!\n");
}

// Remove a book
void removeBook() {
    int id, index, i;

    if (!readInt("\nEnter Book ID to remove: ", &id)) return;

    index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
        return;
    }

    if (books[index].issued > 0) {
        printf("Cannot remove a book with issued copies!\n");
        return;
    }

    for (i = index; i < count - 1; i++) {
        books[i] = books[i + 1];
    }

    count--;

    printf("Book removed successfully!\n");
}

// Main menu
int main() {
    int choice;

    do {
        printf("\n\n====== LIBRARY MANAGEMENT SYSTEM ======\n");
        printf("1. Add Book\n");
        printf("2. Display All Books\n");
        printf("3. Search Book\n");
        printf("4. Update Book Quantity\n");
        printf("5. Remove Book\n");
        printf("6. Issue Book\n");
        printf("7. Return Book\n");
        printf("8. Exit\n");

        if (!readInt("Enter your choice: ", &choice)) {
            printf("\nInput ended. Exiting.\n");
            return 0;
        }

        switch (choice) {
            case 1: addBook();      break;
            case 2: displayBooks(); break;
            case 3: searchBook();   break;
            case 4: updateBook();   break;
            case 5: removeBook();   break;
            case 6: issueBook();    break;
            case 7: returnBook();   break;
            case 8:
                printf("\nExiting Library Management System.\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 8);

    return 0;
}