#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VOTERS 100
#define MAX_CANDIDATES 10
#define MAX_VOTES 10

typedef struct {
    int voterId;
    char username[50];
    char password[50];
    char srn[50];
} Voter;

typedef struct {
    int candidateId;
    char name[50];
    char party[50];
} Candidate;

typedef struct {
    int candidateId;
    int votes;
} VoteTally;

unsigned long hashFunction(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

void registerUser(Voter *voters, int *numVoters) {
    if (*numVoters >= MAX_VOTERS) {
        printf("Error: Maximum number of voters reached.\n");
        return;
    }

    Voter newVoter;
    printf("Enter username: ");
    scanf("%49s", newVoter.username);

    for (int i = 0; i < *numVoters; i++) {
        if (strcmp(voters[i].username, newVoter.username) == 0) {
            printf("Error: Username already exists. Please choose another username.\n");
            return;
        }
    }

    printf("Enter password: ");
    scanf("%49s", newVoter.password);

    printf("Enter SRN: ");
    scanf("%49s", newVoter.srn);

    newVoter.voterId = *numVoters + 1;

    voters[*numVoters] = newVoter;
    (*numVoters)++;

    FILE *userFile = fopen("users.txt", "a");
    if (userFile == NULL) {
        printf("Error: Unable to open user file.\n");
        return;
    }

    fprintf(userFile, "%d %s %s %s\n", newVoter.voterId, newVoter.username, newVoter.password, newVoter.srn);
    fclose(userFile);

    printf("User registered successfully!\n");
}
// Function for user authentication
int authenticateUser(Voter *voters, int numVoters, char *username, char *password) {
    // Check if the provided username and password match any registered user
    for (int i = 0; i < numVoters; i++) {
        if (strcmp(voters[i].username, username) == 0 && strcmp(voters[i].password, password) == 0) {
            // Authentication successful
            printf("Authentication successful. Welcome, %s!\n", username);
            return voters[i].voterId;
        }
    }

    // Authentication failed
    printf("Authentication failed. Invalid username or password.\n");
    return 0;
}

// Function to cast a vote
void castVote(Voter *voters, int numVoters, Candidate *candidates, int numCandidates, VoteTally *tallies, int *numVotes) {
    // Check if there are registered voters and candidates
    if (numVoters == 0 || numCandidates == 0) {
        printf("Error: No registered voters or candidates available.\n");
        return;
    }

    // Collect voter's username and password for authentication
    char inputUsername[50];
    char inputPassword[50];

    printf("Enter your username: ");
    scanf("%49s", inputUsername);

    printf("Enter your password: ");
    scanf("%49s", inputPassword);

    // Authenticate the user
    int voterId = authenticateUser(voters, numVoters, inputUsername, inputPassword);

    // If authentication fails, exit the function
    if (voterId == 0) {
        printf("Vote casting failed. Authentication unsuccessful.\n");
        return;
    }

    // Display the list of candidates
    printf("List of Candidates:\n");
    for (int i = 0; i < numCandidates; i++) {
        printf("%d. %s (%s)\n", candidates[i].candidateId, candidates[i].name, candidates[i].party);
    }

    // Collect the candidate ID for the vote
    int selectedCandidateId;
    printf("Enter the candidate ID you want to vote for: ");
    scanf("%d", &selectedCandidateId);

    // Validate the selected candidate ID
    int validCandidate = 0;
    for (int i = 0; i < numCandidates; i++) {
        if (candidates[i].candidateId == selectedCandidateId) {
            validCandidate = 1;
            break;
        }
    }

    // If the selected candidate is valid, cast the vote
    if (validCandidate) {
        // Update the vote tally
        int candidateFound = 0;
        for (int i = 0; i < *numVotes; i++) {
            if (tallies[i].candidateId == selectedCandidateId) {
                tallies[i].votes++;
                candidateFound = 1;
                break;
            }
        }

        // If the candidate is not found in the tally, add a new entry
        if (!candidateFound) {
            tallies[*numVotes].candidateId = selectedCandidateId;
            tallies[*numVotes].votes = 1;
            (*numVotes)++;
        }

        printf("Vote cast successfully for Candidate %d!\n", selectedCandidateId);
    } else {
        printf("Invalid candidate ID. Vote casting failed.\n");
    }
}

// Function to register a new candidate with party information
void registerCandidate(Candidate *candidates, int *numCandidates) {
    // Ensure there is space for a new candidate
    if (*numCandidates >= 10) {
        printf("Error: Maximum number of candidates reached.\n");
        return;
    }

    // Collect candidate information
    Candidate newCandidate;
    printf("Enter candidate name: ");
    scanf("%49s", newCandidate.name);

    printf("Enter candidate party: ");
    scanf("%49s", newCandidate.party);

    // Generate a unique candidateId
    newCandidate.candidateId = *numCandidates + 1;

    // Add the new candidate to the list of candidates
    candidates[*numCandidates] = newCandidate;
    (*numCandidates)++;

    // Store candidate details in a file
    FILE *candidateFile = fopen("candidates.txt", "a");
    if (candidateFile == NULL) {
        printf("Error: Unable to open candidates file.\n");
        return;
    }

    fprintf(candidateFile, "%d %s %s\n", newCandidate.candidateId, newCandidate.name, newCandidate.party);
    fclose(candidateFile);

    printf("Candidate registered successfully!\n");
}

// Function to display election results
void displayResults(Candidate *candidates, int numCandidates, VoteTally *tallies, int numVotes) {
    printf("\nFinal Vote Tallies:\n");
    for (int i = 0; i < numVotes; i++) {
        for (int j = 0; j < numCandidates; j++) {
            if (tallies[i].candidateId == candidates[j].candidateId) {
                printf("Candidate %d (%s, %s): %d votes\n", tallies[i].candidateId, candidates[j].name, candidates[j].party, tallies[i].votes);
                break;
            }
        }
    }
}

// Function to display data from "candidates.txt"
void displayCandidateData() {
    printf("\nData from candidates.txt:\n");
    FILE *candidateFile = fopen("candidates.txt", "r");
    if (candidateFile != NULL) {
        int candidateId;
        char name[50];
        char party[50];

        while (fscanf(candidateFile, "%d %s %s", &candidateId, name, party) != EOF) {
            printf("Candidate %d: %s (%s)\n", candidateId, name, party);
        }

        fclose(candidateFile);
    } else {
        printf("Error: Unable to open candidates file.\n");
    }
}

// Function to display user details from "users.txt" with password verification
void displayUserDetails() {
    char inputPassword[50];

    printf("Enter the password to display user details: ");
    scanf("%49s", inputPassword);

    // Hardcoded password for verification
    const char *correctPassword = "PESU_UNIVERSITY";

    if (strcmp(inputPassword, correctPassword) == 0) {
        printf("\nUser Details from users.txt:\n");

        FILE *userFile = fopen("users.txt", "r");
        if (userFile != NULL) {
            int voterId;
            char username[50];
            char password[50];
            char srn[50];

            while (fscanf(userFile, "%d %s %s %s", &voterId, username, password, srn) != EOF) {
                printf("Voter ID: %d\nUsername: %s\nPassword: %s\nSRN: %s\n\n", voterId, username, password, srn);
            }

            fclose(userFile);
        } else {
            printf("Error: Unable to open users file.\n");
        }
    } else {
        printf("Incorrect password. User details cannot be displayed.\n");
    }
}

// Other functions remain the same...

int main() {
    Voter voters[MAX_VOTERS];
    Candidate candidates[MAX_CANDIDATES];
    VoteTally tallies[MAX_VOTES];

    int numVoters = 0;
    int numCandidates = 0;
    int numVotes = 0;

    FILE *userFile = fopen("users.txt", "r");
    if (userFile != NULL) {
        while (fscanf(userFile, "%d %s %s %s", &voters[numVoters].voterId, voters[numVoters].username, voters[numVoters].password, voters[numVoters].srn) != EOF) {
            numVoters++;
        }
        fclose(userFile);
    }

    FILE *candidateFile = fopen("candidates.txt", "r");
    if (candidateFile != NULL) {
        while (fscanf(candidateFile, "%d %s %s", &candidates[numCandidates].candidateId, candidates[numCandidates].name, candidates[numCandidates].party) != EOF) {
            numCandidates++;
        }
        fclose(candidateFile);
    }

    int choice;

    do {printf("\n1. Register User\n");
        printf("2. Authenticate User and Cast Vote\n");
        printf("3. Register Candidate\n");
        printf("4. Display Election Results\n");
        printf("5. Display Candidate Data\n");
        printf("6. Display User Details\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser(voters, &numVoters);
                break;

            case 2:
                castVote(voters, numVoters, candidates, numCandidates, tallies, &numVotes);
                break;

            case 3:
                registerCandidate(candidates, &numCandidates);
                break;

            case 4:
                displayResults(candidates, numCandidates, tallies, numVotes);
                break;

            case 5:
                displayCandidateData();
                break;

            case 6:
                displayUserDetails();
                break;

            case 7:
                printf("Exiting the program.\n");
                break;

            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }


    } while (choice != 7);

    return 0;
}