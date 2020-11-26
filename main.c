#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    Actors are anything that are "interactable".
    Players, Zombies, stairs, and examinable items are examples.
*/ 
struct Actor {
    char type[7];
    char icon[3];
    char xcoord;
    char ycoord;
    char startingX;
    char startingY;
};

/*
    Tiles are things that cannot be relocated and have the ability to exist "underneath" interactables.
    The floor and walls are examples.
*/
struct Tile {
    char type[6];
    char icon[3];
    struct Actor* occupant;
};

char moveZombieLeft(struct Actor*, struct Actor*, struct Tile[21][25]);
char moveZombieRight(struct Actor*, struct Actor*, struct Tile[21][25]);
char moveZombieUp(struct Actor*, struct Actor*, struct Tile[21][25]);
char moveZombieDown(struct Actor*, struct Actor*, struct Tile[21][25]);
void AggressiveZombieMotion(struct Actor*, struct Actor*, struct Actor*, struct Tile[21][25]);
void randomPassiveZombieMotion(struct Actor*, struct Actor*, struct Tile[21][25]);
void resetGame(char[100], struct Actor*, struct Actor[5], char, struct Actor*, struct Actor*, struct Tile[21][25], char*);
char restartGame(char, char*);

int main() {

    // The Nothing Actor exists to avoid manipulating null pointers and serves as a comparator to be certain that nothing is occupying a Tile.
    struct Actor Nothing;

    // Horizontal walls
    struct Tile hWall;
    strcpy(hWall.type, "wall");
    strcpy(hWall.icon, "- ");
    hWall.occupant = &Nothing;

    // Vertical walls
    struct Tile vWall;
    strcpy(vWall.type, "wall");
    strcpy(vWall.icon, "| ");
    vWall.occupant = &Nothing;

    // Junction walls
    struct Tile jWall;
    strcpy(jWall.type, "wall");
    strcpy(jWall.icon, "+ ");
    jWall.occupant = &Nothing;

    // Floor tiles
    struct Tile Floor;
    strcpy(Floor.type, "floor");
    strcpy(Floor.icon, "  ");
    Floor.occupant = &Nothing;

    /*
        Tile Map
        + - - - - + - - + - - + - + - - + - - + - - - - +
        |         |     |     |   |     |     |         |
        |   + +   + -   + -   |   |   - +   - +         |
        |   + +   |                           |         |
        |                                               |
        + + + - - -           + + +           - - - + + +
        + + +                 + + +                 + + +
        |   |                                       |   |
        |       |   - +   - +       + -   + -   |       |
        + - +   |     |     |       |     |     |   + - +
        |   |   + - - + - - +       + - - + - - +   |   |
        |       |     |     |       |     |     |       |
        + - -   |   - +   - + + - - + -   + -   |   - - +
        |                     |   |                     |
        |                     + - +                     |
        |                                               |
        + + + + + + + + + + -       - + + + + + + + + + +
        + + + + + + + + + +           + + + + + + + + + +
        + + + + + + + + + +           + + + + + + + + + +
        + + + + + + + + + +           + + + + + + + + + +
        + + + + + + + + + + - -   - - + + + + + + + + + +

    */

    struct Tile map[21][25] = {
        {jWall, hWall, hWall, hWall, hWall, jWall, hWall, hWall, jWall, hWall, hWall, jWall, hWall, jWall, hWall, hWall, jWall, hWall, hWall, jWall, hWall, hWall, hWall, hWall, jWall},
        {vWall, Floor, Floor, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, Floor, Floor, vWall},
        {vWall, Floor, jWall, jWall, Floor, jWall, hWall, Floor, jWall, hWall, Floor, vWall, Floor, vWall, Floor, hWall, jWall, Floor, hWall, jWall, Floor, Floor, Floor, Floor, vWall},
        {vWall, Floor, jWall, jWall, Floor, vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall, Floor, Floor, Floor, Floor, vWall},
        {vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall},
        {jWall, jWall, jWall, hWall, hWall, hWall, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, hWall, hWall, hWall, jWall, jWall, jWall},
        {jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall},
        {vWall, Floor, vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall, Floor, vWall},
        {vWall, Floor, Floor, Floor, vWall, Floor, hWall, jWall, Floor, hWall, jWall, Floor, Floor, Floor, jWall, hWall, Floor, jWall, hWall, Floor, vWall, Floor, Floor, Floor, vWall},
        {jWall, hWall, jWall, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, jWall, hWall, jWall},
        {vWall, Floor, vWall, Floor, jWall, hWall, hWall, jWall, hWall, hWall, jWall, Floor, Floor, Floor, jWall, hWall, hWall, jWall, hWall, hWall, jWall, Floor, vWall, Floor, vWall},
        {vWall, Floor, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, vWall, Floor, Floor, Floor, vWall},
        {vWall, hWall, hWall, Floor, vWall, Floor, hWall, jWall, Floor, hWall, jWall, jWall, hWall, jWall, jWall, hWall, Floor, jWall, hWall, Floor, vWall, Floor, hWall, hWall, vWall},
        {vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall, Floor, vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall},
        {vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, jWall, hWall, jWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall},
        {vWall, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, Floor, vWall},
        {jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, hWall, Floor, Floor, Floor, hWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall},
        {jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall},
        {jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall},
        {jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, Floor, Floor, Floor, Floor, Floor, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall},
        {jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, hWall, hWall, Floor, hWall, hWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall, jWall}
    };

    // Player generation
    struct Actor Player;
    strcpy(Player.icon, "X ");
    strcpy(Player.type, "player");
    Player.startingX = 9;
    Player.startingY = 1;

    // Zombie generation
    struct Actor Zombie1;
    strcpy(Zombie1.icon, "Z ");
    strcpy(Zombie1.type, "zombie");
    Zombie1.startingX = 11;
    Zombie1.startingY = 19;

    struct Actor Zombie2;
    strcpy(Zombie2.icon, "Z ");
    strcpy(Zombie2.type, "zombie");
    Zombie2.startingX = 13;
    Zombie2.startingY = 19;

    struct Actor zombies[2] = {Zombie1, Zombie2};

    // Escape door generation
    struct Actor Escape;
    strcpy(Escape.icon, "  ");
    Escape.startingX = 12;
    Escape.startingY = 20;

    // Set up variables common to each round
    char input;
    char message[100];
    char continueGame = 1;
    char playerDied = 0;
    srand(time(0));

    // Set the starting message
    resetGame(message, &Player, zombies, sizeof(zombies)/sizeof(zombies[0]), &Nothing, &Escape, map, &playerDied);

    while (continueGame) {

        // Print out the map row by row, replacing tile icons with actor icons if applicable
        for (char row = 0; row < 21; row++) {
            for (char column = 0; column < 25; column++) {
                if (map[row][column].occupant == &Nothing) {
                    printf("%s", map[row][column].icon);
                }
                else {
                    printf("%s", map[row][column].occupant -> icon);
                }
            }
            printf("\n");
        }
        printf("\n");

        // Print the message of the previous round
        printf("%s\n", message);
        strcpy(message, "");
        
        // Handle Player movement - Note that multiple inputs "wwwww" all count, and is effectively like taking 5 turns at once
        scanf(" %c", &input);
        switch (input) {
        case 'w':
            if (strcmp(map[Player.ycoord - 1][Player.xcoord].type, "floor") == 0) {
                map[Player.ycoord - 1][Player.xcoord].occupant = &Player;
                map[Player.ycoord][Player.xcoord].occupant = &Nothing;
                Player.ycoord--;
                strcpy(Player.icon, "^ ");
            }
            else {
                strcpy(message, "I can't go there.");
            }
            break;
        case 'a':
            if (strcmp(map[Player.ycoord][Player.xcoord - 1].type, "floor") == 0) {
                map[Player.ycoord][Player.xcoord - 1].occupant = &Player;
                map[Player.ycoord][Player.xcoord].occupant = &Nothing;
                Player.xcoord--;
                strcpy(Player.icon, "< ");
            }
            else {
                strcpy(message, "I can't go there.");
            }
            break;
        case 's':
            if (strcmp(map[Player.ycoord + 1][Player.xcoord].type, "floor") == 0) {
                map[Player.ycoord + 1][Player.xcoord].occupant = &Player;
                map[Player.ycoord][Player.xcoord].occupant = &Nothing;
                Player.ycoord++;
                strcpy(Player.icon, "v ");
            }
            else {
                strcpy(message, "I can't go there.");
            }
            break;
        case 'd':
            if (strcmp(map[Player.ycoord][Player.xcoord + 1].type, "floor") == 0) {
                map[Player.ycoord][Player.xcoord + 1].occupant = &Player;
                map[Player.ycoord][Player.xcoord].occupant = &Nothing;
                Player.xcoord++;
                strcpy(Player.icon, "> ");
            }
            else {
                strcpy(message, "I can't go there.");
            }
            break;
        default:
            break;
        }

        // Handle Zombie movement - If within 4 tiles of a zombie, they will purposefully come after you. Otherwise, they meander randomly
        for (char i = 0; i < sizeof(zombies)/sizeof(zombies[0]); i++) {
            if (abs(zombies[i].xcoord - Player.xcoord) <= 4 || abs(zombies[i].ycoord - Player.ycoord) <= 4) {
                AggressiveZombieMotion(&zombies[i], &Player, &Nothing, map);
            }
            else {
                randomPassiveZombieMotion(&zombies[i], &Nothing, map);
            }
        }

        // Check if the Player won or lost the game
        for (char i = 0; i < sizeof(zombies)/sizeof(zombies[0]); i++) {
            if (Player.xcoord == zombies[i].xcoord && Player.ycoord == zombies[i].ycoord) {
                playerDied = 1;
            }
        }

        if ((Player.xcoord == Escape.xcoord && Player.ycoord == Escape.ycoord) || playerDied) {
            if (restartGame(playerDied, &input)) {
                resetGame(message, &Player, zombies, sizeof(zombies)/sizeof(zombies[0]), &Nothing, &Escape, map, &playerDied);
            }
            else {
                continueGame = 0;
            }
        }
    }
    return 0;
}

void AggressiveZombieMotion(struct Actor* Zombie, struct Actor* Player, struct Actor* Nothing, struct Tile map[21][25]) {
    if (Zombie -> xcoord == Player -> xcoord && Zombie -> ycoord == Player -> ycoord) {
        map[Zombie -> ycoord][Zombie -> xcoord].occupant = Zombie;
    }
    else if (Zombie -> xcoord == Player -> xcoord && Zombie -> ycoord != Player -> ycoord) {
        if (Zombie -> ycoord > Player -> ycoord) {
            moveZombieUp(Zombie, Nothing, map);
        }
        else {
            moveZombieDown(Zombie, Nothing, map);
        }
    }
    else if (Zombie -> ycoord == Player -> ycoord && Zombie -> xcoord != Player -> xcoord) {
        if (Zombie -> xcoord > Player -> xcoord) {
            moveZombieLeft(Zombie, Nothing, map);
        }
        else {
            moveZombieRight(Zombie, Nothing, map);
        }
    }
    else {
        char xdirection[6];
        char ydirection[5];
        if (Zombie -> xcoord > Player -> xcoord) {
            strcpy(xdirection, "right");
        }
        else {
            strcpy(xdirection, "left");
        }
        if (Zombie -> ycoord > Player -> ycoord) {
            strcpy(ydirection, "down");
        }
        else {
            strcpy(ydirection, "up");
        }

        if (abs(Zombie -> xcoord - Player -> xcoord) > abs(Zombie -> ycoord - Player -> ycoord)) {
            if (strcmp(xdirection, "left")) {
                if (!moveZombieLeft(Zombie, Nothing, map)) {
                    if (strcmp(ydirection, "up")) {
                        moveZombieUp(Zombie, Nothing, map);
                    }
                    else {
                        moveZombieDown(Zombie, Nothing, map);
                    }
                }
            }
            else {
                if (!moveZombieRight(Zombie, Nothing, map)) {
                    if (strcmp(ydirection, "up")) {
                        moveZombieUp(Zombie, Nothing, map);
                    }
                    else {
                        moveZombieDown(Zombie, Nothing, map);
                    }
                }
            }
        }
        else {
            if (strcmp(ydirection, "up")) {
                if (!moveZombieUp(Zombie, Nothing, map)) {
                    if (strcmp(xdirection, "left")) {
                        moveZombieLeft(Zombie, Nothing, map);
                    }
                    else {
                        moveZombieRight(Zombie, Nothing, map);
                    }
                }
            }
            else {
                if (!moveZombieDown(Zombie, Nothing, map)) {
                    if (strcmp(xdirection, "left")) {
                        moveZombieLeft(Zombie, Nothing, map);
                    }
                    else {
                        moveZombieRight(Zombie, Nothing, map);
                    }
                }
            }
        }
    }
}

void randomPassiveZombieMotion(struct Actor* Zombie, struct Actor* Nothing, struct Tile map[21][25]) {
    if (rand() % 2 == 0) {
        // Zombie will move in the y direction
        if (rand() % 2 == 0) {
            // The Zombie will move down
            moveZombieDown(Zombie, Nothing, map);
        }
        else {
            // The Zombie will move up
            moveZombieUp(Zombie, Nothing, map);
        }
    }
    else {
        // Zombie will move in the x direction
        if (rand() % 2 == 0) {
            // The Zombie will move to the right
            moveZombieRight(Zombie, Nothing, map);
        }
        else {
            // The Zombie will move to the left
            moveZombieLeft(Zombie, Nothing, map);
        }
    }
}

char moveZombieLeft(struct Actor* Zombie, struct Actor* Nothing, struct Tile map[21][25]) {
    if (strcmp(map[Zombie -> ycoord][Zombie -> xcoord - 1].type, "floor") == 0
        && strcmp(map[Zombie -> ycoord][Zombie -> xcoord - 1].occupant -> type, "zombie") != 0) {
        // The Zombie is trying to move to a valid tile
        map[Zombie -> ycoord][Zombie -> xcoord - 1].occupant = Zombie;
        if (map[Zombie -> ycoord][Zombie -> xcoord].occupant == Zombie) {
            map[Zombie -> ycoord][Zombie -> xcoord].occupant = Nothing;
        }
        Zombie -> xcoord--;
        return 1;
    }
    else {
        return 0;
    }
}

char moveZombieRight(struct Actor* Zombie, struct Actor* Nothing, struct Tile map[21][25]) {
    if (strcmp(map[Zombie -> ycoord][Zombie -> xcoord + 1].type, "floor") == 0
        && strcmp(map[Zombie -> ycoord][Zombie -> xcoord + 1].occupant -> type, "zombie") != 0) {
        // The Zombie is trying to move to a valid tile
        map[Zombie -> ycoord][Zombie -> xcoord + 1].occupant = Zombie;
        if (map[Zombie -> ycoord][Zombie -> xcoord].occupant == Zombie) {
            map[Zombie -> ycoord][Zombie -> xcoord].occupant = Nothing;
        }
        Zombie -> xcoord++;
        return 1;
    }
    else {
        return 0;
    }
}

char moveZombieUp(struct Actor* Zombie, struct Actor* Nothing, struct Tile map[21][25]) {
    if (strcmp(map[Zombie -> ycoord - 1][Zombie -> xcoord].type, "floor") == 0
        && strcmp(map[Zombie -> ycoord - 1][Zombie -> xcoord].occupant -> type, "zombie") != 0) {
        // The Zombie is trying to move to a valid tile
        map[Zombie -> ycoord - 1][Zombie -> xcoord].occupant = Zombie;
        if (map[Zombie -> ycoord][Zombie -> xcoord].occupant == Zombie) {
            map[Zombie -> ycoord][Zombie -> xcoord].occupant = Nothing;
        }
        Zombie -> ycoord--;
        return 1;
    }
    else {
        return 0;
    }
}

char moveZombieDown(struct Actor* Zombie, struct Actor* Nothing, struct Tile map[21][25]) {
    if (strcmp(map[Zombie -> ycoord + 1][Zombie -> xcoord].type, "floor") == 0
        && strcmp(map[Zombie -> ycoord + 1][Zombie -> xcoord].occupant -> type, "zombie") != 0) {
        // The Zombie is trying to move to a valid tile
        map[Zombie -> ycoord + 1][Zombie -> xcoord].occupant = Zombie;
        if (map[Zombie -> ycoord][Zombie -> xcoord].occupant == Zombie) {
            map[Zombie -> ycoord][Zombie -> xcoord].occupant = Nothing;
        }
        Zombie -> ycoord++;
        return 1;
    }
    else {
        return 0;
    }
}

char restartGame(char playerDied, char* input) {
    if (playerDied) {
        printf("You died! Want to play again? (y/n)");
    }
    else {
        printf("You escaped! Want to play again? (y/n)");
    }
    scanf(" %c", input);
    switch (*input) {
    case 'y':
        return 1;
    case 'n':
        return 0;
    default:
        return 0;
    }
}

void resetGame(char message[100], struct Actor* Player, struct Actor zombies[5], char numZombies, struct Actor* Nothing, struct Actor* Escape, struct Tile map[21][25], char* playerDied) {
    // Set up miscellaneous
    strcpy(message, "Escape the office without getting eaten by hungry zombies! Use w a s d to move.");
    strcpy(Player -> icon, "X ");
    *playerDied = 0;

    // Clear the spaces of the player and zombies
    map[Player -> ycoord][Player -> xcoord].occupant = Nothing;
    for (char i = 0; i < numZombies; i++) {
        map[zombies[i].ycoord][zombies[i].xcoord].occupant = Nothing;
    }
    
    // Reset the coordinates of all actors
    Player -> xcoord = Player -> startingX;
    Player -> ycoord = Player -> startingY;
    for (char i = 0; i < numZombies; i++) {
        zombies[i].xcoord = zombies[i].startingX;
        zombies[i].ycoord = zombies[i].startingY;
    }
    Escape -> xcoord = Escape -> startingX;
    Escape -> ycoord = Escape -> startingY;

    // Place the actors on the map
    map[Player -> ycoord][Player -> xcoord].occupant = Player;
    for (char i = 0; i < numZombies; i++) {
        map[zombies[i].ycoord][zombies[i].xcoord].occupant = &zombies[i];
    }
    map[Escape -> ycoord][Escape -> xcoord].occupant = Escape;
}