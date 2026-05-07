/*
 Michael Million, Eric Garcia, Jesse Garcia, Nam Vu, Tony Ngo
 ECE131L-003
 Final Project: Library Catalogue (ECE131 Notes Catalogue)
 
 This program acts as a library catalogue repurposed for our ECE131L notes to study. It features functions that let the user
 check in and out books/topics, search through available content, and add and remove books/concepts from the catalogue.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Defines a maximum number of topics to 50 and max amount of characters stored in each topic struct.
#define MAX_TOPICS 50
#define MAX_NOTES 5000

typedef struct {
    char topic[50];
    char synopsis[1200];
    char notes[MAX_NOTES]; // holds the notes content loaded from the file
    int availability;
} Topic;

// Failsafe: Flushes leftover garbage out of the input buffer after a misused scanf
void flushInput() {
    while (getchar() != '\n');
}

// Failsafe: Reads an integer from the user, keeps asking if they type non-numbers
int readInt(const char *prompt) {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("%s", prompt);
        flushInput();
    }
    return value;
}

// Builds a tag like "[DATA TYPES]" from a topic name like "Data Types"
void buildTag(const char *topicName, char *tag, int tagSize) {
    snprintf(tag, tagSize, "[");
    for (int j = 0; topicName[j]; j++) {
        char upper[2] = { toupper(topicName[j]), '\0' };
        strncat(tag, upper, tagSize - strlen(tag) - 1);
    }
    strncat(tag, "]", tagSize - strlen(tag) - 1);
}

// Searches notes.txt for [TOPIC NAME] and reads until [END], loads it into the struct
void loadNotes(Topic topArray[], int length) {
    FILE *file = fopen("notes.txt", "r");
    if (file == NULL) {
        printf("Warning: could not open notes.txt. Full descriptions will be unavailable.\n");
        return;
    }

    char line[2048];
    char tag[60];

    for (int i = 0; i < length; i++) {
        // Build the tag we're looking for, e.g. "[DATA TYPES]"
        buildTag(topArray[i].topic, tag, sizeof(tag));

        // Rewind to search from the top each time
        rewind(file);
        topArray[i].notes[0] = '\0'; // clear it out before loading

        int found = 0;
        while (fgets(line, sizeof(line), file)) {
            // Strip the newline for clean tag comparison
            line[strcspn(line, "\r\n")] = '\0';
            if (strcmp(line, tag) == 0) {
                found = 1;
                break;
            }
        }

        if (found) {
            // Read everything until we hit [END] and store it
            while (fgets(line, sizeof(line), file)) {
                line[strcspn(line, "\r\n")] = '\0';
                if (strcmp(line, "[END]") == 0) break;
                strncat(topArray[i].notes, line, MAX_NOTES - strlen(topArray[i].notes) - 2);
                strcat(topArray[i].notes, "\n");
            }
        }
    }

    fclose(file);
}

// Rewrites notes.txt from scratch using whatever is currently in the structs
void saveNotes(Topic topArray[], int length) {
    FILE *file = fopen("notes.txt", "w");
    if (file == NULL) {
        printf("Error: could not save notes.txt.\n");
        return;
    }

    char tag[60];
    for (int i = 0; i < length; i++) {
        // Build and write the tag, then the notes content, then [END]
        buildTag(topArray[i].topic, tag, sizeof(tag));
        fprintf(file, "%s\n", tag);
        fprintf(file, "%s", topArray[i].notes);
        fprintf(file, "[END]\n\n");
    }

    fclose(file);
}

// Prints the notes content stored in the struct to the user
void printSection(Topic t) {
    if (strlen(t.notes) == 0) {
        printf("\nNo notes found for \"%s\". Make sure notes.txt has a [%s] section.\n", t.topic, t.topic);
        return;
    }
    printf("\n--- %s ---\n", t.topic);
    printf("%s\n", t.notes);
}

// Lets the user pick a topic and view its synopsis or full description
void search(Topic topArray[], int length) {
    char synSelect, descSelect;

    // Print the list of topics for the user to choose from
    printf("\nPlease select which topic you would like to search:\n");
    for (int i = 0; i < length; i++) {
        printf("%d. %s\n", i+1, topArray[i].topic);
    }

    // Failsafe: Keep asking until we get a valid number in range
    int topSelect = readInt("Invalid input, please enter a number: ");
    while (topSelect < 1 || topSelect > length) {
        printf("Invalid selection. Please enter a number between 1 and %d: ", length);
        topSelect = readInt("Invalid input, please enter a number: ");
    }

    // Ask if they want a synopsis, full description, or nothing
    printf("\nYou've selected \"%s\". Enter \"S\" for a synopsis.\nWould you like to open the topic description? (Y or N)\n", topArray[(topSelect-1)].topic);
    scanf(" %c", &synSelect);

    // Failsafe: Reject anything that isn't Y, N, or S
    while (synSelect != 'Y' && synSelect != 'N' && synSelect != 'y' && synSelect != 'n' && synSelect != 's' && synSelect != 'S') {
        printf("Invalid input, please enter \"Y\" or \"N\" to open the topic description, or \"S\" for a synopsis\n");
        scanf(" %c", &synSelect);
    }

    if (synSelect == 's' || synSelect == 'S') {
        // Print the synopsis and then ask if they want the full notes too
        printf("\n%s\n\n", topArray[(topSelect-1)].synopsis);
        printf("Would you like to access the full topic description? (Y or N)\n");
        scanf(" %c", &descSelect);

        // Validate the follow-up answer
        while (descSelect != 'Y' && descSelect != 'N' && descSelect != 'y' && descSelect != 'n') {
            printf("Invalid input, please enter \"Y\" or \"N\"\n");
            scanf(" %c", &descSelect);
        }
        if (descSelect == 'Y' || descSelect == 'y') {
            // Pull and print the matching section from notes.txt
            printSection(topArray[topSelect-1]);
        }
    } else if (synSelect == 'Y' || synSelect == 'y') {
        // Skip straight to the full notes from the file
        printSection(topArray[topSelect-1]);
    } else if (synSelect == 'N' || synSelect == 'n') {
        // User didn't want anything, just fall back to the main menu
    }
}

// Prints every topic along with it's availability status
void availability(Topic topArray[], int length) {
    printf("\n--- Topic Availability ---\n");
    for (int i = 0; i < length; i++) {
        if (topArray[i].availability == 0) {
            printf("%d. %-20s [Available]\n", i+1, topArray[i].topic);
        } else {
            printf("%d. %-20s [Checked Out]\n", i+1, topArray[i].topic);
        }
    }
    printf("--------------------------\n");
}

// Lets the user check out a topic if it isn't already taken
void checkout(Topic topArray[], int length) {
    printf("\n--- Check Out ---\n");
    printf("Please select a topic to check out:\n");

    // Show all available topics so the user knows what's available before picking
    for (int i = 0; i < length; i++) {
        if (topArray[i].availability == 0) {
            printf("%d. %-20s [Available]\n", i+1, topArray[i].topic);
        } else {
            printf("%d. %-20s [Checked Out]\n", i+1, topArray[i].topic);
        }
    }

    // Failsafe: Keep asking until we get a valid number in range
    int topSelect = readInt("Invalid input, please enter a number: ");
    while (topSelect < 1 || topSelect > length) {
        printf("Invalid selection. Please enter a number between 1 and %d: ", length);
        topSelect = readInt("Invalid input, please enter a number: ");
    }

    // Failsafe: Block the checkout if someone already has it
    if (topArray[topSelect-1].availability == 1) {
        printf("\nSorry, \"%s\" is already checked out and unavailable.\n", topArray[topSelect-1].topic);
    } else {
        // Mark it as checked out and confirm to the user
        topArray[topSelect-1].availability = 1;
        printf("\nYou have successfully checked out \"%s\".\n", topArray[topSelect-1].topic);
    }
}

// Lets the user return a topic they previously checked out
void checkin(Topic topArray[], int length) {
    int anyCheckedOut = 0;

    printf("\n--- Check In ---\n");
    printf("Currently checked out topics:\n");

    // Only show topics that are currently checked out
    for (int i = 0; i < length; i++) {
        if (topArray[i].availability == 1) {
            printf("%d. %s\n", i+1, topArray[i].topic);
            anyCheckedOut = 1;
        }
    }

    // Failsafe: return early if nothing is checked out
    if (!anyCheckedOut) {
        printf("No topics are currently checked out.\n");
        return;
    }

    // Failsafe: Keep asking until we get a valid number in range
    printf("\nEnter the number of the topic you would like to return: ");
    int topSelect = readInt("Invalid input, please enter a number: ");
    while (topSelect < 1 || topSelect > length) {
        printf("Invalid selection. Please enter a number between 1 and %d: ", length);
        topSelect = readInt("Invalid input, please enter a number: ");
    }

    // Failsafe: Make sure the chosen topic is actually checked out before returning it
    if (topArray[topSelect-1].availability == 0) {
        printf("\n\"%s\" is not currently checked out.\n", topArray[topSelect-1].topic);
    } else {
        // Mark it as available again and let the user know it went through
        topArray[topSelect-1].availability = 0;
        printf("\nYou have successfully returned \"%s\".\n", topArray[topSelect-1].topic);
    }
}

// Adds a new topic to the array and saves it to the file
void addEntry(Topic topArray[], int *length) {
    if (*length >= MAX_TOPICS) {
        printf("Catalogue is full, cannot add more topics.\n");
        return;
    }

    Topic newTopic;
    newTopic.availability = 0;
    newTopic.notes[0] = '\0';

    // Clear input buffer before reading strings
    flushInput();

    printf("\nEnter the topic name: ");
    fgets(newTopic.topic, sizeof(newTopic.topic), stdin);
    newTopic.topic[strcspn(newTopic.topic, "\n")] = '\0';

    printf("Enter the synopsis: ");
    fgets(newTopic.synopsis, sizeof(newTopic.synopsis), stdin);
    newTopic.synopsis[strcspn(newTopic.synopsis, "\n")] = '\0';

    // Let the user type notes line by line. Typing "END" in a new line will complete
    printf("Enter the full notes (type END on a new line when done):\n");
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        strncat(newTopic.notes, line, MAX_NOTES - strlen(newTopic.notes) - 2);
        strcat(newTopic.notes, "\n");
    }

    // Add to the array and increment the count
    topArray[*length] = newTopic;
    (*length)++;

    // Rewrite the file so the new section gets added
    saveNotes(topArray, *length);
    printf("\nTopic \"%s\" has been added successfully.\n", newTopic.topic);
}

// Removes a topic from the array and rewrites the file without it
void removeEntry(Topic topArray[], int *length) {
    printf("\n--- Remove Entry ---\n");
    printf("Select a topic to remove:\n");

    for (int i = 0; i < *length; i++) {
        printf("%d. %s\n", i+1, topArray[i].topic);
    }

    // Keep asking until we get a valid number in range
    int topSelect = readInt("Invalid input, please enter a number: ");
    while (topSelect < 1 || topSelect > *length) {
        printf("Invalid selection. Please enter a number between 1 and %d: ", *length);
        topSelect = readInt("Invalid input, please enter a number: ");
    }

    // Confirm before deleting since this can't be undone
    char confirm;
    printf("Are you sure you want to remove \"%s\"? This cannot be undone. (Y or N): ", topArray[topSelect-1].topic);
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("Removal cancelled.\n");
        return;
    }

    printf("\nRemoving \"%s\"...\n", topArray[topSelect-1].topic);

    // Shift everything after the deleted entry one spot to the left to fill the gap
    for (int i = topSelect - 1; i < *length - 1; i++) {
        topArray[i] = topArray[i + 1];
    }
    (*length)--;

    // Rewrite the file without the removed entry
    saveNotes(topArray, *length);
    printf("Topic removed successfully.\n");
}

int main() {

    // Each topic has a name, synopsis, notes loaded from file, and availability status
    Topic topArray[MAX_TOPICS] = {
        {"Data Types", "The data types section focuses on data types and how variables store and manipulate data in C. Data types such as integers, floating-point numbers, doubles, characters, and Booleans define what kind of values can be stored and what operations can be executed.", "", 0},
        {"Loops", "The \"Loops\" section introduces loops and control structures, which are important for controlling program flow and repeating tasks efficiently. The three main loop types are for, while, and do-while. Each serves a different purpose depending on whether the number of iterations is known or condition-based.", "", 0},
        {"Arrays", "The \"Arrays\" section focuses on arrays, which is a data structure in C used to store multiple values of the same data type under a single variable.", "", 0},
        {"Functions", "The \"Functions\" section focuses on functions in C; their purpose, structure, and how they improve organization. Functions allow programs to be broken into manageable parts, making code more readable, reusable, and easier to maintain.", "", 0},
        {"Pointers", "The \"Pointers\" section focuses on pointers in C and how they are used to access and manipulate memory. A pointer is a variable that stores the address of another variable, allowing programs to work with memory more effectively.", "", 0},
        {"Structures", "The \"Structures\" section introduces structures in C, which are user-defined data types that allow grouping variables of different types under a single named variable.", "", 0},
        {"Strings", "The \"Strings\" section focuses on strings in C and how they are stored, manipulated, and handled in memory. In C, strings are arrays of characters terminated by a null character.", "", 0}
    };

    int length = 7; // tracks actual number of topics in use

    // Load notes content from the file into each struct on startup
    loadNotes(topArray, length);

    int opSelect = 0;
    printf("Welcome to the ECE131L notes catalogue.\n");

    // Keep the menu running until the user chooses to exit
    while (opSelect != 7) {
        printf("\nPlease enter an operation number:\n1. Search \n2. Availability\n3. Check out\n4. Check in\n5. Add\n6. Remove\n7. Exit\n");
        opSelect = readInt("Invalid input, please enter a number: ");
        switch (opSelect) {
            case 1:
                search(topArray, length);
                break;
            case 2:
                availability(topArray, length);
                break;
            case 3:
                checkout(topArray, length);
                break;
            case 4:
                checkin(topArray, length);
                break;
            case 5:
                addEntry(topArray, &length);
                break;
            case 6:
                removeEntry(topArray, &length);
                break;
            case 7:
                printf("Thank you for using the ECE131L Notes Catalogue. See you soon.\n");
                break;
            default:
                printf("Invalid option, please enter a number between 1 and 7.\n");
                break;
        }
    }
    return 0;
}
