#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 20
#define TILE_SIZE 32
#define NUM_BOMBS 10
typedef struct {
    bool hasBomb;
    bool revealed;
    bool flagged;
} Tile;

typedef struct {
    Tile board[BOARD_SIZE][BOARD_SIZE];
    int lives;
    int revealedTiles;
    bool gameOver;
    bool gameWon;
} GameState;

// Prototipos das funções auxiliares
void InitializeBoard(GameState *game, int numBombas);
void PlaceBombs(GameState *game, int numBombas);
int CalculateBombsAround(GameState *game, int x, int y);
void RevealTile(GameState *game, int x, int y, int numBombas);
void SaveGame(GameState *game);
bool LoadGame(GameState *game);

// Funções auxiliares
void InitializeBoard(GameState *game, int numBombas) {
    srand(time(NULL));
    game->lives = 3;
    game->revealedTiles = 0;
    game->gameOver = false;
    game->gameWon = false;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            game->board[y][x] = (Tile){false, false, false};
        }
    }
    PlaceBombs(game, numBombas);
}

void PlaceBombs(GameState *game, int numBombas) {
    for (int i = 0; i < numBombas; i++) {
        int x, y;
        do {
            x = rand() % BOARD_SIZE;
            y = rand() % BOARD_SIZE;
        } while (game->board[y][x].hasBomb);
        game->board[y][x].hasBomb = true;
    }
}

int CalculateBombsAround(GameState *game, int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && game->board[ny][nx].hasBomb) {
                count++;
            }
        }
    }
    return count;
}

void RevealTile(GameState *game, int x, int y, int numBombas) {
    if (!game->board[y][x].revealed) {
        game->board[y][x].revealed = true;
        if (game->board[y][x].hasBomb) {
            game->lives--;
            if (game->lives <= 0) {
                game->gameOver = true;
            }
        } else {
            game->revealedTiles++;
            if (game->revealedTiles == (BOARD_SIZE * BOARD_SIZE) - numBombas) {
                game->gameWon = true;
            }
        }
    }
}

void SaveGame(GameState *game) {
    FILE *file = fopen("savegame.csv", "w");
    if (file) {
        fprintf(file, "%d,%d,%d\n", game->lives, game->revealedTiles, game->gameOver);
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Tile *t = &game->board[y][x];
                fprintf(file, "%d,%d,%d\n", t->hasBomb, t->revealed, t->flagged);
            }
        }
        fclose(file);
        printf("Game saved successfully!\n");
    } else {
        printf("Error saving game: Could not open file.\n");
    }
}

bool LoadGame(GameState *game) {
    FILE *file = fopen("savegame.csv", "r");
    if (!file) {
        printf("Error loading game: Could not open file.\n");
        return false;
    }

    if (fscanf(file, "%d,%d,%d\n", &game->lives, &game->revealedTiles, &game->gameOver) != 3) {
        printf("Error loading game: Invalid header format.\n");
        fclose(file);
        return false;
    }

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (fscanf(file, "%d,%d,%d\n", &game->board[y][x].hasBomb, &game->board[y][x].revealed, &game->board[y][x].flagged) != 3) {
                printf("Error loading game: Invalid tile data at (%d, %d).\n", x, y);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    printf("Game loaded successfully!\n");
    return true;
}



int ShowDifficultyMenu() {
    int selection = 0;
    while (selection == 0) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Escolha a dificuldade:", 10, 10, 20, DARKGRAY);
        DrawText("1: Fácil (50 Bombas)", 10, 40, 20, DARKGRAY);
        DrawText("2: Médio (100 Bombas)", 10, 70, 20, DARKGRAY);
        DrawText("3: Difícil (150 Bombas)", 10, 100, 20, DARKGRAY);
        DrawText("4: Extremo (200 Bombas)", 10, 130, 20, DARKGRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) {
            selection = 50;
        } else if (IsKeyPressed(KEY_TWO)) {
            selection = 100;
        } else if (IsKeyPressed(KEY_THREE)) {
            selection = 150;
        } else if (IsKeyPressed(KEY_FOUR)) {
            selection = 200;
        }
    }
    return selection;
}

// Função principal
int main() {
    InitWindow(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE + 50, "Campo Minado");
    SetTargetFPS(60);
    InitAudioDevice();

    Sound ganhou = LoadSound("ganhou.wav");
    Sound perdeu = LoadSound("perdeu.wav");
    Texture2D texture = LoadTexture("bomba.png");

    GameState game = {0};
    bool paused = false;
    bool inMenu = true;
    
    int perdas = 0;
    int numBombas = 0;
    
    // Menu inicial
    while (inMenu) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawText("1: Novo Jogo", 10, 10, 20, DARKGRAY);
        DrawText("2: Carregar Jogo", 10, 40, 20, DARKGRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) {
            
            numBombas = ShowDifficultyMenu();
            
            InitializeBoard(&game, numBombas);
            inMenu = false;
        } else if (IsKeyPressed(KEY_TWO)) {
            if (!LoadGame(&game)) {
                DrawText("Falha ao carregar jogo!", 10, 70, 20, RED);
            } else {
                inMenu = false;
                LoadGame(&game);
            }
        }
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        if (paused) {
            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("Jogo Pausado", 10, 10, 20, WHITE);
            DrawText("1: Retornar", 10, 40, 20, LIGHTGRAY);
            DrawText("2: Salvar e Sair", 10, 70, 20, LIGHTGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ONE)) {
                paused = false;
            } else if (IsKeyPressed(KEY_TWO)) {
                SaveGame(&game);
                break;
            }
            continue;
        }

        if (game.gameOver || game.gameWon) {
            if (game.gameOver && !IsSoundPlaying(perdeu)) {
                PlaySound(perdeu);
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            if (game.gameWon) {
                DrawText("Você ganhou!", 10, 10, 20, GREEN);
                PlaySound(ganhou);
            } else {
                DrawText("Você perdeu!", 10, 10, 20, RED);
            }
            DrawText("Pressione ESC para sair", 10, 40, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ESCAPE)) break;
            continue;
        }

        if (IsKeyPressed(KEY_P)) paused = true;

        int mouseX = GetMouseX() / TILE_SIZE;
        int mouseY = GetMouseY() / TILE_SIZE;
        bool mouseOverBoard = mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE;

        if (mouseOverBoard && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            RevealTile(&game, mouseX, mouseY, numBombas);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Rectangle tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Color tileColor = LIGHTGRAY;

                if (game.board[y][x].revealed) {
                    if (game.board[y][x].hasBomb) {
                        DrawTexture(texture, x * TILE_SIZE, y * TILE_SIZE, WHITE);
                        perdas ++;
                    } else {
                        tileColor = GREEN;
                        DrawRectangleRec(tileRect, tileColor);
                    }
                } else {
                    DrawRectangleRec(tileRect, tileColor);
                }
                DrawRectangleLines(tileRect.x, tileRect.y, TILE_SIZE, TILE_SIZE, DARKGRAY);

                if (mouseOverBoard && mouseX == x && mouseY == y) {
                    int bombsAround = CalculateBombsAround(&game, x, y);
                    char buffer[16];
                    sprintf(buffer, "Bombas: %d", bombsAround);
                    DrawText(buffer, 10, BOARD_SIZE * TILE_SIZE + 10, 20, BLACK);
                }
            }
        }

        DrawText(TextFormat("Vidas: %d", game.lives), 10, BOARD_SIZE * TILE_SIZE + 30, 20, BLACK);
        EndDrawing();
    }

    UnloadSound(perdeu);
    UnloadSound(ganhou);
    UnloadTexture(texture);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
