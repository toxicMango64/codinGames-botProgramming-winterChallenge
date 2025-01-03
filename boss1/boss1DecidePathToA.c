#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WALL 'W'
#define ROOT 'R'
#define BASIC 'B'
#define A_PROTEIN 'A'
#define EMPTY '.'

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int width;                // columns in the game grid
    int height;               // rows in the game grid
    int entity_count;         // number of entities in the game
    struct {
        int x;                // grid coordinate x
        int y;                // grid coordinate y
        char type[33];        // type of the entity
        int owner;            // 1 if your organ, 0 if enemy organ, -1 if neither
        int organ_id;         // id of this entity if it's an organ, 0 otherwise
        char organ_dir[2];    // N, E, S, W or X if not an organ
        int organ_parent_id;   // parent id of the organ
        int organ_root_id;     // root id of the organ
    } entities[100];          // assuming a maximum of 100 entities
    int my_proteins[4];       // your protein stock: myA, myB, myC, myD
    int opp_proteins[4];      // opponent's protein stock: oppA, oppB, oppC, oppD
    int required_actions_count; // your number of organisms, output an action for each one in any order
} GameState;

/* #############  PRINT_MAP ################################################### */

// Function to print the current state of the game map
void print_map(GameState *gameState) {
    char grid[gameState->height][gameState->width];

    // Initialize the grid with empty spaces
    for (int i = 0; i < gameState->height; i++) {
        for (int j = 0; j < gameState->width; j++) {
            grid[i][j] = EMPTY; // Default empty space
        }
    }

    // Place entities on the grid
    for (int i = 0; i < gameState->entity_count; i++) {
        int x = gameState->entities[i].x;
        int y = gameState->entities[i].y;
        if (x >= 0 && x < gameState->width && y >= 0 && y < gameState->height) {
            if (strcmp(gameState->entities[i].type, "WALL") == 0) {
                grid[y][x] = WALL; // Represent WALL
            } else if (strcmp(gameState->entities[i].type, "ROOT") == 0) {
                grid[y][x] = ROOT; // Represent ROOT
            } else if (strcmp(gameState->entities[i].type, "BASIC") == 0) {
                grid[y][x] = BASIC; // Represent BASIC
            } else if (strcmp(gameState->entities[i].type, "A") == 0) {
                grid[y][x] = A_PROTEIN; // Represent A protein source
            }
        }
    }

    // Print the grid to stderr
    for (int i = 0; i < gameState->height; i++) {
        for (int j = 0; j < gameState->width; j++) {
            fprintf(stderr, "%c ", grid[i][j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

/* ################################################################################# */

// Directions for moving in the grid (N, E, S, W)
int directions[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
char direction_chars[4] = {'N', 'E', 'S', 'W'};

// Function to check if a point is within the grid bounds
bool is_within_bounds(int x, int y, GameState *gameState) {
    return (x >= 0 && x < gameState->width && y >= 0 && y < gameState->height);
}

// Function to print the GROW command
void print_grow_command(int parent_id, int x, int y) {
    printf("GROW %d %d %d BASIC\n", parent_id, x, y);
}

// Function to find the A protein source using BFS and return the path
Point find_a_protein(GameState *gameState, int start_x, int start_y, Point parent[][gameState->width]) {
    // Queue for BFS
    Point queue[1000]; // Assuming a maximum of 1000 points in the queue
    int front = 0, rear = 0;

    // Visited array to keep track of visited positions
    bool visited[gameState->height][gameState->width];
    memset(visited, false, sizeof(visited));

    // Start from the initial position
    queue[rear++] = (Point){start_x, start_y};
    visited[start_y][start_x] = true;

    // Initialize parent array
    memset(parent, -1, sizeof(Point) * gameState->height * gameState->width);

    while (front < rear) {
        Point current = queue[front++];

        // Check if the current position is an A protein source
        for (int i = 0; i < gameState->entity_count; i++) {
            if (gameState->entities[i].x == current.x && gameState->entities[i].y == current.y &&
                strcmp(gameState->entities[i].type, "A") == 0) {
                // Return the position of the A protein source
                return current; // Return the position of the A protein source
            }
        }

        // Explore adjacent positions
        for (int i = 0; i < 4; i++) {
            int new_x = current.x + directions[i][0];
            int new_y = current.y + directions[i][1];

            // Check if the new position is within bounds and not a wall
            if (is_within_bounds(new_x, new_y, gameState) && !visited[new_y][new_x]) {
                // Check if the new position is a wall
                bool is_wall = false;
                for (int j = 0; j < gameState->entity_count; j++) {
                    if (gameState->entities[j].x == new_x && gameState->entities[j].y == new_y &&
                        strcmp(gameState->entities[j].type, "WALL") == 0) {
                        is_wall = true;
                        break;
                    }
                }

                if (!is_wall) {
                    visited[new_y][new_x] = true;
                    parent[new_y][new_x] = current; // Set parent for path reconstruction
                    queue[rear++] = (Point){new_x, new_y}; // Add to queue
                }
            }
        }
    }

    return (Point){-1, -1}; // Return an invalid point if no A protein source is found
}

// Function to decide the next action for growing an organ
void decide_next_action(GameState *gameState) {
    Point parent[gameState->height][gameState->width]; // Declare parent array here

    if (gameState->my_proteins[0] > 0) { // Check if there are enough A proteins
        // Iterate through all owned organs
        for (int i = 0; i < gameState->entity_count; i++) {
            if (gameState->entities[i].owner == 1) { // Find owned organ
                int parent_id = gameState->entities[i].organ_id;
                int start_x = gameState->entities[i].x;
                int start_y = gameState->entities[i].y;

                // Find the nearest A protein source starting from this organ
                Point a_protein_location = find_a_protein(gameState, start_x, start_y, parent);

                if (a_protein_location.x != -1 && a_protein_location.y != -1) {
                    // Print the path taken to grow the new organ
                    Point path_point = a_protein_location;
                    while (parent[path_point.y][path_point.x].x != -1) {
                        Point p = parent[path_point.y][path_point.x];
                        // Print the direction taken
                        for (int d = 0; d < 4; d++) {
                            if (p.x == path_point.x + directions[d][0] && p.y == path_point.y + directions[d][1]) {
                                fprintf(stderr, "Move %c to (%d , %d)\n", direction_chars[d], p.x, p.y);
                                // print_grow_command(parent_id, p.x, p.y); // maybe should try here // :thinking:
                                break;
                            }
                        }
                        path_point = p; // Move to the parent
                    }
                    // print_grow_command(parent_id, p.x, p.y); // this is how i want it to be printed out
                    print_grow_command(parent_id, a_protein_location.x, a_protein_location.y);
                    return; // Exit after issuing the grow command
                }
            }
        }

        // If no A protein source is found, attempt to grow in an adjacent empty space
        for (int i = 0; i < gameState->entity_count; i++) {
            if (gameState->entities[i].owner == 1) { // Find owned organ
                int parent_id = gameState->entities[i].organ_id;
                int start_x = gameState->entities[i].x;
                int start_y = gameState->entities[i].y;

                // Check adjacent positions for empty spaces
                for (int j = 0; j < 4; j++) {
                    int new_x = start_x + (j == 0 ? -1 : (j == 1 ? 1 : 0));
                    int new_y = start_y + (j == 2 ? -1 : (j == 3 ? 1 : 0));

                    if (is_within_bounds(new_x, new_y, gameState) && 
                        gameState->entities[new_y * gameState->width + new_x].owner == -1) { // Check if it's empty
                        print_grow_command(parent_id, new_x, new_y);
                        return; // Exit after issuing the grow command
                    }
                }
            }
        }
    } else {
        fprintf(stderr, "Not enough proteins to grow.\n");
    }
}

/* ################################################################################# */

int main() {
    GameState gameState;

    // Read width and height
    scanf("%d%d", &gameState.width, &gameState.height);

    // Game loop
    while (1) {
        // Read entity count
        scanf("%d", &gameState.entity_count);
        for (int i = 0; i < gameState.entity_count; i++) {
            // Read entity data
            scanf("%d%d%s%d%d%s%d%d", 
                  &gameState.entities[i].x, 
                  &gameState.entities[i].y, 
                  gameState.entities[i].type, 
                  &gameState.entities[i].owner, 
                  &gameState.entities[i].organ_id, 
                  gameState.entities[i].organ_dir, 
                  &gameState.entities[i].organ_parent_id, 
                  &gameState.entities[i].organ_root_id);
        }

        // Read your protein stock
        scanf("%d%d%d%d", 
              &gameState.my_proteins[0], 
              &gameState.my_proteins[1], 
              &gameState.my_proteins[2], 
              &gameState.my_proteins[3]);

        // Read opponent's protein stock
        scanf("%d%d%d%d", 
              &gameState.opp_proteins[0], 
              &gameState.opp_proteins[1], 
              &gameState.opp_proteins[2], 
              &gameState.opp_proteins[3]);

        // Read required actions count
        scanf("%d", &gameState.required_actions_count);

        // Print the current state of the game map
        print_map(&gameState);

        // Decide the next action for growing an organ
        decide_next_action(&gameState);
    }

    return 0;
}