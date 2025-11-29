//este código foi criado em par intensivo com o gemini da seguinte forma:
//todos os menus/quase todos os outputs foram redesenhados
//boa parte da refatoração foi melhorada
//criação e integração das ASCII's
//eu não sabia fazer com que o limite de memória fosse infinito e ainda não entendi esta parte
//balanceamentos matemáticos
//o que não está claro se deve ao fato de que foram MUITAS coisas, considere as palavras "par intensivo"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Configs pro Terminal ---
#ifdef _WIN32
    #include <windows.h>
    #define PAUSA_MS(x) Sleep(x)
    #define LIMPA_TELA system("cls")
#else
    #include <unistd.h>
    #define PAUSA_MS(x) usleep((x) * 1000)
    #define LIMPA_TELA system("clear")
#endif

// --- Definições ---
#define MAX_NOME 50
#define ARQUIVO_SAVE "hall_da_fama.csv"
#define ASCII_HEIGHT 13 

// Enums
typedef enum { MAGO = 1, LADINO = 2, GUERREIRO = 3 } Classe;
typedef enum { NORMAL = 1, DIFICIL = 2 } Dificuldade;

// Struct do Personagem
typedef struct {
    char nome[MAX_NOME];
    Classe classe;
    int hp, hp_max;
    
    // Atributos
    int forca;    
    int defesa;   
    int esquiva;
    
    // Limites (Caps)
    int cap_for, cap_def, cap_esq;

    // Controle
    int total_pontos; 
    char item_equipado[MAX_NOME]; 
    int bonus_item_tipo; 
    int bonus_item_val;
    int eh_jogador;
} Gladiador;


Dificuldade dificuldade_jogo = NORMAL;

// ARTES ASCII criadas pelo temini 3.0, foi ele também quem aplicou elas

const char *ART_GUERREIRO[] = {
    "      .---.      ",
    "     /_____\\     ",
    "     ( '.' )     ",
    "      \\_-_/      ",
    "    .-'`'V'``-.  ",
    "   /   /   \\   \\ ",
    "  /   /     \\   \\",
    " /   /       \\   \\",
    " \\   \\       /   /",
    "  \\___\\     /___/",
    "   |   |   |   | ",
    "   |   |   |   | ",
    "   '---'   '---' "
};

const char *ART_MAGO[] = {
    "      _   _      ",
    "     ( `^` )     ",
    "     |  |  |     ",
    "    _\\  |  /_    ",
    "   '  \\ | /  '   ",
    "  / _  \\|/  _ \\  ",
    " ( ( \\  |  / ) ) ",
    "  \\ \\/  |  \\/ /  ",
    "   \\    |    /   ",
    "    |   |   |    ",
    "   /    |    \\   ",
    "  (_/\\__|__/\\_)  ",
    "     |__|__|     "
};

const char *ART_LADINO[] = {
    "      .....      ",
    "     C . . D     ",
    "      \\_-_/      ",
    "    /`     `\\    ",
    "   /  /   \\  \\   ",
    "  |  /     \\  |  ",
    " /  /| . . |\\  \\ ",
    " \\  \\| . . |/  / ",
    "  \\  \\     /  /  ",
    "   \\ |     | /   ",
    "    ||     ||    ",
    "    ||     ||    ",
    "   [__]   [__]   "
};

const char *ART_MORTE[] = {
    "   _.--\"\"\"--._   ",
    "  /  _   _  \\  ",
    " |  (x) (x)  |  ",
    " |     |     |  ",
    " |     ^     |  ",
    "  \\  _____  /   ",
    "   `-.   .-'    ",
    "     _| |_      ",
    "    |     |     ",
    "    | R.I.P |   ",
    "    |     |     ",
    "    |_____|     ",
    "   /_______\\    "
};

const char *ART_VITORIA[] = {
    "      _   _      ",
    "     (_)-(_)     ",
    "    _ | . | _    ",
    "   ( \\|   |/ )   ",
    "    \\ \\   / /    ",
    "     \\_\\ /_/     ",
    "     /     \\     ",
    "    /       \\    ",
    "   /    |    \\   ",
    "  /     |     \\  ",
    " /      |      \\ ",
    "/_______|_______\\",
    "    (VICTORY)    "
};

// Protótipos
int d6();
int d100();
Gladiador criar_char(char* nome, Classe c, int eh_player, int pts);
void salvar_gameover(Gladiador p, Gladiador e, int round);
void processar_turno(Gladiador *atk, Gladiador *def, char *log);
void renderizar_cena(Gladiador p1, Gladiador p2, char *log, int turno, int round, int estado_jogo);
void mostrar_regras();
void mostrar_historia();
void mostrar_hall_fama(); 
Gladiador menu_criacao();
void game_loop();
void esperar_enter();
void auto_distribuir(Gladiador *g, int pts);
void manual_distribuir(Gladiador *g, int pts);
void equipar_item(Gladiador *g, char *nome, int tipo, int bonus);
void gerarArtefatoAleatorio(Gladiador *g, char *nomeBuffer, int *tipoBuffer, int *bonusBuffer);

//inicio da brincadeira, o design foi modificado pelo gemini 3.0
//todos os desgins foram modificados pelo gemini, para eu não ter ficar repetindo

int main() {
    srand(time(NULL)); 
    int op;

    do {
        LIMPA_TELA;
        printf("\n+=======================================+\n");
        printf("|     [X] Altair o Renascido: ROGUELIKE AUTO BATTLER [X]     |\n");
        printf("+=======================================+\n");
        printf("| 1. Novo Jogo                          |\n");
        printf("| 2. Historia                           |\n");
        printf("| 3. Hall da Fama (Records)             |\n");
        printf("| 4. Manual de Regras                   |\n");
        printf("| 5. Sair                               |\n");
        printf("+=======================================+\n");
        printf("Opcao > ");
        scanf("%d", &op);
        while(getchar() != '\n'); 

        switch (op) {
            case 1: game_loop(); break;
            case 2: mostrar_historia(); break;
            case 3: mostrar_hall_fama(); break;
            case 4: mostrar_regras(); break;
            case 5: printf("Saindo da arena...\n"); break;
            default: printf("Opcao invalida.\n"); PAUSA_MS(500);
        }
    } while (op != 5);
    
    return 0;
}


//REGRAS/HISTORIA/HALL DA FAMA
void mostrar_hall_fama() {
    LIMPA_TELA;
    printf("=== HALL DA FAMA (HISTORICO DE MORTES) ===\n\n");
    
    FILE *f = fopen(ARQUIVO_SAVE, "r");
    if (!f) {
        printf(">> Nenhum registro encontrado. Seja o primeiro a morrer!\n");
    } else {
        //esse limite foi sugerido pelo gemini 3.0 pois eu não tenho ete tipo de noção
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f)) {
            printf("%s", buffer);
        }
        fclose(f);
    }
    printf("\n[Pressione ENTER para voltar]");
    getchar();
}

void mostrar_regras() {
    LIMPA_TELA;
    printf("+---------------------------------------------------+\n");
    printf("|             MANUAL TECNICO DA ARENA               |\n");
    printf("+---------------------------------------------------+\n\n");
    
    printf("1. MECANICA ROGUELIKE (LOOP INFINITO):\n");
    printf("   O jogo nao possui 'zeramento'. O objetivo é atingir o maior Round possivel.\n");
    printf("   O inimigo, ALTAIR, é uma entidade que renasce a cada derrota.\n\n");

    printf("2. FUNCIONAMENTO DO INIMIGO (ALTAIR):\n");
    printf("   - Classe: A cada vitória, ele sorteia uma nova classe.\n");
    printf("   - Evolucao: Recebe os mesmos pontos totais que voce. Mas ele não equipa itens\n");
    printf("   - Inteligencia: Foca em atributos principais. Excesso vai para vida.\n\n");

    printf("3. SISTEMA DE ATRIBUTOS E CAPS:\n");
    printf("   [FOR] Forca: Aumenta dano.\n");
    printf("   [DEF] Defesa: Reduz dano total.\n");
    printf("   [AGI] Esquiva: 5%% de chance por ponto (Max 60%%).\n");
    printf("   [CAPS] Cada classe começa com determinados limites de capacidade.\n\n")

    printf("4. PROGRESSAO DO JOGADOR:\n");
    printf("   Vitoria concede: +1 em TODOS os Caps, +3 Pontos de Status.\n");
    printf("   Recompensa: Item (aumenta atributos), Limite Break (aumenta em 2 um CAPS) ou Vinganca (5 de dano no próximo Altair).\n");
    printf("   CAPS: Ao final de cada combate todos os CAPS aumentam em 1 e recebe 3 pontos para distribuir.\n")
    printf("   Vida: 1 Ponto de Atributo pode ser trocado por +1 HP MAX.\n\n");

    printf("5. COMBATE:\n");
    printf("   Dano = (Forca + 1d6) - Defesa Inimigo. (Minimo 0).\n\n");

    printf("[Pressione ENTER para voltar]");
    getchar(); 
}

void mostrar_historia() {
    LIMPA_TELA;
    printf("=== LORE ===\n\n[tenho só a ideia]\n\n[ENTER]");
    getchar();
}

void esperar_enter() {
    printf("\n>>> [ENTER] para continuar...");
    getchar();
}

//LÓGICA DE ITENS
void equipar_item(Gladiador *g, char *nome, int tipo, int bonus) {
    // remove o item anterior
    if (g->bonus_item_tipo != 0) {
        if (g->bonus_item_tipo == 1) g->forca -= g->bonus_item_val;
        else if (g->bonus_item_tipo == 2) g->defesa -= g->bonus_item_val;
        else if (g->bonus_item_tipo == 3) g->esquiva -= g->bonus_item_val;
    }

    // Aplica novo
    strcpy(g->item_equipado, nome);
    
    if (tipo == 0) {
        //arma
        g->forca += bonus; g->bonus_item_tipo = 1; 
    //armadura
    } else if (tipo == 1) {
        g->defesa += bonus; g->bonus_item_tipo = 2;
    //acc
    } else {
        g->esquiva += bonus; g->bonus_item_tipo = 3;
    }
    g->bonus_item_val = bonus;
    
    printf(">> Item [%s] equipado! Status antigo removido, novo aplicado.\n", nome);
}

//isso aqui estava uma bagunça e o gemini me mostrou essa forma muito inteligente de resolver
void gerarArtefatoAleatorio(Gladiador *g, char *nomeBuffer, int *tipoBuffer, int *bonusBuffer) {
    int tipo = rand() % 3; 
    char sufixo[20];
    char *sufixos[] = {"de Altair", "do Renascido", "Ancestral", "Maldito"};
    strcpy(sufixo, sufixos[rand() % 4]);
    *tipoBuffer = tipo;

    if (g->classe == MAGO) {
        if (tipo == 0) { sprintf(nomeBuffer, "Cajado %s", sufixo); *bonusBuffer = 2; }
        else if (tipo == 1) { sprintf(nomeBuffer, "Capuz %s", sufixo); *bonusBuffer = 1; }
        else { sprintf(nomeBuffer, "Anel %s", sufixo); *bonusBuffer = 1; }
    } 
    else if (g->classe == GUERREIRO) {
        if (tipo == 0) { sprintf(nomeBuffer, "Espada %s", sufixo); *bonusBuffer = 1; }
        else if (tipo == 1) { sprintf(nomeBuffer, "Armadura %s", sufixo); *bonusBuffer = 2; }
        else { sprintf(nomeBuffer, "Grevas %s", sufixo); *bonusBuffer = 1; }
    }
    else { 
        if (tipo == 0) { sprintf(nomeBuffer, "Faca %s", sufixo); *bonusBuffer = 1; }
        else if (tipo == 1) { sprintf(nomeBuffer, "Manto %s", sufixo); *bonusBuffer = 1; }
        else { sprintf(nomeBuffer, "Botas %s", sufixo); *bonusBuffer = 2; }
    }
}

//SETUP E DISTRIBUIÇÃO

void auto_distribuir(Gladiador *g, int pts) {
    int sobra = pts;
    g->forca = 0; g->defesa = 0; g->esquiva = 0;
    
    int tentativas = 0;
    while (sobra > 0 && tentativas < 2000) {
        int r = rand() % 4;
        
        if (r == 0 && g->forca < g->cap_for) { g->forca++; sobra--; }
        else if (r == 1 && g->defesa < g->cap_def) { g->defesa++; sobra--; }
        else if (r == 2 && g->esquiva < g->cap_esq) { g->esquiva++; sobra--; }
        
        else if (g->forca >= g->cap_for && g->defesa >= g->cap_def && g->esquiva >= g->cap_esq) {
            g->hp_max += 1;
            g->hp += 1;
            sobra--;
        }
        tentativas++;
    }
    g->hp = g->hp_max;
}

void manual_distribuir(Gladiador *g, int pts_novos) {
    int pts = pts_novos;
    int val = 0;

    while (pts > 0) {
        LIMPA_TELA;
        printf("\n=== UPGRADE: %s ===\n", g->nome);
        printf("Pontos: %d\n", pts);
        printf("Limites: FOR %d | DEF %d | AGI %d\n", g->cap_for, g->cap_def, g->cap_esq);
        printf("Atuais:  FOR %d | DEF %d | AGI %d | HP %d\n\n", g->forca, g->defesa, g->esquiva, g->hp_max);

        printf("Distribuir:\n[1] FORCA\n[2] DEFESA\n[3] ESQUIVA\n[4] VIDA (+1 HP)\n> ");
        int escolha; scanf("%d", &escolha); while(getchar()!='\n');

        printf("Quantos pontos? ");
        scanf("%d", &val); while(getchar()!='\n');

        if (val > 0 && val <= pts) {
            if (escolha == 4) {
                g->hp_max += val;
                g->hp += val;
                pts -= val;
                printf(">> Gastou %d pontos. Ganhou +%d HP Max!\n", val, val);
                esperar_enter();
            } 
            else {
                int valor_base = 0;
                if (escolha == 1) {
                    valor_base = g->forca - (g->bonus_item_tipo == 1 ? g->bonus_item_val : 0);
                    if ((valor_base + val) <= g->cap_for) { g->forca += val; pts -= val; }
                    else { printf("Passou do limite de Forca!\n"); esperar_enter(); }
                }
                else if (escolha == 2) {
                    valor_base = g->defesa - (g->bonus_item_tipo == 2 ? g->bonus_item_val : 0);
                    if ((valor_base + val) <= g->cap_def) { g->defesa += val; pts -= val; }
                    else { printf("Passou do limite de Defesa!\n"); esperar_enter(); }
                }
                else if (escolha == 3) {
                    valor_base = g->esquiva - (g->bonus_item_tipo == 3 ? g->bonus_item_val : 0);
                    if ((valor_base + val) <= g->cap_esq) { g->esquiva += val; pts -= val; }
                    else { printf("Passou do limite de Esquiva!\n"); esperar_enter(); }
                }
            }
        } else {
            printf("Valor invalido!\n"); esperar_enter();
        }
    }
}

Gladiador criar_char(char* nome, Classe c, int eh_player, int total_pts) {
    Gladiador g;
    strcpy(g.nome, nome);
    strcpy(g.item_equipado, "Nada");
    g.bonus_item_tipo = 0;
    g.bonus_item_val = 0;
    g.classe = c;
    g.eh_jogador = eh_player;
    g.total_pontos = total_pts;

    switch (c) {
        case MAGO:      g.hp_max = 12; g.cap_for=6; g.cap_def=4; g.cap_esq=4; break;
        case LADINO:    g.hp_max = 9;  g.cap_for=4; g.cap_def=4; g.cap_esq=6; break;
        case GUERREIRO: g.hp_max = 15; g.cap_for=4; g.cap_def=6; g.cap_esq=4; break;
    }
    
    g.hp = g.hp_max;
    return g;
}

Gladiador menu_criacao() {
    LIMPA_TELA;
    char nome[MAX_NOME];
    int classe_op;
    
    printf("\n--- CRIAR HEROI ---\n");
    printf("Nome: ");
    fgets(nome, MAX_NOME, stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("\nDificuldade:\n[1] Normal\n[2] Dificil (Altair Buffado)\n> ");
    int dif; scanf("%d", &dif); while(getchar()!='\n');
    dificuldade_jogo = (dif == 2) ? DIFICIL : NORMAL;

    printf("\nClasse:\n[1] Mago\n[2] Ladino\n[3] Guerreiro\n> ");
    scanf("%d", &classe_op); while(getchar()!='\n'); 

    if (classe_op < 1 || classe_op > 3) classe_op = 1;

    Gladiador g = criar_char(nome, (Classe)classe_op, 1, 10);
    g.forca = 0; g.defesa = 0; g.esquiva = 0;
    manual_distribuir(&g, 10);

    return g;
}

//LOOP DO JOGO

void game_loop() {
    Gladiador player = menu_criacao();
    int round = 1;
    int dano_surpresa = 0;

    while (1) {
        // --- INIMIGO ---
        Classe c_bot = (rand() % 3) + 1;
        char *str_classes[] = {"", "Mago", "Ladino", "Guerreiro"};
        char nome_bot[60];
        
        if (round == 1) strcpy(nome_bot, "Altair");
        else sprintf(nome_bot, "Altair Renascido");

        int pts_bot = player.total_pontos;
        if (dificuldade_jogo == DIFICIL) pts_bot += (round - 1) * 2; 

        Gladiador inimigo = criar_char(nome_bot, c_bot, 0, pts_bot);
        
        int bonus_caps = round - 1;
        if (dificuldade_jogo == DIFICIL) bonus_caps += round;

        inimigo.cap_for += bonus_caps;
        inimigo.cap_def += bonus_caps;
        inimigo.cap_esq += bonus_caps;

        auto_distribuir(&inimigo, pts_bot);

        //DANO SURPRESA
        int hp_inimigo_antes = inimigo.hp;
        if (dano_surpresa > 0) {
            inimigo.hp -= dano_surpresa;
            if (inimigo.hp < 1) inimigo.hp = 1;
        }

        char log[200];
        // LOG ESPECÍFICO DO ATAQUE SURPRESA
        if (dano_surpresa > 0) 
            snprintf(log, sizeof(log), "ATAQUE SURPRESA! HP Inimigo reduzido de %d para %d!", hp_inimigo_antes, inimigo.hp);
        else 
            strcpy(log, "Encontro iniciado!");
        
        dano_surpresa = 0; 
        int turno = 1;

        renderizar_cena(player, inimigo, log, turno, round, 0); 
        esperar_enter();

        //COMBATE
        while (player.hp > 0 && inimigo.hp > 0) {
            processar_turno(&player, &inimigo, log);
            renderizar_cena(player, inimigo, log, turno, round, 0);
            
            // VERIFICA MORTE DO INIMIGO
            if (inimigo.hp <= 0) {
                esperar_enter();
                break;
            }
            esperar_enter();

            processar_turno(&inimigo, &player, log);
            renderizar_cena(player, inimigo, log, turno, round, 0);
            
            //  VERIFICA MORTE DO JOGADOR
            if (player.hp <= 0) {
                esperar_enter();
                break;
            }
            esperar_enter();
            turno++;
        }

        //RESULTADO
        if (player.hp <= 0) {
            renderizar_cena(player, inimigo, "Voce caiu em combate...", turno, round, 2); 
            salvar_gameover(player, inimigo, round);
            esperar_enter();
            break; 
        } else {
            renderizar_cena(player, inimigo, "Altair foi derrotado!", turno, round, 1); 
            player.hp = player.hp_max; 
            
            char nomeArt[MAX_NOME]; int tipoArt, bonusArt;
            gerarArtefatoAleatorio(&player, nomeArt, &tipoArt, &bonusArt);
            
            char descBonus[30];
            if (tipoArt==0) sprintf(descBonus, "(Arma: +%d Dano)", bonusArt);
            else if (tipoArt==1) sprintf(descBonus, "(Armadura: +%d Def)", bonusArt);
            else sprintf(descBonus, "(Acessorio: +%d Esq)", bonusArt);

            printf("\n--- SAQUE DE GUERRA ---\n");
            printf("[1] LIMITE BREAK: +2 no Limite de um status\n");
            printf("[2] ITEM: %s %s\n", nomeArt, descBonus);
            printf("[3] VINGANCA: 5 Dano no proximo round\n> ");
            
            int esc; scanf("%d", &esc); while(getchar()!='\n');

            if (esc == 1) {
                printf("Qual limite upar? [1]FOR [2]DEF [3]AGI: ");
                int x; scanf("%d", &x); while(getchar()!='\n');
                if (x==1) player.cap_for+=2; else if (x==2) player.cap_def+=2; else player.cap_esq+=2;
            } 
            else if (esc == 2) {
                equipar_item(&player, nomeArt, tipoArt, bonusArt);
            }
            else if (esc == 3) dano_surpresa = 5;

            printf("\n--- LEVEL UP ---\nTodos os limites +1.\nGanhou +3 Pontos.\n");
            player.cap_for++; player.cap_def++; player.cap_esq++;
            player.total_pontos += 3;
            esperar_enter();

            manual_distribuir(&player, 3);
            round++;
        }
    }
}

//gemini melhorou isto aqui de uma forma incrível
void processar_turno(Gladiador *atk, Gladiador *def, char *log) {
    int chance_esquiva = def->esquiva * 5;
    if (chance_esquiva > 60) chance_esquiva = 60;

    if (d100() <= chance_esquiva) {
        snprintf(log, 128, ">>> %s atacou mas %s ESQUIVOU! (Chance: %d%%)", atk->nome, def->nome, chance_esquiva);
        return;
    }

    int dano_base = atk->forca + d6();
    int dano_final = dano_base - def->defesa;
    if (dano_final < 0) dano_final = 0;

    def->hp -= dano_final;
    if (def->hp < 0) def->hp = 0;

    snprintf(log, 128, ">>> %s ACERTOU! (ATK %d - DEF %d = %d Dano)", 
            atk->nome, dano_base, def->defesa, dano_final);
}

//RENDERIZADOR
//eu simplesmente não sabia fazer isto, obrigado gemini, não ia sair
void renderizar_cena(Gladiador p1, Gladiador p2, char *log, int turno, int round, int estado_jogo) {
    LIMPA_TELA;
    
    const char **arte_atual;
    char titulo_arte[30];

    if (estado_jogo == 2) { 
        arte_atual = ART_MORTE; 
        strcpy(titulo_arte, "GAME OVER"); 
    }
    else if (estado_jogo == 1) { 
        arte_atual = ART_VITORIA; 
        strcpy(titulo_arte, "VITORIA!"); 
    }
    else {
        char c_curta[4];
        if(p2.classe==MAGO) strcpy(c_curta,"MAG");
        else if(p2.classe==LADINO) strcpy(c_curta,"LAD");
        else strcpy(c_curta,"GUE");
        
        snprintf(titulo_arte, sizeof(titulo_arte), "ALTAIR [%s]", c_curta);

        if (p2.classe == GUERREIRO) arte_atual = ART_GUERREIRO;
        else if (p2.classe == MAGO) arte_atual = ART_MAGO;
        else arte_atual = ART_LADINO;
    }

    char b1[25], b2[25];
    int p_b1 = (p1.hp * 10) / p1.hp_max; if(p_b1>10) p_b1=10; if(p1.hp>0 && p_b1==0) p_b1=1;
    int p_b2 = (p2.hp * 10) / p2.hp_max; if(p_b2>10) p_b2=10; if(p2.hp>0 && p_b2==0) p_b2=1;

    strcpy(b1, "["); strcpy(b2, "[");
    for(int i=0; i<10; i++) { strcat(b1, (i<p_b1?"#":" ")); strcat(b2, (i<p_b2?"#":" ")); }
    strcat(b1, "]"); strcat(b2, "]");

    printf("\n=== ROUND %d | TURNO %d ===\n\n", round, turno);

    printf("%-50s   %s\n", "+------------------- HUD ------------------+", titulo_arte);
    
    char linha_hud[128]; 

    // Prepara descrição do item para o HUD
    char descItem[30] = "";
    if (p1.bonus_item_tipo == 1) sprintf(descItem, "(ATK +%d)", p1.bonus_item_val);
    else if (p1.bonus_item_tipo == 2) sprintf(descItem, "(DEF +%d)", p1.bonus_item_val);
    else if (p1.bonus_item_tipo == 3) sprintf(descItem, "(AGI +%d)", p1.bonus_item_val);

    for (int i = 0; i < ASCII_HEIGHT; i++) {
        memset(linha_hud, ' ', sizeof(linha_hud));
        linha_hud[50] = '\0'; 
        
        if (i == 0) snprintf(linha_hud, sizeof(linha_hud), "| VOCE: %s (%d/%d)", p1.nome, p1.hp, p1.hp_max);
        if (i == 1) snprintf(linha_hud, sizeof(linha_hud), "| HP: %s", b1);
        if (i == 2) snprintf(linha_hud, sizeof(linha_hud), "| ATK:%d DEF:%d AGI:%d", p1.forca, p1.defesa, p1.esquiva);
        
        if (i == 3) snprintf(linha_hud, sizeof(linha_hud), "| ITEM: %s %s", p1.item_equipado, descItem);
        
        if (i == 4) snprintf(linha_hud, sizeof(linha_hud), "|"); 
        if (i == 5) snprintf(linha_hud, sizeof(linha_hud), "| INIMIGO: %s", p2.nome);
        if (i == 6) snprintf(linha_hud, sizeof(linha_hud), "| HP: %s (%d/%d)", b2, p2.hp, p2.hp_max);
        if (i == 7) snprintf(linha_hud, sizeof(linha_hud), "| STATUS: ATK:%d DEF:%d AGI:%d", p2.forca, p2.defesa, p2.esquiva);
        if (i == 8) snprintf(linha_hud, sizeof(linha_hud), "+--------------------------------------------------+");

        int len = strlen(linha_hud);
        if (len < 52) {
            for(int k=len; k<52; k++) linha_hud[k] = ' ';
            linha_hud[52] = '\0';
        } else {
            linha_hud[52] = '\0';
        }
        
        printf("%s %s\n", linha_hud, arte_atual[i]);
    }

    printf("\nLOG: %s\n", log);
}

int d6() { return 1 + rand() % 6; }
int d100() { return 1 + rand() % 100; }

void salvar_gameover(Gladiador p, Gladiador e, int round) {
    FILE *f = fopen(ARQUIVO_SAVE, "a");
    if (f) { 
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(f, "%02d/%02d/%d | Round %d | %s (%s) Morto por %s | Item: %s\n", 
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 
                round, p.nome, (p.classe==1?"Mago":(p.classe==2?"Ladino":"Guerreiro")), e.nome, p.item_equipado);
        fclose(f);
        printf(">>> Salvo no Hall da Fama.\n");
    }
}
