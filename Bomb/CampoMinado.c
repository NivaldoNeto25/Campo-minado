#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 20
#define TILE_SIZE 32
#define NUM_BOMBS 40
#define MAX_LIVES 3

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

// Funções auxiliares
void InitializeBoard(GameState *game);
void PlaceBombs(GameState *game);
int CalculateBombsAround(GameState *game, int x, int y);
void SaveGame(GameState *game);
bool LoadGame(GameState *game);
void RevealTile(GameState *game, int x, int y);

// Função principal
int main() {
    InitWindow(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE + 50, "Campo Minado");
    SetTargetFPS(60);
    
    GameState game = {0};
    bool paused = false;
    bool inMenu = true;
    
    Texture2D texture = LoadTexture("exp32.png");
    
    // Menu inicial
    while (inMenu) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("1: Novo Jogo", 10, 10, 20, DARKGRAY);
        DrawText("2: Carregar Jogo", 10, 40, 20, DARKGRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) {
            InitializeBoard(&game);
            inMenu = false;
        } else if (IsKeyPressed(KEY_TWO)) {
            if (!LoadGame(&game)) {
                DrawText("Falha ao carregar jogo!", 10, 70, 20, RED);
            } else {
                inMenu = false;
            }
        }
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        if (paused) {
            // Menu de pausa
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

        // Verifica vitória/derrota
        if (game.gameOver || game.gameWon) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            if (game.gameWon) {
                DrawText("Você ganhou!", 10, 10, 20, GREEN);
            } else {
                DrawText("Você perdeu!", 10, 10, 20, RED);
            }
            DrawText("Pressione ESC para sair", 10, 40, 20, DARKGRAY);
            EndDrawing();
            if (IsKeyPressed(KEY_ESCAPE)) break;
            continue;
        }

        // Jogo ativo
        if (IsKeyPressed(KEY_P)) paused = true;

        int mouseX = GetMouseX() / TILE_SIZE;
        int mouseY = GetMouseY() / TILE_SIZE;
        bool mouseOverBoard = mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE;

        if (mouseOverBoard && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            RevealTile(&game, mouseX, mouseY);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha o tabuleiro
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Rectangle tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Color tileColor = LIGHTGRAY;

                if (game.board[y][x].revealed) {
                    if (game.board[y][x].hasBomb) {
                        // Desenha a textura de explosão
                        DrawTexture(texture, x * TILE_SIZE, y * TILE_SIZE, WHITE);
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

    CloseWindow();
    return 0;
}

// Funções auxiliares
void InitializeBoard(GameState *game) {
    srand(time(NULL));
    game->lives = MAX_LIVES;
    game->revealedTiles = 0;
    game->gameOver = false;
    game->gameWon = false;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            game->board[y][x] = (Tile){false, false, false};
        }
    }
    PlaceBombs(game);
}

void PlaceBombs(GameState *game) {
    for (int i = 0; i < NUM_BOMBS; i++) {
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
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -4; dx <= 4; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && game->board[ny][nx].hasBomb) {
                count++;
            }
        }
    }
    return count;
}

void RevealTile(GameState *game, int x, int y) {
    if (!game->board[y][x].revealed) {
        game->board[y][x].revealed = true;
        if (game->board[y][x].hasBomb) {
            game->lives--;
            if (game->lives <= 0) {
                game->gameOver = true;
            }
        } else {
            game->revealedTiles++;
            if (game->revealedTiles == (BOARD_SIZE * BOARD_SIZE) - NUM_BOMBS) {
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
                fprintf(file, "%d,%d,%d,", t->hasBomb, t->revealed, t->flagged);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}

bool LoadGame(GameState *game) {
    FILE *file = fopen("savegame.csv", "r");
    if (!file) return false;

    fscanf(file, "%d,%d,%d\n", &game->lives, &game->revealedTiles, &game->gameOver);
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Tile *t = &game->board[y][x];
            fscanf(file, "%d,%d,%d,", &t->hasBomb, &t->revealed, &t->flagged);
        }
    }
    fclose(file);
    return true;
}
