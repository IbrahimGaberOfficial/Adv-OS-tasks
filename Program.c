/**
 * Disk Scheduling Algorithms Implementation (Beautified Style)
 *
 * This program implements three disk scheduling algorithms:
 * 1. FCFS (First Come First Served)
 * 2. SCAN (Elevator algorithm) - with selectable direction
 * 3. C-SCAN (Circular SCAN) - with selectable direction
 *
 * It generates random cylinder requests for a disk with
 * 5,000 cylinders (0-4,999) and reports the total head movement
 * for each algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define CYLINDERS 5000 // Total number of cylinders (0-4999)
#define REQUESTS 5     // Number of requests to process

// Direction constants
#define DIRECTION_OUTWARD 0 // Moving toward higher cylinder numbers (0 -> 4999)
#define DIRECTION_INWARD 1  // Moving toward lower cylinder numbers (4999 -> 0)

// Function prototypes
void generateRequests(int requests[], int count);
int fcfs(int requests[], int count, int initialPosition);
int scan(int requests[], int count, int initialPosition, int direction);
int cscan(int requests[], int count, int initialPosition, int direction);

int main()
{
    int initialPosition;
    int direction;
    int requests[REQUESTS];
    int requestsCopy1[REQUESTS]; // For SCAN
    int requestsCopy2[REQUESTS]; // For C-SCAN

    // Read initial position from user
    printf("\n===== Disk Scheduling Simulation =====\n");
    printf("Enter initial head position (0-%d): ", CYLINDERS - 1);
    scanf("%d", &initialPosition);

    // Validate input
    if (initialPosition < 0 || initialPosition >= CYLINDERS)
    {
        printf("\nError: Initial position must be between 0 and %d\n", CYLINDERS - 1);
        return 1;
    }

    // Get direction from user
    printf("\nSelect initial direction of head movement:\n");
    printf("0 - Outward (toward higher cylinder numbers)\n");
    printf("1 - Inward (toward lower cylinder numbers)\n");
    printf("Choice: ");
    scanf("%d", &direction);

    // Validate direction
    if (direction != DIRECTION_OUTWARD && direction != DIRECTION_INWARD)
    {
        printf("\nError: Direction must be 0 (outward) or 1 (inward)\n");
        return 1;
    }

    // Convert direction to user-friendly string for display
    const char *directionStr = (direction == DIRECTION_OUTWARD) ? "Outward (toward higher cylinders)" : "Inward (toward lower cylinders)";

    // Seed the random number generator
    srand(time(NULL));

    // Generate random requests and make copies
    generateRequests(requests, REQUESTS);
    for (int i = 0; i < REQUESTS; i++)
    {
        requestsCopy1[i] = requests[i];
        requestsCopy2[i] = requests[i];
    }

    // Display sample of generated requests
    printf("\nFirst 10 cylinder requests: ");
    for (int i = 0; i < 10 && i < REQUESTS; i++)
    {
        printf("%d ", requests[i]);
    }
    printf("...\n");
    printf("Initial head position: %d\n", initialPosition);
    printf("Initial direction: %s\n\n", directionStr);

    // FCFS algorithm
    printf("\n========== FCFC ==========\n");
    printf("    Formula: Total = sum of |next - current|\n\n");

    int fcfsMovement = fcfs(requests, REQUESTS, initialPosition);
    printf("    [FCFS] Total head movement: %d cylinders\n", fcfsMovement);

    // SCAN algorithm
    printf("\n========== SCAN ==========\n");

    if (direction == DIRECTION_OUTWARD)
    {
        printf("    Formula: (max - initial) + (max - min)\n\n");
    }
    else
    {
        printf("    Formula: (initial - min) + (max - min)\n\n");
    }
    int scanMovement = scan(requestsCopy1, REQUESTS, initialPosition, direction);

    printf("    [SCAN] Total head movement: %d cylinders (Direction: %s)\n",
           scanMovement, directionStr);

    // C-SCAN algorithm
    printf("\n========== C-SCAN ==========\n");
    if (direction == DIRECTION_OUTWARD)
    {
        printf("    Formula: (max - initial) + max + (last_before_initial)\n\n");
    }
    else
    {
        printf("    Formula: (initial - min) + max + (first_after_initial)\n\n");
    }
    int cscanMovement = cscan(requestsCopy2, REQUESTS, initialPosition, direction);

    printf("    [C-SCAN] Total head movement: %d cylinders (Direction: %s)\n",
           cscanMovement, directionStr);

    return 0;
}

void generateRequests(int requests[], int count)
{
    for (int i = 0; i < count; i++)
    {
        requests[i] = rand() % CYLINDERS;
    }
}

int fcfs(int requests[], int count, int initialPosition)
{
    int totalMovement = 0, currentPosition = initialPosition;
    for (int i = 0; i < count; i++)
    {
        totalMovement += abs(requests[i] - currentPosition);
        currentPosition = requests[i];
    }
    return totalMovement;
}

int scan(int requests[], int count, int initialPosition, int direction)
{
    int totalMovement = 0, currentPosition = initialPosition;
    int *queue = malloc(count * sizeof(int));
    if (!queue)
    {
        printf("Memory error\n");
        exit(1);
    }

    for (int i = 0; i < count; i++)
        queue[i] = requests[i];

    // Sort the request queue
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++)
            if (queue[j] > queue[j + 1])
            {
                int tmp = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = tmp;
            }

    if (direction == DIRECTION_OUTWARD)
    {
        // Moving toward higher cylinder numbers (outward)
        int startIndex = 0;
        while (startIndex < count && queue[startIndex] < currentPosition)
            startIndex++;

        // First serve requests with higher cylinder numbers
        for (int i = startIndex; i < count; i++)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }

        // If we haven't reached the outermost cylinder, go there first before reversing
        if (currentPosition < CYLINDERS - 1)
        {
            totalMovement += (CYLINDERS - 1 - currentPosition);
            currentPosition = CYLINDERS - 1;
        }

        // Then reverse direction and serve requests with lower cylinder numbers
        for (int i = startIndex - 1; i >= 0; i--)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }
    }
    else
    {
        // Moving toward lower cylinder numbers (inward)
        int startIndex = count - 1;
        while (startIndex >= 0 && queue[startIndex] > currentPosition)
            startIndex--;

        // First serve requests with lower cylinder numbers
        for (int i = startIndex; i >= 0; i--)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }

        // If we haven't reached the innermost cylinder, go there first before reversing
        if (currentPosition > 0)
        {
            totalMovement += currentPosition;
            currentPosition = 0;
        }

        // Then reverse direction and serve requests with higher cylinder numbers
        for (int i = startIndex + 1; i < count; i++)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }
    }

    free(queue);
    return totalMovement;
}

int cscan(int requests[], int count, int initialPosition, int direction)
{
    int totalMovement = 0, currentPosition = initialPosition;
    int *queue = malloc(count * sizeof(int));
    if (!queue)
    {
        printf("Memory error\n");
        exit(1);
    }

    for (int i = 0; i < count; i++)
        queue[i] = requests[i];

    // Sort the request queue
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++)
            if (queue[j] > queue[j + 1])
            {
                int tmp = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = tmp;
            }

    if (direction == DIRECTION_OUTWARD)
    {
        // Moving toward higher cylinder numbers (outward)
        int startIndex = 0;
        while (startIndex < count && queue[startIndex] < currentPosition)
            startIndex++;

        // First serve requests with higher cylinder numbers
        for (int i = startIndex; i < count; i++)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }

        // If we haven't reached the outermost cylinder, go there
        if (currentPosition < CYLINDERS - 1)
        {
            totalMovement += (CYLINDERS - 1 - currentPosition);
            currentPosition = CYLINDERS - 1;
        }

        // Jump to the beginning (0) and continue from there
        totalMovement += CYLINDERS - 1; // Cost to jump from end to beginning
        currentPosition = 0;

        // Serve remaining requests (those with lower cylinder numbers than initial)
        for (int i = 0; i < startIndex; i++)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }
    }
    else
    {
        // Moving toward lower cylinder numbers (inward)
        int startIndex = count - 1;
        while (startIndex >= 0 && queue[startIndex] > currentPosition)
            startIndex--;

        // First serve requests with lower cylinder numbers
        for (int i = startIndex; i >= 0; i--)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }

        // If we haven't reached the innermost cylinder, go there
        if (currentPosition > 0)
        {
            totalMovement += currentPosition;
            currentPosition = 0;
        }

        // Jump to the end (CYLINDERS-1) and continue from there
        totalMovement += CYLINDERS - 1; // Cost to jump from beginning to end
        currentPosition = CYLINDERS - 1;

        // Serve remaining requests (those with higher cylinder numbers than initial)
        for (int i = count - 1; i > startIndex; i--)
        {
            totalMovement += abs(queue[i] - currentPosition);
            currentPosition = queue[i];
        }
    }

    free(queue);
    return totalMovement;
}