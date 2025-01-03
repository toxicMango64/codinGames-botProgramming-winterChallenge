#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Define short entity type representations
#define WALL '#'
#define ROOT 'R'
#define BASIC 'B'
#define A_PROTEIN 'A'
#define EMPTY 'E'

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

/* ############# Program Starts Here ############################################### */
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

    }

    return 0;
}
