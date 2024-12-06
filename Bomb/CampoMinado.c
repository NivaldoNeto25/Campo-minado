#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 20
#define TILE_SIZE 32
#define NUM_BOMBS 10
#define SAVE_FILE "savegame.csv"

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
void InitializeBoard(GameState *game, int numBombas);
void PlaceBombs(GameState *game, int numBombas);
int CalculateBombsAround(GameState *game, int x, int y);
void RevealTile(GameState *game, int x, int y, int numBombas);
void SaveGame(GameState *game);
bool LoadGame(GameState *game);

// Inicialização do jogo
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
    FILE *file = fopen(SAVE_FILE, "w");
    if (file) {
        // Salva as variáveis principais: vidas, células reveladas, status do jogo
        fprintf(file, "%d,%d,%d\n", game->lives, game->revealedTiles, game->gameOver);

        // Salva o estado das células
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Tile *t = &game->board[y][x];
                fprintf(file, "%d,%d,%d\n", t->hasBomb, t->revealed, t->flagged);
            }
        }
        fclose(file);
        printf("Jogo salvo com sucesso!\n");
    } else {
        printf("Erro ao salvar o jogo: não foi possível abrir o arquivo.\n");
    }
}



bool LoadGame(GameState *game) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        printf("Arquivo de salvamento não encontrado. Iniciando novo jogo...\n");
        return false;
    }

    // Leitura das variáveis principais (lives, revealedTiles, gameOver)
    int lives, revealedTiles, gameOver;
    if (fscanf(file, "%d,%d,%d\n", &lives, &revealedTiles, &gameOver) != 3) {
        printf("Erro ao carregar o jogo: formato inválido do cabeçalho.\n");
        fclose(file);
        return false;
    }

    // Exibindo dados lidos
    printf("Lidas as seguintes variáveis principais:\n");
    printf("Vidas: %d\n", lives);
    printf("Células reveladas: %d\n", revealedTiles);
    printf("Status do jogo (gameOver): %d\n", gameOver);

    // Atribuindo valores lidos ao estado do jogo
    game->lives = lives;
    game->revealedTiles = revealedTiles;
    game->gameOver = (gameOver != 0); // 0 significa que o jogo não acabou, 1 significa que o jogo acabou

    // Leitura do estado das células
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            int hasBomb, revealed, flagged;
            if (fscanf(file, "%d,%d,%d\n", &hasBomb, &revealed, &flagged) != 3) {
                printf("Erro ao carregar o jogo: dados inválidos da célula (%d, %d).\n", x, y);
                fclose(file);
                return false;
            }
            game->board[y][x].hasBomb = hasBomb;
            game->board[y][x].revealed = revealed;
            game->board[y][x].flagged = flagged;
        }
    }

    fclose(file);
    printf("Jogo carregado com sucesso!\n");
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
                                SaveGame(&game); // Salva o jogo quando o jogador escolhe salvar
                break; // Sai do loop do jogo
            }
            continue;
        }

        if (game.gameOver || game.gameWon) {
            if (game.gameOver && !IsSoundPlaying(perdeu)) {
                PlaySound(perdeu); // Reproduz o som de derrota
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            if (game.gameWon) {
                DrawText("Você ganhou!", 10, 10, 20, GREEN); // Mensagem de vitória
                PlaySound(ganhou); // Reproduz o som de vitória
            } else {
                DrawText("Você perdeu!", 10, 10, 20, RED); // Mensagem de derrota
            }
            DrawText("Pressione ESC para sair", 10, 40, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ESCAPE)) break; // Sai do jogo se pressionar ESC
            continue;
        }

        // Pausa o jogo se o jogador pressionar 'P'
        if (IsKeyPressed(KEY_P)) paused = true;

        // Controle do mouse
        int mouseX = GetMouseX() / TILE_SIZE;
        int mouseY = GetMouseY() / TILE_SIZE;
        bool mouseOverBoard = mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE;

        // Ação do mouse (revelar uma célula ao clicar)
        if (mouseOverBoard && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            RevealTile(&game, mouseX, mouseY, numBombas);
        }

        // Início da renderização da tela
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha o tabuleiro
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Rectangle tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Color tileColor = LIGHTGRAY;

                if (game.board[y][x].revealed) {
                    if (game.board[y][x].hasBomb) {
                        DrawTexture(texture, x * TILE_SIZE, y * TILE_SIZE, WHITE); // Desenha a bomba
                    } else {
                        tileColor = GREEN;
                        DrawRectangleRec(tileRect, tileColor); // Desenha uma célula revelada
                    }
                } else {
                    DrawRectangleRec(tileRect, tileColor); // Desenha uma célula oculta
                }
                DrawRectangleLines(tileRect.x, tileRect.y, TILE_SIZE, TILE_SIZE, DARKGRAY); // Desenha as bordas

                // Exibe o número de bombas ao redor se o mouse estiver sobre a célula
                if (mouseOverBoard && mouseX == x && mouseY == y) {
                    int bombsAround = CalculateBombsAround(&game, x, y);
                    char buffer[16];
                    sprintf(buffer, "Bombas: %d", bombsAround);
                    DrawText(buffer, 10, BOARD_SIZE * TILE_SIZE + 10, 20, BLACK);
                }
            }
        }

        // Exibe a quantidade de vidas restantes
        DrawText(TextFormat("Vidas: %d", game.lives), 10, BOARD_SIZE * TILE_SIZE + 30, 20, BLACK);
        EndDrawing();
    }

    // Libera recursos e fecha a janela
    UnloadSound(perdeu);
    UnloadSound(ganhou);
    UnloadTexture(texture);
    CloseAudioDevice();
    CloseWindow();
   
    return 0;
}
