#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Configs pro Terminal ---
#ifdef _WIN32
#include <windows.h>
void fast_clear() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(hOut, &csbi)) {
        FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
        SetConsoleCursorPosition(hOut, coord);
    }
}
#define PAUSA_MS(x) Sleep(x)
#define LIMPA_TELA fast_clear()
#else
#include <unistd.h>
#define PAUSA_MS(x) usleep((x) * 1000)
#define LIMPA_TELA printf("\033[H\033[J")
#endif

// --- Definições ---
#define MAX_NOME 50
#define ARQUIVO_SAVE "hall_da_fama_rpg.csv"
#define ASCII_HEIGHT 13
#define MAX_DECK 40
#define MAO_TAMANHO 3
#define MANA_MAX 3
#define MAX_LOG_COMBATE 500 
#define MAX_LOG_WIDTH 45 // Largura reduzida para evitar quebra de linha visual

// Settings Globais
int modo_dificil = 0; 

// Enums
typedef enum { MAGO = 1, LADINO = 2, GUERREIRO = 3 } Classe;
typedef enum { TIPO_ATK, TIPO_DEF, TIPO_ESQ, TIPO_ESP } TipoMovimento;

// Struct de Carta (Movimento)
typedef struct {
    char nome[25];
    char descricao[40];
    char origem[20]; 
    TipoMovimento tipo;
    int custo_mana;
    int valor_base;
    int eh_roubo;
    int nivel; // 0 = Base, 1+ = Melhorada
} Carta;

// Struct do Personagem
typedef struct {
    char nome[MAX_NOME];
    Classe classe;
    
    // Stats
    int hp, hp_max;
    int mana, mana_max;
    int escudo_temp; 
    int esquiva_acumulada; 

    // Equipamento
    char item_arma[MAX_NOME];
    char item_armadura[MAX_NOME];
    char item_acessorio[MAX_NOME];

    // Deck
    Carta deck[MAX_DECK];
    int qtd_cartas;
    Carta mao[MAO_TAMANHO];

    // IA
    Carta intencao[3];
    int qtd_intencao;

    // Movimentos Conhecidos
    char conhecidos[30][100];
    int qtd_conhecidos;

    int eh_jogador;
} Gladiador;

// Global Log
char log_global[MAX_LOG_COMBATE][100];
int log_contador = 0;

// --- ARTES ASCII ---
const char *ART_GUERREIRO[] = {
"      .---.      ", "     /\\     ", "     ( '.' )     ", "      \\-/      ", "    .-''V'``-.  ", "   /   /   \\   \\ ", "  /   /     \\   \\", " /   /       \\   \\", " \\   \\       /   /", "  \\___\\     /___/", "   |   |   |   | ", "   |   |   |   | ", "   '---'   '---' " }; 
const char *ART_MAGO[] = { 
"      _   _      ", "     ( ^ )     ", "     |  |  |     ", "    _\\  |  /_    ", "   '  \\ | /  '   ", "  / _  \\|/  _ \\  ", " ( ( \\  |  / ) ) ", "  \\ \\/  |  \\/ /  ", "   \\    |    /   ", "    |   |   |    ", "   /    |    \\   ", "  (_/\\__|__/\\_)  ", "     |__|__|     " }; 
const char *ART_LADINO[] = { 
"      .....      ", "     C . . D     ", "      \\_-_/      ", "    /     \\    ", "   /  /   \\  \\   ", "  |  /     \\  |  ", " /  /| . . |\\  \\ ", " \\  \\| . . |/  / ", "  \\  \\     /  /  ", "   \\ |     | /   ", "    ||     ||    ", "    ||     ||    ", "   [__]   [__]   " }; 
const char *ART_MORTE[] = { 
"   _.--\"\"\"--._   ", "  /  _   _  \\  ", " |  (x) (x)  |  ", " |     |     |  ", " |     ^     |  ", "  \\  _____  /   ", "   -.   .-'    ", "     | |      ", "    |     |     ", "    | R.I.P |   ", "    |     |     ", "    ||     ", "   /_______\\    "
};

// --- Protótipos ---
int d100();
void game_loop();
void mostrar_hall_fama();
void mostrar_regras();
void mostrar_lore();
void salvar_gameover(Gladiador p, char *causa, int round);
void log_add(char *msg);
void log_reset();
void construir_deck(Gladiador *g);
void esperar_enter(); 
Carta criar_carta(char *n, char *d, char *o, TipoMovimento t, int c, int v);
int obter_indice_upgrade(Gladiador *g, int *indices_ignorados, int qtd_ignorados);
void gerar_descricao_dinamica(char* buffer, Carta c, int valor);

// --- MAIN (MENU) ---
int main() {
    setlocale(LC_ALL, ""); 
    srand(time(NULL));
    int op;
    do {
        LIMPA_TELA;
        printf("\n+=======================================+\n");
        printf("|   ALTAIR, o Renascido: ROGUELIKE RPG  |\n");
        printf("+=======================================+\n");
        printf("| 1. Novo Jogo                          |\n");
        printf("| 2. Hall da Fama                       |\n");
        printf("| 3. Manual de Regras                   |\n");
        printf("| 4. Dificuldade: %-10s            |\n", (modo_dificil ? "DIFÍCIL" : "NORMAL"));
        printf("| 5. Lore                               |\n");
        printf("| 6. Sair                               |\n");
        printf("+=======================================+\n");
        printf("Opção > ");
        if(scanf("%d", &op) != 1) op = 0; 
        while(getchar() != '\n');

        switch (op) {
            case 1: game_loop(); break;
            case 2: mostrar_hall_fama(); break;
            case 3: mostrar_regras(); break;
            case 4: modo_dificil = !modo_dificil; break;
            case 5: mostrar_lore(); break;
            case 6: printf("Saindo...\n"); break;
            default: printf("Opção inválida.\n"); PAUSA_MS(500);
        }
    } while (op != 6);
    return 0;
}

// --- LOGICA DE CARTAS E ITENS ---

Carta criar_carta(char* n, char* d, char* o, TipoMovimento t, int c, int v) {
    Carta k;
    strcpy(k.nome, n); strcpy(k.descricao, d); strcpy(k.origem, o);
    k.tipo = t; k.custo_mana = c; k.valor_base = v;
    k.eh_roubo = 0;
    k.nivel = 0;
    return k;
}

void adicionar_cartas_item(Gladiador *g, char *item, char *slot) {
    if (strcmp(item, "Nada") == 0) return;

    if (strstr(item, "Espada")) { 
        g->deck[g->qtd_cartas++] = criar_carta("Lacerar", "7 Dano", item, TIPO_ATK, 2, 7);
        g->deck[g->qtd_cartas++] = criar_carta("Contra-Ataque", "5 Escudo", item, TIPO_DEF, 1, 5);
    }
    else if (strstr(item, "Cajado")) { 
        g->deck[g->qtd_cartas++] = criar_carta("Inferno", "12 Dano", item, TIPO_ATK, 3, 12);
        g->deck[g->qtd_cartas++] = criar_carta("Congelar", "6 Dano + Stun", item, TIPO_ATK, 2, 6);
    }
    else if (strstr(item, "Adaga")) { 
        g->deck[g->qtd_cartas++] = criar_carta("Assassinato", "11 Dano", item, TIPO_ATK, 3, 11);
        g->deck[g->qtd_cartas++] = criar_carta("Bomba Fumaça", "60% Esquiva", item, TIPO_ESQ, 2, 60);
    }

    if (strstr(item, "Placas")) {
        g->deck[g->qtd_cartas++] = criar_carta("Fortaleza", "12 Escudo", item, TIPO_DEF, 2, 12);
        g->deck[g->qtd_cartas++] = criar_carta("Esmagar", "10 Dano Pesado", item, TIPO_ATK, 2, 10);
    }
    else if (strstr(item, "Manto")) {
        g->deck[g->qtd_cartas++] = criar_carta("Reflexo", "6 Escudo", item, TIPO_DEF, 2, 6);
        g->deck[g->qtd_cartas++] = criar_carta("Teleporte", "40% Esquiva", item, TIPO_ESQ, 2, 40);
    }
    else if (strstr(item, "Couro")) {
        g->deck[g->qtd_cartas++] = criar_carta("Sombra", "30% Esquiva", item, TIPO_ESQ, 1, 30);
        g->deck[g->qtd_cartas++] = criar_carta("Revidar", "4 Dano Rápido", item, TIPO_ATK, 0, 4);
    }

    if (strstr(item, "Anel")) {
        g->deck[g->qtd_cartas++] = criar_carta("Sorte", "Cura 5 HP", item, TIPO_ESP, 2, 5);
        g->deck[g->qtd_cartas++] = criar_carta("Foco", "Recupera Mana", item, TIPO_ESP, 0, 0);
    }
}

int obter_indice_upgrade(Gladiador *g, int *indices_ignorados, int qtd_ignorados) {
    int min_nivel = 999;
    for (int i = 0; i < g->qtd_cartas; i++) {
        if (g->deck[i].nivel < min_nivel) min_nivel = g->deck[i].nivel;
    }
    for (int i = 0; i < g->qtd_cartas; i++) {
        if (g->deck[i].nivel == min_nivel) {
            int ignorar = 0;
            for(int j=0; j<qtd_ignorados; j++) {
                if (indices_ignorados[j] == i) ignorar = 1;
            }
            if (!ignorar) return i;
        }
    }
    return -1; 
}

void gerar_descricao_dinamica(char* buffer, Carta c, int valor) {
    if(strstr(c.nome, "Esmagar")) sprintf(buffer, "%d Dano Pesado", valor);
    else if(strstr(c.nome, "Revidar")) sprintf(buffer, "%d Dano Rápido", valor);
    else if(strstr(c.nome, "Congelar")) sprintf(buffer, "%d Dano + Stun", valor);
    else if(strstr(c.nome, "Esquiva") || strstr(c.nome, "Bomba") || strstr(c.nome, "Teleporte") || strstr(c.nome, "Sombra")) 
        sprintf(buffer, "%d%% Esquiva", valor);
    else if(strstr(c.nome, "Sorte") || strstr(c.nome, "Aguentar")) 
        sprintf(buffer, "Cura %d HP", valor);
    else if(c.tipo == TIPO_DEF) sprintf(buffer, "%d Escudo", valor);
    else if(c.tipo == TIPO_ATK) sprintf(buffer, "%d Dano", valor);
    else strcpy(buffer, c.descricao); 
}

void construir_deck(Gladiador *g) {
    typedef struct { char nome[25]; int lvl; } BackupLvl;
    BackupLvl backups[MAX_DECK];
    int qtd_bkp = g->qtd_cartas;
    for(int i=0; i<g->qtd_cartas; i++) {
        strcpy(backups[i].nome, g->deck[i].nome);
        backups[i].lvl = g->deck[i].nivel;
    }

    g->qtd_cartas = 0;

    if (g->classe == GUERREIRO) {
        g->deck[g->qtd_cartas++] = criar_carta("Soco", "3 Dano", "Base", TIPO_ATK, 1, 3);
        g->deck[g->qtd_cartas++] = criar_carta("Defender", "4 Escudo", "Base", TIPO_DEF, 1, 4);
        g->deck[g->qtd_cartas++] = criar_carta("Aguentar", "Cura 3 HP", "Base", TIPO_ESP, 2, 3);
        g->deck[g->qtd_cartas++] = criar_carta("Estocada", "6 Dano", "Base", TIPO_ATK, 2, 6);
        g->deck[g->qtd_cartas++] = criar_carta("Escudada", "5 Dano", "Base", TIPO_ATK, 1, 5);
        g->deck[g->qtd_cartas++] = criar_carta("Defesa Total", "8 Escudo", "Base", TIPO_DEF, 2, 8);

    } else if (g->classe == MAGO) {
        g->deck[g->qtd_cartas++] = criar_carta("Dardo", "4 Dano", "Base", TIPO_ATK, 1, 4);
        g->deck[g->qtd_cartas++] = criar_carta("Barreira", "3 Escudo", "Base", TIPO_DEF, 1, 3);
        g->deck[g->qtd_cartas++] = criar_carta("Concentrar", "Recupera 1 Mana", "Base", TIPO_ESP, 0, 0);
        g->deck[g->qtd_cartas++] = criar_carta("Bola de Fogo", "9 Dano", "Base", TIPO_ATK, 2, 9);
        g->deck[g->qtd_cartas++] = criar_carta("Raio", "5 Dano", "Base", TIPO_ATK, 1, 5);
        g->deck[g->qtd_cartas++] = criar_carta("Dreno", "Recupera Mana", "Base", TIPO_ESP, 0, 0);

    } else { // LADINO
        g->deck[g->qtd_cartas++] = criar_carta("Corte", "2 Dano", "Base", TIPO_ATK, 0, 2);
        g->deck[g->qtd_cartas++] = criar_carta("Esquiva", "20% Esquiva", "Base", TIPO_ESQ, 1, 20);
        g->deck[g->qtd_cartas++] = criar_carta("Truque", "3 Dano", "Base", TIPO_ATK, 1, 3);
        g->deck[g->qtd_cartas++] = criar_carta("Traicao", "7 Dano", "Base", TIPO_ATK, 2, 7);
        g->deck[g->qtd_cartas++] = criar_carta("Golpe Baixo", "6 Dano", "Base", TIPO_ATK, 1, 6);
        Carta c3 = criar_carta("Roubar", "Copia Carta", "Base", TIPO_ESP, 1, 0); c3.eh_roubo = 1;
        g->deck[g->qtd_cartas++] = c3;
    }

    adicionar_cartas_item(g, g->item_arma, "Arma");
    adicionar_cartas_item(g, g->item_armadura, "Armor");
    adicionar_cartas_item(g, g->item_acessorio, "Acc");

    for(int i=0; i<g->qtd_cartas; i++) {
        for(int j=0; j<qtd_bkp; j++) {
            if(strcmp(g->deck[i].nome, backups[j].nome) == 0) {
                g->deck[i].nivel = backups[j].lvl;
                g->deck[i].valor_base += (2 * g->deck[i].nivel);
                
                gerar_descricao_dinamica(g->deck[i].descricao, g->deck[i], g->deck[i].valor_base);
                break;
            }
        }
    }
}

// --- SISTEMA DE COMBATE ---

void log_reset() {
    log_contador = 0;
    for(int i=0; i<MAX_LOG_COMBATE; i++) strcpy(log_global[i], "");
}

// FUNÇÃO DE LOG COM QUEBRA AUTOMÁTICA
void log_add(char *msg) {
    if (log_contador >= MAX_LOG_COMBATE) return;

    int len = strlen(msg);
    
    if (len <= MAX_LOG_WIDTH) {
        strncpy(log_global[log_contador], msg, 99);
        log_contador++;
    } else {
        char part1[MAX_LOG_WIDTH + 1];
        strncpy(part1, msg, MAX_LOG_WIDTH);
        part1[MAX_LOG_WIDTH] = '\0';
        
        strncpy(log_global[log_contador], part1, 99);
        log_contador++;
        
        if (log_contador < MAX_LOG_COMBATE) {
            log_add(msg + MAX_LOG_WIDTH);
        }
    }
}

void executar_acao(Gladiador *atk, Gladiador *def, Carta c) {
    char buffer[60];

    int ja_viu = 0;
    // CORREÇÃO: Inicialização de qtd_conhecidos agora é garantida no game_loop
    for(int i=0; i<def->qtd_conhecidos; i++) {
        if(strstr(def->conhecidos[i], c.nome)) ja_viu=1;
    }
    
    if(!ja_viu && def->qtd_conhecidos < 20) { 
        sprintf(def->conhecidos[def->qtd_conhecidos++], "%s: %s", c.nome, c.descricao); 
    }

    if (c.eh_roubo) {
        if (def->qtd_cartas > 0) {
            Carta roubada = def->deck[rand() % def->qtd_cartas];
            roubada.custo_mana = 0; 
            sprintf(buffer, "%s ROUBOU e usou %s!", atk->nome, roubada.nome);
            log_add(buffer);
            executar_acao(atk, def, roubada); 
            return;
        } else {
            log_add("Nada para roubar!"); return;
        }
    }

    int dano = c.valor_base;

    if (c.tipo == TIPO_ATK) {
        if (def->esquiva_acumulada > 0 && d100() <= def->esquiva_acumulada) {
            sprintf(buffer, "%s atacou mas %s ESQUIVOU!", atk->nome, def->nome);
            log_add(buffer);
            def->esquiva_acumulada = 0; 
            return;
        }
        
        int passante = dano;
        if (def->escudo_temp > 0) {
            if (def->escudo_temp >= passante) {
                def->escudo_temp -= passante;
                passante = 0;
                sprintf(buffer, "%s bloqueou o dano com escudo!", def->nome);
            } else {
                passante -= def->escudo_temp;
                def->escudo_temp = 0;
            }
        }
        
        if (passante > 0) {
            def->hp -= passante;
            sprintf(buffer, "%s usou %s: %d Dano!", atk->nome, c.nome, passante);
        } else if (def->escudo_temp > 0 && passante == 0) {
        }
        
    } else if (c.tipo == TIPO_DEF) {
        atk->escudo_temp += dano;
        sprintf(buffer, "%s %s: +%d Escudo.", atk->nome, c.nome, dano);
        
    } else if (c.tipo == TIPO_ESQ) {
        atk->esquiva_acumulada += dano;
        if (atk->esquiva_acumulada > 80) atk->esquiva_acumulada = 80;
        sprintf(buffer, "%s preparou esquiva: %d%%.", atk->nome, atk->esquiva_acumulada);
        
    } else if (c.tipo == TIPO_ESP) {
        if (strstr(c.nome, "Cura") || strstr(c.nome, "Sorte") || strstr(c.nome, "Aguentar")) {
            atk->hp += dano; if(atk->hp > atk->hp_max) atk->hp = atk->hp_max;
            sprintf(buffer, "%s recuperou %d HP.", atk->nome, dano);
        } else {
            atk->mana++; if(atk->mana > MANA_MAX) atk->mana = MANA_MAX;
            sprintf(buffer, "%s recuperou Mana.", atk->nome);
        }
    }

    if(strlen(buffer) > 0) log_add(buffer);
}

// Helper para centralizar nome com #
void imprimir_barra_nome(char* nome, int largura_total) {
    int len = strlen(nome);
    int total_padding = largura_total - len - 2; 
    int pad_left = total_padding / 2;
    int pad_right = total_padding - pad_left;
    
    for(int i=0; i<pad_left; i++) printf("#");
    printf(" %s ", nome);
    for(int i=0; i<pad_right; i++) printf("#");
}

// --- RENDERIZACAO ---

void render_combate(Gladiador p, Gladiador e, int round) {
    LIMPA_TELA;
    
    const char **art_p = (p.classe == MAGO ? ART_MAGO : (p.classe == LADINO ? ART_LADINO : ART_GUERREIRO));
    const char **art_e = (e.classe == MAGO ? ART_MAGO : (e.classe == LADINO ? ART_LADINO : ART_GUERREIRO));

    char panel[50][60];
    for(int i=0; i<50; i++) strcpy(panel[i], "");
    
    int l = 0;
    sprintf(panel[l++], "--- SEU DECK ---");

    void fmt_card(int idx, char* dest) {
        if(p.deck[idx].nivel > 0)
            sprintf(dest, " . %s+%d (%d): %d Val", p.deck[idx].nome, p.deck[idx].nivel, p.deck[idx].custo_mana, p.deck[idx].valor_base);
        else
            sprintf(dest, " . %s (%d): %s", p.deck[idx].nome, p.deck[idx].custo_mana, p.deck[idx].descricao);
    }

    sprintf(panel[l++], "BASE:");
    for(int i=0; i<p.qtd_cartas; i++) if(strcmp(p.deck[i].origem, "Base")==0) fmt_card(i, panel[l++]);

    if(strcmp(p.item_arma, "Nada")!=0) {
        sprintf(panel[l++], "ARMA: %s", p.item_arma);
        for(int i=0; i<p.qtd_cartas; i++) if(strcmp(p.deck[i].origem, p.item_arma)==0) fmt_card(i, panel[l++]);
    }
    if(strcmp(p.item_armadura, "Nada")!=0) {
        sprintf(panel[l++], "ARMOR: %s", p.item_armadura);
        for(int i=0; i<p.qtd_cartas; i++) if(strcmp(p.deck[i].origem, p.item_armadura)==0) fmt_card(i, panel[l++]);
    }
    if(strcmp(p.item_acessorio, "Nada")!=0) {
        sprintf(panel[l++], "ACES: %s", p.item_acessorio);
        for(int i=0; i<p.qtd_cartas; i++) if(strcmp(p.deck[i].origem, p.item_acessorio)==0) fmt_card(i, panel[l++]);
    }

    // --- HUD ---
    printf("=== ROUND %d ===\n", round);
    
    char p_class_str[15], e_class_str[15];
    strcpy(p_class_str, (p.classe == MAGO ? "Mago" : (p.classe == LADINO ? "Ladino" : "Guerreiro")));
    strcpy(e_class_str, (e.classe == MAGO ? "Mago" : (e.classe == LADINO ? "Ladino" : "Guerreiro")));

    char buf_nome_p[MAX_NOME + 10], buf_classe_p[20], buf_nome_e[MAX_NOME + 10], buf_classe_e[20];
    sprintf(buf_nome_p, "[%s]", p.nome);
    sprintf(buf_classe_p, "(%s)", p_class_str);
    sprintf(buf_nome_e, "[%s]", e.nome);
    sprintf(buf_classe_e, "(%s)", e_class_str);

    const char* fmt_p = "%-7s %-22s %-12s HP:%3d/%-3d | ESC:%3d | ESQ:%3d%% | MP:%d\n";
    const char* fmt_e = "%-7s %-22s %-12s HP:%3d/%-3d | ESC:%3d | ESQ:%3d%% | MP:%d\n";

    printf(fmt_p, "VOCÊ", buf_nome_p, buf_classe_p, p.hp, p.hp_max, p.escudo_temp, p.esquiva_acumulada, p.mana);
    printf(fmt_e, "ALTAIR", buf_nome_e, buf_classe_e, e.hp, e.hp_max, e.escudo_temp, e.esquiva_acumulada, e.mana);

    char intencao_str[100] = "INTENÇÃO: ";
    if (e.qtd_intencao > 0) {
        for(int i=0; i<e.qtd_intencao; i++) {
            char tmp[30]; sprintf(tmp, "[%s] ", e.intencao[i].nome);
            strcat(intencao_str, tmp);
        }
    } else { strcat(intencao_str, "???"); }
    printf("%s\n", intencao_str);

    printf("--------------------------------------------------------------------------------------------------------------------------\n");
    
    int max_rows = (l > ASCII_HEIGHT) ? l : ASCII_HEIGHT;
    int start_log = (log_contador > (max_rows-1)) ? log_contador - (max_rows-1) : 0;
    
    for (int i = 0; i < max_rows; i++) {
        char col_art[42] = "";
        if (i < ASCII_HEIGHT) sprintf(col_art, "%-18s    %-18s", art_p[i], art_e[i]);
        else strcpy(col_art, "                                        ");
        
        char col_deck[45] = "";
        if (i < l) sprintf(col_deck, "%-38.38s", panel[i]); // Truncate at 38
        else sprintf(col_deck, "%-38s", "");

        char col_log[60] = "";
        if (i == 0) {
            strcpy(col_log, " | --- HISTÓRICO ---");
        } else {
            int log_idx = start_log + (i-1);
            if (log_idx < log_contador) sprintf(col_log, " | %.45s", log_global[log_idx]); // Truncate at 45
        }

        printf("%s | %s%s\n", col_art, col_deck, col_log);
    }

    printf("--------------------------------------------------------------------------------------------------------------------------\n");

    int tem_jogada = 0;
    for(int i=0; i<MAO_TAMANHO; i++) {
        if (p.mao[i].custo_mana != -1 && p.mana >= p.mao[i].custo_mana) {
            tem_jogada = 1;
        }
    }

    char status_msg[100];
    if (e.hp <= 0) strcpy(status_msg, "Altair caiu! [Enter] p/ Recompensa!");
    else if (tem_jogada) strcpy(status_msg, "JOGUE OU PASSE (4)");
    else strcpy(status_msg, "PASSE O TURNO (4)");

    if (p.hp > 0) {
        printf("SUA MÃO:%45s %s\n", "", "INSTRUÇÃO");
        
        for(int i=0; i<MAO_TAMANHO; i++) {
            char card_str[60];
            if (p.mao[i].custo_mana > -1) {
                if(p.mao[i].nivel > 0)
                    sprintf(card_str, "[%d] %s+%d (MP:%d) - Val: %d", i+1, p.mao[i].nome, p.mao[i].nivel, p.mao[i].custo_mana, p.mao[i].valor_base);
                else
                    sprintf(card_str, "[%d] %s (MP:%d) - %s", i+1, p.mao[i].nome, p.mao[i].custo_mana, p.mao[i].descricao);
            } else {
                sprintf(card_str, "[%d] -- USADA --", i+1);
            }

            char box_line[100] = "";
            if (i == 0) strcpy(box_line, "+---------------------------------------+");
            else if (i == 1) sprintf(box_line, "| %-37s |", status_msg);
            else if (i == 2) strcpy(box_line, "+---------------------------------------+");

            printf("%-50s   %s\n", card_str, box_line);
        }
        
        if (e.hp > 0) {
            printf("[4] Passar Turno\n[9] Menu (Desistir/Conhecidos)\n> ");
        } else {
            printf("> ");
        }
    }
}

// --- GAME LOOP ---

void game_loop() {
    char nome[MAX_NOME];
    LIMPA_TELA;
    printf("Digite o nome do seu Herói: ");
    fgets(nome, MAX_NOME, stdin); nome[strcspn(nome, "\n")] = 0;

    printf("\nEscolha sua classe:\n[1] Mago (Dano Alto/Frágil)\n[2] Ladino (Roubo/Estratégia)\n[3] Guerreiro (Tank/Defesa Ofensiva)\n> ");
    int op; scanf("%d", &op); while(getchar()!='\n');
    Classe classe_escolhida = (op >= 1 && op <= 3) ? (Classe)op : GUERREIRO;

    Gladiador player;
    strcpy(player.nome, nome); player.classe = classe_escolhida; player.eh_jogador = 1;
    player.mana_max = MANA_MAX; player.escudo_temp = 0; player.esquiva_acumulada = 0;
    player.qtd_conhecidos = 0;

    strcpy(player.item_arma, "Nada");
    strcpy(player.item_armadura, "Nada");
    strcpy(player.item_acessorio, "Nada");

    if(classe_escolhida == GUERREIRO) { player.hp_max=30; }
    else if(classe_escolhida == MAGO) { player.hp_max=20; }
    else { player.hp_max=25; }
    player.hp = player.hp_max;

    construir_deck(&player);

    int round = 1;

    while (player.hp > 0) {
        Gladiador altair;
        Classe c_bot = (rand() % 3) + 1;
        altair.classe = c_bot; altair.eh_jogador = 0;
        altair.mana_max = MANA_MAX; altair.escudo_temp = 0; altair.esquiva_acumulada = 0;
        
        // CORREÇÃO CRÍTICA: Inicializa a pokedex do inimigo
        altair.qtd_conhecidos = 0;

        if (round == 1) {
            strcpy(altair.nome, "Altair (Iniciante)");
            altair.hp_max = (modo_dificil ? 15 : 10);
            strcpy(altair.item_arma, "Nada"); strcpy(altair.item_armadura, "Nada"); strcpy(altair.item_acessorio, "Nada");
        } else {
            strcpy(altair.nome, "Altair (Renascido)");
            if (modo_dificil) {
                altair.hp_max = 25 + (round * 5);
            } else {
                altair.hp_max = 18 + (round * 3);
            }
            
            if (c_bot == GUERREIRO) { strcpy(altair.item_arma, "Espada"); strcpy(altair.item_armadura, "Placas"); }
            else if (c_bot == MAGO) { strcpy(altair.item_arma, "Cajado"); strcpy(altair.item_armadura, "Manto"); }
            else { strcpy(altair.item_arma, "Adaga"); strcpy(altair.item_armadura, "Couro"); }
            strcpy(altair.item_acessorio, "Anel");
        }
        altair.hp = altair.hp_max;
        construir_deck(&altair);

        log_reset();
        log_add("Novo combate iniciado!");
        int fugiu = 0;

        while (player.hp > 0 && altair.hp > 0 && !fugiu) {
            player.mana = player.mana_max; 
            altair.mana = altair.mana_max; 
            
            int indices[MAX_DECK];
            for(int i=0; i < player.qtd_cartas; i++) indices[i] = i;
            for(int i = player.qtd_cartas - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = indices[i]; indices[i] = indices[j]; indices[j] = temp;
            }
            for(int i=0; i<MAO_TAMANHO; i++) player.mao[i] = player.deck[indices[i]];

            altair.qtd_intencao = 0;
            int mana_bot = altair.mana;
            for(int i=0; i<3; i++) {
                Carta c = altair.deck[rand() % altair.qtd_cartas];
                if (c.custo_mana <= mana_bot) {
                    altair.intencao[altair.qtd_intencao++] = c;
                    mana_bot -= c.custo_mana;
                }
            }
            
            player.escudo_temp = 0;

            int turno_ativo = 1;
            while (turno_ativo && player.hp > 0 && altair.hp > 0) {
                render_combate(player, altair, round);
                
                char input_buf[50];
                if (!fgets(input_buf, sizeof(input_buf), stdin)) break;

                for (int k = 0; input_buf[k] != '\0'; k++) {
                    char ch = input_buf[k];
                    if (ch == '\n' || ch == '\r') break;
                    if (ch < '0' || ch > '9') continue;

                    int esc = ch - '0';
                    
                    if (esc == 4) {
                        turno_ativo = 0; 
                        log_add("Você passou o turno.");
                    }
                    else if (esc == 9) {
                        printf("\n[2] Ver Movimentos Conhecidos [3] Desistir [0] Voltar > ");
                        int sub; 
                        if(scanf("%d", &sub)!=1) sub=0; 
                        while(getchar()!='\n');

                        if (sub == 2) {
                            LIMPA_TELA;
                            printf("=== MOVIMENTOS CONHECIDOS DE ALTAIR ===\n");
                            for(int i=0; i<player.qtd_conhecidos; i++) printf("- %s\n", player.conhecidos[i]);
                            esperar_enter();
                        }
                        else if (sub == 3) { fugiu = 1; turno_ativo = 0; }
                    }
                    else if (esc >= 1 && esc <= 3) {
                        int idx = esc - 1;
                        if (player.mao[idx].custo_mana == -1) {
                            log_add("Carta já gasta.");
                        }
                        else if (player.mana >= player.mao[idx].custo_mana) {
                            player.mana -= player.mao[idx].custo_mana;
                            executar_acao(&player, &altair, player.mao[idx]);
                            player.mao[idx].custo_mana = -1; 
                            
                            if (altair.hp <= 0) {
                                render_combate(player, altair, round);
                                char dummy[10]; fgets(dummy, sizeof(dummy), stdin);
                                turno_ativo = 0;
                                break;
                            }

                            render_combate(player, altair, round);
                            PAUSA_MS(500); 
                            
                        } else {
                            log_add("Mana insuficiente.");
                            render_combate(player, altair, round);
                            PAUSA_MS(500); 
                        }
                    }
                    
                    if (!turno_ativo || altair.hp <= 0 || fugiu) break;
                }
            }

            if (fugiu) break;

            if (altair.hp > 0 && player.hp > 0) {
                altair.escudo_temp = 0;
                log_add(">> Turno de Altair...");
                render_combate(player, altair, round);
                PAUSA_MS(800);
                for(int i=0; i<altair.qtd_intencao; i++) {
                    if(player.hp > 0 && altair.hp > 0) {
                        executar_acao(&altair, &player, altair.intencao[i]);
                        PAUSA_MS(800);
                        render_combate(player, altair, round);
                    }
                }
            }
        }

        if (player.hp <= 0 || fugiu) {
            LIMPA_TELA;
            printf(ART_MORTE[0]);
            printf("\n\nVOCÊ MORREU (Round %d). Altair venceu.\n", round);
            salvar_gameover(player, (fugiu?"Desistência":"Combate"), round);
            printf("[ENTER] para Menu Principal...");
            getchar(); 
            break;
        } else {
            // TELA DE VITORIA
            LIMPA_TELA;
            printf("=== VITÓRIA! ===\nAltair caiu. Escolha sua recompensa:\n\n");
            
            char *op_arma = (player.classe==MAGO?"Cajado Arcano":(player.classe==GUERREIRO?"Espada Longa":"Adaga Sombria"));
            char *op_armor = (player.classe==MAGO?"Manto Místico":(player.classe==GUERREIRO?"Placas Reais":"Couro Batido"));
            char *op_acc = "Anel do Poder";

            int ignorados[3];
            int qtd_ign = 0;
            
            void print_upgrade_info(char* prefix, Carta c) {
                char desc_atual[50], desc_prox[50];
                gerar_descricao_dinamica(desc_atual, c, c.valor_base);
                gerar_descricao_dinamica(desc_prox, c, c.valor_base + 2);
                
                printf("%s (%s) [Nv. %d] (%s) -> [Nv. %d] (%s)\n", 
                       prefix, c.nome, c.nivel, desc_atual, c.nivel+1, desc_prox);
            }

            int id_up1 = -1;
            if (strcmp(player.item_arma, "Nada") == 0) {
                printf("[1] ARMA: %s\n", op_arma);
                if (strstr(op_arma, "Espada")) printf("    + Lacerar (7 Dano)\n    + Contra-Ataque (5 Escudo)\n");
                else if (strstr(op_arma, "Cajado")) printf("    + Inferno (12 Dano)\n    + Congelar (6 Dano + Stun)\n");
                else if (strstr(op_arma, "Adaga")) printf("    + Assassinato (11 Dano)\n    + Bomba Fumaça (60%% Esquiva)\n");
            } else {
                id_up1 = obter_indice_upgrade(&player, ignorados, qtd_ign);
                if(id_up1 != -1) {
                    ignorados[qtd_ign++] = id_up1;
                    print_upgrade_info("[1] MELHORAR:", player.deck[id_up1]);
                } else printf("[1] (Nenhuma carta disponível para upgrade)\n");
            }

            int id_up2 = -1;
            if (strcmp(player.item_armadura, "Nada") == 0) {
                printf("[2] ARMADURA: %s\n", op_armor);
                if (strstr(op_armor, "Placas")) printf("    + Fortaleza (12 Escudo)\n    + Esmagar (10 Dano Pesado)\n");
                else if (strstr(op_armor, "Manto")) printf("    + Reflexo (6 Escudo)\n    + Teleporte (40%% Esquiva)\n");
                else if (strstr(op_armor, "Couro")) printf("    + Sombra (30%% Esquiva)\n    + Revidar (4 Dano Rápido)\n");
            } else {
                id_up2 = obter_indice_upgrade(&player, ignorados, qtd_ign);
                if(id_up2 != -1) {
                    ignorados[qtd_ign++] = id_up2;
                    print_upgrade_info("[2] MELHORAR:", player.deck[id_up2]);
                } else printf("[2] (Nenhuma carta disponível para upgrade)\n");
            }

            int id_up3 = -1;
            if (strcmp(player.item_acessorio, "Nada") == 0) {
                printf("[3] ACESSÓRIO: %s\n", op_acc);
                printf("    + Sorte (Cura 5 HP)\n    + Foco (Recupera Mana)\n");
            } else {
                id_up3 = obter_indice_upgrade(&player, ignorados, qtd_ign);
                if(id_up3 != -1) {
                    ignorados[qtd_ign++] = id_up3;
                    print_upgrade_info("[3] MELHORAR:", player.deck[id_up3]);
                } else printf("[3] (Nenhuma carta disponível para upgrade)\n");
            }

            printf("[4] Apenas descansar (Recuperar Vida)\n> ");
            
            int r; 
            if (scanf("%d", &r) != 1) r = 0;
            while(getchar()!='\n');
            
            char msg_recompensa[100] = "";

            if (r==1) {
                if(id_up1 != -1) {
                    player.deck[id_up1].nivel++;
                    player.deck[id_up1].valor_base += 2;
                    sprintf(msg_recompensa, "Carta %s melhorada!", player.deck[id_up1].nome);
                } else if(strcmp(player.item_arma, "Nada") == 0) {
                    strcpy(player.item_arma, op_arma);
                    sprintf(msg_recompensa, "Você equipou: %s!", op_arma);
                }
            }
            else if (r==2) {
                if(id_up2 != -1) {
                    player.deck[id_up2].nivel++;
                    player.deck[id_up2].valor_base += 2;
                    sprintf(msg_recompensa, "Carta %s melhorada!", player.deck[id_up2].nome);
                } else if(strcmp(player.item_armadura, "Nada") == 0) {
                    strcpy(player.item_armadura, op_armor);
                    sprintf(msg_recompensa, "Você vestiu: %s!", op_armor);
                }
            }
            else if (r==3) {
                if(id_up3 != -1) {
                    player.deck[id_up3].nivel++;
                    player.deck[id_up3].valor_base += 2;
                    sprintf(msg_recompensa, "Carta %s melhorada!", player.deck[id_up3].nome);
                } else if(strcmp(player.item_acessorio, "Nada") == 0) {
                    strcpy(player.item_acessorio, op_acc);
                    sprintf(msg_recompensa, "Você colocou: %s!", op_acc);
                }
            }
            else if (r==4) {
                sprintf(msg_recompensa, "Você descansou e recuperou as energias.");
            } else {
                sprintf(msg_recompensa, "Nenhuma escolha válida feita. Apenas descanso.");
            }

            player.hp = player.hp_max;
            player.hp_max += 5;
            
            construir_deck(&player);
            round++;
            
            printf("\n------------------------------------------------\n");
            printf("%s\n", msg_recompensa);
            printf("Vida totalmente recuperada! Max HP aumentou.\n");
            printf("------------------------------------------------\n");
            
            esperar_enter();
        }
    }
}

// --- Funções Básicas ---
int d100() { return 1 + rand() % 100; }

void esperar_enter() {
    printf("\n[ENTER] para voltar/continuar...");
    getchar();
}

void mostrar_lore() {
    LIMPA_TELA;
    printf("=== LORE (EM CONSTRUÇÃO) ===\n\n");
    printf("[Aqui futuramente estará a história do jogo...]\n");
    esperar_enter();
}

void salvar_gameover(Gladiador p, char *causa, int round) {
    FILE *f = fopen(ARQUIVO_SAVE, "a");
    if (f) {
        fprintf(f, "Round %d | %s (%s) | Causa: %s\n", round, p.nome, (p.classe==MAGO?"Mago":(p.classe==GUERREIRO?"Guerreiro":"Ladino")), causa);
        fclose(f);
    }
}

void mostrar_hall_fama() {
    LIMPA_TELA;
    printf("=== HALL DA FAMA ===\n");
    FILE *f = fopen(ARQUIVO_SAVE, "r");
    if (f) {
        char buffer[200];
        while(fgets(buffer, 200, f)) printf("%s", buffer);
        fclose(f);
    } else printf("Nenhum registro ainda.\n");
    esperar_enter();
}

void mostrar_regras() {
    LIMPA_TELA;
    printf("=== MANUAL DE COMBATE ===\n");
    printf("0. CONTROLE: Digite o número da carta na mão e dê enter. Você pode digitar mais de um número sem espaço e passar o turno de uma vez.\n");
    printf("1. MANA: Recarrega todo turno (Max 3).\n");
    printf("2. ITENS: Substituem os slots. Itens novos dão cartas poderosas.\n");
    printf("3. DECK: Você tem 6 cartas base + cartas de itens. Elas tem custo de mana e voltam para o deck depois de usadas no final do turno.\n");
    printf("4. CARTAS: Causam dano, levantam escudo ou esquiva, ou tem efeitos especiais. Upgrades aumentam valores (+2).\n");
    printf("4.1 ESCUDO: Apara o dano. ESQUIVA: % de evitar totalmente o dano. EFEITOS: no momento somente recarregam mana ou curam vida.\n");
    printf("5. ALTAIR: Renasce com outra classe e mais forte.\n");
    printf("6. CONHECIMENTOS: Você pode ver a lista de movimentos que Altair te mostrou no combate presente.\n");
    printf("7. HISTORICO: Mostra seus últimos movimentos e os de Altair.\n");
    printf("8. HALL DA FAMA: Registra os últimos combates.\n");
    printf("9. DIFICULDADE: Durante meus testes no 'Difícil' era quase impossível ganhar a partir do round 2, exceto de Ladino. Boa sorte!\n");
    esperar_enter();
}
