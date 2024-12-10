#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO_TABULEIRO 20
#define TAMANHO_CELULA 32

typedef struct {
    bool temBomba;
    bool revelado;
} Celula;

typedef struct {
    Celula tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    int vidas;
    int celulasReveladas;
    bool jogoEncerrado;
    bool jogoVencido;
} EstadoJogo;

void InicializarTabuleiro(EstadoJogo *jogo, int numeroBombas);
void ColocarBombas(EstadoJogo *jogo, int numeroBombas);
int ContarBombasAoRedor(EstadoJogo *jogo, int x, int y);
void RevelarCelula(EstadoJogo *jogo, int x, int y, int numeroBombas, Sound explosao);
void SalvarJogo(EstadoJogo *jogo, int numeroBombas);
bool CarregarJogo(EstadoJogo *jogo, int *numeroBombas);


void InicializarTabuleiro(EstadoJogo *jogo, int numeroBombas) {
    srand(time(NULL));
    jogo->vidas = 3;
    jogo->celulasReveladas = 0;
    jogo->jogoEncerrado = false;
    jogo->jogoVencido = false;

    for (int y = 0; y < TAMANHO_TABULEIRO; y++) {
        for (int x = 0; x < TAMANHO_TABULEIRO; x++) {
            jogo->tabuleiro[y][x] = (Celula){false, false};
        }
    }
    ColocarBombas(jogo, numeroBombas);
}

void ColocarBombas(EstadoJogo *jogo, int numeroBombas) {
    for (int i = 0; i < numeroBombas; i++) {
        int x, y;
        do {
            x = rand() % TAMANHO_TABULEIRO;
            y = rand() % TAMANHO_TABULEIRO;
        } while (jogo->tabuleiro[y][x].temBomba);
        jogo->tabuleiro[y][x].temBomba = true;
    }
}

int ContarBombasAoRedor(EstadoJogo *jogo, int x, int y) {
    int nx, ny, contador = 0;
    //cima e baixo
    for (int dy = -4; dy <= 4; dy++) {
        nx= x;
        ny= y + dy;
        if (nx >= 0 && nx < TAMANHO_TABULEIRO && ny >= 0 && ny < TAMANHO_TABULEIRO && jogo->tabuleiro[ny][nx].temBomba) {
                contador++;
        }
    }
        //esquerda e direita
    for (int dx = -4; dx <= 4; dx++) {
    nx = x + dx;
    ny = y;
    	if ((nx != x || ny != y) && nx >= 0 && nx < TAMANHO_TABULEIRO && ny >= 0 && ny < TAMANHO_TABULEIRO && jogo->tabuleiro[ny][nx].temBomba){
                contador++;
            }
        }
    return contador;
}

void RevelarCelula(EstadoJogo *jogo, int x, int y, int numeroBombas, Sound explosao) {

    if (!jogo->tabuleiro[y][x].revelado) {
        jogo->tabuleiro[y][x].revelado = true;
        if (jogo->tabuleiro[y][x].temBomba) {
            PlaySound(explosao);
            jogo->vidas--;
            if (jogo->vidas <= 0) {
                jogo->jogoEncerrado = true;
            }
        } else {
            jogo->celulasReveladas++;
            if (jogo->celulasReveladas == (TAMANHO_TABULEIRO * TAMANHO_TABULEIRO) - numeroBombas) {
                jogo->jogoVencido = true;
            }
        }
    }
}

void SalvarJogo(EstadoJogo *jogo, int numeroBombas) {
    FILE *arquivo = fopen("salvajogo.csv", "w");

    if (arquivo) {
        fprintf(arquivo, "%d,%d,%d,%d\n", jogo->vidas, jogo->celulasReveladas, jogo->jogoEncerrado, numeroBombas);

        for (int y = 0; y < TAMANHO_TABULEIRO; y++) {
            for (int x = 0; x < TAMANHO_TABULEIRO; x++) {
                Celula *c = &jogo->tabuleiro[y][x];
                fprintf(arquivo, "%d,%d\n", c->temBomba, c->revelado);
            }
        }

        fclose(arquivo);
        printf("Jogo salvo com sucesso!\n");
    } else {
        printf("não foi possível abrir o arquivo.\n");
    }
}

bool CarregarJogo(EstadoJogo *jogo, int *numeroBombas) {
    FILE *arquivo = fopen("salvajogo.csv", "r");

    if (!arquivo) {
        printf("Arquivo de salvamento não encontrado.\n");
        return false;
    }

    int vidas, celulasReveladas, jogoEncerrado;
    if (fscanf(arquivo, "%d,%d,%d,%d\n", &vidas, &celulasReveladas, &jogoEncerrado, numeroBombas) != 4) {
        printf("Formato inválido do cabeçalho.\n");
        fclose(arquivo);
        return false;
    }

    jogo->vidas = vidas;
    jogo->celulasReveladas = celulasReveladas;
    jogo->jogoEncerrado = (jogoEncerrado != 0);

    for (int y = 0; y < TAMANHO_TABULEIRO; y++) {
        for (int x = 0; x < TAMANHO_TABULEIRO; x++) {
            int temBomba, revelado;
            if (fscanf(arquivo, "%d,%d\n", &temBomba, &revelado) != 2) {
                printf("Dados inválidos da célula (%d, %d).\n", x, y);
                fclose(arquivo);
                return false;
            }
            jogo->tabuleiro[y][x].temBomba = temBomba;
            jogo->tabuleiro[y][x].revelado = revelado;
        }
    }

    fclose(arquivo);
    printf("Jogo carregado com sucesso!\n");
    return true;
}

int ExibirMenuDificuldade() {
    int selecao = 0;
    while (selecao == 0) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Escolha a dificuldade:", 10, 10, 20, DARKGRAY);
        DrawText("1: Fácil (10 Bombas)", 10, 40, 20, DARKGRAY);
        DrawText("2: Médio (50 Bombas)", 10, 70, 20, DARKGRAY);
        DrawText("3: Difícil (150 Bombas)", 10, 100, 20, DARKGRAY);
        DrawText("4: Extremo (200 Bombas)", 10, 130, 20, DARKGRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) {
            selecao = 10;
        } else if (IsKeyPressed(KEY_TWO)) {
            selecao = 50;
        } else if (IsKeyPressed(KEY_THREE)) {
            selecao = 150;
        } else if (IsKeyPressed(KEY_FOUR)) {
            selecao = 200;
        }
    }
    return selecao;
}

int main() {
    InitWindow(TAMANHO_TABULEIRO * TAMANHO_CELULA, TAMANHO_TABULEIRO * TAMANHO_CELULA + 50, "Campo Minado");
    SetTargetFPS(60);
    InitAudioDevice();

    Sound perdeu = LoadSound("perdeu.wav");
    Sound explosao = LoadSound("explosao.wav");
    Music vitoria = LoadMusicStream("vitoria.mp3");
    Music trilha = LoadMusicStream("trilha.mp3");
    
    PlayMusicStream(vitoria);
    PlayMusicStream(trilha);
    
    Texture2D texturaBomba = LoadTexture("bomba.png");

    EstadoJogo jogo = {0};
    bool pausado = false;
    bool noMenu = true;
   
    int numeroBombas = 0;
   
    while (noMenu) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("1: Novo Jogo", 10, 10, 20, DARKGRAY);
        DrawText("2: Carregar Jogo", 10, 40, 20, DARKGRAY);
        
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) {
            numeroBombas = ExibirMenuDificuldade();
            InicializarTabuleiro(&jogo, numeroBombas);
            noMenu = false;
        } else if (IsKeyPressed(KEY_TWO)) {
            if (!CarregarJogo(&jogo, &numeroBombas)) {
                DrawText("Falha ao carregar jogo!", 10, 70, 20, RED);
            } else {
                noMenu = false;
            }
        }
    }

    while (!WindowShouldClose()) {
        if (pausado) {
            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("Jogo Pausado", 10, 10, 20, WHITE);
            DrawText("1: Retornar", 10, 40, 20, LIGHTGRAY);
            DrawText("2: Salvar e Sair", 10, 70, 20, LIGHTGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ONE)) {
                pausado = false;
            } else if (IsKeyPressed(KEY_TWO)) {
                SalvarJogo(&jogo, numeroBombas);
                break;
            }
            continue;
        }

        if (jogo.jogoEncerrado || jogo.jogoVencido) {
            if (jogo.jogoEncerrado && !IsSoundPlaying(perdeu)) {
                PlaySound(perdeu);
                remove("salvajogo.csv");
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            if (jogo.jogoVencido) {
                DrawText("Você ganhou!", 10, 10, 20, GREEN);
                UpdateMusicStream(vitoria);
            } else {
                DrawText("Você perdeu!", 10, 10, 20, RED);
            }
            DrawText("Pressione ESC para sair", 10, 40, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ESCAPE)) break;
            continue;
        }

        if (IsKeyPressed(KEY_P)) pausado = true;
        
        UpdateMusicStream(trilha);

        int mouseX = GetMouseX() / TAMANHO_CELULA;
        int mouseY = GetMouseY() / TAMANHO_CELULA;
        bool mouseDentroTabuleiro = mouseX >= 0 && mouseX < TAMANHO_TABULEIRO && mouseY >= 0 && mouseY < TAMANHO_TABULEIRO;

        if (mouseDentroTabuleiro && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            RevelarCelula(&jogo, mouseX, mouseY, numeroBombas, explosao);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int y = 0; y < TAMANHO_TABULEIRO; y++) {
            for (int x = 0; x < TAMANHO_TABULEIRO; x++) {
                Rectangle celulaRect = {x * TAMANHO_CELULA, y * TAMANHO_CELULA, TAMANHO_CELULA, TAMANHO_CELULA};
                Color corCelula = LIGHTGRAY;

                if (jogo.tabuleiro[y][x].revelado) {
                    if (jogo.tabuleiro[y][x].temBomba) {
                        DrawTexture(texturaBomba, x * TAMANHO_CELULA, y * TAMANHO_CELULA, WHITE);
                    } else {
                        corCelula = GREEN;
                        DrawRectangleRec(celulaRect, corCelula);
                    }
                } else {
                    DrawRectangleRec(celulaRect, corCelula);
                }
                DrawRectangleLines(celulaRect.x, celulaRect.y, TAMANHO_CELULA, TAMANHO_CELULA, DARKGRAY);

                if (mouseDentroTabuleiro && mouseX == x && mouseY == y) {
                    int bombasAoRedor = ContarBombasAoRedor(&jogo, x, y);
                    char buffer[16];
                    sprintf(buffer, "Bombas: %d", bombasAoRedor);
                    DrawText(buffer, 10, TAMANHO_TABULEIRO * TAMANHO_CELULA + 10, 20, BLACK);
                }
            }
        }

        DrawText(TextFormat("Vidas: %d", jogo.vidas), 10, TAMANHO_TABULEIRO * TAMANHO_CELULA + 30, 20, BLACK);
        EndDrawing();
    }

    UnloadSound(perdeu);
    UnloadSound(explosao);
    UnloadTexture(texturaBomba);
    UnloadMusicStream(vitoria);
    UnloadMusicStream(trilha);
    CloseAudioDevice();
    CloseWindow();
   
    return 0;
}
