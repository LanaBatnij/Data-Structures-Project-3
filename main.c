/*Lana Batnij 1200308 Section 1*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//maximum number of synonyms
#define MAX_SYNONYMS 3

//hash table size
#define TABLE_SIZE 150

//struct for each word entry
typedef struct {
    char word[50];
    char synonyms[MAX_SYNONYMS][150];
    int num_synonyms;
} WordEntry;

//struct for each hash table
typedef struct {
    WordEntry table[TABLE_SIZE];
     int num_elements;
} HashTable;

// Functions prototypes
void loadWordsFromFile(HashTable *hashTable);
int hashFunction(char *key, int tableSize);
int doubleHashFunction(char *key, int tableSize);
int insert(HashTable *hashTable, char *word, char *synonyms);
int search(HashTable *hashTable, char *word);
int deleteEntry(HashTable *hashTable, char *word);
void printTable(HashTable *hashTable);
void printStats(HashTable *hashTable, int collisions);
void saveToFile(HashTable *hashTable);
// -------------------------------------------------------------
int main() {
    //declare the used Hash Tables
    HashTable openAddressingTable, doubleHashingTable;
    int Choice, collisions = 0;

     // Initialize tables
     /*memset is function to initialize two tables
      and thats by:setting all the bytes in each table to zero */
    memset(&openAddressingTable, 0, sizeof(openAddressingTable));
    memset(&doubleHashingTable, 0, sizeof(doubleHashingTable));

    openAddressingTable.num_elements = 0;
    doubleHashingTable.num_elements = 0;

     int openAddressingCollisions = 0;
     int doubleHashingCollisions = 0;


    //Loading the words from the file
    loadWordsFromFile(&openAddressingTable);
    loadWordsFromFile(&doubleHashingTable);
    printf("               ------------------------------------\n");
    printf("               --------WELCOME TO PROJECT 3--------\n");
    printf("               -------------Dictionary-------------\n");
    printf("               ------------------------------------\n");
    do {
        // Display menu
        printf("\nOptions:\n");
        printf("1. Print hashed tables.\n");
        printf("2. Print out table size and the load factor.\n");
        printf("3. Print out the used hash functions.\n");
        printf("4. Insert a new record to hash table.\n");
        printf("5. Search for a specific word.\n");
        printf("6. Delete a specific word.\n");
        printf("7. Compare between the two methods in terms of collisions.\n");
        printf("8. Save hash table back to a file.\n");
        printf("9. Exit.\n");
        printf("Enter your choice: ");
        scanf("%d", &Choice);

        switch (Choice) {
            case 1:
                //Print the hash tables
                printf("\nOpen Addressing Hash Table:\n");
                printTable(&openAddressingTable);
                printf("\nDouble Hashing Hash Table:\n");
                printTable(&doubleHashingTable);
                break;
            case 2:
                //Print the table size and the Load factor and collisions
                printf("\nTable Size: %d\n", TABLE_SIZE);
                printf("Load Factor: %f\n", (float)(openAddressingTable.num_elements) / TABLE_SIZE);
                break;
            case 3:
                //Print the used hash Function
                printf("\nUsed hash functions:\n");
                printf("1. Basic Hashing (Open Addressing)\n");
                printf("-(hash MOD tableSize)\n");
                printf("2. Double Hashing\n");
                printf("-((hash1 +  hash2) MOD tableSize)\n");
                break;
            case 4: {
                //insert a word with its meanings to the tables
                char Line[200], newWord[50], synonyms[150];
                printf("\nEnter new word and synonyms (max %d synonyms separated by '#'): ", MAX_SYNONYMS);
                if (scanf(" %[^\n]", Line) == 1) {
                    char *token = strtok(Line, ":");
                    if (token != NULL) {
                        strncpy(newWord, token, sizeof(newWord) - 1);
                        newWord[sizeof(newWord) - 1] = '\0';
                        token = strtok(NULL, "\n");
                        if (token != NULL) {
                            strncpy(synonyms, token, sizeof(synonyms) - 1);
                            synonyms[sizeof(synonyms) - 1] = '\0';
                            openAddressingCollisions += insert(&openAddressingTable, newWord, synonyms);
                            doubleHashingCollisions += insert(&doubleHashingTable, newWord, synonyms);
                        } else {
                            printf("Invalid input format. Synonyms are missing.\n");
                        }
                    } else {
                        printf("Invalid input format. Word is missing.\n");
                    }
                } else {
                    printf("Failed to read input.\n");
                }
                break;
            }
            case 5: {
                //Search for a word
                char searchWord[50];
                printf("\nEnter word to search: ");
                scanf("%s", searchWord);
                int openAddressingResult = search(&openAddressingTable, searchWord);
                int doubleHashingResult = search(&doubleHashingTable, searchWord);
                if (openAddressingResult != -1)
                    printf("Open Addressing: Found at index %d\n", openAddressingResult);
                else
                    printf("Open Addressing: Not found\n");

                if (doubleHashingResult != -1)
                    printf("Double Hashing: Found at index %d\n", doubleHashingResult);
                else
                    printf("Double Hashing: Not found\n");
                break;
            }
            case 6: {
                //Delete a word and its meanings
                char deleteWord[50];
                printf("\nEnter word to delete: ");
                scanf("%s", deleteWord);
                deleteEntry(&openAddressingTable, deleteWord);
                deleteEntry(&doubleHashingTable, deleteWord);
                break;
            }
            case 7:
                //compare the collisions if happened
                printf("\nComparison of collisions:\n");
                printf("Open Addressing Collisions: %d\n", openAddressingCollisions);
                printf("Double Hashing Collisions: %d\n", doubleHashingCollisions);
                break;
            case 8:
                //save to file
                saveToFile(&doubleHashingTable);
                printf("\nHash table saved to file.\n");
                break;
            case 9:
                break;
            default:
                printf("\nInvalid Choice. Please try again.\n");
        }
    } while (Choice != 9);

    return 0;
}
// -------------------------------------------------------------
// Function to load words from file into hash table
void loadWordsFromFile(HashTable *hashTable) {
    FILE *file = fopen("Dir.txt", "r");
    if (file == NULL) {
        printf("Error opening file 'Dir.txt'\n");
        exit(1);
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *word = strtok(line, ":");
        char *synonyms = strtok(NULL, "\n");
        // Insert the word and synonyms into the hash table
        insert(hashTable, word, synonyms);
    }
    fclose(file);
}
// -------------------------------------------------------------
// Basic hash function for open addressing
/*Key is a pointer to a character array which
  is the word that will be hashed, hash is to
  iterates through each character at key and adds
  the ASCII value*/
int hashFunction(char *key, int tableSize) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i];
    }
    return hash % tableSize;
}
// -------------------------------------------------------------
// Double hash function for double hashing
/*Key is a  pointer to a character array that will
  representing the key that needs to be hashed.
  This combination of Hash 1 and hash 2
  attempts to find a new index in the hash table,
  incorporating both hash values.*/
int doubleHashFunction(char *key, int tableSize) {
    int hash1 = hashFunction(key, tableSize);
    int hash2 = 11 - (hashFunction(key, tableSize) % 11);
    return (hash1 +  hash2) % tableSize;
}
// -------------------------------------------------------------
// Function to insert a word and its synonyms
int insert(HashTable *hashTable, char *word, char *synonyms) {
    int index = hashFunction(word, TABLE_SIZE);
    int collisions = 0;
    //after calculated the hash,
    //a loop to handle collisions using quadratic probing
    while (hashTable->table[index].word[0] != '\0') {
        collisions++;

        if (collisions == TABLE_SIZE) {
            // Hash table is full
            printf("\nError: Hash table is full. Cannot insert '%s'.\n", word);
            return collisions;
        }
        //Quadratic Probing
        index = (index + collisions * collisions) % TABLE_SIZE;
    }
    // Copy the new word to the WordEntry
    strcpy(hashTable->table[index].word, word);
    // Reset synonyms before adding new ones
    for (int k = 0; k < MAX_SYNONYMS; k++) {
        hashTable->table[index].synonyms[k][0] = '\0';
    }
    // Token the synonyms using '#'
    char tempSynonyms[150];
    strncpy(tempSynonyms, synonyms, sizeof(tempSynonyms) - 1);
    tempSynonyms[sizeof(tempSynonyms) - 1] = '\0';
    char *token = strtok(tempSynonyms, "#");
    int j = 0;
    while (token != NULL && j < MAX_SYNONYMS) {
        strncpy(hashTable->table[index].synonyms[j], token, sizeof(hashTable->table[index].synonyms[j]) - 1);
        hashTable->table[index].synonyms[j][sizeof(hashTable->table[index].synonyms[j]) - 1] = '\0';  // Ensure null-terminated
        token = strtok(NULL, "#");
        j++;
    }
    hashTable->table[index].num_synonyms = j;
    // Update the number of elements
    hashTable->num_elements++;
    return collisions;
}
// -------------------------------------------------------------
// Search for a word in the hash table
int search(HashTable *hashTable, char *word) {
    int index = hashFunction(word, TABLE_SIZE);
    /*enters a loop that continues until an
    empty slot is found in the hash table */
    while (hashTable->table[index].word[0] != '\0') {
        //check if its aimed word
        if (strcmp(hashTable->table[index].word, word) == 0) {
            return index;
        }
        index = (index + 1) % TABLE_SIZE;
    }
    return -1; // Word not found
}
// -------------------------------------------------------------
// Delete an entry from the hash table
int deleteEntry(HashTable *hashTable, char *word) {
    int index = search(hashTable, word);
    if (index != -1) { //if the word id found
        // Clear the entire WordEntry
        hashTable->table[index].word[0] = '\0';
        for (int k = 0; k < MAX_SYNONYMS; k++) {
            hashTable->table[index].synonyms[k][0] = '\0';
        }
        printf("\nWord '%s' deleted.\n", word);
    } else {
        printf("\nWord '%s' not found.\n", word);
        return -1;  // Word not found, no need to continue
    }
    // Handle collisions if any
    int i = 1;
    int deletedIndex = index;
    while (1) {
        index = (deletedIndex + i * i) % TABLE_SIZE;
        if (hashTable->table[index].word[0] == '\0') {
            // Found an empty slot, stop searching
            break;
        }
        // Move the WordEntry to the deleted slot
        strcpy(hashTable->table[deletedIndex].word, hashTable->table[index].word);
        for (int k = 0; k < MAX_SYNONYMS; k++) {
            strcpy(hashTable->table[deletedIndex].synonyms[k], hashTable->table[index].synonyms[k]);
        }
        hashTable->table[deletedIndex].num_synonyms = hashTable->table[index].num_synonyms;
        // Clear the WordEntry from the current slot
        hashTable->table[index].word[0] = '\0';
        for (int k = 0; k < MAX_SYNONYMS; k++) {
            hashTable->table[index].synonyms[k][0] = '\0';
        }
        deletedIndex = index;
        i++;
    }
    return deletedIndex;
}
// -------------------------------------------------------------
// Print the entire hash table
void printTable(HashTable *hashTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("%d: %s - ", i, hashTable->table[i].word);
        for (int j = 0; j < hashTable->table[i].num_synonyms; j++) {
            printf("%s", hashTable->table[i].synonyms[j]);
            if (j < hashTable->table[i].num_synonyms - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }
}
// -------------------------------------------------------------
// Save hash table back to a file (double hashing method)
void saveToFile(HashTable *hashTable) {
    FILE *file = fopen("saved_words.txt", "w");
    for (int i = 0; i < TABLE_SIZE; i++) {
        fprintf(file, "%s:", hashTable->table[i].word);
        for (int j = 0; j < hashTable->table[i].num_synonyms; j++) {
            fprintf(file, "%s", hashTable->table[i].synonyms[j]);
            if (j < hashTable->table[i].num_synonyms - 1) {
                fprintf(file, "#");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
// -------------------------------------------------------------
