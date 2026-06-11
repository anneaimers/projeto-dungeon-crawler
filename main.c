#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // input do teclado
#include <time.h>  // movimento aleatório 

// tamanho interno do mapa 
#define TAM_VILA 12
#define TAM_ANDAR1 12
#define TAM_ANDAR2 17
#define TAM_ANDAR3 27
#define VIDAS_INICIAIS 3

int playerY = 1, playerX = 1; // posição inicial do jogador (linha, coluna)
char player_simb = 'v';       // símbolo atual do jogador
int vidas = VIDAS_INICIAIS;

// a partir do 1 é uma opção
int arma_escolhida = 0;      
int andar_atual = 0;  
        
int chaves_coletadas = 0;     

// alerta do boss
int alerta_boss_exibido = 0;

// controla clonagem do boss
int contador_turnos = 0;

// pode clonar nessa coordenada?
int boss_pode_multiplicar = 1; 

// controle dos monstros ativos no mapa
typedef struct {
    int x; // coluna
    int y; // linha
    char tipo; // X, Y, Z
    int ativo;
} Inimigo;

Inimigo lista_monstros[100]; // evita estouro de memória com muitos clones
int total_monstros = 0;

char vila[TAM_VILA][TAM_VILA + 1] = {
{"************"},
{"*         L*"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*          *"},
{"*     N    *"},
{"************"}
};


char andar1[TAM_ANDAR1][TAM_ANDAR1 + 1] = {
{"************"},
{"*LD    *   *"},
{"****** *   *"},
{"*@kkkkk*  **"},
{"*kkkk****  *"},
{"*       *  *"},
{"*       *  *"},
{"*kkkk***** *"},
{"*kkkk*     *"},
{"*    *  ****"},
{"*          *"},
{"************"}

};


char andar2[TAM_ANDAR2][TAM_ANDAR2 + 1] = {
{"*****************"},
{"* *@D           *"},
{"* *********     *"},
{"*k #   #        *"},
{"*k   #   #      *"},
{"*k  #########   *"},
{"*k  #     #     *"},
{"* ###  L  #     *"},
{"*   #     #   ##*"},
{"*## #######     *"},
{"*            kkk*"},
{"*     ***    k  *"},
{"*     *OD    k @*"},
{"*     ***    k  *"},
{"*            kkk*"},
{"*               *"},
{"*****************"}
};


char andar3[TAM_ANDAR3][TAM_ANDAR3 + 1] = {
{"***************************"},
{"*O#                       *"},
{"*D#                       *"},
{"* #                       *"},
{"* #                       *"},
{"* #########               *"},
{"*    k    #               *"},
{"*  @ k    #               *"},
{"*#   k    #               *"},
{"* #########               *"},
{"*                         *"},
{"*                         *"},
{"*                         *"},
{"*                         *"},
{"*                         *"},
{"*@                        *"},
{"*                         *"},
{"*                         *"},
{"*                 *******D*"},
{"*                 *@  *   *"},
{"*#D######         *   *   *"},
{"*       #         *       *"},
{"*       #         *********"},
{"*   O   #                 *"},
{"*       #                 *"},
{"*       #                 *"},
{"***************************"}
};

// matriz para o andar_atual - ajusta o tamanho máximo da string e do caracter nulo
char mapa_ativo[TAM_ANDAR3][TAM_ANDAR3 + 1];
int tam_mapa_atual = TAM_VILA;

// funções
void tela_menu();
void tela_tutorial();
void iniciando_vila();
void carregar_andar(int andar);
void desenho_mapa();
void mover_jogador(char comando);
void interagir();
void atacar();
void turno_monstros();
void checar_colisoes();

int ler_inteiro(int min, int max) {
    int valor;
    while (1) {
        if (scanf("%d", &valor) != 1) {
            while (getchar() != '\n'); 
            printf("Opcao invalida. Digite um numero: ");
            continue;
        }
        while (getchar() != '\n');
        if (valor < min || valor > max) {
            printf("Opcao invalida. Digite entre %d e %d: ", min, max);
            continue;
        }
        return valor;
    }
}

int main() {
    srand(time(NULL));
    tela_menu();
    return 0;
}

void tela_menu() {
    int opcao = 0;
    while(opcao != 3) {
        system("cls");
        printf("====================\n");
        printf("  DUNGEON CRAWLER  \n");
        printf("====================\n\n");
        printf("1. Jogar\n");
        printf("2. Tutorial\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");
        opcao = ler_inteiro(1, 3);

        switch(opcao) {
            case 1:
                vidas = VIDAS_INICIAIS;
                arma_escolhida = 0;
                carregar_andar(0);
                break;
            case 2:
                tela_tutorial();
                break;
            case 3:
                system("cls");
                printf("\nCreditos: Desenvolvido por Anne Gracie.\nObrigada por jogar!\n");
                system("pause");
                break;
        }
    }
}

void tela_tutorial() {
    system("cls");
    printf("=== TUTORIAL ===\n");
    printf("Com a arma de sua preferencia, retome o controle da masmorra. Monstros rastejantes e caçadores invadiram os corredores edistribuiram espinhos perigosos por toda a parte bloqueando passagens.\nPreciso de voce para abrir o caminho ate a camara principal, onde o Imperador Zurg Zurg Zurg Zurg Zurg Zurg espera. Vai ousar desafia-lo?\n\n");
    printf("Comandos:\n w - Cima\n a - Esquerda\n s - Baixo\n d - Direita\n i - Interagir\n o - Atacar\n\n");
    printf("Simbolos:\n < - Jogador olhando para a esquerda\n ^ - Jogador olhando para cima\n > - Jogador olhando para a direita\n v - Jogador olhando para baixo\n * - Parede: o jogador nao pode passar\n # - Espinho: o jogador morre ao passar por cima\n k - Caixa: o jogador nao pode passar, mas pode ser destruida com ataque\n O - Botao: aciona efeitos especificos em cada andar\n D - Porta fechada: o jogador nao pode passar\n @ - Chave: abre uma porta fechada especifica ao interagir\n = - Porta aberta: o jogador pode passar\n L - Escada: leva o jogador para a proxima fase\n X - Monstro Tipo 1\n Y - Monstro Tipo 2\n Z - Boss Final\n\n");
    system("pause");
}

void carregar_andar(int andar) {
    andar_atual = andar;
    chaves_coletadas = 0;
    total_monstros = 0;
    alerta_boss_exibido = 0; 
    contador_turnos = 0;     
    
    boss_pode_multiplicar = 1; 

    int i, j;
    if (andar == 0) {
        tam_mapa_atual = TAM_VILA;
        playerY = 2; playerX = 2; player_simb = 'v';
        for(i=0; i<tam_mapa_atual; i++)
            for(j=0; j<tam_mapa_atual; j++) mapa_ativo[i][j] = vila[i][j];
    }
    else if (andar == 1) {
        tam_mapa_atual = TAM_ANDAR1;
        playerY = 1; playerX = 8; player_simb = '>';
        for(i=0; i<tam_mapa_atual; i++)
            for(j=0; j<tam_mapa_atual; j++) mapa_ativo[i][j] = andar1[i][j];
    }
    else if (andar == 2) {
        tam_mapa_atual = TAM_ANDAR2;
        playerY = 1; playerX = 1; player_simb = '<';
        for(i=0; i<tam_mapa_atual; i++)
            for(j=0; j<tam_mapa_atual; j++) mapa_ativo[i][j] = andar2[i][j];

        lista_monstros[total_monstros++] = (Inimigo){15, 1, 'X', 1};
        lista_monstros[total_monstros++] = (Inimigo){15, 15, 'X', 1};
    }
    else if (andar == 3) {
        tam_mapa_atual = TAM_ANDAR3;
        playerY = 7; playerX = 7; player_simb = '^';
        for(i=0; i<tam_mapa_atual; i++)
            for(j=0; j<tam_mapa_atual; j++) mapa_ativo[i][j] = andar3[i][j];
        
        lista_monstros[total_monstros++] = (Inimigo){25, 1, 'X', 1};
        lista_monstros[total_monstros++] = (Inimigo){25, 13, 'X', 1};
        lista_monstros[total_monstros++] = (Inimigo){25, 25, 'X', 1};
        lista_monstros[total_monstros++] = (Inimigo){14, 14, 'Y', 1};
        lista_monstros[total_monstros++] = (Inimigo){10, 20, 'Z', 1}; 
    }

    iniciando_vila();
}

void iniciando_vila() {
    char comando;
    while(vidas > 0 && andar_atual >= 0) {
        system("cls");
        printf("Vidas: %d | Andar: %d | Chaves: %d | Arma: %s\n", 
               vidas, andar_atual, chaves_coletadas, (arma_escolhida == 1) ? "Espada" : (arma_escolhida == 2) ? "Arco e Flecha" : (arma_escolhida == 3) ? "Cajado" : "Nenhuma");
        
        if(andar_atual == 3) {
            if (boss_pode_multiplicar) {
                printf("[CUIDADO]: O Imperador Zurg Zurg Zurg Zurg Zurg Zurg está criando clones que podem aparecer um qualquer lugar!");
            } else {
                printf("[BLOQUEIO]: O Imperador Zurg Zurg Zurg Zurg Zurg Zurg perdeu a capacidade de clonagem!\n");
            }
        }
        printf("\n");

        desenho_mapa();

        comando = getch();

        if (comando == 'w' || comando == 'a' || comando == 's' || comando == 'd') {
            mover_jogador(comando);
            turno_monstros();
            checar_colisoes();
        } else if (comando == 'i') {
            interagir();
        } else if (comando == 'o') {
            atacar();
            turno_monstros();
            checar_colisoes();
        }
    }
}

void desenho_mapa() {
    char display[TAM_ANDAR3][TAM_ANDAR3 + 1];
    int i, j;
    for(i = 0; i < tam_mapa_atual; i++) {
        for(j = 0; j < tam_mapa_atual; j++) {
            display[i][j] = mapa_ativo[i][j];
        }
    }

    for(i = 0; i < total_monstros; i++) {
        if(lista_monstros[i].ativo) {
            display[lista_monstros[i].y][lista_monstros[i].x] = lista_monstros[i].tipo;
        }
    }

    for(i = 0; i < tam_mapa_atual; i++) {
        for(j = 0; j < tam_mapa_atual; j++) {
            if(i == playerY && j == playerX) {
                printf("%c ", player_simb);
            } else {
                printf("%c ", display[i][j]);
            }
        }
        printf("\n");
    }
}

void mover_jogador(char comando) {
    int proxY = playerY;
    int proxX = playerX;

    if(comando == 'w') { proxY--; player_simb = '^'; }
    if(comando == 's') { proxY++; player_simb = 'v'; }
    if(comando == 'a') { proxX--; player_simb = '<'; }
    if(comando == 'd') { proxX++; player_simb = '>'; }

    if(proxY >= 0 && proxY < tam_mapa_atual && proxX >= 0 && proxX < tam_mapa_atual) {
        char destino = mapa_ativo[proxY][proxX];
        if(destino != '*' && destino != 'k' && destino != 'D') {
            playerY = proxY;
            playerX = proxX;
        }
    }
}

void interagir() {
    int frenteY = playerY;
    int frenteX = playerX;

    if(player_simb == '^') frenteY--;
    if(player_simb == 'v') frenteY++;
    if(player_simb == '<') frenteX--;
    if(player_simb == '>') frenteX++;

    if (frenteY < 0 || frenteY >= tam_mapa_atual || frenteX < 0 || frenteX >= tam_mapa_atual) return;

    char alvo = mapa_ativo[frenteY][frenteX];

// interagir NPC
    if (andar_atual == 0 && (mapa_ativo[playerY-1][playerX] == 'N' || mapa_ativo[playerY+1][playerX] == 'N' || mapa_ativo[playerY][playerX-1] == 'N' || mapa_ativo[playerY][playerX+1] == 'N')) {
        system("cls");
        printf("=== NPC FERREIRO ===\n");
        printf("Ola Guerreiro! Estou pronto para forjar uma arma poderosa.");
        printf("Escolha sua arma para a jornada:\n");
        printf("1. Espada (Ataque em area 3x2)\n");
        printf("2. Arco e Flecha (Linha reta de 4 celulas)\n");
        printf("3. Cajado (Ataque circular de 8 celulas)\n");
        printf("Faça sua escolha: ");
        arma_escolhida = ler_inteiro(1, 3);
        printf("\nArma escolhida com sucesso! Siga para a escada.\n");
        system("pause");
        return;
    }

// coleta chaves e abre portas específicas
    if (alvo == '@') {
        mapa_ativo[frenteY][frenteX] = ' '; 
        chaves_coletadas++;
        
        int portaY = -1, portaX = -1;

		
        if (andar_atual == 1) {
            if (frenteY == 3 && frenteX == 1) {
                portaY = 1; portaX = 2; 
            }
        } 

        else if (andar_atual == 2) {
            if (frenteY == 12 && frenteX == 15) {
                portaY = 1; portaX = 4; 
            } 
            else if (frenteY == 1 && frenteX == 3) {
                portaY = 12; portaX = 8; 
            }
        } 

        else if (andar_atual == 3) {
            if (frenteY == 7 && frenteX == 3) {
                portaY = 2; portaX = 1; 
            } 
            else if (frenteY == 15 && frenteX == 1) {
                portaY = 18; portaX = 25; 
            } 
            else if (frenteY == 19 && frenteX == 19) {
                portaY = 20; portaX = 2; 
            }
        }

        if (portaY != -1 && portaX != -1 && mapa_ativo[portaY][portaX] == 'D') {
            mapa_ativo[portaY][portaX] = '='; 
            printf("\nChave Coletada! Uma porta especifica sera aberta\n");
        } else {
            printf("\nChave Coletada! Guardada no seu inventario.\n");
        }
        system("pause");
    }

// botões com funções diferentes
    if (alvo == 'O') {
        // --- BOTÃO DO ANDAR 2 ---
        if(andar_atual == 2) {
            if (frenteY == 12 && frenteX == 7) { 
                printf("\n[BOTAO ATIVADO]: Removendo barreira de espinhos ao redor da escada...\n");
                int i, j;
                for(i = 5; i <= 9; i++){
                    for(j = 4; j <= 10; j++){
                        if(mapa_ativo[i][j] == '#'){
                            mapa_ativo[i][j] = ' '; 
                        }
                    }
                }
            }
        }
        // --- BOTÕES DO ANDAR 3 ---
        else if(andar_atual == 3) {
            // FUNÇÃO DO BOTÃO 1
            if (frenteY == 1 && frenteX == 1) {
               printf("\n[BOTAO ATIVADO]: Desativando espinhos protetores...\n");
                    int i, j;
                    for(i = 1; i <= 3; i++){
                        for(j = 2; j <= 2; j++){
                            if(mapa_ativo[i][j] == '#'){
                                mapa_ativo[i][j] = ' ';
                            }
                        }
                    }
            }
            // FUNÇÃO DO BOTÃO 2
            else if (frenteY == 23 && frenteX == 4) {
                int i;
                int copias_deletadas = 0;
                int primeiro_Z_encontrado = 0;

                boss_pode_multiplicar = 0; // trava clonagem

                // varre a lista de monstros para desativar os clones e limpar o mapa
                for (i = 0; i < total_monstros; i++) {
                    if (lista_monstros[i].tipo == 'Z' && lista_monstros[i].ativo) { 
                        if (!primeiro_Z_encontrado) {
                            primeiro_Z_encontrado = 1; // o primeiro 'Z' encontrado é o boss real, ele continua ativo
                        } else {
                            // clone que vira ' '
                            mapa_ativo[lista_monstros[i].y][lista_monstros[i].x] = ' ';
                            
                            // desativa o monstro na lista
                            lista_monstros[i].ativo = 0; 
                            copias_deletadas++;
                        }
                    }
                }

                printf("\n[BOTAO ATIVADO]: Onda de choque de energia acionada!\n");
                printf("\n[BLOQUEIO]: O Imperador Zurg Zurg Zurg Zurg Zurg Zurg perdeu a capacidade de clonagem!\n"); 
                
                if (copias_deletadas > 0) {
                    printf("[SUCESSO]: %d clones do Imperador Zurg Zurg Zurg Zurg Zurg Zurg Zurg foram derrotadas!\n", copias_deletadas);
                } else {
                    printf("Nenhum clone ativo do Imperador Zurg Zurg Zurg Zurg Zurg Zurg Zurg foi detectado no momento.\n");
                }
                if (primeiro_Z_encontrado) {
                    printf("Apenas o Imperador Zurg Zurg Zurg Zurg Zurg Zurg original permanece ativo no mapa. Acabe com ele!\n");
                }
            }
		}
        system("pause");
    }

    // 4. Mudar de Andar usando a Escada ('L')
    if (alvo == 'L') {
        if (arma_escolhida == 0) {
            printf("\nEscolha sua arma com o NPC primeiro.\n");
            system("pause");
        } else {
            printf("\nAvancando para o proximo andar...\n");
            system("pause");
            carregar_andar(andar_atual + 1);
        }
    }
}

void aplicar_dano(int y, int x) {
    if(y < 0 || y >= tam_mapa_atual || x < 0 || x >= tam_mapa_atual) return;

    if(mapa_ativo[y][x] == 'k') {
        mapa_ativo[y][x] = ' ';
    }

    int i;
    for(i = 0; i < total_monstros; i++) {
        if(lista_monstros[i].ativo && lista_monstros[i].y == y && lista_monstros[i].x == x) {
            lista_monstros[i].ativo = 0;
            
            if(lista_monstros[i].tipo == 'Z') {
                int restam_bosses = 0;
                int j;
                
                for(j = 0; j < total_monstros; j++) {
                    if(lista_monstros[j].tipo == 'Z' && lista_monstros[j].ativo == 1) {
                        restam_bosses = 1;
                        break;
                    }
                }

                if(!restam_bosses) {
                    system("cls");
                    printf("=======================================\n");
                    printf("                 VITORIA               \n");
                    printf("=======================================\n");
                    printf("Voce derrotou o boss final e todas as suas copias!\n");
                    printf("Voce restaurou a paz do reino.\n");
                    system("pause");
                    andar_atual = -1;
                } else {
                    printf("\n[INFO]: -1 inimigo! Continue a batalha\n");
                    system("pause");
                }
            }
        }
    }
}

void atacar() {
    if(arma_escolhida == 0) return;

    if(arma_escolhida == 1) {
        int i;
        if(player_simb == '^') {
            for(i = -1; i <= 1; i++) { aplicar_dano(playerY-1, playerX+i); aplicar_dano(playerY-2, playerX+i); }
        } else if(player_simb == 'v') {
            for(i = -1; i <= 1; i++) { aplicar_dano(playerY+1, playerX+i); aplicar_dano(playerY+2, playerX+i); }
        } else if(player_simb == '<') {
            for(i = -1; i <= 1; i++) { aplicar_dano(playerY+i, playerX-1); aplicar_dano(playerY+i, playerX-2); }
        } else if(player_simb == '>') {
            for(i = -1; i <= 1; i++) { aplicar_dano(playerY+i, playerX+1); aplicar_dano(playerY+i, playerX+2); }
        }
    }
    else if(arma_escolhida == 2) {
        int i;
        for(i = 1; i <= 4; i++) {
            if(player_simb == '^') aplicar_dano(playerY-i, playerX);
            if(player_simb == 'v') aplicar_dano(playerY+i, playerX);
            if(player_simb == '<') aplicar_dano(playerY, playerX-i);
            if(player_simb == '>') aplicar_dano(playerY, playerX+i);
        }
    }
    else if(arma_escolhida == 3) {
        int dy, dx;
        for(dy = -1; dy <= 1; dy++) {
            for(dx = -1; dx <= 1; dx++) {
                if(dy != 0 || dx != 0) aplicar_dano(playerY + dy, playerX + dx);
            }
        }
    }
}

void turno_monstros() {
    int mY[] = {-1, 1, 0, 0};
    int mX[] = {0, 0, -1, 1};

    int i;
    int monstros_atuais_turno = total_monstros; 

    if (andar_atual == 3 && boss_pode_multiplicar) {
        contador_turnos++;
    }

    for(i = 0; i < monstros_atuais_turno; i++) {
        if(!lista_monstros[i].ativo) continue;

        if(lista_monstros[i].tipo == 'X') {
            int r = rand() % 4;
            int nY = lista_monstros[i].y + mY[r];
            int nX = lista_monstros[i].x + mX[r];
            if(nY >= 0 && nY < tam_mapa_atual && nX >= 0 && nX < tam_mapa_atual && mapa_ativo[nY][nX] != '*') {
                lista_monstros[i].y = nY;
                lista_monstros[i].x = nX;
            }
        }
        else if(lista_monstros[i].tipo == 'Y') {
            int difY = playerY - lista_monstros[i].y;
            int difX = playerX - lista_monstros[i].x;
            if(abs(difY) >= abs(difX) && difY != 0) {
                int nY = lista_monstros[i].y + ((difY > 0) ? 1 : -1);
                if(mapa_ativo[nY][lista_monstros[i].x] != '*') lista_monstros[i].y = nY;
            } else if(difX != 0) {
                int nX = lista_monstros[i].x + ((difX > 0) ? 1 : -1);
                if(mapa_ativo[lista_monstros[i].y][nX] != '*') lista_monstros[i].x = nX;
            }
        }
        else if(lista_monstros[i].tipo == 'Z') {
            int r = rand() % 4;
            int nY = lista_monstros[i].y + mY[r];
            int nX = lista_monstros[i].x + mX[r];

            if(nY >= 0 && nY < tam_mapa_atual && nX >= 0 && nX < tam_mapa_atual && mapa_ativo[nY][nX] != '*') {
                lista_monstros[i].y = nY;
                lista_monstros[i].x = nX;
            }
        }
    }

    if (andar_atual == 3 && boss_pode_multiplicar && contador_turnos >= 30) {
        int houve_multiplicacao = 0;

        for (i = 0; i < monstros_atuais_turno; i++) {
            if (lista_monstros[i].tipo == 'Z' && lista_monstros[i].ativo) {
                int c;
                for (c = 0; c < 2; c++) {
                    if (total_monstros < 100) {
                        int spawnY, spawnX, tentativas = 0;
                        do {
                            spawnY = rand() % tam_mapa_atual;
                            spawnX = rand() % tam_mapa_atual;
                            tentativas++;
                        } while ((mapa_ativo[spawnY][spawnX] != ' ' || (spawnY == playerY && spawnX == playerX)) && tentativas < 100);

                        lista_monstros[total_monstros++] = (Inimigo){spawnY, spawnX, 'Z', 1};
                        houve_multiplicacao = 1;
                    }
                }
            }
        }

        contador_turnos = 0;

        
    }
}

void checar_colisoes() {
    int perdeu_vida = 0;

    if(mapa_ativo[playerY][playerX] == '#') {
        perdeu_vida = 1;
    }

    int i;
    for(i = 0; i < total_monstros; i++) {
        if(lista_monstros[i].ativo && lista_monstros[i].y == playerY && lista_monstros[i].x == playerX) {
            perdeu_vida = 1;
        }
    }

    if(perdeu_vida) {
        vidas--; 
        printf("\nVoce foi atingido! Perdeu 1 vida.\n");
        system("pause");

        if(vidas <= 0) {
            system("cls");
            printf("=======================\n");
            printf("        GAME OVER      \n");
            printf("=======================\n");
            system("pause");
            andar_atual = -1; 
        } else {
            carregar_andar(andar_atual); 
        }
    }
}
