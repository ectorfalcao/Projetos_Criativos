#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h> 
#pragma comment(lib, "winmm.lib") 

// --- CONFIGURACOES VISUAIS ---
#define W_MENU 50   
#define W_HUD 45
#define W_LISTA 45 
#define GAP "    "  
#define ALTURA_TELA 35

// --- CONFIGURACOES DE JOGO ---
#define MAX_MANA 100
#define CUSTO_INVOCACAO 15
#define DECOMPOSICAO_INICIAL 6

#define TELA_COMANDO 1
#define TELA_ENTER 2

// --- CUSTOS E STATUS DE INVOCACAO ---
// ESQUELETO
#define C_MANA_ESQ 15
#define C_CORPO_ESQ 1
#define ATK_ESQ 5
#define HP_ESQ 10

// ZUMBI 
#define C_MANA_ZUM 25
#define C_CORPO_ZUM 2
#define ATK_ZUM 8
#define HP_ZUM 40

// CARNICAL 
#define C_MANA_CAR 40
#define C_CORPO_CAR 3
#define ATK_CAR 20
#define HP_CAR 35

// ABOMINACAO 
#define C_MANA_ABO 100
#define C_CORPO_ABO 5
#define ATK_ABO 60
#define HP_ABO 250

// --- INVOCAÇÕES ESPECIAIS ---

//SOMBRA
#define C_MANA_SOM 30
#define C_CORPO_SOM 1

// LEGIAO 
#define C_MANA_LEG 35
#define C_CORPO_LEG 2
#define HP_LEG 25

//COME-MANA 
#define C_MANA_COM 10
#define C_CORPO_COM 1
#define HP_COM 15

// DEVORADOR 
#define C_MANA_DEV 60
#define ATK_DEV 10

// LICH 
#define C_MANA_LIC 75
#define C_CORPO_LIC 1
#define ATK_LIC 50
#define HP_LIC 1

// SIFAO 
#define C_MANA_SIF 50
#define C_CORPO_SIF 1
#define HP_SIF 30  
#define ATK_SIF 2   

// --- DEFINES DE CLASSE ---
#define CLASSE_COVEIRO 1
#define CLASSE_MAGO_SANGUE 2
#define CLASSE_ESPIRITUALISTA 3

// --- CONSUMÍVEIS (IDs) ---
#define ITEM_ELIXIR 1
#define ITEM_ETER 2
#define ITEM_OSSOS 3
#define ITEM_AFIA 4
#define ITEM_BARREIRA 5
#define ITEM_CONTRATO 6
#define ITEM_INCENSO 7
#define ITEM_GRANADA 8
#define ITEM_CRISTAL 9
#define ITEM_BERRANTE 10

// --- TIPOS DE INIMIGOS (IDs) ---
#define INI_SOLDADO 0
#define INI_GRANADEIRO 1
#define INI_BERSERKER 2
#define INI_ESCUDEIRO 3
#define INI_DUELISTA 4
#define INI_CLERIGO 5
#define INI_VAMPIRO 6
#define INI_ARQUEIRO 7
#define INI_ANULADOR 8
#define INI_LADRAO 9
#define INI_FANATICO 10
#define INI_FANTASMA 11
#define INI_CACADOR 12

// --- LACAIOS DE EVENTO (IDs 11-20) ---
#define LAC_CAVALEIRO  11
#define LAC_PALADINO   12
#define LAC_HOMUNCULO  13
#define LAC_BESTA      14
#define LAC_ARQUEIRO   15
#define LAC_SACERDOTE  16
#define LAC_ESPECTRO   17
#define LAC_GOLEM_OSSO 18
#define LAC_VAMPIRO    19
#define LAC_VERDUGO    20

// --- CORES  ---
#define RST  "\x1b[0m"
#define RED  "\x1b[91m" // Vermelho Claro
#define GRN  "\x1b[92m" // Verde Claro
#define YEL  "\x1b[93m" // Amarelo Claro
#define CYN  "\x1b[96m" // Ciano
#define WHT  "\x1b[97m" // Branco
#define MAG  "\x1b[95m" // Magenta (Roxo Claro)
#define GRN2 "\x1b[32m" // Verde Escuro (Para intercalar)

// Retorna a cor baseada no tipo do inimigo
const char* get_cor_inimigo_tipo(int tipo) {
    switch(tipo) {
        case INI_SOLDADO:   return CYN; // Soldados = Azul
        case INI_ESCUDEIRO: return WHT; // Neutro
        // Agressivos (Fisico) = Vermelho
        case INI_GRANADEIRO:
        case INI_BERSERKER: 
        case INI_VAMPIRO:
        case INI_FANATICO:  return RED; 
        // Mágicos/Especiais = Magenta
        case INI_ANULADOR: 
        case INI_FANTASMA:
        case INI_CACADOR:
        case INI_CLERIGO:
        case INI_LADRAO:    return MAG;
        // Habilidade = Amarelo
        case INI_DUELISTA: 
        case INI_ARQUEIRO:  return YEL; 
        default: return WHT;
    }
}

// Função auxiliar para mudar a cor
void cor(int cor_id) {
    // Força o sistema a escrever o que estava pendente antes de mudar a cor
    fflush(stdout); 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor_id);
}

// --- PERFIL GLOBAL (PERSISTÊNCIA ENTRE RUNS) ---
typedef struct {
    int moedas_caronte;
    int paginas_necronomicon[6];
    int paginas_desbloqueadas[12];
    int upgrades_classe[4];
    int evolucoes_lacaios[5];
} PerfilGlobal;

// --- PREÇO DAS PÁGINAS ---
#define PRECO_PAGINA 300 // Custo de cada item novo
#define PRECO_UPGRADE 1000
#define PRECO_EVOLUCAO 2000

// --- ESTRUTURAS ---
typedef struct Minion {
    int id;
    char nome[30];
    char tipo[20];
    int codigo_origem; // Importante
    int ataque;
    int hp;
    int decomposicao;
    struct Minion* proximo;
} Minion;

typedef struct {
    int id_tipo;
    char nome[20];
    int hp;
    int max_hp;
    int atk;
    int vivo;
} InimigoUnit;

typedef struct {
    char nome_classe[20];
    char causa_morte[50];
    int turno_alcancado;
    int estagio_final;
    int hp_final;
    int atk_exercito_final;
    int artefatos_coletados[12];
} RegistroCemiterio;

typedef struct {
    int mana;
    int corpos;
    int hp;
    int max_hp;
    int classe;
    
    int turno_global;
    int estagio_marcha;
    int progresso_local;
    int escavacoes_turno;
    
    // Combate
    int hp_inimigo_atual;
    int hp_inimigo_max;
    int atk_inimigo_atual;
    int buff_profanacao;
    
    // Boss e Esquadrão
    int boss_id_atual;
    int boss_fase_2;
    char nome_inimigo[50];
    InimigoUnit esquadrao[5]; 
    int qtd_inimigos_ativos;  
    
    // Itens e Habilidades
    int artefatos[12]; 
    int inventario[3];
    int flag_item_barreira; 
    int flag_item_contrato;
    int flag_item_cristal;
    int ultimo_tipo_morto;
    
    int habilidade_usada;
    int flag_forma_eterea;
    
    // Cheat
    int cheat_evento_ativo; 
    
    // Flag de Evento Local
    int evento_local_concluido;

    int eventos_realizados[31];

    int lacaios_descobertos[21];

    int evolucoes_ativas[5];

    int total_minions;
    char relatorio[300]; 
    Minion* exercito_head;
} Necromante;

// --- CONTROLE DE CONSOLE (ANTI-FLICKER) ---

void esconderCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE; // Torna o cursor invisivel
    SetConsoleCursorInfo(consoleHandle, &info);
}

void irParaInicio() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)0, (SHORT)0 };
    SetConsoleCursorPosition(hOut, coord);
}

//prototipos
void desenhar_interface(Necromante* player, int tipo_exibicao);
void atualizar_relatorio(Necromante* player, char* texto);
void pausa_vitoria(Necromante* player, char* texto);
const char* get_descricao_boss(Necromante* p);
void tela_escolher_recompensa(Necromante* p);
int menu_selecao_classe();
void menu_consumiveis(Necromante* p);
void ativar_habilidade_classe(Necromante* p);
void tela_vitoria_final();
int tela_derrota();
int tela_confirmar_reinicio();
void registrar_morte(Necromante* p, const char* causa);
void print_centralizado(const char* texto, const char* cor);
void print_ajuda_centralizado(const char* texto, const char* cor);
void salvar_jogo(Necromante* p);
int carregar_jogo(Necromante* p);
void deletar_save();
void executar_run(int carregar_save);
void menu_ajuda_lacaios(Necromante* p);
void carregar_perfil(PerfilGlobal* pg);
void salvar_perfil(PerfilGlobal* pg);
void obter_ascii_consumivel(item_id, a1, a2);

void configurar_inimigo_unit(InimigoUnit* u, int tipo, int base_hp, int base_atk) {
    u->id_tipo = tipo;
    u->vivo = 1;
    
    // Balanceamento base: Distribui o status base dependendo da classe
    switch(tipo) {
        case INI_SOLDADO: 
            strcpy(u->nome, "Soldado"); 
            u->max_hp = base_hp; u->atk = base_atk; 
            break;
        case INI_GRANADEIRO: 
            strcpy(u->nome, "Granadeiro"); 
            u->max_hp = base_hp * 0.8; u->atk = base_atk * 0.8;
            break;
        case INI_BERSERKER: 
            strcpy(u->nome, "Berserker"); 
            u->max_hp = base_hp * 1.2; u->atk = base_atk * 1.5; 
            break;
        case INI_ESCUDEIRO: 
            strcpy(u->nome, "Escudeiro"); 
            u->max_hp = base_hp * 1.5; u->atk = base_atk * 0.5; 
            break;
        case INI_DUELISTA: 
            strcpy(u->nome, "Duelista"); 
            u->max_hp = base_hp; u->atk = base_atk * 1.2; 
            break;
        case INI_CLERIGO: 
            strcpy(u->nome, "Clerigo"); 
            u->max_hp = base_hp * 0.7; u->atk = base_atk * 0.5; 
            break;
        case INI_VAMPIRO: 
            strcpy(u->nome, "Vampiro"); 
            u->max_hp = base_hp; u->atk = base_atk; 
            break;
        case INI_ARQUEIRO: 
            strcpy(u->nome, "Arqueiro"); 
            u->max_hp = base_hp * 0.6; u->atk = base_atk * 1.2; 
            break;
        case INI_ANULADOR: 
            strcpy(u->nome, "Mago Anulador"); 
            u->max_hp = base_hp * 0.6; u->atk = base_atk * 0.8; 
            break;
        case INI_LADRAO: 
            strcpy(u->nome, "Ladrao"); 
            u->max_hp = base_hp * 0.8; u->atk = base_atk; 
            break;
        case INI_FANATICO: 
            strcpy(u->nome, "Fanatico"); 
            u->max_hp = base_hp * 0.5; u->atk = 0; 
            break;
        case INI_FANTASMA: 
            strcpy(u->nome, "Fantasma"); 
            u->max_hp = base_hp * 0.5; u->atk = base_atk; 
            break;
        case INI_CACADOR: 
            strcpy(u->nome, "Cacador Mago"); 
            u->max_hp = base_hp; u->atk = base_atk; 
            break;
    }
    
    u->hp = u->max_hp;
}

//gerador de inimigos
void gerar_novo_inimigo(Necromante* p) {
    p->habilidade_usada = 0; 
    p->flag_forma_eterea = 0;

    int base_hp = (p->turno_global * 50) + 50; 
    int base_atk = (p->turno_global * 12) + 8;
    
    base_hp += p->buff_profanacao * 30;
    int bonus_atk = p->buff_profanacao * 5;
    if (p->artefatos[6] == 2) bonus_atk = 0; 
    base_atk += bonus_atk;

    if (p->estagio_marcha == 11) p->boss_id_atual = 11;
    else if (p->progresso_local == 10) p->boss_id_atual = p->estagio_marcha; 
    else p->boss_id_atual = 0; 
    p->boss_fase_2 = 0; 

    for(int i=0; i<5; i++) p->esquadrao[i].vivo = 0;

    if (p->boss_id_atual == 0) {
        // --- INIMIGOS COMUNS ---
        char* titulos_controlador[] = { "Sargento", "Tenente", "Capitao", "Major", "Veterano", "Inquisidor", "Coronel", "General", "Marechal", "Campeao" };
        int idx_titulo = p->estagio_marcha - 1;
        if (idx_titulo < 0) idx_titulo = 0; if (idx_titulo > 9) idx_titulo = 9;

        sprintf(p->nome_inimigo, "Escolta do %s", titulos_controlador[idx_titulo]);
        
        int qtd = 1 + (p->turno_global / 3); 
        if (qtd > 5) qtd = 5;
        if (qtd < 2) qtd = 2;
        p->qtd_inimigos_ativos = qtd;
        
        int hp_lacaio = base_hp / qtd;
        int atk_lacaio = base_atk / qtd;
        if (hp_lacaio < 10) hp_lacaio = 10;
        if (atk_lacaio < 5) atk_lacaio = 5;

        // --- BUFFS DE LOCAL  ---
        if(p->estagio_marcha == 1) hp_lacaio = (int)(hp_lacaio * 1.10); 
        if(p->estagio_marcha == 5) atk_lacaio += 20; 
        if(p->estagio_marcha == 6) hp_lacaio += 200; 

        for(int i=0; i < qtd - 1; i++) {
            int tipo_sorteado = INI_SOLDADO;
            int chance_especial = (p->progresso_local * 10); 
            
            if ((rand() % 100) < chance_especial) {
                if (p->estagio_marcha >= 7) {
                    int r_counter = rand() % 100;
                    if (r_counter < 30) tipo_sorteado = INI_CACADOR; 
                    else if (r_counter < 50) tipo_sorteado = INI_FANTASMA; 
                    else tipo_sorteado = (rand() % 12) + 1; 
                } 
                else if (p->estagio_marcha >= 5 && (rand()%100 < 50)) {
                    int r = rand() % 3;
                    if(r==0) tipo_sorteado = INI_GRANADEIRO;
                    if(r==1) tipo_sorteado = INI_ANULADOR;
                    if(r==2) tipo_sorteado = INI_ARQUEIRO;
                } else {
                    tipo_sorteado = (rand() % 12) + 1; 
                }
            }
            configurar_inimigo_unit(&p->esquadrao[i], tipo_sorteado, hp_lacaio, atk_lacaio);
        }

        int idx_lider = qtd - 1;
        int hp_lider = hp_lacaio * 2.5;
        int atk_lider = atk_lacaio * 1.5;
        configurar_inimigo_unit(&p->esquadrao[idx_lider], INI_SOLDADO, hp_lider, atk_lider);
        strcpy(p->esquadrao[idx_lider].nome, titulos_controlador[idx_titulo]);
    } 
    else {
        // --- CHEFES ---
        p->qtd_inimigos_ativos = 1; 
        int boss_hp = base_hp * 8;
        int boss_atk = base_atk * 3;
        int minion_hp = base_hp * 0.5;
        int minion_atk = base_atk * 0.5;

        int t1=-1, t2=-1, t3=-1, t4=-1;
        
        switch(p->boss_id_atual) {
            case 1: strcpy(p->nome_inimigo, "Capitao Aldric"); boss_hp += 200; t1 = INI_SOLDADO; t2 = INI_SOLDADO; t3 = INI_ESCUDEIRO; break;
            case 2: strcpy(p->nome_inimigo, "Sargento Miller"); boss_atk += 40; t1 = INI_BERSERKER; t2 = INI_BERSERKER; break;
            case 3: strcpy(p->nome_inimigo, "Padre Miller"); t1 = INI_CLERIGO; t2 = INI_CLERIGO; t3 = INI_FANATICO; break;
            case 4: strcpy(p->nome_inimigo, "Inquisidor Vane"); t1 = INI_FANATICO; t2 = INI_FANATICO; t3 = INI_GRANADEIRO; break;
            case 5: strcpy(p->nome_inimigo, "Cruzado Kael"); t1 = INI_ESCUDEIRO; t2 = INI_CLERIGO; t3 = INI_ESCUDEIRO; break;
            case 6: strcpy(p->nome_inimigo, "Comandante Darius"); boss_hp += 500; t1 = INI_DUELISTA; t2 = INI_DUELISTA; t3 = INI_ESCUDEIRO; t4 = INI_ARQUEIRO; break;
            case 7: strcpy(p->nome_inimigo, "Arquimago Jinx"); boss_atk += 80; t1 = INI_ANULADOR; t2 = INI_ANULADOR; t3 = INI_VAMPIRO; break;
            case 8: strcpy(p->nome_inimigo, "Bobo Pezao"); t1 = INI_LADRAO; t2 = INI_LADRAO; t3 = INI_VAMPIRO; break;
            case 9: strcpy(p->nome_inimigo, "Principe Tybalt"); boss_hp += 400; boss_atk += 50; t1 = INI_VAMPIRO; t2 = INI_VAMPIRO; t3 = INI_BERSERKER; break;
            case 10: strcpy(p->nome_inimigo, "Guardiao Golem"); boss_hp *= 2; boss_atk -= 20; t1 = INI_ESCUDEIRO; t2 = INI_ESCUDEIRO; t3 = INI_ESCUDEIRO; t4 = INI_ESCUDEIRO; break;
            case 11: strcpy(p->nome_inimigo, "REI DA CIDADELA"); boss_hp *= 3; boss_atk *= 2; t1 = INI_ANULADOR; t2 = INI_VAMPIRO; t3 = INI_DUELISTA; t4 = INI_GRANADEIRO; break;
        }

        int idx = 0;
        if(t1 != -1) configurar_inimigo_unit(&p->esquadrao[idx++], t1, minion_hp, minion_atk);
        if(t2 != -1) configurar_inimigo_unit(&p->esquadrao[idx++], t2, minion_hp, minion_atk);
        if(t3 != -1) configurar_inimigo_unit(&p->esquadrao[idx++], t3, minion_hp, minion_atk);
        if(t4 != -1) configurar_inimigo_unit(&p->esquadrao[idx++], t4, minion_hp, minion_atk);
        
        configurar_inimigo_unit(&p->esquadrao[idx], INI_SOLDADO, boss_hp, boss_atk);
        strcpy(p->esquadrao[idx].nome, "CHEFE"); 
        
        p->qtd_inimigos_ativos = idx + 1;
    }
    
    p->hp_inimigo_atual = 0; p->hp_inimigo_max = 0; p->atk_inimigo_atual = 0;
    for(int i=0; i < p->qtd_inimigos_ativos; i++) {
        p->hp_inimigo_atual += p->esquadrao[i].hp;
        p->hp_inimigo_max += p->esquadrao[i].max_hp;
        p->atk_inimigo_atual += p->esquadrao[i].atk;
    }
}

// --- CONTROLE DE CONSOLE ---
void configurarSistema() {
    // Habilita ANSI no Windows (OBRIGATÓRIO)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; 
            SetConsoleMode(hOut, dwMode);
        }
    }

    //Configura Janela
    system("mode con: cols=160 lines=45");
    HWND consoleWindow = GetConsoleWindow(); 
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    
    //Esconde Cursor
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE; 
    SetConsoleCursorInfo(hOut, &info);
}

void tocar_musica_fundo() {
    //Fecha qualquer musica que esteja tocando antes (garantia)
    mciSendString("close musica_tema", NULL, 0, 0);
    
    //Abre o arquivo mp3 e cria um "alias" (apelido) chamado 'musica_tema'
    //Certifique-se que o arquivo "tema.mp3" esta na mesma pasta do executavel!
    mciSendString("open \"tema.mp3\" type mpegvideo alias musica_tema", NULL, 0, 0);
    
    //Toca em loop (repeat)
    mciSendString("play musica_tema repeat", NULL, 0, 0);
}

void parar_musica() {
    mciSendString("close musica_tema", NULL, 0, 0);
}


void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limparTela() {
    system("cls");
}

// --- ENGINE DE TEXTO (ALINHAMENTO PERFEITO) ---

void gerar_borda(char* buffer, int largura, char canto, char meio) {
    buffer[0] = canto;
    for (int i = 1; i < largura - 1; i++) buffer[i] = meio;
    buffer[largura - 1] = canto;
    buffer[largura] = '\0';
}

void gerar_linha_texto(char* buffer, int largura, const char* texto) {
    int espaco_util = largura - 4; 
    int len = strlen(texto);
    
    char texto_seguro[200];
    strncpy(texto_seguro, texto, espaco_util);
    texto_seguro[espaco_util] = '\0';
    
    sprintf(buffer, "| %-*s |", espaco_util, texto_seguro);
}

void gerar_visual_exercito(char* buffer, int largura, int qtd, char* boneco) {
    char arte[100] = "";
    int max_visual = 5;
    int desenhar = (qtd > max_visual) ? max_visual : qtd;
    
    if (qtd == 0) strcat(arte, "(Vazio)");
    else {
        for(int i=0; i<desenhar; i++) {
            strcat(arte, boneco);
            strcat(arte, " ");
        }
        if (qtd > max_visual) {
            char resto[20];
            sprintf(resto, "+%d", qtd - max_visual);
            strcat(arte, resto);
        }
    }
    gerar_linha_texto(buffer, largura, arte);
}

// Esta funcao desenha o exercito em varias linhas.
// index_linha: qual linha do bloco de desenho estamos desenhando (0, 1, 2...)
// largura: largura da coluna
// total_soldados: quantos tem no total
// boneco: string do boneco (ex: "\o/")
void gerar_visual_extenso(char* buffer, int largura, int total_soldados, int index_linha, char* boneco, int usar_cor_player, char* cor_base_inimigo) {
    int espaco_util = largura - 4;
    int boneco_len = strlen(boneco) + 1; 
    int cabem_por_linha = espaco_util / boneco_len;
    
    int soldados_ja_desenhados = index_linha * cabem_por_linha;
    int restantes = total_soldados - soldados_ja_desenhados;
    
    char arte[1000] = ""; 
    int len_visual = 0; 

    if (restantes > 0) {
        int desenhar_agora = (restantes > cabem_por_linha) ? cabem_por_linha : restantes;
        
        for(int i=0; i < desenhar_agora; i++) {
            if (usar_cor_player) {
                int padrao = (soldados_ja_desenhados + i) % 3;
                if(padrao == 0) strcat(arte, GRN);
                else if(padrao == 1) strcat(arte, CYN);
                else strcat(arte, GRN2);
            } else {
                strcat(arte, cor_base_inimigo);
            }
            strcat(arte, boneco);
            strcat(arte, " ");
        }
        strcat(arte, WHT);
 
        len_visual = desenhar_agora * boneco_len;
        
    } else if (index_linha == 0 && total_soldados == 0) {
        strcat(arte, "(Campo Vazio)");
        len_visual = 13;
    }

    int padding = espaco_util - len_visual;
    if (padding < 0) padding = 0;

    sprintf(buffer, "| %s%*s |", arte, padding, "");
}

//lógica da classe espiritualista para mana
int obter_limite_mana(Necromante* p) {
    // Se for da classe Espiritualista, o limite já é 250 nativo
    if (p->classe == CLASSE_ESPIRITUALISTA) return 250;

    // Se tiver o artefato Pedra Filosofal (Boss 7, Item 1)
    if (p->artefatos[7] == 1) return 250;
    
    // Padrão
    return 100;
}

// --- FUNÇÃO AUXILIAR PARA NOMES DOS LOCAIS ---
const char* get_nome_local(int estagio) {
    switch(estagio) {
        case 1: return "Posto da Fronteira";
        case 2: return "Estrada Real";
        case 3: return "Monasterio da Luz";
        case 4: return "Portoes da Cidade";
        case 5: return "Praca dos Martires";
        case 6: return "QG da Guarda";
        case 7: return "Distrito da Alq.";
        case 8: return "Jardins Reais";
        case 9: return "Salao de Baile";
        case 10: return "Ante-Sala Trono";
        case 11: return "TRONO DOURADO";
        case 12: return "CIDADELA (DOMINADA)";
        default: return "Desconhecido";
    }
}

//descrição dos chefes
const char* get_descricao_boss(Necromante* p) {
    int id = p->boss_id_atual;
    int local = p->estagio_marcha;

    if (id > 0) {
        switch(id) {
            case 1: return "Resiste a 50% do dano Fisico (Esq/Zumbi/Carn/Abom)"; 
            case 2: return "Ao matar lacaio: 10 de dano em VOCE";    
            case 3: return "Causa DANO DOBRADO em seus lacaios";
            case 4: return "Ganha +10 ATK por cada lacaio vivo";     
            case 5: return "Ao morrer: Revive 1 vez com 100% HP";   
            case 6: return "General: Status base muito elevados";
            case 7: return "Veneno: -15 HP em todos lacaios/turno";  
            case 8: return "Ao matar lacaio: Se cura (Vida Max)";
            case 9: return "Furia: +20 ATK a cada turno de luta";   
            case 10: return "IMUNE a qualquer ataque menor que 150"; 
            case 11: return "O REI: Poder Absoluto e Esmagador";
            default: return "Chefe Desconhecido";
        }
    }
    
    switch(local) {
        case 1: return "Chuva e Lama: Inimigos tem +10% de HP";
        case 2: return "Rota de Comercio: Escavar gera +1 Corpo";
        case 3: return "Solo Consagrado: Magia Vil custa 25 HP";
        case 4: return "Torres: Chuva de flechas no inicio (20 Dano)"; 
        case 5: return "Fervor: Inimigos tem +20 de Ataque";          
        case 6: return "Elite: Inimigos tem +200 de HP";              
        case 7: return "Residuos: Vitoria gera +10 Mana extra";
        case 8: return "Labirinto: Inimigo tem 20% de chance de errar";
        case 9: return "Marmore: Invocacoes Especiais custam so 5 HP";
        case 10: return "Aura Real: -5 HP a cada acao fora de combate"; 
        default: return "Sem efeitos ambientais.";
    }
}


// --- BANCO DE DADOS DE ARTEFATOS ---
void get_info_artefato(int boss_id, int opcao, char* nome_buffer, char* desc_buffer) {
    switch(boss_id) {
        case 1: // Aldric
            if(opcao==1) { strcpy(nome_buffer, "Cutelo Enferrujado"); strcpy(desc_buffer, "Esqueletos e Zumbis ganham +3 ATK."); }
            if(opcao==2) { strcpy(nome_buffer, "Estandarte da Fronteira"); strcpy(desc_buffer, "Escavar gera +1 Corpo extra sempre."); }
            if(opcao==3) { strcpy(nome_buffer, "Elmo do Capitao"); strcpy(desc_buffer, "Seus Zumbis tem +20 HP."); }
            if(opcao==4) { strcpy(nome_buffer, "Pa Ancestral"); strcpy(desc_buffer, "Escavar cura +5 HP."); }
            break;
        case 2: // Miller
            if(opcao==1) { strcpy(nome_buffer, "Coleira de Espinhos"); strcpy(desc_buffer, "Seu lacaio morre -> Inimigo toma 5 dano."); }
            if(opcao==2) { strcpy(nome_buffer, "Ferramentas de Tortura"); strcpy(desc_buffer, "Sacrificar custa 0 HP para QUALQUER lacaio."); }
            if(opcao==3) { strcpy(nome_buffer, "Botas de Chumbo"); strcpy(desc_buffer, "Imune a danos de terreno (Local)."); }
            if(opcao==4) { strcpy(nome_buffer, "Mordaca de Ferro"); strcpy(desc_buffer, "Sifao nao mata aliado, deixa com 1 HP."); }
            break;
        case 3: // Padre
            if(opcao==1) { strcpy(nome_buffer, "Calice Profano"); strcpy(desc_buffer, "Come-Mana e Lich tem +20 HP."); }
            if(opcao==2) { strcpy(nome_buffer, "Rosario Amaldicoado"); strcpy(desc_buffer, "+5 Mana apos vencer batalhas."); }
            if(opcao==3) { strcpy(nome_buffer, "Mortalha do Martir"); strcpy(desc_buffer, "Sifao cura +10 HP extra."); }
            if(opcao==4) { strcpy(nome_buffer, "Mitra Corrompida"); strcpy(desc_buffer, "Esqueletos nascem como Zumbis."); }
            break;
        case 4: // Vane
            if(opcao==1) { strcpy(nome_buffer, "Totem de Cinzas"); strcpy(desc_buffer, "Legiao custa apenas 1 Corpo."); }
            if(opcao==2) { strcpy(nome_buffer, "Fosforo do Inquisidor"); strcpy(desc_buffer, "Abominacao reflete 10 dano ao ser atacada."); }
            if(opcao==3) { strcpy(nome_buffer, "Manto Ignifugo"); strcpy(desc_buffer, "Reduz pela metade o dano de Instabilidade Arcana."); }
            if(opcao==4) { strcpy(nome_buffer, "Chama Negra"); strcpy(desc_buffer, "Abominacao custa 3 Corpos e 75 Mana."); }
            break;
        case 5: // Kael
            if(opcao==1) { strcpy(nome_buffer, "Coracao Imortal"); strcpy(desc_buffer, "Zumbis revivem 1 vez com 1 HP."); }
            if(opcao==2) { strcpy(nome_buffer, "Escudo da Luz"); strcpy(desc_buffer, "Bloqueia o primeiro ataque do inimigo na batalha."); }
            if(opcao==3) { strcpy(nome_buffer, "Bencao Corrompida"); strcpy(desc_buffer, "Sombra copia 60% dos status (era 50%)."); }
            if(opcao==4) { strcpy(nome_buffer, "Prisma do Eclipse"); strcpy(desc_buffer, "Sombra explode ao morrer (Dano em Area)."); }
            break;
        case 6: // Darius
            if(opcao==1) { strcpy(nome_buffer, "Coroa de Ferro"); strcpy(desc_buffer, "+50 Vida Maxima imediatamente."); }
            if(opcao==2) { strcpy(nome_buffer, "Mapa de Guerra"); strcpy(desc_buffer, "Magia Vil nao aumenta mais o ATK do inimigo."); }
            if(opcao==3) { strcpy(nome_buffer, "Berrante de Guerra"); strcpy(desc_buffer, "Todos os lacaios ganham +2 ATK."); }
            break;
        case 7: // Jinx
            if(opcao==1) { strcpy(nome_buffer, "Pedra Filosofal"); strcpy(desc_buffer, "Limite de Mana sobe para 250."); }
            if(opcao==2) { strcpy(nome_buffer, "Mascara de Gas"); strcpy(desc_buffer, "Lacaios imunes a veneno e efeitos de chefe."); }
            if(opcao==3) { strcpy(nome_buffer, "Frasco de Morte"); strcpy(desc_buffer, "Lich custa 40 Mana (era 75)."); }
            break;
        case 8: // Pezao
            if(opcao==1) { strcpy(nome_buffer, "Dados Viciados"); strcpy(desc_buffer, "Escavar tem 50% chance de achar Mana extra."); }
            if(opcao==2) { strcpy(nome_buffer, "Caixa de Surpresas"); strcpy(desc_buffer, "Devorador ao morrer spawna 3 Esqueletos."); }
            if(opcao==3) { strcpy(nome_buffer, "Ouro de Tolo"); strcpy(desc_buffer, "Sacrificar gera +20% Mana."); }
            break;
        case 9: // Tybalt
            if(opcao==1) { strcpy(nome_buffer, "Rapieira Real"); strcpy(desc_buffer, "Carnicais causam DANO DOBRADO."); }
            if(opcao==2) { strcpy(nome_buffer, "Decreto Real"); strcpy(desc_buffer, "Magia Vil nao tem penalidade de Local/Emboscada."); }
            if(opcao==3) { strcpy(nome_buffer, "Anel de Noivado"); strcpy(desc_buffer, "Sifao drena vida do lacaio sem matar."); }
            break;
        case 10: // Golem
            if(opcao==1) { strcpy(nome_buffer, "Nucleo de Golem"); strcpy(desc_buffer, "Devorador ganha +5 ATK por corpo comido."); }
            if(opcao==2) { strcpy(nome_buffer, "Quebra-Cercos"); strcpy(desc_buffer, "Lacaios causam +20 dano em Chefes."); }
            if(opcao==3) { strcpy(nome_buffer, "Chave do Trono"); strcpy(desc_buffer, "Comeca a luta contra o Rei com 100 Mana/5 Corpos."); }
            break;
    }
}

const char* get_nome_item(int id) {
    switch(id) {
        case 1: return "Elixir de Sangue";
        case 2: return "Frasco de Eter";
        case 3: return "Saco de Ossos";
        case 4: return "Pedra de Afiar";
        case 5: return "Perg. da Barreira";
        case 6: return "Contrato Branco";
        case 7: return "Incenso Purif.";
        case 8: return "Fogo Grego";
        case 9: return "Cristal Estab.";
        case 10: return "Berrante da Ress.";
        default: return "Vazio";
    }
}

void gerar_drop_item(Necromante* p) {
    if ((rand() % 100) < 25) { // 25% chance
        int slot = -1;
        for(int i=0; i<3; i++) if(p->inventario[i] == 0) { slot = i; break; }
        
        if (slot == -1) {
            strcat(p->relatorio, " (Item perdido: Mochila cheia)");
            return;
        }
        
        int r = rand() % 100;
        int item_id = 0;
        if (r < 50) item_id = (rand() % 4) + 1;
        else if (r < 85) item_id = (rand() % 3) + 5;
        else item_id = (rand() % 3) + 8;
        
        p->inventario[slot] = item_id;
        
        // Pega ASCIIs
        char a1[10], a2[10];
void obter_ascii_consumivel(item_id, a1, a2);
        
        // Limpa espaços extras das strings ASCII para caber melhor no log
        // (Ex: " .o. " vira ".o.") - Opcional, mas ajuda no log
        
        char drop_msg[100];
        // Formato: Achou: ( .o. / (_) ) Nome
        sprintf(drop_msg, " | Achou: (%s/%s) %s!", a1, a2, get_nome_item(item_id));
        
        // Remove espaços duplos se houver para economizar espaço no log
        // (Não implementado aqui para manter simples, o log aguenta)
        
        strcat(p->relatorio, drop_msg);
    }
}

// Função inteligente para alinhar texto colorido sem quebrar a borda
void gerar_linha_colorida(char* buffer, int largura, const char* texto_limpo, const char* cor_inicio, const char* cor_fim) {
    int espaco_util = largura - 4; 
    int len_texto = strlen(texto_limpo);
    int padding = espaco_util - len_texto;
    
    if (padding < 0) padding = 0; 

    // Monta: "| " + COR + TEXTO + RESET + ESPAÇOS + " |"
    // O truque: Os espaços (%*s) são calculados baseados no texto LIMPO
    sprintf(buffer, "| %s%s%s%*s |", cor_inicio, texto_limpo, cor_fim, padding, "");
}

void gerar_linha_hud_colorida(char* buffer, int largura, const char* texto_limpo, const char* cor) {
    int espaco_util = largura - 4; 
    int len = strlen(texto_limpo);
    int padding = espaco_util - len;
    if (padding < 0) padding = 0;
    
    sprintf(buffer, "| %s%s%s%*s |", cor, texto_limpo, WHT, padding, "");
}

// --- FUNÇÃO DE INTERFACE ---
void desenhar_interface(Necromante* player, int tipo_exibicao) {
    // ANTI-FLICKER
    irParaInicio(); 

    char buffer_renderizacao[32000] = ""; 
    char linha_temp[1000];

    char tela_esq[ALTURA_TELA][W_MENU + 100];
    char tela_dir[ALTURA_TELA][W_HUD + 100];
    char tela_lista[ALTURA_TELA][W_LISTA + 100];

    // --- CALCULOS GERAIS ---
    int atkTotal = 0, hpTropa = 0;
    Minion* p = player->exercito_head;
    while(p) { atkTotal += p->ataque; hpTropa += p->hp; p = p->proximo; }
    
    int hpInimigo = player->hp_inimigo_atual; 
    if (hpInimigo < 0) hpInimigo = 0;
    int atkInimigo = player->atk_inimigo_atual;  
    
    // Visual HUD Central
    int qtdSoldadosVisuais = (hpInimigo <= 0) ? 0 : player->qtd_inimigos_ativos;

    // Calcula o Limite de Escavação (Coveiro = 7, Outros = 5)
    int max_escavar = (player->classe == CLASSE_COVEIRO) ? 7 : 5;
    
    // Obtém o Limite de Mana Real (Espiritualista/Pedra = 250, Outros = 100)
    int max_mana_atual = obter_limite_mana(player);

    // --- PREPARACAO DOS TEXTOS ---
    char txt_turno[50], txt_mana[80], txt_invocar[50], txt_escavar[50];
    char txt_local[50], txt_diff[50];

    sprintf(txt_diff, "DIFICULDADE GLOBAL: %d", player->turno_global);
    sprintf(txt_local, "LOCAL: %s [%d/10]", get_nome_local(player->estagio_marcha), player->progresso_local);

    // --- Usa o limite dinâmico para decidir o aviso ---
    if(player->mana > max_mana_atual) {
        sprintf(txt_mana, "MANA: %d/%d | CPS: %d [INSTABILIDADE]", player->mana, max_mana_atual, player->corpos);
    } else {
        sprintf(txt_mana, "MANA: %d/%d | CORPOS: %d", player->mana, max_mana_atual, player->corpos);
    }

    sprintf(txt_escavar, "1. Escavar Tumulos (%d/%d)", player->escavacoes_turno, max_escavar);
    
    sprintf(txt_invocar, "2. MENU DE INVOCACAO (Especial)");

    char raw_necro_hp[100], raw_status1[100], raw_status2[100];
    char raw_enemy_nome[100], raw_enemy_stats[100], raw_enemy_atk[100];
    
    char nome_classe[30];
    char status_ult[10] = "";
    if (player->habilidade_usada == 0 && player->classe > 0) strcpy(status_ult, "[*]");
    else strcpy(status_ult, "[ ]");

    if (player->classe == CLASSE_COVEIRO) sprintf(nome_classe, "O COVEIRO %s", status_ult);
    else if (player->classe == CLASSE_MAGO_SANGUE) sprintf(nome_classe, "MAGO DE SANGUE %s", status_ult);
    else if (player->classe == CLASSE_ESPIRITUALISTA) sprintf(nome_classe, "ESPIRITUALISTA %s", status_ult);
    else strcpy(nome_classe, "NENHUM");

    sprintf(raw_necro_hp, "SUA VIDA: %d / %d | %s", player->hp, player->max_hp, nome_classe);
    sprintf(raw_status1, "Qtd: %-3d   Dano: %-4d", player->total_minions, atkTotal);
    sprintf(raw_status2, "HP Tropa: %-4d", hpTropa);
    
    sprintf(raw_enemy_nome, "ALVO: %s", player->nome_inimigo);
    sprintf(raw_enemy_stats, "Esquadrao: %dun  HP Total: %d", player->qtd_inimigos_ativos, hpInimigo);
    sprintf(raw_enemy_atk, "Dano Total: %-4d", atkInimigo);


// ================= COLUNA 1: MENU (COLORIDO) =================
    // Título em Magenta
    gerar_borda(tela_esq[0], W_MENU, '=', '=');
    sprintf(tela_esq[1], "| %s%-46s%s |", MAG, "NECRONQUISTA", WHT);
    gerar_borda(tela_esq[2], W_MENU, '=', '=');
    
    // Status Gerais em Amarelo
    sprintf(tela_esq[3], "| %s%-46s%s |", YEL, txt_local, WHT);
    sprintf(tela_esq[4], "| %s%-46s%s |", YEL, txt_diff, WHT);
    
    // Mana (Lógica especial de alinhamento mantida, mas agora forçando Magenta)
    if (player->mana > max_mana_atual) {
        char clean_mana[100];
        sprintf(clean_mana, "MANA: %d/%d | CPS: %d [INSTABILIDADE]", player->mana, max_mana_atual, player->corpos);
        int pad = W_MENU - 4 - strlen(clean_mana);
        // Usa MAG para Mana e RED para o aviso de Instabilidade
        sprintf(tela_esq[5], "| %sMANA: %d/%d | CPS: %d %s[INSTABILIDADE]%s%*s |", MAG, player->mana, max_mana_atual, player->corpos, RED, WHT, pad, "");
    } else {
        sprintf(tela_esq[5], "| %s%-46s%s |", MAG, txt_mana, WHT);
    }

    gerar_borda(tela_esq[6], W_MENU, '-', '-');
    
    // Seção Gerenciamento (Verde)
    sprintf(tela_esq[7], "| %s%-46s%s |", GRN, "ACOES DE GERENCIAMENTO:", WHT);
    sprintf(tela_esq[8], "| %s%-46s%s |", GRN, txt_escavar, WHT);
    sprintf(tela_esq[9], "| %s%-46s%s |", GRN, txt_invocar, WHT);
    sprintf(tela_esq[10], "| %s%-46s%s |", GRN, "3. Ver Detalhes do Exercito", WHT);
    
    gerar_linha_texto(tela_esq[11], W_MENU, "");
    
    // Seção Combate (Vermelho)
    sprintf(tela_esq[12], "| %s%-46s%s |", RED, "ACOES DE COMBATE:", WHT);
    sprintf(tela_esq[13], "| %s%-46s%s |", RED, "4. Batalhar (COMBATE SIMULTANEO)", WHT);
    sprintf(tela_esq[14], "| %s%-46s%s |", RED, "5. Magia Vil (Custo: 5 HP)", WHT);
    sprintf(tela_esq[15], "| %s%-46s%s |", RED, "6. Sacrificar Lacaio (-5 HP / +Mana)", WHT);
    
    // Habilidades e Consumíveis (Amarelo/Especial)
    sprintf(tela_esq[16], "| %s%-46s%s |", YEL, "*. Assinatura (Habilidade Especial)", WHT);
    sprintf(tela_esq[17], "| %s%-46s%s |", YEL, "+. Consumiveis", WHT);
    
    // Seção Sistema (Ciano)
    sprintf(tela_esq[18], "| %s%-46s%s |", CYN, "SISTEMA:", WHT);
    sprintf(tela_esq[19], "| %s%-46s%s |", CYN, "-. Ver Artefatos Equipados", WHT);
    sprintf(tela_esq[20], "| %s%-46s%s |", CYN, "7. Lacaios (Enciclopedia)", WHT);
    sprintf(tela_esq[21], "| %s%-46s%s |", CYN, "8. Regras (Manual do Jogo)", WHT);
    sprintf(tela_esq[22], "| %s%-46s%s |", CYN, "9. Reiniciar Jogo", WHT);
    sprintf(tela_esq[23], "| %s%-46s%s |", CYN, "0. Sair do Jogo", WHT);
    
    for(int i=24; i<ALTURA_TELA-1; i++) gerar_linha_texto(tela_esq[i], W_MENU, "");
    gerar_borda(tela_esq[ALTURA_TELA-1], W_MENU, '=', '=');


    // ================= COLUNA 2: HUD =================
    int l = 0;
    char* c_ini = (player->boss_id_atual > 0) ? YEL : CYN;

    gerar_borda(tela_dir[l++], W_HUD, '+', '=');
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_necro_hp, GRN);
    gerar_borda(tela_dir[l++], W_HUD, '|', '-');
    
    gerar_linha_texto(tela_dir[l++], W_HUD, "SEUS LACAIOS:");
    for(int i=0; i<8; i++) {
        gerar_visual_extenso(tela_dir[l++], W_HUD, player->total_minions, i, "\\o/", 1, "");
    }
    
    gerar_linha_texto(tela_dir[l++], W_HUD, "STATUS DO EXERCITO:");
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_status1, GRN);
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_status2, GRN);
    
    gerar_borda(tela_dir[l++], W_HUD, '|', '='); 
    
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_enemy_nome, c_ini);
    for(int i=0; i<8; i++) {
        gerar_visual_extenso(tela_dir[l++], W_HUD, qtdSoldadosVisuais, i, "[T]", 0, c_ini);
    }
    
    gerar_linha_texto(tela_dir[l++], W_HUD, "STATUS DO INIMIGO:");
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_enemy_stats, RED);
    gerar_linha_hud_colorida(tela_dir[l++], W_HUD, raw_enemy_atk, RED);
    
    gerar_linha_texto(tela_dir[l++], W_HUD, "");
    gerar_linha_texto(tela_dir[l++], W_HUD, "HABILIDADE ESPECIAL:");
    gerar_linha_texto(tela_dir[l++], W_HUD, get_descricao_boss(player));
    while(l < ALTURA_TELA-1) gerar_linha_texto(tela_dir[l++], W_HUD, "");
    gerar_borda(tela_dir[ALTURA_TELA-1], W_HUD, '+', '=');


    // ================= COLUNA 3: LISTA TÁTICA (CORRIGIDO ALINHAMENTO) =================
    for(int i=0; i<ALTURA_TELA; i++) sprintf(tela_lista[i], "%-*s", W_LISTA, " ");
    l = 0;
    gerar_borda(tela_lista[l++], W_LISTA, '+', '-');
    
    // Cabeçalho
    gerar_linha_hud_colorida(tela_lista[l++], W_LISTA, "OPONENTES (Retaguarda = Topo)", c_ini);
    sprintf(tela_lista[l++], "| %-18s | %-6s | %-5s |", "Nome", "HP", "ATK");
    gerar_borda(tela_lista[l++], W_LISTA, '|', '-');
    
    int slots_inimigo = 9;
    
    // --- LOOP INVERTIDO (CIMA P/ BAIXO) ---
    for(int i=0; i<slots_inimigo; i++) {
        int idx_mostrar = (player->qtd_inimigos_ativos - 1) - i;
        
        if(idx_mostrar >= 0 && player->esquadrao[idx_mostrar].vivo) {
            char nome_mob[30];
            char nome_truncado[19];
            
            sprintf(nome_mob, "#%d %s", idx_mostrar+1, player->esquadrao[idx_mostrar].nome);
            
            // Trunca nome para 18 chars para não quebrar tabela
            strncpy(nome_truncado, nome_mob, 18);
            nome_truncado[18] = '\0';

            const char* c_tipo = get_cor_inimigo_tipo(player->esquadrao[idx_mostrar].id_tipo);
            
            // --- CORREÇÃO PADDING RÍGIDO ---
            char linha_clean[100];
            sprintf(linha_clean, "%-18s | %-6d | %-5d", 
                nome_truncado, player->esquadrao[idx_mostrar].hp, player->esquadrao[idx_mostrar].atk);
            
            // Calcula o espaço que falta para chegar em W_LISTA - 4
            int padding = (W_LISTA - 4) - strlen(linha_clean);
            if(padding < 0) padding = 0;

            sprintf(tela_lista[l++], "| %s%-18s%s | %-6d | %-5d%*s |", 
                c_tipo, nome_truncado, WHT, 
                player->esquadrao[idx_mostrar].hp, player->esquadrao[idx_mostrar].atk,
                padding, ""
            );
        } else {
            gerar_linha_texto(tela_lista[l++], W_LISTA, " ");
        }
    }
    
    gerar_borda(tela_lista[l++], W_LISTA, '+', '=');
    gerar_linha_hud_colorida(tela_lista[l++], W_LISTA, "          VS (Combate)          ", RED);
    gerar_borda(tela_lista[l++], W_LISTA, '+', '=');
    
    gerar_linha_hud_colorida(tela_lista[l++], W_LISTA, "SEU EXERCITO (Frente = Baixo)", GRN);
    sprintf(tela_lista[l++], "| %-18s | %-6s | %-5s |", "Tecla/ID/Nome", "HP", "ATK");
    gerar_borda(tela_lista[l++], W_LISTA, '|', '-');
    int linha_comeco_player = l; 
    int linha_fim_player = ALTURA_TELA - 2; 
    int slots_player = linha_fim_player - linha_comeco_player + 1;
    Minion* m_player = player->exercito_head;
    
    for(int i=0; i < slots_player; i++) {
        int linha_atual = linha_fim_player - i;
        if (m_player != NULL) {
            char nome_com_id[30];
            char nome_truncado[19];
            
            sprintf(nome_com_id, "[%d] %s", i+1, m_player->nome);
            
            // Trunca nome
            strncpy(nome_truncado, nome_com_id, 18);
            nome_truncado[18] = '\0';
            
            const char* c_row = GRN;
            if (i % 3 == 1) c_row = CYN;
            if (i % 3 == 2) c_row = GRN2;

            // --- CORREÇÃO PADDING RÍGIDO (PLAYER) ---
            char linha_clean[100];
            sprintf(linha_clean, "%-18s | %-6d | %-5d", 
                nome_truncado, m_player->hp, m_player->ataque);
            
            int padding = (W_LISTA - 4) - strlen(linha_clean);
            if(padding < 0) padding = 0;

            sprintf(tela_lista[linha_atual], "| %s%-18s%s | %-6d | %-5d%*s |", 
                c_row, nome_truncado, WHT, 
                m_player->hp, m_player->ataque,
                padding, ""
            );
            
            m_player = m_player->proximo;
        } else {
            gerar_linha_texto(tela_lista[linha_atual], W_LISTA, "");
        }
    }
    gerar_borda(tela_lista[ALTURA_TELA-1], W_LISTA, '+', '-');

    // =================================================================================
    // RENDERIZACAO FINAL
    // =================================================================================
    for(int i=0; i<ALTURA_TELA; i++) {
        sprintf(linha_temp, "%s%s   %s   %s%s\n", 
            WHT, tela_esq[i],   
            tela_dir[i],        
            tela_lista[i],      
            RST);
        strcat(buffer_renderizacao, linha_temp);
    }

// --- RODAPÉ ---
    int largura_total = W_MENU + 3 + W_HUD + 3 + W_LISTA;
    char linha_borda[200], linha_texto[200];
    
    strcat(buffer_renderizacao, "\n");
    gerar_borda(linha_borda, largura_total, '=', '='); 
    sprintf(linha_temp, "%s%s%s\n", WHT, linha_borda, RST); strcat(buffer_renderizacao, linha_temp);
    
    // Título LOG em Amarelo
    gerar_linha_texto(linha_texto, largura_total, "LOG DE EVENTOS");
    // Aqui usamos um truque: substituir o texto branco por colorido na string gerada seria difícil
    // Então montamos a linha manualmente com cor:
    int pad_log = largura_total - 4 - 14; // 14 é len("LOG DE EVENTOS")
    sprintf(linha_temp, "%s| %sLOG DE EVENTOS%s%*s |%s\n", WHT, YEL, WHT, pad_log, "", RST);
    strcat(buffer_renderizacao, linha_temp);
    
    // Conteúdo do Log em Ciano (Destaque de leitura)
    int espaco_texto = largura_total - 8;
    sprintf(linha_temp, "%s|  > %s%-*s%s  |%s\n", WHT, CYN, espaco_texto, player->relatorio, WHT, RST); 
    strcat(buffer_renderizacao, linha_temp);
    
    gerar_borda(linha_borda, largura_total, '=', '='); 
    sprintf(linha_temp, "%s%s%s\n", WHT, linha_borda, RST); strcat(buffer_renderizacao, linha_temp);

    // Prompts
    if (tipo_exibicao == TELA_COMANDO) sprintf(linha_temp, "\n%s[COMANDO] Digite sua opcao: %s", WHT, RST); 
    else if (tipo_exibicao == TELA_ENTER) sprintf(linha_temp, "\n%s[AVISO] Pressione ENTER%s", YEL, RST); 
    else sprintf(linha_temp, "\n"); 
    
    strcat(buffer_renderizacao, linha_temp);
    strcat(buffer_renderizacao, "          "); 

    printf("%s", buffer_renderizacao);
}
// --- LOGICA DE ATUALIZACAO ---

void atualizar_relatorio(Necromante* player, char* texto) {
    // Apenas atualiza o texto na memória
    strncpy(player->relatorio, texto, 299);
    player->relatorio[299] = '\0';
    
    // Redesenha a tela JÁ com o prompt de comando
    desenhar_interface(player, TELA_COMANDO);
}

void pausa_vitoria(Necromante* player, char* texto) {
    strncpy(player->relatorio, texto, 299);
    player->relatorio[299] = '\0';
    
    desenhar_interface(player, TELA_ENTER);
    
    // Loop que espera especificamente a tecla ENTER (código 13)
    // _getch() lê a tecla sem imprimir na tela.
    while(_getch() != 13); 
}

// --- GAMEPLAY ---

Minion* criar_minion_especifico(Necromante* p, int id, int tipo, int extra_info) {
    Minion* novo = (Minion*) calloc(1, sizeof(Minion));
    if(!novo) exit(1);
    
    novo->id = id;
    novo->decomposicao = DECOMPOSICAO_INICIAL + (rand()%3);
    novo->proximo = NULL;
    novo->codigo_origem = tipo;
    novo->hp = 1;      
    novo->ataque = 1;  
    
    switch(tipo) {
        case 1: 
            if(p->artefatos[3] == 4) {
                strcpy(novo->nome, "Zumbi (Mitra)"); strcpy(novo->tipo, "Tank"); 
                novo->ataque = ATK_ZUM; novo->hp = HP_ZUM;
                if(p->artefatos[1] == 1) novo->ataque += 3;
                if(p->artefatos[1] == 3) novo->hp += 20;
            } else {
                strcpy(novo->nome, "Esqueleto"); strcpy(novo->tipo, "Infant"); 
                novo->ataque = ATK_ESQ; novo->hp = HP_ESQ;
                if(p->artefatos[1] == 1) novo->ataque += 3;
            }
            break;
        case 2: strcpy(novo->nome, "Zumbi"); strcpy(novo->tipo, "Tank"); novo->ataque = ATK_ZUM; novo->hp = HP_ZUM; if(p->artefatos[1] == 1) novo->ataque += 3; if(p->artefatos[1] == 3) novo->hp += 20; break;
        case 3: strcpy(novo->nome, "Carnical"); strcpy(novo->tipo, "Dano"); novo->ataque = ATK_CAR; novo->hp = HP_CAR; if(p->artefatos[9] == 1) novo->ataque *= 2; break;
        case 4: strcpy(novo->nome, "ABOMINACAO"); strcpy(novo->tipo, "TITAN"); novo->ataque = ATK_ABO; novo->hp = HP_ABO; break;
            
        // --- CORREÇÃO DA SOMBRA AQUI ---
        case 5: // SOMBRA
            strcpy(novo->nome, "Sombra"); strcpy(novo->tipo, "Mirror");
            float fator = (p->artefatos[5] == 3) ? 0.6 : 0.5;
            
            // Identifica o Líder/Chefe (Última unidade do vetor)
            int idx_lider = p->qtd_inimigos_ativos - 1;
            int hp_alvo = 0, atk_alvo = 0;
            
            if (idx_lider >= 0) {
                hp_alvo = p->esquadrao[idx_lider].max_hp;
                atk_alvo = p->esquadrao[idx_lider].atk;
            }
            
            // Se o inimigo não existir (ex: criando sombra fora de batalha/cheat), usa valores de fallback
            if (hp_alvo == 0) { hp_alvo = 10; atk_alvo = 2; }

            novo->hp = (int)(hp_alvo * fator);
            novo->ataque = (int)(atk_alvo * fator);
            
            if (novo->hp < 1) novo->hp = 1;
            if (novo->ataque < 1) novo->ataque = 1;
            break;

        case 6: strcpy(novo->nome, "Legiao"); strcpy(novo->tipo, "Swarm"); novo->hp = HP_LEG; novo->ataque = 5 + (p->total_minions * 2); break;
        case 7: strcpy(novo->nome, "Come-Mana"); strcpy(novo->tipo, "Magic"); novo->hp = HP_COM; if(p->artefatos[3] == 1) novo->hp += 20; novo->ataque = extra_info; break;
        case 8: strcpy(novo->nome, "Devorador"); strcpy(novo->tipo, "Beast"); novo->ataque = ATK_DEV; if(p->artefatos[10] == 1) novo->ataque += (extra_info * 5); int hp_calc = extra_info * 15; if(hp_calc<=0) hp_calc=15; novo->hp = hp_calc; break;
        case 9: strcpy(novo->nome, "Lich"); strcpy(novo->tipo, "Mage"); novo->hp = HP_LIC; if(p->artefatos[3] == 1) novo->hp += 20; novo->ataque = ATK_LIC; break;
        case 10: strcpy(novo->nome, "Sifao"); strcpy(novo->tipo, "Support"); novo->ataque = ATK_SIF; novo->hp = HP_SIF; break;

        // Lacaios de Evento
        case 11: strcpy(novo->nome, "Cavaleiro S.C."); strcpy(novo->tipo, "Elite"); novo->ataque = 80; novo->hp = 80; break;
        case 12: strcpy(novo->nome, "Paladino Caido"); strcpy(novo->tipo, "Tank"); novo->ataque = 20; novo->hp = 200; break;
        case 13: strcpy(novo->nome, "Homunculo"); strcpy(novo->tipo, "Bomb"); novo->ataque = 5; novo->hp = 50; break;
        case 14: strcpy(novo->nome, "Besta Cripta"); strcpy(novo->tipo, "Beast"); novo->ataque = 40; novo->hp = 120; break;
        case 15: strcpy(novo->nome, "Arq. Esqueleto"); strcpy(novo->tipo, "Ranged"); novo->ataque = 60; novo->hp = 40; break;
        case 16: strcpy(novo->nome, "Sacerdote Som."); strcpy(novo->tipo, "Support"); novo->ataque = 15; novo->hp = 60; break;
        case 17: strcpy(novo->nome, "Espectro"); strcpy(novo->tipo, "Debuff"); novo->ataque = 30; novo->hp = 70; break;
        case 18: strcpy(novo->nome, "Golem Ossos"); strcpy(novo->tipo, "Tank"); novo->ataque = 30; novo->hp = 150; break;
        case 19: strcpy(novo->nome, "Vampiro Nobre"); strcpy(novo->tipo, "Elite"); novo->ataque = 45; novo->hp = 90; break;
        case 20: strcpy(novo->nome, "O Verdugo"); strcpy(novo->tipo, "Elite"); novo->ataque = 70; novo->hp = 100; break;
    }
    
    if(p->artefatos[6] == 3) novo->ataque += 2;
    return novo;
}

void menu_invocacao(Necromante* player) {
    if(player->estagio_marcha == 10 && player->boss_id_atual == 0) {
        if(player->artefatos[2] != 3) player->hp -= 5;
    }

    int limite_mana = obter_limite_mana(player);
    int estava_instavel = (player->mana > limite_mana);

    char menu_texto[300];
    sprintf(menu_texto, "[1]Esqueleto [2]Zumbi [3]Carnical [4]ABOMINACAO | [ESPECIAIS -10 HP]: [5]Sombra [6]Legiao [7]Come-Mana [8]Devorador [9]Lich [+]Sifao");
    strncpy(player->relatorio, menu_texto, 299);
    desenhar_interface(player, TELA_COMANDO);
    
    char ch = _getch();
    if (ch == 27 || ch == '0') { 
        atualizar_relatorio(player, "Invocacao cancelada.");
        return;
    }
    
    int escolha = 0;
    if(ch >= '1' && ch <= '9') escolha = ch - '0';
    else if (ch == '+') escolha = 10; 
    else {
        atualizar_relatorio(player, "Opcao invalida.");
        return;
    }
    
    int custo_mana = 0, custo_corpo = 0;
    int custo_hp_base = 0; 
    int extra_info = 0;
    
    switch(escolha) {
        case 1: custo_mana = C_MANA_ESQ; custo_corpo = C_CORPO_ESQ; break;
        case 2: custo_mana = C_MANA_ZUM; custo_corpo = C_CORPO_ZUM; break;
        case 3: custo_mana = C_MANA_CAR; custo_corpo = C_CORPO_CAR; break;
        
        case 4: // ABOMINACAO
            custo_mana = C_MANA_ABO; 
            custo_corpo = C_CORPO_ABO;
            // Artefato 4-4: Chama Negra (Desconto)
            if(player->artefatos[4] == 4) {
                custo_mana = 75;
                custo_corpo = 3;
            }
            break;
        
        case 5: custo_mana = C_MANA_SOM; custo_corpo = C_CORPO_SOM; custo_hp_base = 10; break;
        case 6: custo_mana = C_MANA_LEG; custo_corpo = C_CORPO_LEG; custo_hp_base = 10; 
            if(player->artefatos[4] == 1) custo_corpo = 1; 
            break;
        case 7: custo_mana = C_MANA_COM; custo_corpo = C_CORPO_COM; custo_hp_base = 10; break;
        case 8: custo_mana = C_MANA_DEV; custo_corpo = 1;           custo_hp_base = 10; break;
        case 9: custo_mana = C_MANA_LIC; custo_corpo = C_CORPO_LIC; custo_hp_base = 10; 
            if(player->artefatos[7] == 3) custo_mana = 40; 
            break;
        case 10: custo_mana = C_MANA_SIF; custo_corpo = C_CORPO_SIF; custo_hp_base = 10; break;
        default: return;
    }
    
    if (custo_hp_base > 0) { 
        if (player->classe == CLASSE_MAGO_SANGUE) custo_hp_base = 15;
        else if (player->classe == CLASSE_ESPIRITUALISTA) custo_hp_base = 0;
        
        if(player->estagio_marcha == 9 && player->boss_id_atual == 0 && player->classe != CLASSE_ESPIRITUALISTA) {
            custo_hp_base = 5;
        }
    }

    if (player->flag_item_contrato == 1 && custo_hp_base > 0) {
        custo_hp_base = 0;
        player->flag_item_contrato = 0; 
    }

    if(player->mana < custo_mana || player->corpos < custo_corpo) {
        char erro[200];
        sprintf(erro, "FALHA: Requer %d Mana e %d Corpos.", custo_mana, custo_corpo);
        atualizar_relatorio(player, erro);
        return;
    }
    
    if(escolha == 8) {
        extra_info = player->corpos;
        player->corpos = 0; 
        player->mana -= custo_mana;
    } 
    else if (escolha == 7) {
        extra_info = player->mana;
        player->mana = 0;
        player->corpos -= custo_corpo;
    }
    else {
        player->mana -= custo_mana;
        player->corpos -= custo_corpo;
    }

    Minion* novo = criar_minion_especifico(player, player->total_minions + 1, escolha, extra_info);
    
    int dano_instabilidade = 0;
    if(estava_instavel && escolha >= 5) {
        dano_instabilidade = (int)(novo->hp * 0.2);
        if (dano_instabilidade < 5) dano_instabilidade = 5;
        
        if(player->artefatos[4] == 3) dano_instabilidade /= 2;
        if(player->flag_item_cristal == 1) {
            dano_instabilidade = 0;
            player->flag_item_cristal = 0; 
        }
    }
    
    int custo_hp_total = custo_hp_base + dano_instabilidade;
    if (player->hp <= custo_hp_total) {
        char erro[200];
        sprintf(erro, "PERIGO MORTAL: Custo %d HP. Voce tem %d HP. Invocacao cancelada.", custo_hp_total, player->hp);
        atualizar_relatorio(player, erro);
        Beep(200, 300);
        return;
    }
    player->hp -= custo_hp_total;

    player->total_minions++;
    novo->proximo = player->exercito_head;
    player->exercito_head = novo;

    //efeito sonoro
    Beep(700, 100); 
    
    char sucesso[200];
    if(dano_instabilidade > 0) {
        sprintf(sucesso, "%s invocado! (Custo: -%d HP [-Instab.] | ATK: %d | HP: %d)", 
                novo->nome, custo_hp_total, novo->ataque, novo->hp);
    } else if(custo_hp_base > 0) {
        sprintf(sucesso, "%s invocado! (Custo: -%d HP | ATK: %d | HP: %d)", 
                novo->nome, custo_hp_base, novo->ataque, novo->hp);
    } else {
        sprintf(sucesso, "%s invocado! (ATK: %d | HP: %d)", 
                novo->nome, novo->ataque, novo->hp);
    }
        
    atualizar_relatorio(player, sucesso);
}


void matar_minion(Necromante* p, Minion* alvo, Minion* anterior) {
    if(anterior == NULL) p->exercito_head = alvo->proximo;
    else anterior->proximo = alvo->proximo;
    free(alvo);
    p->total_minions--;
}

void ativar_passiva_sifao(Necromante* p) {
    // Contagem rigorosa de Sifões
    int qtd_sifoes = 0;
    Minion* m = p->exercito_head;
    while(m != NULL) {
        if(strcmp(m->nome, "Sifao") == 0) {
            qtd_sifoes++;
        }
        m = m->proximo;
    }
    
    if(qtd_sifoes == 0) return; 
    
    int total_curado = 0;
    int total_afetados = 0;
    
    // Loop de alimentação
    for(int i = 0; i < qtd_sifoes; i++) {
        if(p->exercito_head == NULL) break;

        Minion* atual = p->exercito_head;
        Minion* alvo = NULL;
        Minion* prev_alvo = NULL;
        Minion* prev = NULL;
        
        int menor_hp_encontrado = 99999;
        
        // Busca alvo (Prioriza não-Sifão)
        while(atual != NULL) {
            if(strcmp(atual->nome, "Sifao") != 0) {
                if(atual->hp < menor_hp_encontrado) {
                    menor_hp_encontrado = atual->hp;
                    alvo = atual;
                    prev_alvo = prev;
                }
            }
            prev = atual;
            atual = atual->proximo;
        }
        
        // Se só tem Sifões
        if(alvo == NULL) {
            atual = p->exercito_head;
            prev = NULL;
            menor_hp_encontrado = 99999;
            while(atual != NULL) {
                if(atual->hp < menor_hp_encontrado) {
                    menor_hp_encontrado = atual->hp;
                    alvo = atual;
                    prev_alvo = prev;
                }
                prev = atual;
                atual = atual->proximo;
            }
        }
        
        if(alvo != NULL) {
            // --- LÓGICA DA MORDAÇA DE FERRO (Item 2-4) ---
            // Se tiver o item, não mata. Drena até 1 HP.
            if(p->artefatos[2] == 4) {
                if(alvo->hp > 1) { // Só drena se tiver sangue
                    alvo->hp = 1;
                    total_curado += 5;
                    total_afetados++;
                }
                // Não remove da lista!
            } 
            else {
                // Comportamento Padrão: Mata
                if(prev_alvo == NULL) p->exercito_head = alvo->proximo;
                else prev_alvo->proximo = alvo->proximo;
                
                free(alvo);
                p->total_minions--;
                total_curado += 5;
                total_afetados++;
            }
            
            p->hp += 5;
            if(p->hp > p->max_hp) p->hp = p->max_hp;
        }
    }
    
    if(total_curado > 0) {
        char relatorio_extra[200];
        if(p->artefatos[2] == 4)
            sprintf(relatorio_extra, " | SIFOES: %d lacaios drenados (vivos). +%d HP.", total_afetados, total_curado);
        else
            sprintf(relatorio_extra, " | SIFOES: %d lacaios devorados. +%d HP.", total_afetados, total_curado);
            
        strcat(p->relatorio, relatorio_extra);
    }
}

//magia vil
void magia_vil(Necromante* p) {
    char msg[200];
    
    if(p->estagio_marcha == 10 && p->boss_id_atual == 0 && p->artefatos[2] != 3) {
        p->hp -= 5;
    }

    int custo_vida = 5; 
    
    if(p->estagio_marcha == 3 && p->boss_id_atual == 0) custo_vida = 15; 
    
    // --- MAGO DE SANGUE PAGA MENOS ---
    if(p->classe == CLASSE_MAGO_SANGUE) custo_vida = 2; 

    if(p->artefatos[9] == 2) custo_vida = 5; // Decreto Real sobrescreve

    p->hp -= custo_vida; 
    
    if(p->hp <= 0) {
        strcpy(p->relatorio, "O ritual exigiu demais da sua vitalidade...");
        return;
    }
    
    p->mana += 15;
    int limite_atual = obter_limite_mana(p);
    if(p->mana > limite_atual) p->mana = limite_atual;
    
    p->buff_profanacao++;    
    p->escavacoes_turno = 0; 
    
    gerar_novo_inimigo(p);   

    int mortos_podridacao = 0;
    Minion* atual = p->exercito_head;
    Minion* prev = NULL;
    while(atual != NULL) {
        atual->decomposicao--;
        if(atual->decomposicao <= 0) {
            Minion* next = atual->proximo;
            if(prev == NULL) p->exercito_head = next;
            else prev->proximo = next;
            free(atual);
            p->total_minions--;
            atual = next;
            mortos_podridacao++;
        } else {
            prev = atual;
            atual = atual->proximo;
        }
    }

    if(mortos_podridacao > 0) 
        sprintf(msg, "RITUAL (-%d HP): Inimigos reforcados. %d lacaios apodreceram.", custo_vida, mortos_podridacao);
    else 
        sprintf(msg, "RITUAL (-%d HP): Sacrificio aceito. Inimigo renovado.", custo_vida);
    
    strcpy(p->relatorio, msg);
    ativar_passiva_sifao(p);
}

void mostrar_lista(Necromante* p) {
    limparTela();
    
    printf("\n\n\n");
    print_centralizado("======================================================================", WHT);
    print_centralizado("RELATORIO COMPLETO DO EXERCITO", YEL);
    print_centralizado("======================================================================", WHT);
    printf("\n");

    char header[200];
    sprintf(header, "| %-6s | %-18s | %-14s | %-6s | %-6s |", "ID", "NOME", "CLASSE", "ATK", "HP");
    print_centralizado(header, WHT);
    
    print_centralizado("|--------|--------------------|----------------|--------|--------|", WHT);

    Minion* m = p->exercito_head;
    
    if(!m) {
        printf("\n");
        print_centralizado("(Nenhum lacaio invocado no momento)", RED);
        printf("\n");
    }
    
    int count = 0;
    while(m) {
        char row[200];
        char* cor_linha = WHT; // Valor padrão de segurança
        
        // Se for lacaio de evento (11 a 20) -> Amarelo
        if (m->codigo_origem >= 11 && m->codigo_origem <= 20) {
            cor_linha = YEL;
        } 
        else {
            // Intercala cores para comuns
            if (count % 3 == 0) cor_linha = GRN;
            else if (count % 3 == 1) cor_linha = CYN;
            else cor_linha = GRN2; 
        }

        // Monta a linha
        sprintf(row, "| #%-5d | %-18s | %-14s | %-6d | %-6d |", 
            m->id, m->nome, m->tipo, m->ataque, m->hp);
            
        print_centralizado(row, cor_linha);
        
        m = m->proximo;
        count++;
    }
    
    print_centralizado("======================================================================", WHT);
    
    printf("\n");
    char total_info[100];
    sprintf(total_info, "TOTAL DE UNIDADES: %d", count);
    print_centralizado(total_info, WHT);
    
    printf("\n\n");
    print_centralizado("[Pressione qualquer tecla para voltar ao comando...]", YEL);
    
    _getch();
    limparTela(); 
}

void escavar(Necromante* p) {
    char msg[200];
    if(p->artefatos[1] == 4) {
        p->hp += 5;
        if(p->hp > p->max_hp) p->hp = p->max_hp;
    }
    if(p->estagio_marcha == 10 && p->boss_id_atual == 0) {
        if(p->artefatos[2] != 3) p->hp -= 5;
    }

    // --- COVEIRO TEM LIMITE 7 ---
    int limite_escavar = (p->classe == CLASSE_COVEIRO) ? 7 : 5;

    if(p->escavacoes_turno >= limite_escavar) {
        sprintf(msg, "O solo esta esgotado (%d/%d). Avance ou use Magia Vil.", p->escavacoes_turno, limite_escavar);
        atualizar_relatorio(p, msg);
        return;
    }

    int qtd = (rand()%3)+1;
    
    if((p->estagio_marcha == 2 && p->boss_id_atual == 0) || p->artefatos[1] == 2) qtd++;

    // --- COVEIRO GANHA +1 CORPO ---
    if(p->classe == CLASSE_COVEIRO) qtd++;

    p->corpos += qtd;
    
    int chance_mana = (p->artefatos[8] == 1) ? 50 : 30;
    int limite_atual = obter_limite_mana(p);

    if(rand()%100 < chance_mana) {
        p->mana += 20; 
        if(p->mana > limite_atual) p->mana = limite_atual;
        sprintf(msg, "Achou %d corpos e Essencia Negra (+20 Mana)!", qtd);
    } else {
        sprintf(msg, "Profanacao concluida. %d corpos coletados.", qtd);
    }
    
    p->escavacoes_turno++; 
    atualizar_relatorio(p, msg);
}

//gerar evento
void gerar_evento_aleatorio(Necromante* p) {
    // CHECAGEM DE CHEAT (Prioridade Máxima)
    int forcar_evento = 0;
    
    if (p->cheat_evento_ativo == 1) {
        forcar_evento = 1;
        p->cheat_evento_ativo = 0;     
    }

    // LÓGICA DE AGENDAMENTO (3 Eventos por Local)
    if (!forcar_evento) {
        if (p->estagio_marcha > 10) return; // Fim de jogo não tem evento
        
        // O evento ocorre APENAS se o progresso atual for 3, 6 ou 9
        if (p->progresso_local != 3 && p->progresso_local != 6 && p->progresso_local != 9) {
            return; 
        }
    }

    // SORTEIO ÚNICO (Garante não repetir eventos já vistos)
    int id_evento;
    int tentativas = 0;
    int eventos_disponiveis = 0;

    for(int i=1; i<=30; i++) {
        if(p->eventos_realizados[i] == 0) eventos_disponiveis++;
    }

    if(eventos_disponiveis == 0) return; 

    do {
        id_evento = (rand() % 30) + 1;
        tentativas++;
    } while (p->eventos_realizados[id_evento] == 1 && tentativas < 500);

    // Fallback de segurança
    if (p->eventos_realizados[id_evento] == 1) {
        for(int i=1; i<=30; i++) {
            if(p->eventos_realizados[i] == 0) {
                id_evento = i;
                break;
            }
        }
    }

    // Marca como realizado
    p->eventos_realizados[id_evento] = 1;
    
    limparTela();
    irParaInicio();
    
    char* cor_borda = "\x1b[95m"; // Magenta
    char* cor_titulo = "\x1b[93m"; // Amarelo
    
    printf("\n\n");
    print_centralizado("==================================================================================", cor_borda);
    print_centralizado("ENCONTRO INESPERADO NA ESTRADA", cor_titulo);
    print_centralizado("==================================================================================", cor_borda);
    printf("\n");

    char titulo[100], desc1[200], desc2[200];
    char op1[100], op2[100], op3[100];
    
    switch(id_evento) {
        case 1: 
            print_centralizado("      _.-._", WHT); print_centralizado("    _| | | |_", WHT); print_centralizado("   ( | | | | )", WHT); print_centralizado("   X_|_|_|_|_X", RED);
            strcpy(titulo, "O CAMPO DE BATALHA"); strcpy(desc1, "Restos de uma escaramuca sangrenta."); strcpy(desc2, "Um cavaleiro de elite morto na lama.");
            strcpy(op1, "[1] Revirar corpos"); strcpy(op2, "[2] Reanimar Cavaleiro "); strcpy(op3, "[3] Acalmar espirito"); break;
        case 2:
            print_centralizado("      /\\", YEL); print_centralizado("     /  \\", YEL); print_centralizado("    |  + |", YEL); print_centralizado("    |____|", YEL);
            strcpy(titulo, "O TEMPLO PROFANADO"); strcpy(desc1, "Um santuario da luz vandalizado."); strcpy(desc2, "Um Paladino com uma adaga no peito.");
            strcpy(op1, "[1] Roubar calices"); strcpy(op2, "[2] Puxar a adaga"); strcpy(op3, "[3] Queimar templo"); break;
        case 3:
            print_centralizado("      ( )", CYN); print_centralizado("     /   \\", CYN); print_centralizado("    | XXX |", CYN); print_centralizado("     \\___/", CYN);
            strcpy(titulo, "LABORATORIO ALQUIMICO"); strcpy(desc1, "Frascos borbulham."); strcpy(desc2, "Uma massa de carne inerte espera.");
            strcpy(op1, "[1] Beber frasco"); strcpy(op2, "[2] Injetar energia"); strcpy(op3, "[3] Ler diario"); break;
        case 4:
            print_centralizado("    [|||||]", WHT); print_centralizado("    | @ @ |", RED); print_centralizado("    |__V__|", WHT);
            strcpy(titulo, "A JAULA DA BESTA"); strcpy(desc1, "Uma caravana de circo destruida."); strcpy(desc2, "Uma besta acorrentada na jaula.");
            strcpy(op1, "[1] Fundir correntes"); strcpy(op2, "[2] Reanimar besta"); strcpy(op3, "[3] Cutucar"); break;
        case 5:
            print_centralizado("      | |", WHT); print_centralizado("     [| |]", WHT); print_centralizado("     /___\\", WHT);
            strcpy(titulo, "A TORRE DE VIGIA"); strcpy(desc1, "Um esqueleto vigia o horizonte."); strcpy(desc2, "Segura um arco, esperando ordens.");
            strcpy(op1, "[1] Derrubar torre"); strcpy(op2, "[2] Infundir energia"); strcpy(op3, "[3] Subir"); break;
        case 6:
            print_centralizado("     ( * )", MAG); print_centralizado("    /  |  \\", MAG);
            strcpy(titulo, "O CULTO DO FIM"); strcpy(desc1, "Cultistas mortos em circulo."); strcpy(desc2, "O lider segura um cajado negro.");
            strcpy(op1, "[1] Absorver energia"); strcpy(op2, "[2] Levantar lider"); strcpy(op3, "[3] Vestir manto"); break;
        case 7:
            print_centralizado("    ~ ~ ~ ~", GRN); print_centralizado("   ~ (X_X) ~", GRN);
            strcpy(titulo, "O PANTANO DOENTE"); strcpy(desc1, "Nevoa verde e agua podre."); strcpy(desc2, "Um espirito da praga flutua.");
            strcpy(op1, "[1] Coletar agua"); strcpy(op2, "[2] Convocar espirito"); strcpy(op3, "[3] Respirar fundo"); break;
        case 8:
            print_centralizado("    /     \\", WHT); print_centralizado("   | X X X |", WHT); print_centralizado("    \\_____/", WHT);
            strcpy(titulo, "A FOSSA COMUM"); strcpy(desc1, "Centenas de ossos misturados."); strcpy(desc2, "A energia de morte e palpavel.");
            strcpy(op1, "[1] Pegar o que der"); strcpy(op2, "[2] Fundir os ossos"); strcpy(op3, "[3] Pular dentro"); break;
        case 9: // PEGADINHA
            print_centralizado("     \\_|_/", RED); print_centralizado("      | |", RED); print_centralizado("     /___\n", RED);
            strcpy(titulo, "O BANQUETE DO NOBRE"); strcpy(desc1, "Um jantar com assassinato."); strcpy(desc2, "Vinho e sangue na mesa.");
            strcpy(op1, "[1] Beber vinho"); strcpy(op2, "[2] Roubar prataria"); strcpy(op3, "[3] Comer restos"); break;
        case 10:
            print_centralizado("      ___", WHT); print_centralizado("     /   \\", WHT); print_centralizado("     |___|", WHT);
            strcpy(titulo, "A PRACA DA EXECUCAO"); strcpy(desc1, "Um carrasco morto no cepo."); strcpy(desc2, "Seu machado ainda esta afiado.");
            strcpy(op1, "[1] Pegar machado"); strcpy(op2, "[2] Reanimar executor"); strcpy(op3, "[3] Deitar no cepo"); break;
        case 11: 
            print_centralizado("      .|.", WHT); print_centralizado("    --+--", WHT); print_centralizado("      |  ", WHT);
            strcpy(titulo, "ARVORE DOS ENFORCADOS"); strcpy(desc1, "Varios corpos balancam na corda."); strcpy(desc2, "Um deles ainda segura uma espada.");
            strcpy(op1, "[1] Cortar as cordas"); strcpy(op2, "[2] Reanimar o soldado"); strcpy(op3, "[3] Tentar salvar um"); break;
        case 12: 
            print_centralizado("     [///]", CYN); print_centralizado("     |///|", CYN); print_centralizado("     |___|", CYN);
            strcpy(titulo, "O ESPELHO NEGRO"); strcpy(desc1, "Um espelho que nao reflete luz."); strcpy(desc2, "Sua propria sombra parece querer sair.");
            strcpy(op1, "[1] Quebrar o vidro"); strcpy(op2, "[2] Puxar sua sombra"); strcpy(op3, "[3] Olhar fixamente"); break;
        case 13: 
            print_centralizado("    /___\\", MAG); print_centralizado("   ( @ @ )", MAG); print_centralizado("    \\ - /", MAG);
            strcpy(titulo, "A BIBLIOTECA PROIBIDA"); strcpy(desc1, "Livros de pele humana."); strcpy(desc2, "Um esqueleto de manto le um tomo.");
            strcpy(op1, "[1] Roubar os livros"); strcpy(op2, "[2] Escravizar o mago"); strcpy(op3, "[3] Ler em voz alta"); break;
        case 14: 
            print_centralizado("     /\\", RED); print_centralizado("    /  \\", RED); print_centralizado("   /____\\", RED);
            strcpy(titulo, "ACAMPAMENTO MERCENARIO"); strcpy(desc1, "Mercenarios mortos por uma doenca."); strcpy(desc2, "Um deles era um berserker famoso.");
            strcpy(op1, "[1] Saquear ouro"); strcpy(op2, "[2] Reanimar o berserker"); strcpy(op3, "[3] Dormir na tenda"); break;
        case 15: 
            print_centralizado("   #######", GRN); print_centralizado("   # x x #", GRN); print_centralizado("   #######", GRN);
            strcpy(titulo, "A VALA COLETIVA"); strcpy(desc1, "O cheiro e insuportavel."); strcpy(desc2, "Um corpo gigante se destaca.");
            strcpy(op1, "[1] Pegar partes"); strcpy(op2, "[2] Reanimar o gigante"); strcpy(op3, "[3] Vomitar"); break;
        case 16: 
            print_centralizado("    (O_O)", RED); print_centralizado("   /|___|\\", RED); print_centralizado("    |   |", RED);
            strcpy(titulo, "O COLOSSO CAIDO"); strcpy(desc1, "Um monstro feito de varios corpos."); strcpy(desc2, "A costura se soltou, mas pode ser refeita.");
            strcpy(op1, "[1] Desmontar"); strcpy(op2, "[2] Recosturar e ligar"); strcpy(op3, "[3] Chutar o corpo"); break;
        case 17: 
            print_centralizado("   .. .. ..", WHT); print_centralizado("  :  :  :  :", WHT); print_centralizado("   '' '' ''", WHT);
            strcpy(titulo, "ENXAME DE RATOS"); strcpy(desc1, "Milhares de ratos comendo um cavalo."); strcpy(desc2, "Eles agem como uma mente coletiva.");
            strcpy(op1, "[1] Espantar"); strcpy(op2, "[2] Controlar a mente"); strcpy(op3, "[3] Tentar pegar um"); break;
        case 18: 
            print_centralizado("      /\\", CYN); print_centralizado("     <  >", CYN); print_centralizado("      \\/", CYN);
            strcpy(titulo, "CRISTAL DE MANA PURO"); strcpy(desc1, "Um cristal flutuando, pulsando poder."); strcpy(desc2, "Ele parece ter vontade propria.");
            strcpy(op1, "[1] Quebrar e absorver"); strcpy(op2, "[2] Dar vida ao cristal"); strcpy(op3, "[3] Tocar com a mao nua"); break;
        case 19: 
            print_centralizado("    \\|||/", RED); print_centralizado("    (o o)", RED); print_centralizado("     (O)", RED);
            strcpy(titulo, "O BANQUETE CANIBAL"); strcpy(desc1, "Uma criatura gorda morreu comendo."); strcpy(desc2, "Sua barriga ainda se mexe.");
            strcpy(op1, "[1] Abrir a barriga"); strcpy(op2, "[2] Reanimar o glutao"); strcpy(op3, "[3] Comer tambem"); break;
        case 20: 
            print_centralizado("    ( @ )", MAG); print_centralizado("     ) ( ", MAG); print_centralizado("    ( @ )", MAG);
            strcpy(titulo, "RITUAL DE SANGUE"); strcpy(desc1, "Sinais de um ritual vampirico."); strcpy(desc2, "Um orbe de sangue flutua no centro.");
            strcpy(op1, "[1] Beber o sangue"); strcpy(op2, "[2] Materializar o orbe"); strcpy(op3, "[3] Chutar o orbe"); break;
        case 21: // PEGADINHA 2
            print_centralizado("    [x_x]", WHT);
            strcpy(titulo, "A CAVEIRA DOURADA"); strcpy(desc1, "Um esqueleto com dentes de ouro."); strcpy(desc2, "Ele parece valioso.");
            strcpy(op1, "[1] Pegar o ouro"); strcpy(op2, "[2] Reanimar o esqueleto"); strcpy(op3, "[3] Esmagar o cranio"); break;
        case 22:
            print_centralizado("   |RIP|", WHT);
            strcpy(titulo, "A TUMBA DO HEROI"); strcpy(desc1, "Aqui jaz um heroi esquecido."); strcpy(desc2, "Sua armadura esta intacta.");
            strcpy(op1, "[1] Roubar a armadura"); strcpy(op2, "[2] Profanar o heroi"); strcpy(op3, "[3] Ler a inscricao"); break;
        case 23:
            print_centralizado("   % # %", RED);
            strcpy(titulo, "A CARNIFICINA"); strcpy(desc1, "Lobos despedacaram estes soldados."); strcpy(desc2, "Sobrou pouca coisa inteira.");
            strcpy(op1, "[1] Juntar pedacos"); strcpy(op2, "[2] Criar um monstro"); strcpy(op3, "[3] Cacar os lobos"); break;
        case 24:
            print_centralizado("   [ O ]", YEL);
            strcpy(titulo, "O GIGANTE MORTO"); strcpy(desc1, "Um ogro morto com uma lanca."); strcpy(desc2, "Um alvo perfeito para necromancia.");
            strcpy(op1, "[1] Cortar carne"); strcpy(op2, "[2] Reanimar ogro"); strcpy(op3, "[3] Roubar a lanca"); break;
        case 25:
            print_centralizado("    . .", CYN); print_centralizado("    ~ ~", CYN);
            strcpy(titulo, "O ESPIRITO INQUIETO"); strcpy(desc1, "Um fantasma chora por vinganca."); strcpy(desc2, "Ele busca um corpo para habitar.");
            strcpy(op1, "[1] Exorcizar"); strcpy(op2, "[2] Oferecer servico"); strcpy(op3, "[3] Conversar"); break;
        case 26:
            print_centralizado("   i i i", WHT);
            strcpy(titulo, "COVA RASA"); strcpy(desc1, "Dezenas de soldados enterrados as pressas."); strcpy(desc2, "Seus espiritos estao ligados.");
            strcpy(op1, "[1] Exumar todos"); strcpy(op2, "[2] Unir as almas"); strcpy(op3, "[3] Pisar em cima"); break;
        case 27:
            print_centralizado("    (@)", MAG);
            strcpy(titulo, "ORBE DE ENERGIA"); strcpy(desc1, "Magia pura condensada."); strcpy(desc2, "Vibra com poder instavel.");
            strcpy(op1, "[1] Drenar"); strcpy(op2, "[2] Dar forma"); strcpy(op3, "[3] Quebrar"); break;
        case 28:
            print_centralizado("   #####", GRN);
            strcpy(titulo, "O LIXAO DA CIDADE"); strcpy(desc1, "Restos de comida e cadaveres."); strcpy(desc2, "Algo vive no meio do lixo.");
            strcpy(op1, "[1] Vasculhar"); strcpy(op2, "[2] Atrair a coisa"); strcpy(op3, "[3] Atear fogo"); break;
        case 29:
            print_centralizado("    /|\\", MAG);
            strcpy(titulo, "O MAGO EXILADO"); strcpy(desc1, "Um mago morreu tentando um ritual."); strcpy(desc2, "Seu esqueleto ainda brilha.");
            strcpy(op1, "[1] Roubar grimorio"); strcpy(op2, "[2] Completar o ritual"); strcpy(op3, "[3] Rir dele"); break;
        case 30:
            print_centralizado("    {o}", RED);
            strcpy(titulo, "O PARASITA"); strcpy(desc1, "Um inseto gigante grudado em um corpo."); strcpy(desc2, "Ele suga sangue e vida.");
            strcpy(op1, "[1] Esmagar"); strcpy(op2, "[2] Domar o inseto"); strcpy(op3, "[3] Deixar te picar"); break;
    }

    printf("\n");
    print_centralizado(titulo, YEL);
    printf("\n");
    print_centralizado(desc1, WHT);
    print_centralizado(desc2, WHT);
    printf("\n");

    print_centralizado(op1, CYN);
    print_centralizado(op2, CYN);
    print_centralizado(op3, CYN);

    printf("\n");
    print_centralizado("Faca sua escolha [1-3]:", YEL);
    
    char ch = _getch();
    int esc = ch - '0';
    
    char resultado[200];
    int ganhou_lacaio = 0;
    int lacaio_id = 0;
    
    // --- PEGADINHA 1 (EVENTO 9) ---
    if (id_evento == 9) {
        if (esc == 1) { p->hp -= 20; sprintf(resultado, "O vinho era veneno puro! -20 HP."); }
        else if (esc == 2) { p->mana += 25; sprintf(resultado, "Voce derreteu a prata. +25 Mana."); }
        else if (esc == 3) { ganhou_lacaio = 1; lacaio_id = LAC_VAMPIRO; sprintf(resultado, "O sangue na carne te chamou. VAMPIRO NOBRE reanimado!"); }
    }
    // --- PEGADINHA 2 (EVENTO 21) ---
    else if (id_evento == 21) {
        if (esc == 1) { 
             int slot=-1; for(int k=0;k<3;k++) if(p->inventario[k]==0) slot=k;
             if(slot!=-1) { p->inventario[slot]= (rand()%3)+1; sprintf(resultado, "Era ouro real! Achou um item."); }
             else sprintf(resultado, "Achou ouro, mas inventario cheio.");
        }
        else if (esc == 2) { ganhou_lacaio = 1; lacaio_id = 1; sprintf(resultado, "Esqueleto reanimado."); } 
        else if (esc == 3) { p->hp -= 15; sprintf(resultado, "O cranio explodiu. -15 HP."); }
    } 
    // --- EVENTOS NORMAIS ---
    else {
        if (esc == 1) { // RECURSOS
            int r_mana = 0; int r_corpo = 0; int r_item = 0; int r_hp = 0;
            
            if(id_evento==1 || id_evento==5 || id_evento==8 || id_evento==11 || id_evento==15 || id_evento==16 || id_evento==17 || id_evento==23 || id_evento==24 || id_evento==26 || id_evento==30) r_corpo = (rand()%3)+3;
            else if(id_evento==2 || id_evento==6 || id_evento==12 || id_evento==14 || id_evento==18 || id_evento==27 || id_evento==29) r_mana = (rand()%20)+20;
            else if(id_evento==4 || id_evento==7 || id_evento==10 || id_evento==13 || id_evento==22 || id_evento==25 || id_evento==28) r_item = 1;
            else if(id_evento==3 || id_evento==20) r_hp = 1;

            if(r_corpo > 0) { p->corpos += r_corpo; sprintf(resultado, "Saqueou +%d Corpos.", r_corpo); }
            if(r_mana > 0) { p->mana += r_mana; sprintf(resultado, "Absorveu +%d Mana.", r_mana); }
            if(r_hp > 0) { p->hp = p->max_hp; sprintf(resultado, "Vida totalmente restaurada!"); }
            if(r_item > 0) {
                 int slot=-1; for(int k=0;k<3;k++) if(p->inventario[k]==0) slot=k;
                 if(slot!=-1) { int it = (rand()%10)+1; p->inventario[slot]=it; sprintf(resultado, "Achou item: %s!", get_nome_item(it)); } 
                 else sprintf(resultado, "Achou item, mas inventario cheio.");
            }
        }
        else if (esc == 2) { // LACAIO ESPECIAL
            ganhou_lacaio = 1;
            // Mapa 1-10
            if(id_evento==1) lacaio_id=LAC_CAVALEIRO;
            if(id_evento==2) lacaio_id=LAC_PALADINO;
            if(id_evento==3) lacaio_id=LAC_HOMUNCULO;
            if(id_evento==4) lacaio_id=LAC_BESTA;
            if(id_evento==5) lacaio_id=LAC_ARQUEIRO;
            if(id_evento==6) lacaio_id=LAC_SACERDOTE;
            if(id_evento==7) lacaio_id=LAC_ESPECTRO;
            if(id_evento==8) lacaio_id=LAC_GOLEM_OSSO;
            if(id_evento==10) lacaio_id=LAC_VERDUGO;
            
            // Mapa 11-20
            if(id_evento==11) lacaio_id=1; 
            if(id_evento==12) lacaio_id=5; 
            if(id_evento==13) lacaio_id=9; 
            if(id_evento==14) lacaio_id=3; 
            if(id_evento==15) lacaio_id=2; 
            if(id_evento==16) lacaio_id=4; 
            if(id_evento==17) lacaio_id=6; 
            if(id_evento==18) lacaio_id=7; 
            if(id_evento==19) lacaio_id=8; 
            if(id_evento==20) lacaio_id=10; 
            
            // Mapa 22-30
            if(id_evento==22) lacaio_id=2;
            if(id_evento==23) lacaio_id=3;
            if(id_evento==24) lacaio_id=4;
            if(id_evento==25) lacaio_id=5;
            if(id_evento==26) lacaio_id=6;
            if(id_evento==27) lacaio_id=7;
            if(id_evento==28) lacaio_id=8;
            if(id_evento==29) lacaio_id=9;
            if(id_evento==30) lacaio_id=10;
            
            sprintf(resultado, "SUCESSO! Voce obteve um novo lacaio.");
        }
        else if (esc == 3) { // RUIM
            int dano = (p->turno_global * 2) + 5;
            p->hp -= dano;
            sprintf(resultado, "FRACASSO! Voce se feriu na tentativa. -%d HP.", dano);
        }
    }

    if (ganhou_lacaio) {
        int extra = 0;
        // Simulação de status para Sombra
        if (lacaio_id == 5) { p->hp_inimigo_atual = (p->turno_global * 50) + 50; p->atk_inimigo_atual = (p->turno_global * 12) + 8; }
        // Recursos para outros
        if (lacaio_id == 7) extra = p->mana;
        if (lacaio_id == 8) extra = 5; 

        Minion* novo = criar_minion_especifico(p, p->total_minions+1, lacaio_id, extra);

        if(lacaio_id >= 11 && lacaio_id <= 20) {
            p->lacaios_descobertos[lacaio_id] = 1;
        }

        novo->proximo = p->exercito_head;
        p->exercito_head = novo;
        p->total_minions++;
        strcat(resultado, " (Adicionado ao exercito)");
        
        // Reset simulacao Sombra
        if (lacaio_id == 5) { p->hp_inimigo_atual = 0; p->atk_inimigo_atual = 0; }
    }
    
    printf("\n\n");
    print_centralizado(resultado, (ganhou_lacaio ? GRN : (esc==3 && id_evento!=9 ? RED : YEL)));
    
    printf("\n");
    print_centralizado("[Pressione qualquer tecla para marchar...]", WHT);
    _getch();
    limparTela();
}

void dissipar_sombras_extras(Necromante* p) {
    Minion* atual = p->exercito_head;
    Minion* prev = NULL;
    int sombras_encontradas = 0;
    int sombras_removidas = 0;

    while (atual != NULL) {
        // Verifica se é Sombra
        if (strcmp(atual->nome, "Sombra") == 0) {
            sombras_encontradas++;
            
            // Se já encontrou uma antes, esta é extra e deve ser removida
            if (sombras_encontradas > 1) {
                Minion* para_remover = atual;
                
                // Lógica de remoção de Lista Encadeada
                if (prev == NULL) {
                    p->exercito_head = atual->proximo;
                    atual = p->exercito_head; // Avança o ponteiro atual
                } else {
                    prev->proximo = atual->proximo;
                    atual = prev->proximo; // Avança o ponteiro atual
                }
                
                free(para_remover);
                p->total_minions--;
                sombras_removidas++;
                
                // Continue o loop sem avançar o 'prev', pois o 'atual' já mudou
                continue; 
            }
        }
        // Avanço normal
        prev = atual;
        atual = atual->proximo;
    }
    
    // Adiciona ao relatório se houve dissipação
    if (sombras_removidas > 0) {
        char msg_extra[100];
        sprintf(msg_extra, " | SOMBRAS: %d copias instaveis se dissiparam.", sombras_removidas);
        strcat(p->relatorio, msg_extra);
    }
}

//batalhar
void batalhar(Necromante* p) {
    char msg[200];
    
    if(!p->exercito_head) {
        atualizar_relatorio(p, "Voce nao tem lacaios para a linha de frente!");
        return;
    }

    if(p->hp_inimigo_atual <= 0) gerar_novo_inimigo(p);

    int modo_cheat = (p->hp_inimigo_atual == 1 && p->atk_inimigo_atual == 0);
    int botas_protegem = (p->artefatos[2] == 3);
    
    // Local 4: Torres 
    if(!modo_cheat && p->estagio_marcha == 4 && p->boss_id_atual == 0 && !botas_protegem) {
        int danoFlechas = 20; 
        if(p->exercito_head) {
            p->exercito_head->hp -= danoFlechas;
            if(p->exercito_head->hp <= 0) {
                p->ultimo_tipo_morto = p->exercito_head->codigo_origem;
                Minion* morto = p->exercito_head;
                p->exercito_head = morto->proximo;
                free(morto);
                p->total_minions--;
            }
        } else {
            p->hp -= danoFlechas;
        }
    }

    int danoEfetivoPlayer = 0;
    int danoMagicoTotal = 0;
    int danoFisicoTotal = 0;
    int debuff_inimigo_acumulado = 0; 
    Minion* m = p->exercito_head;
    
    while(m) { 
        int danoDeste = m->ataque;
        
        // Boss 1: Aldric 
        if(p->boss_id_atual == 1) {
            if(strcmp(m->nome, "Esqueleto")==0 || strcmp(m->nome, "Zumbi")==0 || 
               strcmp(m->nome, "Carnical")==0  || strcmp(m->nome, "ABOMINACAO")==0) {
                danoDeste = (int)(danoDeste * 0.5); 
            }
        }
        // Boss 10: Golem 
        if(!modo_cheat && p->boss_id_atual == 10) {
            if(danoDeste < 150) danoDeste = 0; 
        }
        if(p->artefatos[10] == 2 && p->boss_id_atual > 0) danoDeste += 20;

        if (m->codigo_origem == LAC_SACERDOTE) danoMagicoTotal += (p->total_minions * 2);
        if (m->codigo_origem == LAC_ESPECTRO) debuff_inimigo_acumulado += 5;
        if (m->codigo_origem == LAC_VAMPIRO) { p->hp += 2; if(p->hp > p->max_hp) p->hp = p->max_hp; }
        if (m->codigo_origem == LAC_VERDUGO) { if (p->hp_inimigo_atual < (p->hp_inimigo_max * 0.2)) danoDeste += 999; }

        int tipo = m->codigo_origem;
        if (tipo == 5 || tipo == 7 || tipo == 9 || tipo == 10 || tipo == 13 || tipo == 16 || tipo == 17) {
            danoMagicoTotal += danoDeste;
        } else {
            danoFisicoTotal += danoDeste;
        }
        m = m->proximo; 
    }
    danoEfetivoPlayer = danoFisicoTotal + danoMagicoTotal;

    // --- CÁLCULO INIMIGO ---
    int atkInimigoTotal = 0;
    for(int i=0; i < p->qtd_inimigos_ativos; i++) {
        if(p->esquadrao[i].vivo) atkInimigoTotal += p->esquadrao[i].atk;
    }
    
    // Boss 4: Vane 
    if(!modo_cheat && p->boss_id_atual == 4) atkInimigoTotal += (p->total_minions * 10); // ERA 5
    
    atkInimigoTotal -= debuff_inimigo_acumulado;
    if(atkInimigoTotal < 0) atkInimigoTotal = 0;
    p->atk_inimigo_atual = atkInimigoTotal;

    // --- RESOLUÇÃO ---
    int barreira_item_ativa = (p->flag_item_barreira == 1);
    if(barreira_item_ativa) p->flag_item_barreira = 0;
    int escudo_luz_ativo = (p->artefatos[5] == 2 && p->hp_inimigo_atual == p->hp_inimigo_max);
    
    int inimigo_errou = 0;
    if(p->estagio_marcha == 8 && p->boss_id_atual == 0 && !botas_protegem) {
        if((rand() % 100) < 20) inimigo_errou = 1; 
    }

    int danoPendente = atkInimigoTotal;
    if(barreira_item_ativa || escudo_luz_ativo || inimigo_errou) {
        danoPendente = 0;
        if(escudo_luz_ativo) sprintf(msg, "O Escudo da Luz bloqueou o ataque inicial!");
        if(inimigo_errou) sprintf(msg, "O inimigo se perdeu no Labirinto e errou!");
        if(!barreira_item_ativa && danoPendente==0) atualizar_relatorio(p, msg);
    }

    int lacaiosMortos = 0;

    while(danoPendente > 0 && p->exercito_head != NULL) {
        Minion* defensor = p->exercito_head;
        int reducao = 0;
        if (p->evolucoes_ativas[2] == 1 && strcmp(defensor->nome, "Zumbi") == 0) reducao = 5;
        if (defensor->codigo_origem == LAC_PALADINO) reducao = 5;
        
        int dano_aplicado = danoPendente - reducao;
        if(dano_aplicado < 0) dano_aplicado = 0;

        int hp_efetivo = defensor->hp;
        if(p->boss_id_atual == 3) { hp_efetivo = defensor->hp / 2; if(hp_efetivo < 1) hp_efetivo = 1; }
        
        if(hp_efetivo <= dano_aplicado) {
            if(p->boss_id_atual == 3) danoPendente -= (hp_efetivo + reducao);
            else danoPendente -= (defensor->hp + reducao);
            
            if(p->boss_id_atual == 8) { 
                int idx_boss = p->qtd_inimigos_ativos - 1;
                if(p->esquadrao[idx_boss].vivo) p->esquadrao[idx_boss].hp += 20; 
            }
            if(p->artefatos[2] == 1) p->hp_inimigo_atual -= 5;
            if (defensor->codigo_origem == LAC_HOMUNCULO) {
                 for(int i=0; i<p->qtd_inimigos_ativos; i++) {
                    if(p->esquadrao[i].vivo) { p->esquadrao[i].hp -= 50; if(p->esquadrao[i].hp <= 0) p->esquadrao[i].vivo = 0; break; }
                }
            }
            if (p->evolucoes_ativas[1] == 1 && strcmp(defensor->nome, "Esqueleto") == 0 && (rand() % 2 == 0)) p->corpos++;
            if (defensor->codigo_origem == LAC_GOLEM_OSSO) p->corpos++;

            p->ultimo_tipo_morto = defensor->codigo_origem;
            p->exercito_head = defensor->proximo;
            free(defensor);
            lacaiosMortos++;
            p->total_minions--;
        } else {
            if(p->artefatos[4] == 2 && strcmp(defensor->nome, "ABOMINACAO") == 0) p->hp_inimigo_atual -= 10;
            if(p->boss_id_atual == 3) defensor->hp -= (dano_aplicado * 2);
            else defensor->hp -= dano_aplicado;
            danoPendente = 0;
        }
    }

    // Boss 2: Miller 
    if(!modo_cheat && p->boss_id_atual == 2 && lacaiosMortos > 0) p->hp -= (lacaiosMortos * 10); 
    
    // Boss 7: Jinx 
    if(!modo_cheat && p->boss_id_atual == 7 && p->artefatos[7] != 2) { 
        Minion* t = p->exercito_head;
        while(t) { t->hp -= 15; if(t->hp < 1) t->hp = 1; t = t->proximo; }
    }
    if(danoPendente > 0) p->hp -= danoPendente;

    // --- JOGADOR ATACA ESQUADRÃO ---
    int fisicoPend = danoFisicoTotal;
    int magicoPend = danoMagicoTotal;
    if(p->boss_id_atual == 1) fisicoPend = (int)(fisicoPend * 0.5); 

    for(int i=0; i < p->qtd_inimigos_ativos; i++) {
        if(fisicoPend <= 0 && magicoPend <= 0) break;
        if(p->esquadrao[i].vivo) {
            int tipo_inimigo = p->esquadrao[i].id_tipo;
            int danoNestaUnidade = 0;
            int gastouFisico = 0; int gastouMagico = 0;

            if (tipo_inimigo != INI_FANTASMA) { danoNestaUnidade += fisicoPend; gastouFisico = fisicoPend; }
            if (tipo_inimigo != INI_CACADOR) { danoNestaUnidade += magicoPend; gastouMagico = magicoPend; }
            if (i > 0 && p->evolucoes_ativas[4] == 1) danoNestaUnidade += 30; 

            if (danoNestaUnidade > 0) {
                if(p->esquadrao[i].hp <= danoNestaUnidade) {
                    int vida_inimigo = p->esquadrao[i].hp;
                    p->esquadrao[i].hp = 0; p->esquadrao[i].vivo = 0;
                    int dano_para_abater = vida_inimigo;
                    if (gastouFisico > 0) {
                        if (fisicoPend >= dano_para_abater) { fisicoPend -= dano_para_abater; dano_para_abater = 0; }
                        else { dano_para_abater -= fisicoPend; fisicoPend = 0; }
                    }
                    if (gastouMagico > 0 && dano_para_abater > 0) { magicoPend -= dano_para_abater; if(magicoPend < 0) magicoPend = 0; }
                } else {
                    p->esquadrao[i].hp -= danoNestaUnidade;
                    if (gastouFisico > 0) fisicoPend = 0;
                    if (gastouMagico > 0) magicoPend = 0;
                }
            }
        }
    }
    
    p->hp_inimigo_atual = 0;
    int inimigos_vivos_count = 0;
    for(int i=0; i < p->qtd_inimigos_ativos; i++) {
        if(p->esquadrao[i].vivo) { p->hp_inimigo_atual += p->esquadrao[i].hp; inimigos_vivos_count++; }
    }

    // Boss 9: Tybalt 
    if(!modo_cheat && p->boss_id_atual == 9 && p->hp_inimigo_atual > 0) p->atk_inimigo_atual += 20; 

    // --- VITÓRIA ---
    if(p->hp_inimigo_atual <= 0) {
        // Boss 5: Kael 
        if(!modo_cheat && p->boss_id_atual == 5 && p->boss_fase_2 == 0) {
            p->boss_fase_2 = 1; 
            int idx = p->qtd_inimigos_ativos - 1;
            p->esquadrao[idx].vivo = 1;
            p->esquadrao[idx].hp = p->esquadrao[idx].max_hp; // 100%
            p->hp_inimigo_atual = p->esquadrao[idx].hp;
            sprintf(msg, "CRUZADO KAEL: 'A Luz me restaura TOTALMENTE!' (Voltou com 100%% HP).");
            atualizar_relatorio(p, msg);
            return; 
        }
        
        p->mana += 30; p->corpos += 2;
        if(p->estagio_marcha == 7 && p->boss_id_atual == 0) p->mana += 10;
        if(p->artefatos[3] == 2) p->mana += 5;
        int limite_mana = obter_limite_mana(p);
        if(p->mana > limite_mana) p->mana = limite_mana;
        
        if(danoPendente == 0 && (escudo_luz_ativo || barreira_item_ativa)) sprintf(msg, "VITORIA PERFEITA! Baixas: %d.", lacaiosMortos);
        else sprintf(msg, "VITORIA! Baixas: %d.", lacaiosMortos);
            
        strcpy(p->relatorio, msg);
        gerar_drop_item(p);
        ativar_passiva_sifao(p);
        dissipar_sombras_extras(p);
        pausa_vitoria(p, p->relatorio);
        gerar_evento_aleatorio(p);

        if(p->boss_id_atual > 0 && p->boss_id_atual < 11) tela_escolher_recompensa(p);

        p->turno_global++; p->escavacoes_turno = 0; p->buff_profanacao = 0; 
        if (p->estagio_marcha == 11) { tela_vitoria_final(); registrar_morte(p, "CONQUISTOU A CIDADELA");p->estagio_marcha = 12; return; }
        
        p->progresso_local++;
        if(p->progresso_local > 10) {
            p->estagio_marcha++; p->progresso_local = 1;
            int ganho_vida = (p->classe == CLASSE_ESPIRITUALISTA) ? 25 : 50;
            p->max_hp += ganho_vida; p->hp = p->max_hp;  
        }
        if(p->estagio_marcha == 11 && p->progresso_local == 1 && p->artefatos[10] == 3) { p->mana = 100; p->corpos = 5; }
        gerar_novo_inimigo(p);
        if(p->progresso_local == 1 && p->estagio_marcha <= 10) sprintf(msg, "AVANCO DE LOCAL! HP Restaurado.");
        else sprintf(msg, "A marcha continua...");
        strcpy(p->relatorio, msg);

    } else {
        sprintf(msg, "%s resiste (%d HP). Suas baixas: %d.", p->nome_inimigo, p->hp_inimigo_atual, lacaiosMortos);
        atualizar_relatorio(p, msg);
    }
}
//reinicializador
void iniciar_jogo(Necromante* p, int classe_escolhida) {
    for(int i=0; i<31; i++) p->eventos_realizados[i] = 0;
    
    // Zera descobertas
    for(int i=0; i<21; i++) p->lacaios_descobertos[i] = 0;
    
    p->total_minions = 0;


    p->classe = classe_escolhida;
    
    // Status Base Padrão
    p->mana = 50;
    p->corpos = 2;
    p->hp = 100;
    p->max_hp = 100;
    p->cheat_evento_ativo = 0;

    PerfilGlobal perfil;
    carregar_perfil(&perfil);
    for(int i=1; i<=4; i++) {
        p->evolucoes_ativas[i] = perfil.evolucoes_lacaios[i];
    }

    // --- APLICAÇÃO DA CLASSE ---
    if (p->classe == CLASSE_COVEIRO) {
        p->hp = 80;
        p->max_hp = 80;
        p->mana = 40;
    }
    else if (p->classe == CLASSE_MAGO_SANGUE) {
        p->hp = 150;
        p->max_hp = 150;
    }
    else if (p->classe == CLASSE_ESPIRITUALISTA) {
        p->hp = 50;
        p->max_hp = 50;
    }
    for(int i=0; i<3; i++) p->inventario[i] = 0;
    p->flag_item_barreira = 0;
    p->flag_item_contrato = 0;
    p->flag_item_cristal = 0;
    p->ultimo_tipo_morto = 0;

    p->turno_global = 1;
    p->estagio_marcha = 1;
    p->progresso_local = 1;
    p->escavacoes_turno = 0;
    p->buff_profanacao = 0;
    
    for(int i=0; i<12; i++) p->artefatos[i] = 0;
    
    p->total_minions = 0;
    p->exercito_head = NULL;
    strcpy(p->relatorio, "A Marcha para a Cidadela comeca...");
    
    gerar_novo_inimigo(p); 
}

// Função auxiliar para ler números sem sujar a tela com "Enter"
int ler_numero_visual() {
    char buffer[10] = {0}; // Guarda os caracteres digitados
    int pos = 0;
    char ch;

    while(1) {
        ch = _getch(); // Captura a tecla sem imprimir automaticamente

        // Se for número (0 a 9)
        if(ch >= '0' && ch <= '9') {
            if(pos < 5) { // Limite de 5 digitos para não quebrar o layout
                printf("%c", ch); // Imprime o numero para o jogador ver
                buffer[pos++] = ch;
            }
        }
        // Se for Backspace (Apagar)
        else if(ch == 8) { 
            if(pos > 0) {
                printf("\b \b"); // Truque visual para apagar o caractere no console
                buffer[--pos] = 0;
            }
        }
        // Se for Enter
        else if(ch == 13) { 
            break; // Sai do loop mantendo o cursor na mesma linha
        }
    }
    
    // Se não digitou nada, retorna -1 (inválido)
    if(pos == 0) return -1;
    
    return atoi(buffer); // Converte texto para numero inteiro
}

//sacrificar
void sacrificar(Necromante* p) {
    // Verifica se tem alguém
    if(!p->exercito_head) {
        atualizar_relatorio(p, "Voce nao tem lacaios para sacrificar.");
        return;
    }

    // Prepara a tela
    char msg[200];
    sprintf(msg, "Pressione o numero [1-9] do lacaio (Custo: 5 HP ou 20%% se for Especial):");
    strncpy(p->relatorio, msg, 299);
    
    desenhar_interface(p, TELA_COMANDO); 
    
    // Input
    char ch = _getch(); 
    if(ch == 27 || ch == '0') {
        atualizar_relatorio(p, "Sacrificio cancelado.");
        return;
    }
    
    int slot_escolhido = ch - '0';
    if(slot_escolhido < 1 || slot_escolhido > 9) {
        atualizar_relatorio(p, "Opcao invalida. Use os numeros de 1 a 9.");
        return;
    }
    
    //Busca o lacaio
    Minion* atual = p->exercito_head;
    Minion* anterior = NULL;
    int indice_atual = 1;
    int encontrou = 0;
    
    while(atual != NULL) {
        if(indice_atual == slot_escolhido) {
            encontrou = 1;
            break;
        }
        anterior = atual;
        atual = atual->proximo;
        indice_atual++;
    }
    
    if(!encontrou) {
        atualizar_relatorio(p, "Nao existe lacaio nesta posicao.");
        return;
    }
    
    // LÓGICA DO CUSTO E GANHO
    int custo_vida = 0;
    int mana_ganha = atual->hp; 

    // Verifica se é especial
    if (strcmp(atual->nome, "Sombra") == 0 ||
        strcmp(atual->nome, "Legiao") == 0 ||
        strcmp(atual->nome, "Come-Mana") == 0 ||
        strcmp(atual->nome, "Devorador") == 0 ||
        strcmp(atual->nome, "Lich") == 0 ||
        strcmp(atual->nome, "Sifao") == 0) {
        
        // Especiais: 20% da vida
        custo_vida = (int)(atual->hp * 0.20);
        if (custo_vida < 1) custo_vida = 1; 
        
    } else {
        // Comuns: Custo 5
        custo_vida = 5;
    }
        if(p->artefatos[2] == 2) {
        custo_vida = 0;
    }
        // --- PROTEÇÃO CONTRA SUICÍDIO ---
    if (p->hp <= custo_vida) {
        // Se o custo for maior que a vida atual
        char aviso[200];
        sprintf(aviso, "PERIGO: Este sacrificio custa %d HP. Voce so tem %d HP. Acao cancelada para evitar morte.", custo_vida, p->hp);
        atualizar_relatorio(p, aviso);
        
        // Emite som de erro
        Beep(200, 300);
        return; // Cancela a ação
    }

    // ARTEFATO [8-3]: Ouro de Tolo (+20% Mana no sacrificio)
    if(p->artefatos[8] == 3) {
        mana_ganha = (int)(mana_ganha * 1.2);
    }

    // Aplicações
    p->hp -= custo_vida;
    p->mana += mana_ganha; // Soma direta

    // Guarda NOME
    char nome_sacrificado[30];
    strcpy(nome_sacrificado, atual->nome);
    
    // 6. Remove o lacaio
    if(anterior == NULL) p->exercito_head = atual->proximo;
    else anterior->proximo = atual->proximo;
    
    free(atual);
    p->total_minions--;
    
    // 7. Feedback
    int limite_atual = obter_limite_mana(p);
    
    if(p->mana > limite_atual) {
        sprintf(msg, "SACRIFICIO SUPREMO! (%s) consumido. Mana transbordando: %d/%d!", nome_sacrificado, p->mana, limite_atual);
    } else {
        sprintf(msg, "[%d] %s sacrificado (-%d HP). +%d Mana absorvida.", slot_escolhido, nome_sacrificado, custo_vida, mana_ganha);
    }
    
    atualizar_relatorio(p, msg);
}
//centralizador obrigado gemini
void print_ajuda_centralizado(const char* texto, const char* cor) {
    int largura_tela = 160;
    int len = strlen(texto); // Nota: Texto não pode ter códigos de cor embutidos aqui para o len funcionar
    int padding = (largura_tela - len) / 2;
    if (padding < 0) padding = 0;
    
    printf("%*s%s%s%s\n", padding, "", cor, texto, WHT);
}

//enciclopédia de lacaios
void menu_ajuda_lacaios(Necromante* p) {
    while(1) {
        limparTela(); 
        printf("\n\n\n");
        
        print_centralizado("==================================================================================", WHT);
        print_centralizado("ENCICLOPEDIA DE GUERRA", YEL);
        print_centralizado("==================================================================================", WHT);
        printf("\n\n");

        print_centralizado("Selecione o topico que deseja estudar:", WHT);
        printf("\n");

        print_centralizado("[ 1 ] EXERCITO DO NECROMANTE (Seus Lacaios)", GRN);
        print_centralizado("[ 2 ] ESQUADRAO INIMIGO (Oponentes)", RED);
        print_centralizado("[ 3 ] LACAIOS DE EVENTO (Raros)", YEL); // Nova Opção
        printf("\n\n");
        print_centralizado("[ 0 ] FECHAR ENCICLOPEDIA", WHT);

        printf("\n\n");
        print_centralizado("Digite sua escolha:", YEL);

        char ch = _getch();

        if (ch == '0' || ch == 27) break;

        // --- PÁGINA 1 (JOGADOR) ---
        if (ch == '1') {
            Beep(600, 30);
            limparTela();
            printf("\n");
            print_centralizado("=== SEUS LACAIOS (NECROMANTE) ===", GRN);
            printf("\n");

            print_centralizado("[ COMUNS ]", WHT);
            print_ajuda_centralizado("1. Esqueleto (15 Mn / 1 Cp): 5 ATK | 10 HP | Infantaria basica.", GRN);
            print_ajuda_centralizado("2. Zumbi (25 Mn / 2 Cp): 8 ATK | 40 HP | Tanque.", CYN);
            print_ajuda_centralizado("3. Carnical (40 Mn / 3 Cp): 20 ATK | 35 HP | Dano fisico.", GRN2);
            print_ajuda_centralizado("4. ABOMINACAO (100 Mn / 5 Cp): 60 ATK | 250 HP | Tita.", GRN);
            printf("\n");
            print_centralizado("[ ESPECIAIS ] (Custo: 10 HP)", WHT);
                        print_ajuda_centralizado("5. Sombra: Copia 50% dos status. (Instavel: Apenas 1 sobrevive pos-batalha).", CYN);
            print_ajuda_centralizado("6. Legiao: HP 10 | ATK = 5 + (2 x Total de Lacaios).", GRN2);
            print_ajuda_centralizado("7. Come-Mana: HP 15 |  ATK = Toda a sua Mana restante.", GRN);
            print_ajuda_centralizado("8. Devorador: HP = 15 x Corpos consumidos | ATK 10.", CYN);
            print_ajuda_centralizado("9. Lich: 50 ATK | 1 HP | Canhao de vidro.", GRN2);
            print_ajuda_centralizado("10. Sifao: HP 30 | ATK 2 | Passiva: Come aliado p/ curar 5 HP/turno.", GRN);

            printf("\n");
            print_centralizado("[Pressione qualquer tecla para voltar...]", YEL);
            _getch();
        }

        // --- PÁGINA 2 (INIMIGOS) ---
        else if (ch == '2') {
            Beep(600, 30);
            limparTela();
            printf("\n");
            print_centralizado("=== ESQUADRAO INIMIGO (OPONENTES) ===", RED);
            printf("\n");
            
            print_centralizado("[ ANTI-ENXAME ]", RED);
            print_ajuda_centralizado("- Granadeiro: Causa dano em area (Todos).", RED);
            print_ajuda_centralizado("- Berserker: Ataca de novo se matar.", RED);
            print_ajuda_centralizado("- Escudeiro: Imune a ataques fracos.", WHT);
            printf("\n");
            print_centralizado("[ ANTI-TANQUE ]", YEL);
            print_ajuda_centralizado("- Duelista: Dano critico em alvos gigantes.", YEL);
            print_ajuda_centralizado("- Clerigo: Marca alvo para execucao.", MAG);
            print_ajuda_centralizado("- Vampiro: Cura a si mesmo atacando.", RED);
            printf("\n");
            print_centralizado("[ DISRUPTORES ]", MAG);
            print_ajuda_centralizado("- Arqueiro: Dano direto no Necromante.", YEL);
            print_ajuda_centralizado("- Mago Anulador: Queima sua Mana.", MAG);
            print_ajuda_centralizado("- Ladrao de Tumulos: Rouba seus Corpos.", MAG);
            printf("\n");
            print_centralizado("[ ESPECIALISTAS ]", WHT);
            print_ajuda_centralizado("- Fanatico: Explode no 3o turno.", RED);
            print_ajuda_centralizado("- Fantasma: IMUNE a Dano Fisico.", MAG);
            print_ajuda_centralizado("- Cacador de Magos: IMUNE a Dano Magico.", MAG);

            printf("\n");
            print_centralizado("[Pressione qualquer tecla para voltar...]", YEL);
            _getch();
        }
        
        // --- PÁGINA 3 (SECRETOS) ---
        else if (ch == '3') {
            limparTela();
            printf("\n");
            print_centralizado("=== LACAIOS SECRETOS (EVENTOS) ===", YEL);
            print_centralizado("(Encontre-os no mapa para liberar as informacoes)", WHT);
            printf("\n");

            // Helper macro para simplificar
            #define MOSTRAR_SECRETO(ID, NOME, DESC) \
                if(p->lacaios_descobertos[ID]) { \
                    print_ajuda_centralizado(NOME, YEL); \
                    print_ajuda_centralizado(DESC, YEL); \
                } else { \
                    print_ajuda_centralizado("??? (Desconhecido)", WHT); \
                    print_ajuda_centralizado("Explore o mundo para encontrar...", WHT); \
                } \
                printf("\n");

            MOSTRAR_SECRETO(11, "11. Cavaleiro Sem Cabeca (80/80)", "Dano explosivo e resistencia media.");
            MOSTRAR_SECRETO(12, "12. Paladino Caido (20/200)", "Defesa suprema. Reduz dano recebido.");
            MOSTRAR_SECRETO(13, "13. Homunculo (5/50)", "Instavel. Explode causando 50 dano ao morrer.");
            MOSTRAR_SECRETO(14, "14. Besta da Cripta (40/120)", "Frenesi: Ganha ataque a cada turno vivo.");
            MOSTRAR_SECRETO(15, "15. Arqueiro Esqueleto (60/40)", "Retaguarda: Chance de evitar dano fisico.");
            MOSTRAR_SECRETO(16, "16. Sacerdote Sombrio (15/60)", "Aura: Aumenta o ATK de todos os aliados.");
            MOSTRAR_SECRETO(17, "17. Espectro da Praga (30/70)", "Contagio: Reduz o ATK do inimigo.");
            MOSTRAR_SECRETO(18, "18. Golem de Ossos (30/150)", "Reciclagem: Dropa corpos ao morrer.");
            MOSTRAR_SECRETO(19, "19. Vampiro Nobre (45/90)", "Sede: Cura o Necromante ao atacar.");
            MOSTRAR_SECRETO(20, "20. O Verdugo (70/100)", "Guilhotina: Hit Kill em inimigos com <20% HP.");

            print_centralizado("[Pressione qualquer tecla para voltar...]", YEL);
            _getch();
        }
    }
    
    limparTela(); 
}

void menu_regras_jogo() {
    int pagina_atual = 1;
    int total_paginas = 2;

    while(1) {
        limparTela();
        printf("\n\n");
        
        char titulo[100];
        sprintf(titulo, "MANUAL DO NECROMANTE: PAGINA %d/%d", pagina_atual, total_paginas);

        print_centralizado("==================================================================================", WHT);
        print_centralizado(titulo, MAG);
        print_centralizado("==================================================================================", WHT);
        printf("\n");

        if (pagina_atual == 1) {
            // --- CONTEÚDO PÁGINA 1: BÁSICO ---
            
            print_centralizado("[ 1. A TRINDADE DE RECURSOS ]", YEL);
            printf("\n");
            print_centralizado("HP (VIDA): Se chegar a 0, sua marcha acaba. E usado como moeda para rituais.", WHT);
            print_centralizado("MANA: Energia para invocar. Regenera com Magia Vil ou Sacrificio.", WHT);
            print_centralizado("Limite Padrao: 100. (Espiritualistas ou portadores da Pedra Filosofal tem 250).", CYN);
            print_centralizado("CORPOS: Municao para seu exercito. Obtidos escavando ou vencendo batalhas.", WHT);
            printf("\n");
            
            print_centralizado("----------------------------------------------------------------------------------", WHT);
            printf("\n");

            print_centralizado("[ 2. RITUAIS E GERENCIAMENTO ]", YEL);
            printf("\n");
            
            print_centralizado("ESCAVAR TUMULOS:", GRN);
            print_centralizado("Gera 1 a 3 Corpos. Tem chance de encontrar Essencia (+20 Mana).", WHT);
            print_centralizado("Limite: Voce so pode escavar 5 vezes (ou 7, se Coveiro) antes de esgotar o solo.", WHT);
            printf("\n");

            print_centralizado("MAGIA VIL (O PRECO DO PODER):", RED);
            print_centralizado("Custo: 5 de HP (Varia com Local/Classe).", RED);
            print_centralizado("Efeito: +15 Mana e RESETTA o limite de escavar.", GRN);
            print_centralizado("Consequencia: O Inimigo se cura 100% e ganha BUFFS permanentes de atributo.", RED);
            print_centralizado("Risco: Seus lacaios sofrem decomposicao a cada uso.", WHT);
            printf("\n");

            print_centralizado("SACRIFICAR (QUEBRA DE LIMITES):", CYN);
            print_centralizado("Custo: 5 de HP (Comuns) ou 20% da Vida do Lacaio (Especiais).", RED);
            print_centralizado("Efeito: Mata um lacaio e converte a Vida dele em Mana.", GRN);
            print_centralizado("REGRA DE OURO: E a unica forma de ultrapassar o limite maximo de Mana.", YEL);
        } 
        else {
            // --- CONTEÚDO PÁGINA 2: AVANÇADO ---

            print_centralizado("[ 3. INSTABILIDADE ARCANA ]", RED);
            printf("\n");
            print_centralizado("Se sua Mana estiver acima do limite (ex: 150/100) devido a Sacrificios:", WHT);
            print_centralizado("Invocar Lacaios COMUNS e seguro.", GRN);
            print_centralizado("Invocar Lacaios ESPECIAIS causa dano de rebote em voce.", RED);
            print_centralizado("Dano = 20% da Vida Total da unidade invocada.", RED);
            printf("\n");

            print_centralizado("----------------------------------------------------------------------------------", WHT);
            printf("\n");

            print_centralizado("[ 4. A ARTE DA GUERRA ]", YEL);
            printf("\n");
            print_centralizado("LINHA DE FRENTE: O dano inimigo atinge seus lacaios na ordem inversa (LIFO).", WHT);
            print_centralizado("O ultimo lacaio invocado e o primeiro a receber dano.", CYN);
            print_centralizado("DANO EXCEDENTE: Se um lacaio morre, o dano restante passa para o proximo.", RED);
            print_centralizado("Se nao houver lacaios, o dano vai direto na SUA VIDA.", RED);
            print_centralizado("OBJETIVO: Vencer 10 Locais (10 batalhas cada) para chegar ao Rei.", YEL);
            printf("\n");
            
            print_centralizado("----------------------------------------------------------------------------------", WHT);
            printf("\n");

            print_centralizado("[ 5. MISTERIOS ]", MAG);
            printf("\n");
            print_centralizado("Consumiveis / Habilidade Especial: Teste para descobrir!", YEL);
            print_centralizado("A enciclopedia nao fala tudo, jogue e perceba as nuances!", YEL);
            print_centralizado("Alguns inimigos tem imunidades que nao estao obvias a primeira vista.", WHT);
        }

        // --- RODAPÉ DE NAVEGAÇÃO ---
        printf("\n\n");
        print_centralizado("==================================================================================", WHT);
        
        if (pagina_atual == 1)
            print_centralizado("NAVEGACAO: Use [<- / ->] para Rolar | [ESC/ENTER] SAIR", YEL);
        else
            print_centralizado("NAVEGACAO: Use [<- / ->] para Rolar | [ESC/ENTER] SAIR", YEL);
        
        // --- LÓGICA DE INPUT (SETAS) ---
        char ch = _getch();
        
        // Sair (ESC, 0 ou ENTER)
        if (ch == 27 || ch == '0' || ch == 13) { 
            break; 
        }

        // Setas (enviam dois códigos: -32 ou 224 primeiro)
        if (ch == -32 || ch == 224) {
            char seta = _getch(); // Pega o segundo código
            
            // Seta Esquerda (75)
            if (seta == 75) {
                if (pagina_atual > 1) pagina_atual--; Beep(700, 30);
            }
            // Seta Direita (77)
            if (seta == 77) {
                if (pagina_atual < total_paginas) pagina_atual++; Beep(700, 30);
            }
        }
        
        // Atalho numérico (Opcional, mantém compatibilidade)
        if (ch == '1') pagina_atual = 1;
        if (ch == '2') pagina_atual = 2;
    }
    
    limparTela(); 
}

//cheat para ir para o próximo chefe
void cheat_pular_para_boss(Necromante* p) {
    // Se já está no Final Boss, não faz nada
    if (p->estagio_marcha >= 11) {
        strcpy(p->relatorio, "[CHEAT] Voce ja esta no Trono Dourado!");
        return;
    }

    // Lógica do Pulo:
    // Se eu já estou na parte 10 (Boss atual), pulo para o Boss do PRÓXIMO estágio.
    // Se eu estou nas partes 1-9 (Inimigos comuns), pulo para a parte 10 (Boss DESTE estágio).
    if (p->progresso_local == 10) {
        p->estagio_marcha++;
        p->progresso_local = 10; // Já cai direto no boss do próximo
    } else {
        p->progresso_local = 10; // Cai no boss atual
    }

    // Se pulou para o estágio 11 (Rei), ajusta
    if (p->estagio_marcha == 11) {
        p->progresso_local = 1; 
    }

    // Aumenta a dificuldade global artificialmente (simula que o tempo passou)
    // Isso evita que você chegue no Boss 5 com status de Turno 1
    p->turno_global += 3; 

    // Reseta status de turno
    p->escavacoes_turno = 0;
    p->buff_profanacao = 0;

    // Gera o inimigo (A função vai detectar que é parte 10 e criará o Chefe)
    gerar_novo_inimigo(p);

    char msg[200];
    sprintf(msg, "[CHEAT] Salto ativado! Voce esta diante de: %s.", p->nome_inimigo);
    strcpy(p->relatorio, msg);
}

// Preenche a matriz 'arte' com 6 linhas de desenho baseadas no item
// Preenche a matriz 'arte' com 6 linhas de desenho COLORIDO
// Preenche a matriz 'arte' com 6 linhas de desenho COLORIDO (SEM TEXTO)
void obter_ascii_artefato(int boss_id, int opcao, char arte[6][60]) {
    // Limpa a matriz
    for(int i=0; i<6; i++) strcpy(arte[i], "                    "); 

    // Fragmento de Alma (Ciano)
    if (boss_id == 99) { 
        sprintf(arte[0], "%s      .---.      %s", CYN, WHT);
        sprintf(arte[1], "%s     /  _  \\     %s", CYN, WHT);
        sprintf(arte[2], "%s    |  ( )  |    %s", CYN, WHT);
        sprintf(arte[3], "%s     \\  ^  /     %s", CYN, WHT);
        sprintf(arte[4], "%s      `---'      %s", CYN, WHT);
        sprintf(arte[5], "%s                 %s", CYN, WHT); // Texto removido
        return;
    }

    switch(boss_id) {
        case 1: // ALDRIC
            if(opcao==1) { // Cutelo
                sprintf(arte[0], "%s      ,___       %s", RED, WHT);
                sprintf(arte[1], "%s     |   _\\      %s", RED, WHT);
                sprintf(arte[2], "%s     |  |xx|     %s", RED, WHT);
                sprintf(arte[3], "%s     |__|''      %s", RED, WHT);
                sprintf(arte[4], "%s      ||         %s", RED, WHT);
                sprintf(arte[5], "%s      ||         %s", RED, WHT);
            } else if(opcao==2) { // Estandarte
                sprintf(arte[0], "%s      |>         %s", CYN, WHT);
                sprintf(arte[1], "%s      |__        %s", CYN, WHT);
                sprintf(arte[2], "%s      |  |       %s", CYN, WHT);
                sprintf(arte[3], "%s      |__|       %s", CYN, WHT);
                sprintf(arte[4], "%s      |          %s", CYN, WHT);
                sprintf(arte[5], "%s     / \\         %s", CYN, WHT);
            } else { // Elmo
                sprintf(arte[0], "%s      _.-._      %s", GRN, WHT);
                sprintf(arte[1], "%s    _| o o |_    %s", GRN, WHT);
                sprintf(arte[2], "%s   (_  ---  _)   %s", GRN, WHT);
                sprintf(arte[3], "%s     |  |  |     %s", GRN, WHT);
                sprintf(arte[4], "%s     |__|__|     %s", GRN, WHT);
                sprintf(arte[5], "%s                 %s", GRN, WHT);
            }
            if(opcao==4) { // Pa (Verde - Vida)
                sprintf(arte[0], "%s      ___        %s", GRN, WHT);
                sprintf(arte[1], "%s      | |        %s", GRN, WHT);
                sprintf(arte[2], "%s      | |        %s", GRN, WHT);
                sprintf(arte[3], "%s     _| |_       %s", GRN, WHT);
                sprintf(arte[4], "%s    [_   _]      %s", GRN, WHT);
                sprintf(arte[5], "%s      \\_/        %s", GRN, WHT);
            }
            break;

        case 2: // MILLER
            if(opcao==1) { // Coleira
                sprintf(arte[0], "%s     ,^,^,^,     %s", RED, WHT);
                sprintf(arte[1], "%s    / /   \\ \\    %s", RED, WHT);
                sprintf(arte[2], "%s   ( (     ) )   %s", RED, WHT);
                sprintf(arte[3], "%s    \\ \\___/ /    %s", RED, WHT);
                sprintf(arte[4], "%s     `vvvvv'     %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Ferramentas
                sprintf(arte[0], "%s      .---.      %s", CYN, WHT);
                sprintf(arte[1], "%s     /   /|      %s", CYN, WHT);
                sprintf(arte[2], "%s    |   | |      %s", CYN, WHT);
                sprintf(arte[3], "%s     \\   \\|      %s", CYN, WHT);
                sprintf(arte[4], "%s      `---'      %s", CYN, WHT);
                sprintf(arte[5], "%s                 %s", CYN, WHT); // Texto removido
            } else { // Botas
                sprintf(arte[0], "%s                 %s", GRN, WHT);
                sprintf(arte[1], "%s      |__|       %s", GRN, WHT);
                sprintf(arte[2], "%s      |  |       %s", GRN, WHT);
                sprintf(arte[3], "%s     _|  |_      %s", GRN, WHT);
                sprintf(arte[4], "%s    |______|     %s", GRN, WHT);
                sprintf(arte[5], "%s                 %s", GRN, WHT); // Texto removido
            }
            if(opcao==4) { // Mordaça (Vermelho - Dor)
                sprintf(arte[0], "%s    _______      %s", RED, WHT);
                sprintf(arte[1], "%s   /       \\     %s", RED, WHT);
                sprintf(arte[2], "%s  (  #####  )    %s", RED, WHT);
                sprintf(arte[3], "%s   \\_______/     %s", RED, WHT);
                sprintf(arte[4], "%s      | |        %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            }
            break;

        case 3: // PADRE
            if(opcao==1) { // Calice
                sprintf(arte[0], "%s    \\~~~~~~/     %s", RED, WHT);
                sprintf(arte[1], "%s     \\    /      %s", RED, WHT);
                sprintf(arte[2], "%s      |  |       %s", RED, WHT);
                sprintf(arte[3], "%s      |  |       %s", RED, WHT);
                sprintf(arte[4], "%s     /____\\      %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT); // Texto removido
            } else if(opcao==2) { // Rosario
                sprintf(arte[0], "%s      _()_       %s", YEL, WHT);
                sprintf(arte[1], "%s     /    \\      %s", YEL, WHT);
                sprintf(arte[2], "%s    (      )     %s", YEL, WHT);
                sprintf(arte[3], "%s     \\_  _/      %s", YEL, WHT);
                sprintf(arte[4], "%s       ||        %s", YEL, WHT);
                sprintf(arte[5], "%s       --        %s", YEL, WHT);
            } else { // Mortalha
                sprintf(arte[0], "%s      .--.       %s", WHT, WHT);
                sprintf(arte[1], "%s     /  _ \\      %s", WHT, WHT);
                sprintf(arte[2], "%s    |  (_) |     %s", WHT, WHT);
                sprintf(arte[3], "%s    |      |     %s", WHT, WHT);
                sprintf(arte[4], "%s     \\____/      %s", WHT, WHT);
                sprintf(arte[5], "%s                 %s", WHT, WHT);
            }
            if(opcao==4) { // Mitra (Ciano - Clerigo)
                sprintf(arte[0], "%s      /\\         %s", CYN, WHT);
                sprintf(arte[1], "%s     /++\\        %s", CYN, WHT);
                sprintf(arte[2], "%s    |    |       %s", CYN, WHT);
                sprintf(arte[3], "%s    |    |       %s", CYN, WHT);
                sprintf(arte[4], "%s    |____|       %s", CYN, WHT);
                sprintf(arte[5], "%s   /      \\      %s", CYN, WHT);
            }
            break;

        case 4: // VANE
            if(opcao==1) { // Totem
                sprintf(arte[0], "%s      (  )       %s", RED, WHT);
                sprintf(arte[1], "%s     ( XX )      %s", RED, WHT);
                sprintf(arte[2], "%s      |  |       %s", RED, WHT);
                sprintf(arte[3], "%s     _|__|_      %s", RED, WHT);
                sprintf(arte[4], "%s    /______\\     %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Fosforo
                sprintf(arte[0], "%s        )        %s", YEL, WHT);
                sprintf(arte[1], "%s       ( )       %s", YEL, WHT);
                sprintf(arte[2], "%s       | |       %s", YEL, WHT);
                sprintf(arte[3], "%s       | |       %s", YEL, WHT);
                sprintf(arte[4], "%s       |_|       %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            } else { // Manto
                sprintf(arte[0], "%s      /\\         %s", CYN, WHT);
                sprintf(arte[1], "%s     /  \\        %s", CYN, WHT);
                sprintf(arte[2], "%s    |    |       %s", CYN, WHT);
                sprintf(arte[3], "%s    |    |       %s", CYN, WHT);
                sprintf(arte[4], "%s   /______\\      %s", CYN, WHT);
                sprintf(arte[5], "%s                 %s", CYN, WHT);
            }
            if(opcao==4) { // Chama (Amarelo - Fogo/Recurso)
                sprintf(arte[0], "%s      (          %s", YEL, WHT);
                sprintf(arte[1], "%s     ) (         %s", YEL, WHT);
                sprintf(arte[2], "%s    (   )        %s", YEL, WHT);
                sprintf(arte[3], "%s    )   (        %s", YEL, WHT);
                sprintf(arte[4], "%s   (_____)       %s", YEL, WHT);
                sprintf(arte[5], "%s    \\___/        %s", YEL, WHT);
            }
            break;

        case 5: // KAEL
            if(opcao==1) { // Coracao
                sprintf(arte[0], "%s     _    _      %s", RED, WHT);
                sprintf(arte[1], "%s    ( \\__/ )     %s", RED, WHT);
                sprintf(arte[2], "%s     \\    /      %s", RED, WHT);
                sprintf(arte[3], "%s      \\  /       %s", RED, WHT);
                sprintf(arte[4], "%s       \\/        %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Escudo
                sprintf(arte[0], "%s    _______      %s", YEL, WHT);
                sprintf(arte[1], "%s   |   |   |     %s", YEL, WHT);
                sprintf(arte[2], "%s   |___|___|     %s", YEL, WHT);
                sprintf(arte[3], "%s    \\     /      %s", YEL, WHT);
                sprintf(arte[4], "%s     \\___/       %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            } else { // Bencao
                sprintf(arte[0], "%s      \\  /       %s", CYN, WHT);
                sprintf(arte[1], "%s    - (**) -     %s", CYN, WHT);
                sprintf(arte[2], "%s      /  \\       %s", CYN, WHT);
                sprintf(arte[3], "%s                 %s", CYN, WHT);
                sprintf(arte[4], "%s      (  )       %s", CYN, WHT); // Texto removido, apenas simbolo
                sprintf(arte[5], "%s                 %s", CYN, WHT);
            }
            if(opcao==4) { // Prisma (Magenta - Magia Negra)
                sprintf(arte[0], "%s      /\\         %s", MAG, WHT);
                sprintf(arte[1], "%s     /  \\        %s", MAG, WHT);
                sprintf(arte[2], "%s    /____\\       %s", MAG, WHT);
                sprintf(arte[3], "%s   /      \\      %s", MAG, WHT);
                sprintf(arte[4], "%s   \\______/      %s", MAG, WHT);
                sprintf(arte[5], "%s                 %s", MAG, WHT);
            }
            break;

        case 6: // DARIUS
            if(opcao==1) { // Coroa
                sprintf(arte[0], "%s    .M.M.M.      %s", YEL, WHT);
                sprintf(arte[1], "%s   |       |     %s", YEL, WHT);
                sprintf(arte[2], "%s   |_______|     %s", YEL, WHT);
                sprintf(arte[3], "%s                 %s", YEL, WHT);
                sprintf(arte[4], "%s                 %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            } else if(opcao==2) { // Mapa
                sprintf(arte[0], "%s    _______      %s", WHT, WHT);
                sprintf(arte[1], "%s   | .--.  |\\    %s", WHT, WHT);
                sprintf(arte[2], "%s   | `--'  | |   %s", WHT, WHT);
                sprintf(arte[3], "%s   |_______| |   %s", WHT, WHT);
                sprintf(arte[4], "%s    \\_______\\|   %s", WHT, WHT);
                sprintf(arte[5], "%s                 %s", WHT, WHT);
            } else { // Berrante
                sprintf(arte[0], "%s     /\\  |\\      %s", RED, WHT);
                sprintf(arte[1], "%s    |  \\_| |     %s", RED, WHT);
                sprintf(arte[2], "%s    |      |     %s", RED, WHT);
                sprintf(arte[3], "%s     \\    /      %s", RED, WHT);
                sprintf(arte[4], "%s      `--'       %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            }
            break;

        case 7: // JINX
            if(opcao==1) { // Pedra Filosofal
                sprintf(arte[0], "%s      / \\        %s", RED, WHT);
                sprintf(arte[1], "%s     /___\\       %s", RED, WHT);
                sprintf(arte[2], "%s    (  O  )      %s", RED, WHT);
                sprintf(arte[3], "%s     \\   /       %s", RED, WHT);
                sprintf(arte[4], "%s      \\_/        %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Mascara Gas
                sprintf(arte[0], "%s     (O O)       %s", GRN, WHT);
                sprintf(arte[1], "%s      \\^/        %s", GRN, WHT);
                sprintf(arte[2], "%s     //|\\\\       %s", GRN, WHT);
                sprintf(arte[3], "%s    (( | ))      %s", GRN, WHT);
                sprintf(arte[4], "%s                 %s", GRN, WHT);
                sprintf(arte[5], "%s                 %s", GRN, WHT);
            } else { // Frasco
                sprintf(arte[0], "%s      |=|        %s", CYN, WHT);
                sprintf(arte[1], "%s     /   \\       %s", CYN, WHT);
                sprintf(arte[2], "%s    | XXX |      %s", CYN, WHT);
                sprintf(arte[3], "%s    | XXX |      %s", CYN, WHT);
                sprintf(arte[4], "%s     \\___/       %s", CYN, WHT);
                sprintf(arte[5], "%s                 %s", CYN, WHT);
            }
            break;

        case 8: // PEZAO
            if(opcao==1) { // Dados
                sprintf(arte[0], "%s      .-------.  %s", WHT, WHT);
                sprintf(arte[1], "%s     /   o   /|  %s", WHT, WHT);
                sprintf(arte[2], "%s    /_______/o|  %s", WHT, WHT);
                sprintf(arte[3], "%s    | o     | |  %s", WHT, WHT);
                sprintf(arte[4], "%s    |   o   |o/  %s", WHT, WHT);
                sprintf(arte[5], "%s    '-------'    %s", WHT, WHT);
            } else if(opcao==2) { // Caixa
                sprintf(arte[0], "%s      _____      %s", CYN, WHT);
                sprintf(arte[1], "%s     |  ?  |     %s", CYN, WHT);
                sprintf(arte[2], "%s     |_____|     %s", CYN, WHT);
                sprintf(arte[3], "%s                 %s", CYN, WHT);
                sprintf(arte[4], "%s                 %s", CYN, WHT);
                sprintf(arte[5], "%s                 %s", CYN, WHT);
            } else { // Ouro
                sprintf(arte[0], "%s      /\"\\        %s", YEL, WHT);
                sprintf(arte[1], "%s     ( $ )       %s", YEL, WHT);
                sprintf(arte[2], "%s      \\_/        %s", YEL, WHT);
                sprintf(arte[3], "%s     o 8 o       %s", YEL, WHT);
                sprintf(arte[4], "%s                 %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            }
            break;

        case 9: // TYBALT
            if(opcao==1) { // Rapieira
                sprintf(arte[0], "%s        /        %s", RED, WHT);
                sprintf(arte[1], "%s   O===|-------->%s", RED, WHT);
                sprintf(arte[2], "%s        \\        %s", RED, WHT);
                sprintf(arte[3], "%s                 %s", RED, WHT);
                sprintf(arte[4], "%s                 %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Decreto
                sprintf(arte[0], "%s     __________  %s", YEL, WHT);
                sprintf(arte[1], "%s    | ~~~~~~~~ | %s", YEL, WHT);
                sprintf(arte[2], "%s    | REi :    | %s", YEL, WHT);
                sprintf(arte[3], "%s    |__________| %s", YEL, WHT);
                sprintf(arte[4], "%s        (O)      %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            } else { // Anel
                sprintf(arte[0], "%s      .---.      %s", CYN, WHT);
                sprintf(arte[1], "%s     /     \\     %s", CYN, WHT);
                sprintf(arte[2], "%s    |   O   |    %s", CYN, WHT);
                sprintf(arte[3], "%s     \\     /     %s", CYN, WHT);
                sprintf(arte[4], "%s      `---'      %s", CYN, WHT);
                sprintf(arte[5], "%s                 %s", CYN, WHT);
            }
            break;

        case 10: // GOLEM
            if(opcao==1) { // Nucleo
                sprintf(arte[0], "%s      {*}        %s", RED, WHT);
                sprintf(arte[1], "%s    -{ * }-      %s", RED, WHT);
                sprintf(arte[2], "%s      {*}        %s", RED, WHT);
                sprintf(arte[3], "%s                 %s", RED, WHT);
                sprintf(arte[4], "%s                 %s", RED, WHT);
                sprintf(arte[5], "%s                 %s", RED, WHT);
            } else if(opcao==2) { // Quebra-Cercos
                sprintf(arte[0], "%s      ____       %s", GRN, WHT);
                sprintf(arte[1], "%s     |####|      %s", GRN, WHT);
                sprintf(arte[2], "%s     |####|      %s", GRN, WHT);
                sprintf(arte[3], "%s       ||        %s", GRN, WHT);
                sprintf(arte[4], "%s       ||        %s", GRN, WHT);
                sprintf(arte[5], "%s                 %s", GRN, WHT);
            } else { // Chave
                sprintf(arte[0], "%s      O______°   %s", YEL, WHT);
                sprintf(arte[1], "%s      /  |_|_    %s", YEL, WHT);
                sprintf(arte[2], "%s                 %s", YEL, WHT);
                sprintf(arte[3], "%s                 %s", YEL, WHT);
                sprintf(arte[4], "%s                 %s", YEL, WHT);
                sprintf(arte[5], "%s                 %s", YEL, WHT);
            }
            break;
    }
}

//centralizador de colunas obrigado gemini
void imprimir_3col_centralizado(const char* t1, const char* t2, const char* t3, const char* cor) {
    int largura_col = 53; 
    int len1 = strlen(t1);
    int len2 = strlen(t2);
    int len3 = strlen(t3);
    
    // Calcula margem esquerda para cada texto
    int pad1 = (largura_col - len1) / 2;
    int pad2 = (largura_col - len2) / 2;
    int pad3 = (largura_col - len3) / 2;
    
    // Segurança para não quebrar se texto for muito grande
    if (pad1 < 0) pad1 = 0;
    if (pad2 < 0) pad2 = 0;
    if (pad3 < 0) pad3 = 0;

    // COLUNA 1
    printf("%*s%s%s%s", pad1, "", cor, t1, WHT); // Espaços, Cor, Texto, Branco
    // Preenche o resto da coluna com espaços
    int resto1 = largura_col - pad1 - len1;
    printf("%*s", resto1, "");

    // COLUNA 2
    printf("%*s%s%s%s", pad2, "", cor, t2, WHT);
    int resto2 = largura_col - pad2 - len2;
    printf("%*s", resto2, "");

    // COLUNA 3
    printf("%*s%s%s%s", pad3, "", cor, t3, WHT);
    printf("\n");
}

void tela_escolher_recompensa(Necromante* p) {
    limparTela();
    char n1[50], d1[100], n2[50], d2[100], n3[50], d3[100];
    
    char arte1[6][60], arte2[6][60], arte3[6][60];

    get_info_artefato(p->boss_id_atual, 1, n1, d1);
    get_info_artefato(p->boss_id_atual, 2, n2, d2);
    get_info_artefato(p->boss_id_atual, 3, n3, d3);
    
    obter_ascii_artefato(p->boss_id_atual, 1, arte1);
    obter_ascii_artefato(p->boss_id_atual, 2, arte2);
    obter_ascii_artefato(p->boss_id_atual, 3, arte3);

    int substituto_1 = 0;
    if (p->artefatos[p->boss_id_atual] == 1) {
        strcpy(n1, "FRAGMENTO DE ALMA");
        strcpy(d1, "Item ja possuido. Ganhe +20 Max HP e Cura.");
        obter_ascii_artefato(99, 1, arte1);
        substituto_1 = 1; 
    }

    // --- VERIFICA DESBLOQUEIO GLOBAL ---
    PerfilGlobal perfil;
    carregar_perfil(&perfil);
    int item_extra_liberado = perfil.paginas_desbloqueadas[p->boss_id_atual];
    // -----------------------------------

    printf("\n\n");
    printf("%s======================================================================================================================================================\n", WHT);
    printf("                                                    VITORIA! O CHEFE CAIU. ESCOLHA SEU ESPOLIO                                                        \n");
    printf("======================================================================================================================================================\n\n");

    imprimir_3col_centralizado("[1] OFENSIVO", "[2] ESTRATEGIA", "[3] DEFESA", YEL);
    
    printf("\n");

    // Loop de Impressão das 3 Colunas
    for(int i=0; i<6; i++) {
        printf("                  %s                                    %s                                    %s\n", 
            arte1[i], arte2[i], arte3[i]);
    }

    printf("\n");

    imprimir_3col_centralizado(n1, n2, n3, CYN);
    imprimir_3col_centralizado("--------------------", "--------------------", "--------------------", WHT);
    imprimir_3col_centralizado(d1, d2, d3, WHT);
    
    // --- EXIBIÇÃO DA OPÇÃO 4 (CORRIGIDA) ---
    if (item_extra_liberado) {
        char n4[50], d4[100];
        char arte4[6][60];
        
        get_info_artefato(p->boss_id_atual, 4, n4, d4);
        obter_ascii_artefato(p->boss_id_atual, 4, arte4);

        printf("\n");
        print_centralizado("--- ARTEFATO SECRETO DESBLOQUEADO (LOJA) ---", MAG);
        printf("\n");
        
        // CORREÇÃO AQUI: Espaçamento fixo (70 espaços) em vez de cálculo automático
        for(int i=0; i<6; i++) {
            printf("                                                                      %s\n", arte4[i]);
        }
        
        printf("\n"); // Espaço entre desenho e nome
        print_centralizado(n4, MAG);
        print_centralizado(d4, WHT);
        printf("\n");
        print_centralizado("[4] ESCOLHER ESTE ARTEFATO", YEL);
        printf("\n");
    }

    printf("\n\n");
    printf("======================================================================================================================================================\n");
    printf("                                                    DIGITE SUA ESCOLHA %s[1]%s, %s[2]%s OU %s[3]%s                                                    \n", YEL, WHT, YEL, WHT, YEL, WHT);
    printf("======================================================================================================================================================%s\n", RST);
    
    while(1) {
        char ch = _getch();
        if(ch >= '1' && ch <= '3') {
            int escolha = ch - '0';
            
            if (escolha == 1 && substituto_1 == 1) {
                p->max_hp += 20;
                p->hp += 20;
            } 
            else {
                p->artefatos[p->boss_id_atual] = escolha; 
                if(p->boss_id_atual == 6 && escolha == 1) {
                    p->max_hp += 50;
                    p->hp += 50;
                }
            }
            break;
        }
        
        // Lógica de escolha da opção 4
        if(ch == '4' && item_extra_liberado) {
             p->artefatos[p->boss_id_atual] = 4;
             break;
        }
    }
    limparTela(); 
}

// Imprime texto centralizado na tela de 160 colunas obrigado gemini
void print_centralizado(const char* texto, const char* cor) {
    int largura_tela = 160;
    int len = strlen(texto);
    int padding = (largura_tela - len) / 2;
    if (padding < 0) padding = 0;
    
    printf("%*s%s%s%s\n", padding, "", cor, texto, WHT);
}

void menu_ver_artefatos(Necromante* p) {
    // 1. Mapear quais itens o jogador possui
    int itens_equipados[12]; 
    int qtd_total = 0;
    
    for(int i=1; i<=10; i++) {
        if(p->artefatos[i] != 0) {
            itens_equipados[qtd_total] = i;
            qtd_total++;
        }
    }

    if (qtd_total == 0) {
        limparTela();
        printf("\n\n\n\n\n");
        print_centralizado("SEU INVENTARIO ESTA VAZIO.", RED);
        print_centralizado("Derrote Chefes para obter Artefatos de Poder.", WHT);
        print_centralizado("[Pressione qualquer tecla para voltar]", YEL);
        _getch();
        return;
    }

    int scroll = 0; 
    int itens_por_pagina = 2; 

    while(1) {
        irParaInicio(); 
        limparTela(); 

        printf("\n");
        printf("%s======================================================================================================================================================\n", WHT);
        print_centralizado("ARTEFATOS EQUIPADOS", YEL);
        printf("======================================================================================================================================================%s\n\n", WHT);

        // --- RENDERIZAÇÃO DOS ITENS ---
        for(int i = scroll; i < scroll + itens_por_pagina; i++) {
            if (i >= qtd_total) break; 

            int boss_id = itens_equipados[i];
            int opcao_escolhida = p->artefatos[boss_id];
            
            char nome[50], desc[200];
            char arte[6][60];

            get_info_artefato(boss_id, opcao_escolhida, nome, desc);
            obter_ascii_artefato(boss_id, opcao_escolhida, arte);

            print_centralizado(nome, CYN);
            
            for(int k=0; k<6; k++) {
                // Centralização manual da arte ASCII (~20 chars em tela de 160)
                printf("                                                                      %s\n", arte[k]);
            }

            print_centralizado(desc, WHT);
            
            printf("\n");
            print_centralizado("-----------------------------------", WHT);
            printf("\n");
        }

        // --- BARRA DE NAVEGAÇÃO ATUALIZADA ---
        printf("\n\n");
        printf("======================================================================================================================================================\n");
        
        char nav[100];
        if (qtd_total > itens_por_pagina) {
            // Texto atualizado para Setas
            sprintf(nav, "Item %d de %d | Use [<- / ->] para Rolar | [ESC/ENTER/0] Voltar", scroll + 1, qtd_total);
        } else {
            sprintf(nav, "Total de Itens: %d | [ESC/ENTER/0] Voltar", qtd_total);
        }
        print_centralizado(nav, YEL);
        printf("======================================================================================================================================================\n");

        // --- INPUT ATUALIZADO ---
        char ch = _getch();
        
        // 1. SAIR (ESC, '0' ou ENTER)
        // 27 = ESC, 13 = ENTER
        if (ch == 27 || ch == '0' || ch == 13) break; 
        
        // 2. NAVEGAÇÃO COM SETAS
        // Teclas especiais enviam -32 ou 224 antes do código real
        if (ch == -32 || ch == 224) {
            char seta = _getch(); // Pega o segundo código
            
            // Seta Esquerda (75) -> Rola para cima/trás
            if (seta == 75) {
                if (scroll > 0) scroll--;Beep(700, 30);
            }
            
            // Seta Direita (77) -> Rola para baixo/frente
            if (seta == 77) {
                if (scroll < qtd_total - 1) scroll++;Beep(700, 30); 
            }
        }
    }
    limparTela();
}


// --- SISTEMA DE PERSISTÊNCIA ---

void salvar_jogo(Necromante* p) {
    FILE *arquivo = fopen("savegame.dat", "wb"); 
    if (!arquivo) return;

    // Salva Struct Principal
    fwrite(p, sizeof(Necromante), 1, arquivo);

    // Salva Lacaios
    int qtd_lacaios = 0;
    Minion* m = p->exercito_head;
    while(m != NULL) { qtd_lacaios++; m = m->proximo; }

    fwrite(&qtd_lacaios, sizeof(int), 1, arquivo);

    m = p->exercito_head;
    while(m != NULL) {
        fwrite(m, sizeof(Minion), 1, arquivo);
        m = m->proximo;
    }

    fclose(arquivo);
}

int carregar_jogo(Necromante* p) {
    FILE *arquivo = fopen("savegame.dat", "rb");
    if (!arquivo) return 0; // Arquivo não existe

    // Lê Struct Principal
    if (fread(p, sizeof(Necromante), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0; // Erro de leitura (tamanho diferente)
    }

    // Limpa ponteiro de memória antigo (segurança)
    p->exercito_head = NULL;

    // Lê Lacaios
    int qtd_lacaios = 0;
    fread(&qtd_lacaios, sizeof(int), 1, arquivo);

    Minion* ultimo = NULL;
    for(int i = 0; i < qtd_lacaios; i++) {
        Minion* novo = (Minion*) calloc(1, sizeof(Minion));
        fread(novo, sizeof(Minion), 1, arquivo);
        novo->proximo = NULL; // Importante: anular o ponteiro lido do disco

        if (p->exercito_head == NULL) p->exercito_head = novo;
        else ultimo->proximo = novo;
        
        ultimo = novo;
    }

    fclose(arquivo);
    return 1; // Sucesso
}

void deletar_save() {
    remove("savegame.dat");
}

void registrar_morte(Necromante* p, const char* causa) {
    RegistroCemiterio reg;
    
    // Preenche dados
    if (p->classe == CLASSE_COVEIRO) strcpy(reg.nome_classe, "O Coveiro");
    else if (p->classe == CLASSE_MAGO_SANGUE) strcpy(reg.nome_classe, "Mago Sangue");
    else if (p->classe == CLASSE_ESPIRITUALISTA) strcpy(reg.nome_classe, "Espiritualista");
    else strcpy(reg.nome_classe, "Desconhecido");

    reg.turno_alcancado = p->turno_global;
    reg.estagio_final = p->estagio_marcha;
    strncpy(reg.causa_morte, causa, 49);
    reg.causa_morte[49] = '\0';
    
    reg.hp_final = p->hp;
    int atk_total = 0;
    Minion* m = p->exercito_head;
    while(m) { atk_total += m->ataque; m = m->proximo; }
    reg.atk_exercito_final = atk_total;

    for(int i=0; i<12; i++) reg.artefatos_coletados[i] = p->artefatos[i];

    // Salva no fim do arquivo
    FILE *arquivo = fopen("cemiterio.dat", "ab");
    if (arquivo) {
        fwrite(&reg, sizeof(RegistroCemiterio), 1, arquivo);
        fflush(arquivo); // Garante a escrita
        fclose(arquivo);
    }
        // --- CÁLCULO DE MOEDAS ---
    int moedas_ganhas = (p->turno_global * 10) + (p->estagio_marcha * 50);
    
    // Bônus de Vitória
    if (p->estagio_marcha == 11 && p->progresso_local == 1) { // Chegou no Rei? Ou venceu?
        // Se venceu (a função é chamada com "CONQUISTOU..."), dá bônus
        if (strstr(causa, "CONQUISTOU") != NULL) moedas_ganhas += 500;
    }

    // Carrega, Soma e Salva
    PerfilGlobal perfil;
    carregar_perfil(&perfil);
    perfil.moedas_caronte += moedas_ganhas;
    salvar_perfil(&perfil);

    // Feedback Visual Rápido antes de apagar o save
    printf("\n\n");
    print_centralizado("--------------------------------------------------", YEL);
    char msg_moedas[100];
    sprintf(msg_moedas, "ALMAS CEIFADAS: +%d MOEDAS DE CARONTE", moedas_ganhas);
    print_centralizado(msg_moedas, YEL);
    print_centralizado("--------------------------------------------------", YEL);
    Sleep(2000); // Pausa para ler
    
    deletar_save(); // Morreu, deleta o save do jogo atual
}


void menu_cemiterio() {
    FILE *arquivo = fopen("cemiterio.dat", "rb");
    
    if (!arquivo) {
        limparTela();
        printf("\n\n\n");
        print_centralizado("O CEMITERIO ESTA VAZIO.", WHT);
        print_centralizado("Nenhum registro encontrado.", WHT);
        _getch();
        return;
    }

    // Lê todos os registros para a memória (limitado a 100 para simplificar)
    RegistroCemiterio lista[100];
    int total = 0;
    while(fread(&lista[total], sizeof(RegistroCemiterio), 1, arquivo) && total < 100) {
        total++;
    }
    fclose(arquivo);

    // Navegação (Scroll) - Igual aos artefatos
    int scroll = 0;
    // Mostramos os mais recentes primeiro? Vamos inverter o loop na exibição.
    // Vamos mostrar 1 ficha grande por vez, ou 2. Vamos tentar 2.
    int itens_por_pagina = 2;

    while(1) {
        irParaInicio(); // Anti-flicker
        limparTela(); // Limpa para garantir fundo preto

        printf("\n");
        printf("%s======================================================================================================================================================\n", WHT);
        print_centralizado("CEMITERIO DOS CAIDOS (HISTORICO)", YEL);
        printf("======================================================================================================================================================%s\n\n", WHT);

        if (total == 0) {
             print_centralizado("Arquivo corrompido ou vazio.", RED);
             _getch();
             return;
        }

        // Loop Reverso (Mostra do mais recente para o mais antigo)
        // O índice visual 0 é o registro [total-1]
        for(int i = scroll; i < scroll + itens_por_pagina; i++) {
            if (i >= total) break;

            // Pega o registro (do fim para o começo)
            int idx_real = (total - 1) - i;
            RegistroCemiterio r = lista[idx_real];

            // --- RENDERIZAÇÃO DO CARTÃO ---
            
            // Título (Classe e Local)
            char titulo[100];
            sprintf(titulo, "%s - %s (Turno %d)", r.nome_classe, get_nome_local(r.estagio_final), r.turno_alcancado);
            print_centralizado(titulo, CYN);

            // Status Finais
            char status[100];
            sprintf(status, "HP Final: %d  |  Poder do Exercito: %d", r.hp_final, r.atk_exercito_final);
            print_centralizado(status, GRN);

            // Causa da Morte / Oponente
            char causa[100];
            // Se ganhou
            if (strstr(r.causa_morte, "CONQUISTOU") != NULL) {
                sprintf(causa, "STATUS: VITORIA (Rei da Cidadela Derrotado)");
                print_centralizado(causa, YEL);
            } 
            // Se morreu para Patrulha (não mostramos o nome "Patrulha", apenas o local)
            else if (strstr(r.causa_morte, "Patrulha") != NULL) {
                sprintf(causa, "CAUSA: Morto em combate comum.");
                print_centralizado(causa, RED);
            } 
            // Se morreu para Chefe
            else {
                sprintf(causa, "ELIMINADO POR: %s", r.causa_morte);
                print_centralizado(causa, RED);
            }

            // Artefatos Equipados (Lista apenas os nomes)
            printf("\n");
            print_centralizado("--- ARTEFATOS ENCONTRADOS ---", WHT);
            
            char linha_arts[200] = "";
            int tem_art = 0;
            char nome_art[50], lixo[100]; // Buffers auxiliares
            
            for(int k=1; k<=10; k++) {
                if (r.artefatos_coletados[k] != 0) {
                    // Pega o nome do artefato usando a função existente
                    get_info_artefato(k, r.artefatos_coletados[k], nome_art, lixo);
                    
                    // Adiciona na linha
                    if(tem_art) strcat(linha_arts, " | ");
                    strcat(linha_arts, nome_art);
                    tem_art = 1;
                }
            }
            
            if (!tem_art) strcpy(linha_arts, "(Nenhum artefato equipado)");
            
            // Imprime a linha de artefatos (pode ser longa, talvez quebre se tiver 10)
            // Se tiver muitos, seria ideal quebrar, mas vamos imprimir direto centralizado
            print_centralizado(linha_arts, YEL);

            printf("\n");
            print_centralizado("----------------------------------------------------------------------", WHT);
            printf("\n");
        }

        // Rodapé de Navegação
        printf("\n\n");
        char nav[100];
        sprintf(nav, "Registro %d de %d | [W/S] Rolar Historico | [ESC] Voltar", scroll + 1, total);
        print_centralizado(nav, WHT);

        char ch = _getch();
        Beep(300, 50);
        if (ch == 27 || ch == '0') break;
        if (ch == 'w' || ch == 'W') { if (scroll > 0) scroll--; }
        if (ch == 's' || ch == 'S') { if (scroll < total - 1) scroll++; }
    }
    limparTela();
}

// Preenche 6 linhas com arte detalhada do consumível grande
void obter_ascii_consumivel_grande(int id, char arte[6][60]) {
    // Limpa
    for(int i=0; i<6; i++) strcpy(arte[i], "                    ");

    switch(id) {
        case 0: // VAZIO (Slot livre)
            sprintf(arte[0], "%s      .---.      %s", WHT, WHT);
            sprintf(arte[1], "%s     /     \\     %s", WHT, WHT);
            sprintf(arte[2], "%s    | VAZIO |    %s", WHT, WHT);
            sprintf(arte[3], "%s     \\     /     %s", WHT, WHT);
            sprintf(arte[4], "%s      `---'      %s", WHT, WHT);
            sprintf(arte[5], "%s                 %s", WHT, WHT);
            break;

        case 1: // Elixir (Verde)
            sprintf(arte[0], "%s      _____      %s", GRN, WHT);
            sprintf(arte[1], "%s     `.___,'     %s", GRN, WHT);
            sprintf(arte[2], "%s      (___)      %s", GRN, WHT);
            sprintf(arte[3], "%s      | + |      %s", GRN, WHT);
            sprintf(arte[4], "%s     /_____\\     %s", GRN, WHT);
            sprintf(arte[5], "%s    (_______)    %s", GRN, WHT);
            break;
            
        case 2: // Eter (Ciano)
            sprintf(arte[0], "%s       |=|       %s", CYN, WHT);
            sprintf(arte[1], "%s      /   \\      %s", CYN, WHT);
            sprintf(arte[2], "%s     | ~~~ |     %s", CYN, WHT);
            sprintf(arte[3], "%s     | MANA|     %s", CYN, WHT);
            sprintf(arte[4], "%s     \\_____/     %s", CYN, WHT);
            sprintf(arte[5], "%s                 %s", CYN, WHT);
            break;
            
        case 3: // Ossos (Branco/Cinza)
            sprintf(arte[0], "%s      .---.      %s", WHT, WHT);
            sprintf(arte[1], "%s     /     \\     %s", WHT, WHT);
            sprintf(arte[2], "%s    |  X X  |    %s", WHT, WHT);
            sprintf(arte[3], "%s    |  OSSOS|    %s", WHT, WHT);
            sprintf(arte[4], "%s     \\_____/     %s", WHT, WHT);
            sprintf(arte[5], "%s                 %s", WHT, WHT);
            break;
            
        case 4: // Pedra de Afiar (Cinza)
            sprintf(arte[0], "%s      _____      %s", WHT, WHT);
            sprintf(arte[1], "%s     / ___ \\     %s", WHT, WHT);
            sprintf(arte[2], "%s    | |___| |    %s", WHT, WHT);
            sprintf(arte[3], "%s    | .---. |    %s", WHT, WHT);
            sprintf(arte[4], "%s    |/_____\\|    %s", WHT, WHT);
            sprintf(arte[5], "%s                 %s", WHT, WHT);
            break;
            
        case 5: // Barreira (Ciano)
            sprintf(arte[0], "%s      /~~~\\      %s", CYN, WHT);
            sprintf(arte[1], "%s     /  |  \\     %s", CYN, WHT);
            sprintf(arte[2], "%s    (  <*>  )    %s", CYN, WHT);
            sprintf(arte[3], "%s     \\  |  /     %s", CYN, WHT);
            sprintf(arte[4], "%s      \\___/      %s", CYN, WHT);
            sprintf(arte[5], "%s                 %s", CYN, WHT);
            break;
            
        case 6: // Contrato (Amarelo)
            sprintf(arte[0], "%s     _______     %s", YEL, WHT);
            sprintf(arte[1], "%s    /      /|    %s", YEL, WHT);
            sprintf(arte[2], "%s   /______/ |    %s", YEL, WHT);
            sprintf(arte[3], "%s   | ~~~~ | |    %s", YEL, WHT);
            sprintf(arte[4], "%s   |      | /    %s", YEL, WHT);
            sprintf(arte[5], "%s   |______|/     %s", YEL, WHT);
            break;
            
        case 7: // Incenso (Magenta)
            sprintf(arte[0], "%s        )        %s", MAG, WHT);
            sprintf(arte[1], "%s       ( )       %s", MAG, WHT);
            sprintf(arte[2], "%s      (   )      %s", MAG, WHT);
            sprintf(arte[3], "%s      _| |_      %s", MAG, WHT);
            sprintf(arte[4], "%s     /_____\\     %s", MAG, WHT);
            sprintf(arte[5], "%s    (_______)    %s", MAG, WHT);
            break;
            
        case 8: // Fogo Grego (Vermelho)
            sprintf(arte[0], "%s        )        %s", RED, WHT);
            sprintf(arte[1], "%s       ( )       %s", RED, WHT);
            sprintf(arte[2], "%s      __|__      %s", RED, WHT);
            sprintf(arte[3], "%s     /  X  \\     %s", RED, WHT);
            sprintf(arte[4], "%s    (       )    %s", RED, WHT);
            sprintf(arte[5], "%s     \\_____/     %s", RED, WHT);
            break;
            
        case 9: // Cristal (Ciano Brilhante)
            sprintf(arte[0], "%s       /\\        %s", CYN, WHT);
            sprintf(arte[1], "%s      /  \\       %s", CYN, WHT);
            sprintf(arte[2], "%s     <    >      %s", CYN, WHT);
            sprintf(arte[3], "%s      \\  /       %s", CYN, WHT);
            sprintf(arte[4], "%s       \\/        %s", CYN, WHT);
            sprintf(arte[5], "%s                 %s", CYN, WHT);
            break;
            
        case 10: // Berrante (Amarelo/Vermelho)
            sprintf(arte[0], "%s      /|         %s", YEL, WHT);
            sprintf(arte[1], "%s     / |         %s", YEL, WHT);
            sprintf(arte[2], "%s    /  |___      %s", YEL, WHT);
            sprintf(arte[3], "%s   (   _   )     %s", YEL, WHT);
            sprintf(arte[4], "%s    \\_/ \\_/      %s", YEL, WHT);
            sprintf(arte[5], "%s                 %s", YEL, WHT);
            break;
    }
}

// Preenche 2 linhas com a arte do consumível pequena
void obter_ascii_consumivel(int id, char* l1, char* l2) {
    switch(id) {
        case 1: // Elixir
            strcpy(l1, " .o. "); 
            strcpy(l2, " (_) "); 
            break;
        case 2: // Eter
            strcpy(l1, " [~] "); 
            strcpy(l2, " [_] "); 
            break;
        case 3: // Ossos
            strcpy(l1, " .-. "); 
            strcpy(l2, " (X) "); 
            break;
        case 4: // Pedra
            strcpy(l1, "  /\\ "); 
            strcpy(l2, " /__\\"); 
            break;
        case 5: // Barreira
            strcpy(l1, " /~~\\"); 
            strcpy(l2, " \\__/ "); 
            break;
        case 6: // Contrato
            strcpy(l1, " |\"\"|"); 
            strcpy(l2, " |__|"); 
            break;
        case 7: // Incenso
            strcpy(l1, "  )  "); 
            strcpy(l2, " \\_/"); 
            break;
        case 8: // Fogo Grego
            strcpy(l1, "  )  "); 
            strcpy(l2, " (O) "); 
            break;
        case 9: // Cristal
            strcpy(l1, "  ^  "); 
            strcpy(l2, " < > "); 
            break;
        case 10: // Berrante
            strcpy(l1, " /<  "); 
            strcpy(l2, " \\_) "); 
            break;
        default: // Vazio
            strcpy(l1, " ... "); 
            strcpy(l2, " ... "); 
            break;
    }
}

void menu_consumiveis(Necromante* p) {
    // 1. Preparação dos Dados
    char n[3][50]; // Nomes
    char d[3][100]; // Descrições curtas
    char arte[3][6][60]; // Artes
    char* cores[3]; // Cores dos nomes

    for(int i=0; i<3; i++) {
        int id = p->inventario[i];
        
        // Pega Arte Grande
        obter_ascii_consumivel_grande(id, arte[i]);
        
        if (id != 0) {
            strcpy(n[i], get_nome_item(id));
            
            // Define descrição curta e cor baseada no ID
            switch(id) {
                case 1: strcpy(d[i], "Cura +25 HP"); cores[i] = GRN; break;
                case 2: strcpy(d[i], "+40 Mana"); cores[i] = CYN; break;
                case 3: strcpy(d[i], "+3 Corpos"); cores[i] = WHT; break;
                case 4: strcpy(d[i], "+2 ATK em todos"); cores[i] = WHT; break;
                case 5: strcpy(d[i], "Imune 1 turno"); cores[i] = CYN; break;
                case 6: strcpy(d[i], "Prox. Esp. 0 HP"); cores[i] = YEL; break;
                case 7: strcpy(d[i], "Remove Buffs Inimigo"); cores[i] = MAG; break;
                case 8: strcpy(d[i], "50 Dano Direto"); cores[i] = RED; break;
                case 9: strcpy(d[i], "Anula Instabilidade"); cores[i] = CYN; break;
                case 10: strcpy(d[i], "Revive ultimo morto"); cores[i] = YEL; break;
            }
        } else {
            strcpy(n[i], "[ ESPACO VAZIO ]");
            strcpy(d[i], "---");
            cores[i] = WHT;
        }
    }

    // Desenha o Menu sobre a interface (simulado com limparTela para overlay)
    
    // Atualiza relatório para fundo (opcional, já que vamos limpar a tela)
    char menu_texto[300];
    sprintf(menu_texto, "INVENTARIO ABERTO.");
    strncpy(p->relatorio, menu_texto, 299);
    
    limparTela();
    printf("\n\n\n");
    print_centralizado("==================================================================================", WHT);
    print_centralizado("CINTO DE UTILIDADES (CONSUMIVEIS)", YEL);
    print_centralizado("==================================================================================", WHT);
    printf("\n\n");

    // --- RENDERIZAÇÃO DAS ARTES (Lado a Lado) ---
    // Espaçamento calculado para 160 colunas (Margem 18, Entre 36)
    for(int k=0; k<6; k++) {
        printf("                  %s                                    %s                                    %s\n", 
            arte[0][k], arte[1][k], arte[2][k]);
    }

    printf("\n");

    // Nomes e Teclas
    char t1[50], t2[50], t3[50];
    sprintf(t1, "[1] %s", n[0]);
    sprintf(t2, "[2] %s", n[1]);
    sprintf(t3, "[3] %s", n[2]);

    imprimir_3col_centralizado(t1, t2, t3, CYN);
    imprimir_3col_centralizado("--------------------", "--------------------", "--------------------", WHT);
    imprimir_3col_centralizado(d[0], d[1], d[2], WHT);

    printf("\n\n");
    print_centralizado("==================================================================================", WHT);
    print_centralizado("[1-3] Usar Item | [ESC/0] Cancelar", YEL);

    // --- LÓGICA DE USO ---
    char ch = _getch();
    
    if (ch == 27 || ch == '0') {
        // Redesenha interface do jogo ao sair
        return; 
    }
    
    int slot = ch - '1'; 
    if (slot < 0 || slot > 2) return;
    
    if (p->inventario[slot] == 0) return;
    
    int item = p->inventario[slot];
    char msg[200];
    
    // EFEITOS
    switch(item) {
        case 1: p->hp += 25; if(p->hp > p->max_hp) p->hp = p->max_hp; sprintf(msg, "Elixir usado: +25 HP recuperado."); break;
        case 2: 
            p->mana += 40; 
            int max_m = obter_limite_mana(p);
            if(p->mana <= max_m + 40 && p->mana > max_m) { if(p->mana > max_m) p->mana = max_m; }
            sprintf(msg, "Eter usado: +40 Mana."); break;
        case 3: p->corpos += 3; sprintf(msg, "Saco de Ossos aberto: +3 Corpos."); break;
        case 4: { Minion* m = p->exercito_head; while(m) { m->ataque += 2; m = m->proximo; } sprintf(msg, "Armas afiadas! +2 ATK para todos."); } break;
        case 5: p->flag_item_barreira = 1; sprintf(msg, "Barreira ativa! Proximo ataque anulado."); break;
        case 6: p->flag_item_contrato = 1; sprintf(msg, "Contrato assinado. Proxima invocacao custa 0 HP."); break;
        case 7: 
            p->buff_profanacao = 0; gerar_novo_inimigo(p); 
            if(p->hp_inimigo_atual > p->hp_inimigo_max) p->hp_inimigo_atual = p->hp_inimigo_max;
            sprintf(msg, "Incenso queimado. Inimigo enfraquecido."); break;
        case 8: p->hp_inimigo_atual -= 50; sprintf(msg, "Fogo Grego! Inimigo sofreu 50 de dano."); break;
        case 9: p->flag_item_cristal = 1; sprintf(msg, "Cristal energizado. Instabilidade anulada."); break;
        case 10: 
            if(p->ultimo_tipo_morto > 0) {
                Minion* novo = criar_minion_especifico(p, p->total_minions+1, p->ultimo_tipo_morto, 0);
                novo->hp = novo->hp / 2; if(novo->hp < 1) novo->hp = 1;
                novo->proximo = p->exercito_head; p->exercito_head = novo; p->total_minions++;
                sprintf(msg, "Berrante soou! Aliado revivido.");
            } else sprintf(msg, "Berrante falhou (Ninguem morreu)."); break;
    }
    
    // Consome
    p->inventario[slot] = 0;
    
    // Som de uso
    Beep(800, 100);
    
    atualizar_relatorio(p, msg);
}

void executar_run(int carregar_save) {
    Necromante player;
    int sucesso_load = 0;

    // Tenta carregar se solicitado
    if (carregar_save) {
        sucesso_load = carregar_jogo(&player);
    }

    // Se não era pra carregar OU se o carregamento falhou (arquivo corrompido/versão antiga)
    if (!sucesso_load) {
        if (carregar_save) {
            // Feedback se falhou ao tentar carregar
            printf("\n Erro ao carregar save (Versao incompativel). Iniciando novo jogo...\n");
            Sleep(1000);
        }
        
        int classe = menu_selecao_classe();
        limparTela();
        iniciar_jogo(&player, classe);
    } else {
        // Sucesso
        limparTela();
        printf("\n Retomando a marcha de onde parou...\n");
        Sleep(1000);
    }

    int rodando = 1;
    while(rodando) {
        // --- GAME OVER ---
        if(player.hp <= 0) {
            registrar_morte(&player, player.nome_inimigo); // Registra no cemitério
            
            int opcao = tela_derrota(); 
            if(opcao == 1) {
                // Reinicia
                Minion* m = player.exercito_head;
                while(m) { Minion* t = m; m=m->proximo; free(t); }
                
                int nova_classe = menu_selecao_classe();
                limparTela();
                iniciar_jogo(&player, nova_classe); 
                continue;
            } else {
                break; 
            }
        }
        
        desenhar_interface(&player, TELA_COMANDO);
        char ch = _getch();
        
        // ... (MANTENHA AQUI OS SEUS CHEATS: P, C, B, E, I) ...
        // Vou resumir para não ficar gigante, mas copie seus cheats aqui
        if (ch == 'p' || ch == 'P') { cheat_pular_para_boss(&player); continue; }
        if (ch == 'c' || ch == 'C' || ch == -121 || ch == 135) { player.corpos += 1000; player.mana = 2000; continue; }
        if (ch == 'b' || ch == 'B') {
             // ... Cheat B de matar boss ...
             for(int i=0; i<5; i++) { player.esquadrao[i].hp = 0; player.esquadrao[i].atk = 0; player.esquadrao[i].vivo = 0; }
             player.esquadrao[0].vivo = 1; player.esquadrao[0].hp = 1; player.esquadrao[0].atk = 0; strcpy(player.esquadrao[0].nome, "CHEFE (MORTO)");
             player.qtd_inimigos_ativos = 1; player.hp_inimigo_atual = 1; player.atk_inimigo_atual = 0; player.flag_item_barreira = 1;
             if (!player.exercito_head) { Minion* cm = criar_minion_especifico(&player, 999, 1, 0); player.exercito_head = cm; player.total_minions++; }
             batalhar(&player); 
            if (player.estagio_marcha == 11) {
                rodando = 0; // Sai do loop
                break;       // Interrompe o switch/while
            }
            if (player.estagio_marcha == 12) {
                rodando = 0;
                break;
            }
             continue;
        }
        if (ch == 'e' || ch == 'E') { player.cheat_evento_ativo = 1; strcpy(player.relatorio, "[CHEAT] Evento forcado."); continue; }
        // --- CHEAT (I): INVENTARIO FULL (EQUIPAR TUDO) ---
        if (ch == 'i' || ch == 'I') {
            // Zera bônus de HP antigos para evitar bugs se usar o cheat 2x
            // (Opcional, mas seguro)
            
            for(int k=1; k<=10; k++) {
                // Sorteia opção 1, 2 ou 3 para cada chefe
                player.artefatos[k] = (rand() % 3) + 1; 
            }
            
            // APLICA EFEITOS IMEDIATOS (Status Passivos)
            // Artefato 6-1: Coroa de Ferro (+50 HP)
            if(player.artefatos[6] == 1) { 
                player.max_hp += 50; 
                player.hp += 50; 
            }
            
            // Artefato 3-1: Cálice Profano (+20 HP em Magos - aplicado na criação)
            // Artefato 1-3: Elmo (+20 HP em Zumbis - aplicado na criação)
            // (Os outros artefatos funcionam na hora da batalha ou invocação)

            strcpy(player.relatorio, "[CHEAT] Inventario de Artefatos preenchido aleatoriamente!");
            continue; // Força redesenhar a tela
        }
        // --- CHEAT (K): KIT DE SOBREVIVÊNCIA (+3 ITENS) ---
        if (ch == 'k' || ch == 'K') {
            int adicionados = 0;
            
            // Tenta adicionar 3 vezes
            for (int i = 0; i < 3; i++) {
                // Busca slot vazio
                int slot = -1;
                for (int j = 0; j < 3; j++) {
                    if (player.inventario[j] == 0) {
                        slot = j;
                        break;
                    }
                }
                
                // Se achou espaço, adiciona item
                if (slot != -1) {
                    // Gera ID de 1 a 10
                    int item_id = (rand() % 10) + 1;
                    player.inventario[slot] = item_id;
                    adicionados++;
                }
            }
            
            if (adicionados > 0)
                strcpy(player.relatorio, "[CHEAT] Kit de Sobrevivencia recebido (+3 Itens).");
            else
                strcpy(player.relatorio, "[CHEAT] Inventario cheio! Esvazie slots primeiro.");
                
            continue;
        }
        // --------------------------------------------------
        // --- CHEAT (L): LEGION (TODOS OS LACAIOS) ---
        if (ch == 'l' || ch == 'L') {
            // Loop por todos os IDs existentes (1 a 10 Comuns/Especiais + 11 a 20 Eventos)
            for(int id = 1; id <= 20; id++) {
                int extra = 0;
                
                // --- SIMULAÇÃO DE STATUS PARA UNIDADES VARIÁVEIS ---
                // Sombra (5): Precisa de inimigo. Simulamos um forte.
                if(id == 5) { 
                    player.hp_inimigo_atual = 500; 
                    player.atk_inimigo_atual = 100; 
                }
                // Come-Mana (7): Precisa de mana gasta. Simulamos 100.
                if(id == 7) extra = 100; 
                // Devorador (8): Precisa de corpos. Simulamos 10 corpos.
                if(id == 8) extra = 10; 

                // Cria o lacaio
                Minion* novo = criar_minion_especifico(&player, player.total_minions + 1, id, extra);
                
                // Adiciona na frente da lista
                novo->proximo = player.exercito_head;
                player.exercito_head = novo;
                player.total_minions++;
                
                // Desbloqueia na Enciclopédia (Página 3)
                if(id >= 11 && id <= 20) {
                    player.lacaios_descobertos[id] = 1;
                }
            }
            
            // Reseta os valores simulados da Sombra para não bugar o jogo real
            if(player.hp_inimigo_atual == 500) {
                player.hp_inimigo_atual = 0;
                player.atk_inimigo_atual = 0;
            }

            strcpy(player.relatorio, "[CHEAT] A LEGIAO CHEGOU! Todos os 20 tipos de lacaios foram invocados.");
            continue;
        }
        // --- CHEAT (S): SHOP UNLOCK (Desbloquear Loja) ---
        if (ch == 's' || ch == 'S') {
            // Carrega o perfil global do disco
            PerfilGlobal perfil_cheat;
            carregar_perfil(&perfil_cheat);
            
            // Desbloqueia todos os itens da loja (1 a 5)
            int desbloqueou_algo = 0;
            for(int i=1; i<=5; i++) {
                if (perfil_cheat.paginas_desbloqueadas[i] == 0) {
                    perfil_cheat.paginas_desbloqueadas[i] = 1;
                    desbloqueou_algo = 1;
                }
            }
            
            // Salva de volta no disco
            salvar_perfil(&perfil_cheat);
            
            // Feedback
            if (desbloqueou_algo)
                strcpy(player.relatorio, "[CHEAT] O Templo foi saqueado! Itens da Loja desbloqueados.");
            else
                strcpy(player.relatorio, "[CHEAT] Voce ja possui todos os itens da Loja.");
                
            continue;
        }
        // -----------------------------------------------------
                //--- ARTEFATOS (-) ---
        if (ch == '-') { Beep(700, 50); menu_ver_artefatos(&player); continue; }
                //--- CONSUMÍVEIS (+) ---
        if (ch == '+') { Beep(700, 50); menu_consumiveis(&player); continue; }
                // --- HABILIDADE DE CLASSE (*)
        if (ch == '*') {ativar_habilidade_classe(&player);continue;}
        
        int op = ch - '0'; 
        switch(op) {
            case 1: escavar(&player); Beep(150, 150);Beep(150, 150); break;
            case 2: menu_invocacao(&player); Beep(600, 50);Beep(600, 50); break;
            case 3: mostrar_lista(&player); Beep(100, 200);Beep(800, 30); break;
            case 4: 
                Beep(300, 150); 
                
                batalhar(&player);
                
                if (player.estagio_marcha == 12) {
                    rodando = 0;
                }
                // --------------------------------
                break;
            case 5: magia_vil(&player);Beep(100, 200); break;
            case 6: sacrificar(&player);Beep(100, 200); break;
            case 7: menu_ajuda_lacaios(&player);Beep(800, 30); break;
            case 8: menu_regras_jogo();Beep(800, 30); break;
            case 9:
            Beep(1000, 100);
                if (tela_confirmar_reinicio() == 1) {
                    deletar_save(); // Apaga save ao reiniciar
                    Minion* m = player.exercito_head;
                    while(m) { Minion* temp = m; m = m->proximo; free(temp); }
                    iniciar_jogo(&player, player.classe);
                }
                break;
            case 0:
                Beep(300, 100);
                salvar_jogo(&player); // Salva ao sair
                rodando = 0; 
                break;
            default: break;
        }
    }
    
    // Limpeza
    Minion* m = player.exercito_head;
    while(m) { Minion* tmp = m; m = m->proximo; free(tmp); }
}

int menu_selecao_classe() {
    while(1) {
        limparTela();
        printf("\n\n");
        printf("%s======================================================================================================================================================\n", WHT);
        printf("                                                        ESCOLHA SUA LINHAGEM NEGRA                                                                    \n");
        printf("======================================================================================================================================================\n\n");

        // --- TÍTULOS ---
        printf(" %s%-52s %s%-52s %s%-52s%s\n", 
            YEL, "              [1] O COVEIRO", 
            RED, "           [2] O MAGO DE SANGUE", 
            CYN, "           [3] O ESPIRITUALISTA", WHT);

        // --- ARTES ASCII (ALINHAMENTO MATEMÁTICO 52 COLUNAS) ---
        
        // Linha 1
        printf(" %s                      _______                     %s%s                       ,   ,                        %s%s                       .---.                        %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 2
        printf(" %s                     /       \\                    %s%s                      / \\ / \\                       %s%s                      /     \\                       %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 3
        printf(" %s                    |   RIP   |                   %s%s                     (   V   )                      %s%s                     (  O O  )                      %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 4
        printf(" %s                    |         |                   %s%s                      \\  |  /                       %s%s                      \\  ^  /                       %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 5
        printf(" %s                    |___| |___|                   %s%s                       \\ | /                        %s%s                       `---'                        %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 6
        printf(" %s                   ^^^^^| |^^^^^                  %s%s                        \\|/                         %s%s                      / /|\\ \\                       %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        // Linha 7
        printf(" %s                        |_|                       %s%s                         V                          %s%s                     `'     `'                      %s\n", 
            YEL, WHT, RED, WHT, CYN, WHT);

        printf("\n");
        
        // --- DESCRIÇÕES ---
        
        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "   Especialista em Recursos e Enxames", 
            WHT, "   Tanque Vivo e Ritualista de Sangue", 
            WHT, "   Mago Arcano de Poder Explosivo", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "   ----------------------------------", 
            WHT, "   ----------------------------------", 
            WHT, "   ----------------------------------", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, " [+] ESCAVACAO MESTRE:", 
            WHT, " [+] VITALIDADE SUPREMA:", 
            WHT, " [+] MENTE EXPANDIDA:", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "     Gera +1 Corpo extra sempre.", 
            WHT, "     Comeca com 150 HP (Max).", 
            WHT, "     Limite de Mana natural e 250.", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "     Pode escavar 7x por turno.", 
            WHT, "     Magia Vil custa apenas 2 HP.", 
            WHT, "     Invocacoes Especiais custam 0 HP.", WHT);

        printf("\n");

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, " [-] CORPO FRAGIL:", 
            WHT, " [-] SANGUE CARO:", 
            WHT, " [-] CORPO MORTO:", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "     Inicia com 80 HP / 40 Mana.", 
            WHT, "     Especiais custam 15 HP.", 
            WHT, "     Inicia com 50 HP.", WHT);

        printf(" %s%-52s %s%-52s %s%-52s\n", 
            WHT, "     Ideal p/ Devorador e Zumbis.", 
            WHT, "     Inimigo ganha menos buff profano.", 
            WHT, "     Ganha apenas +25 HP por fase.", WHT);

        printf("\n\n");
        printf("%s======================================================================================================================================================\n", WHT);
        printf("                                                    DIGITE SUA ESCOLHA %s[1]%s, %s[2]%s OU %s[3]%s                                                                \n", YEL, WHT, RED, WHT, CYN, WHT);
        printf("======================================================================================================================================================%s\n", RST);
        
        char ch = _getch();
        if(ch >= '1' && ch <= '3') {
            Beep(700, 150);
            return ch - '0';
        }
    }
}

void ativar_habilidade_classe(Necromante* p) {
    char msg[200];
    Beep(300, 100);
    // Validação
    if (p->habilidade_usada == 1) {
        atualizar_relatorio(p, "Habilidade em recarga! (Derrote este inimigo para usar novamente)");
        return;
    }

    // Verifica Upgrade Global
    PerfilGlobal perfil;
    carregar_perfil(&perfil);
    int tem_upgrade = perfil.upgrades_classe[p->classe];

    // Efeitos por Classe
    if (p->classe == CLASSE_COVEIRO) {
        // --- EXUMAÇÃO FRENETICA ---
        if (tem_upgrade) {
            p->corpos += 15; // Era 10
            p->hp += 20;     // NOVO: Cura
            sprintf(msg, "[MESTRIA] Exumacao Suprema! +15 Corpos, Solo Renovado e +20 HP.");
        } else {
            p->corpos += 10;
            sprintf(msg, "[ULTIMATE] Exumacao Frenetica! +10 Corpos e Solo Renovado.");
        }
        p->escavacoes_turno = 0; 
    }
    else if (p->classe == CLASSE_MAGO_SANGUE) {
        // --- TRANSFUSÃO SOMBRIA ---
        if (tem_upgrade) {
            int roubo = 60; // Era 30
            p->hp_inimigo_atual -= roubo;
            p->hp += roubo;
            if(p->hp > p->max_hp) p->hp = p->max_hp;
            // NOVO: Ganha Mana
            p->mana += 30; if(p->mana > MAX_MANA) p->mana = MAX_MANA;
            sprintf(msg, "[MESTRIA] Transfusao Vampirica! Roubou 60 Vida e ganhou 30 Mana.");
        } else {
            p->hp_inimigo_atual -= 30;
            p->hp += 30;
            if(p->hp > p->max_hp) p->hp = p->max_hp;
            sprintf(msg, "[ULTIMATE] Transfusao Sombria! Roubou 30 Vida do inimigo.");
        }
    }
    else if (p->classe == CLASSE_ESPIRITUALISTA) {
        // --- FORMA ETÉREA ---
        p->flag_forma_eterea = 1; 
        if (tem_upgrade) {
            // NOVO: Enche a Mana
            p->mana = 250; // Max do Espiritualista
            sprintf(msg, "[MESTRIA] Forma Astral! Imunidade + Mana Maxima restaurada.");
        } else {
            sprintf(msg, "[ULTIMATE] Forma Eterea ativa! Imunidade na proxima batalha.");
        }
    }
    else {
        return;
    }

    // Consumo
    p->habilidade_usada = 1; 
    
    atualizar_relatorio(p, msg);
}

// --- TELA DE DERROTA (GAME OVER) ---
int tela_derrota() {
    limparTela();
    
    // --- TRAVA DE SEGURANÇA ---
    while(kbhit()) getch(); 
    // -------------------------

    printf("\n\n\n\n");
    
    // ASCII PALADINO / LUZ (CIANO E BRANCO)
    // Representa a ordem e a purificação vencendo o caos.
    printf("%s", CYN);
    printf("                                                                        |\n");
    printf("                                                                    \\   |   /\n");
    printf("                                                                     \\  |  /\n");
    printf("                                                                      \\ | /\n");
    printf("                                                                  '-.  \\|/  .-'\n");
    printf("                                                                      ( | )\n");
    printf("                                                                       \\|/\n");
    printf("%s", WHT);
    printf("                                                                      __|__\n");
    printf("                                                                     /     \\\n");
    printf("                                                                    |   +   |\n");
    printf("                                                                    |       |\n");
    printf("                                                                    |   |   |\n");
    printf("                                                                    |   |   |\n");
    printf("                                                                     \\  |  /\n");
    printf("                                                                      \\ | /\n");
    printf("                                                                       \\|/\n");
    printf("                                                                        V\n");
    printf("%s", RST);
    
    printf("\n");
    printf("                                          ==========================================================================\n");
    printf("                                                A LUZ PURIFICOU AS SOMBRAS. SUA CRUZADA TERMINOU.\n");
    printf("                                          ==========================================================================\n\n");
    
    printf("                                                    [ 1 ] TENTAR NOVAMENTE (Reiniciar)\n");
    printf("                                                    [ 2 ] DESISTIR (Sair para o Menu)\n\n");
    
    printf("                                                          Escolha seu destino: ");
    
    // --- DELAY DRAMÁTICO ---
    Sleep(1500); 
    while(kbhit()) getch(); 
    // -----------------------

    while(1) {
        char ch = _getch();
        if(ch == '1') return 1; // Reiniciar
        if(ch == '2') return 0; // Sair
    }
}

// --- TELA DE VITÓRIA (FINAL) ---
void tela_vitoria_final() {
    limparTela();
    printf("\n\n\n\n");
    
    // ASCII CHAMAS VIL (VERDE)
    printf("%s", GRN);
    printf("                                                          (  .      )\n");
    printf("                                                       )           (              )\n");
    printf("                                                             .  '   .   '  .  '  .\n");
    printf("                                                    (    , )       (.   )  (   ,    )\n");
    printf("                                                     .' ) ( . )    ,  (  ,     )   ( .\n");
    printf("                                                  ). , ( .   (  ) ( , ')  .' (  ,    )\n");
    printf("                                                  (_,) . ), ) _) _,')  (, ) '. )  ,. (' )\n");
    printf("                                                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    printf("                                                   |                             |\n");
    printf("                                                   |       %sO REI ESTA%s           |\n", WHT, GRN);
    printf("                                                   |            %sMORTO%s            |\n", RED, GRN);
    printf("                                                   |_____________________________|\n");
    printf("%s", RST);

    printf("\n\n");
    printf("                                      ===============================================================\n");
    printf("                                             A CIDADELA ARDE. O TRONO AGORA E SEU.\n");
    printf("                                      ===============================================================\n\n");
    
    printf("                                           Voce conquistou todos os 10 locais e derrotou o Rei.\n");
    printf("                                             O Reino dos Vivos agora pertence aos Mortos.\n\n");
    
    printf("                                                    [ Pressione qualquer tecla ]\n");
    _getch();
}

// --- TELA DE CONFIRMAÇÃO DE REINÍCIO ---
int tela_confirmar_reinicio() {
    limparTela();
    printf("\n\n\n\n\n\n\n");
    
    // ATENÇÃO (Centralizado)
    printf("%s", RED);
    printf("                                                               /!\\ ATENCAO /!\\ \n\n");
    printf("%s", RST);
    
    printf("                                          ==============================================================\n");
    printf("                                                       DESEJA REALMENTE REINICIAR?\n");
    printf("                                          ==============================================================\n\n");
    
    printf("                                              Todo o seu progresso atual sera perdido para sempre.\n\n");
    
    printf("                                                         [ 1 ] SIM, REINICIAR\n");
    printf("                                                         [ 2 ] NAO, VOLTAR AO JOGO\n\n");
    
    printf("                                                            Sua escolha: ");
    
    while(1) {
        char ch = _getch();
        if(ch == '1'){Beep(1000, 100); return 1;} // Sim
        if(ch == '2'){Beep(1000, 100);return 0;} // Não
    }
}

void salvar_perfil(PerfilGlobal* pg) {
    FILE *f = fopen("perfil.dat", "wb");
    if (f) {
        fwrite(pg, sizeof(PerfilGlobal), 1, f);
        fclose(f);
    }
}

void carregar_perfil(PerfilGlobal* pg) {
    FILE *f = fopen("perfil.dat", "rb");
    if (f) {
        fread(pg, sizeof(PerfilGlobal), 1, f);
        fclose(f);
    } else {
        pg->moedas_caronte = 0;
        for(int i=0; i<12; i++) pg->paginas_desbloqueadas[i] = 0;
        // Zera upgrades
        for(int i=0; i<4; i++) pg->upgrades_classe[i] = 0;
        salvar_perfil(pg);
    }
        for(int i=0; i<4; i++) pg->upgrades_classe[i] = 0;
        for(int i=0; i<5; i++) pg->evolucoes_lacaios[i] = 0; // Zerar novo
        salvar_perfil(pg);
}

void obter_info_upgrade(int classe_id, char* nome, char* desc, char arte[6][60]) {
    for(int i=0; i<6; i++) strcpy(arte[i], "                    "); 

    if (classe_id == CLASSE_COVEIRO) {
        strcpy(nome, "PA DE OURO (Coveiro)");
        strcpy(desc, "Exumacao gera +15 Corpos e CURA 20 HP.");
        // ASCII Pá Dourada (Amarelo)
        sprintf(arte[0], "%s      ___        %s", YEL, WHT);
        sprintf(arte[1], "%s     |   |       %s", YEL, WHT);
        sprintf(arte[2], "%s     |___|       %s", YEL, WHT);
        sprintf(arte[3], "%s      | |        %s", YEL, WHT);
        sprintf(arte[4], "%s     _| |_       %s", YEL, WHT);
        sprintf(arte[5], "%s    (___ _)      %s", YEL, WHT);
    }
    else if (classe_id == CLASSE_MAGO_SANGUE) {
        strcpy(nome, "CALICE ETERNO (Mago)");
        strcpy(desc, "Transfusao rouba 60 HP e gera 30 MANA.");
        // ASCII Cálice (Vermelho)
        sprintf(arte[0], "%s    \\~~~~~/      %s", RED, WHT);
        sprintf(arte[1], "%s     \\___/       %s", RED, WHT);
        sprintf(arte[2], "%s      | |        %s", RED, WHT);
        sprintf(arte[3], "%s      | |        %s", RED, WHT);
        sprintf(arte[4], "%s    __|_|__      %s", RED, WHT);
        sprintf(arte[5], "%s   (_______)     %s", RED, WHT);
    }
    else if (classe_id == CLASSE_ESPIRITUALISTA) {
        strcpy(nome, "MANTO DO VAZIO (Espir.)");
        strcpy(desc, "Forma Eterea tambem ENCHE sua Mana (250).");
        // ASCII Manto (Ciano)
        sprintf(arte[0], "%s      / \\        %s", CYN, WHT);
        sprintf(arte[1], "%s     /   \\       %s", CYN, WHT);
        sprintf(arte[2], "%s    /  O  \\      %s", CYN, WHT);
        sprintf(arte[3], "%s   /_______\\     %s", CYN, WHT);
        sprintf(arte[4], "%s   |   |   |     %s", CYN, WHT);
        sprintf(arte[5], "%s   |___|___|     %s", CYN, WHT);
    }
}

void obter_info_evolucao(int tipo, char* nome, char* desc, char arte[6][60]) {
    for(int i=0; i<6; i++) strcpy(arte[i], "                    "); 

    switch(tipo) {
        case 1: // ESQUELETO
            strcpy(nome, "A LEGIAO ETERNA");
            strcpy(desc, "Esqueletos tem 50% de chance de gerar +1 Corpo ao morrer.");
            sprintf(arte[0], "%s      .---.      %s", WHT, WHT);
            sprintf(arte[1], "%s     /  _  \\     %s", WHT, WHT);
            sprintf(arte[2], "%s    | (X_X) |    %s", WHT, WHT);
            sprintf(arte[3], "%s     \\  ^  /     %s", WHT, WHT);
            sprintf(arte[4], "%s      `---'      %s", WHT, WHT);
            sprintf(arte[5], "%s       (8)       %s", GRN, WHT); // Símbolo infinito
            break;
            
        case 2: // ZUMBI
            strcpy(nome, "CARNE DE FERRO");
            strcpy(desc, "Zumbis reduzem todo dano recebido em -5 (Imunes a dano baixo).");
            sprintf(arte[0], "%s    _______      %s", CYN, WHT);
            sprintf(arte[1], "%s   | .---. |     %s", CYN, WHT);
            sprintf(arte[2], "%s   | |___| |     %s", CYN, WHT);
            sprintf(arte[3], "%s   |_[_|_]_|     %s", CYN, WHT);
            sprintf(arte[4], "%s    \\_____/      %s", CYN, WHT);
            sprintf(arte[5], "%s   [        ]    %s", CYN, WHT);
            break;
            
        case 3: // CARNICAL
            strcpy(nome, "FRENESI SANGRENTO");
            strcpy(desc, "Carnicais tem Roubo de Vida (Curam 50% do dano causado).");
            sprintf(arte[0], "%s      \\ /        %s", RED, WHT);
            sprintf(arte[1], "%s     --X--       %s", RED, WHT);
            sprintf(arte[2], "%s      / \\        %s", RED, WHT);
            sprintf(arte[3], "%s     (   )       %s", RED, WHT);
            sprintf(arte[4], "%s      `v'        %s", RED, WHT);
            sprintf(arte[5], "%s    (   )     %s", RED, WHT);
            break;
            
        case 4: // ABOMINACAO
            strcpy(nome, "O TITA DE CERCO");
            strcpy(desc, "Ataque da Abominacao atinge TODOS os inimigos (50% dano em area).");
            sprintf(arte[0], "%s      ___        %s", YEL, WHT);
            sprintf(arte[1], "%s     /___\\       %s", YEL, WHT);
            sprintf(arte[2], "%s    ( O O )      %s", YEL, WHT);
            sprintf(arte[3], "%s   ==\\___/==     %s", YEL, WHT);
            sprintf(arte[4], "%s     // \\\\       %s", YEL, WHT);
            sprintf(arte[5], "%s    _|| ||_      %s", YEL, WHT);
            break;
    }
}

// --- HELPER: IMPRIME 2 COLUNAS CENTRALIZADAS (LARGURA 80 CADA) ---
void imprimir_2col_centralizado(const char* t1, const char* t2, const char* cor1, const char* cor2) {
    int w = 80; // Metade da tela (160 / 2)
    int l1 = strlen(t1);
    int l2 = strlen(t2);
    
    // Margem esquerda para centralizar no bloco de 80
    int p1 = (w - l1) / 2; if(p1 < 0) p1 = 0;
    int p2 = (w - l2) / 2; if(p2 < 0) p2 = 0;

    // Imprime Coluna 1
    printf("%*s%s%s%s", p1, "", cor1, t1, WHT);
    
    // Preenche o resto da Coluna 1 para chegar no caractere 80
    int fill1 = w - p1 - l1;
    printf("%*s", fill1, "");

    // Imprime Coluna 2
    printf("%*s%s%s%s\n", p2, "", cor2, t2, WHT);
}

// --- FUNÇÃO MENU LOJA ---
void menu_loja() {
    PerfilGlobal perfil;
    carregar_perfil(&perfil);

    int pagina = 1; 

    while(1) {
        limparTela();
        printf("\n");
        printf("%s======================================================================================================================================================\n", WHT);
        
        char titulo[100];
        if (pagina == 1) sprintf(titulo, "TEMPLO DE THANECROTHOS (ARTEFATOS SECRETOS - PAG 1/3)");
        else if (pagina == 2) sprintf(titulo, "TEMPLO DE THANECROTHOS (MAESTRIA DE CLASSE - PAG 2/3)");
        else sprintf(titulo, "TEMPLO DE THANECROTHOS (EVOLUCAO DE LACAIOS - PAG 3/3)");
        
        print_centralizado(titulo, MAG);
        printf("======================================================================================================================================================%s\n", WHT);

        char moeda_str[100];
        sprintf(moeda_str, "MOEDAS DE CARONTE: %d $", perfil.moedas_caronte);
        printf("\n");
        print_centralizado(moeda_str, YEL);
        printf("\n");

        // =================================================================================
        // PÁGINA 1: ARTEFATOS (1-5)
        // =================================================================================
        if (pagina == 1) {
            char n[6][50], d[6][100], s[6][50]; 
            char arte[6][6][60]; 
            char* cor_s[6]; 

            for(int i=1; i<=5; i++) {
                get_info_artefato(i, 4, n[i], d[i]);
                obter_ascii_artefato(i, 4, arte[i]);
                if (perfil.paginas_desbloqueadas[i] == 1) {
                    sprintf(s[i], "[ DESBLOQUEADO ]"); cor_s[i] = GRN;
                } else {
                    sprintf(s[i], "[ PRECO: %d $ ]", PRECO_PAGINA);
                    cor_s[i] = (perfil.moedas_caronte >= PRECO_PAGINA) ? YEL : RED;
                }
            }

            // LINHA 1 (3 ITENS)
            for(int k=0; k<6; k++) printf("                  %s                                    %s                                    %s\n", arte[1][k], arte[2][k], arte[3][k]);
            printf("\n");
            imprimir_3col_centralizado(n[1], n[2], n[3], CYN);
            imprimir_3col_centralizado(d[1], d[2], d[3], WHT);
            
            // Status Manual
            int p1=(53-strlen(s[1]))/2; int p2=(53-strlen(s[2]))/2; int p3=(53-strlen(s[3]))/2;
            printf("%*s%s%s%s", p1,"",cor_s[1],s[1],WHT); printf("%*s", 53-p1-strlen(s[1]),"");
            printf("%*s%s%s%s", p2,"",cor_s[2],s[2],WHT); printf("%*s", 53-p2-strlen(s[2]),"");
            printf("%*s%s%s%s\n\n", p3,"",cor_s[3],s[3],WHT);
            
            print_centralizado("----------------------------------------------------------------------------------", WHT);
            printf("\n");
            
            // LINHA 2 (2 ITENS)
            for(int k=0; k<6; k++) printf("                  %s                                    %s\n", arte[4][k], arte[5][k]);
            printf("\n");
            int pn4=(53-strlen(n[4]))/2; int pn5=(53-strlen(n[5]))/2;
            printf("%*s%s%s%s%*s%*s%s%s%s\n", pn4,"",CYN,n[4],WHT, 53-pn4-strlen(n[4]),"", pn5,"",CYN,n[5],WHT);
            int pd4=(53-strlen(d[4]))/2; int pd5=(53-strlen(d[5]))/2;
            printf("%*s%s%s%s%*s%*s%s%s%s\n", pd4,"",WHT,d[4],WHT, 53-pd4-strlen(d[4]),"", pd5,"",WHT,d[5],WHT);
            int ps4=(53-strlen(s[4]))/2; int ps5=(53-strlen(s[5]))/2;
            printf("%*s%s%s%s%*s%*s%s%s%s\n", ps4,"",cor_s[4],s[4],WHT, 53-ps4-strlen(s[4]),"", ps5,"",cor_s[5],s[5],WHT);
        }

        // =================================================================================
        // PÁGINA 2: MESTRIA DE CLASSE
        // =================================================================================
        else if (pagina == 2) {
            char n[4][50], d[4][100], s[4][50]; 
            char arte[4][6][60]; 
            char* cor_s[4];

            for(int i=1; i<=3; i++) {
                obter_info_upgrade(i, n[i], d[i], arte[i]);
                if (perfil.upgrades_classe[i] == 1) {
                    sprintf(s[i], "[ ADQUIRIDO ]"); cor_s[i] = GRN;
                } else {
                    sprintf(s[i], "[ CUSTO: %d $ ]", PRECO_UPGRADE);
                    cor_s[i] = (perfil.moedas_caronte >= PRECO_UPGRADE) ? YEL : RED;
                }
            }

            for(int k=0; k<6; k++) printf("                  %s                                    %s                                    %s\n", arte[1][k], arte[2][k], arte[3][k]);
            printf("\n");
            imprimir_3col_centralizado(n[1], n[2], n[3], MAG);
            imprimir_3col_centralizado(d[1], d[2], d[3], WHT);

            int p1=(53-strlen(s[1]))/2; int p2=(53-strlen(s[2]))/2; int p3=(53-strlen(s[3]))/2;
            printf("%*s%s%s%s", p1,"",cor_s[1],s[1],WHT); printf("%*s", 53-p1-strlen(s[1]),"");
            printf("%*s%s%s%s", p2,"",cor_s[2],s[2],WHT); printf("%*s", 53-p2-strlen(s[2]),"");
            printf("%*s%s%s%s\n", p3,"",cor_s[3],s[3],WHT);
            
            printf("\n\n\n\n\n\n"); 
        }

        // =================================================================================
        // PÁGINA 3: EVOLUÇÃO DE LACAIOS
        // =================================================================================
        else if (pagina == 3) {
            char n[5][50], d[5][100], s[5][50]; 
            char arte[5][6][60]; 
            char* cor_s[5];

            for(int i=1; i<=4; i++) {
                obter_info_evolucao(i, n[i], d[i], arte[i]);
                if (perfil.evolucoes_lacaios[i] == 1) {
                    sprintf(s[i], "[ EVOLUIDO ]"); cor_s[i] = GRN;
                } else {
                    sprintf(s[i], "[ CUSTO: %d $ ]", PRECO_EVOLUCAO);
                    cor_s[i] = (perfil.moedas_caronte >= PRECO_EVOLUCAO) ? YEL : RED;
                }
            }

            // LINHA 1 (Esqueleto e Zumbi)
            for(int k=0; k<6; k++) printf("                              %s                                        %s\n", arte[1][k], arte[2][k]);
            printf("\n");
            
            imprimir_2col_centralizado(n[1], n[2], CYN, CYN);
            imprimir_2col_centralizado(d[1], d[2], WHT, WHT);
            imprimir_2col_centralizado(s[1], s[2], cor_s[1], cor_s[2]);

            print_centralizado("----------------------------------------------------------------------------------", WHT);
            printf("\n");

            // LINHA 2 (Carniçal e Abominação)
            for(int k=0; k<6; k++) printf("                              %s                                        %s\n", arte[3][k], arte[4][k]);
            printf("\n");
            
            imprimir_2col_centralizado(n[3], n[4], CYN, CYN);
            imprimir_2col_centralizado(d[3], d[4], WHT, WHT);
            imprimir_2col_centralizado(s[3], s[4], cor_s[3], cor_s[4]);
            
            printf("\n");
        }

        printf("\n");
        printf("%s======================================================================================================================================================\n", WHT);
        
        if (pagina == 1) print_centralizado("[1-5] Comprar | NAVEGACAO: Use [<- / ->] para Rolar | [ESC/0] Sair", YEL);
        else if (pagina == 2) print_centralizado("[1-3] Comprar | NAVEGACAO: Use [<- / ->] para Rolar | [ESC/0] Sair", YEL);
        else print_centralizado("[1-4] Comprar Evolucao | NAVEGACAO: Use [<- / ->] para Rolar | [ESC/0] Sair", YEL);
        
        printf("======================================================================================================================================================%s\n", RST);

        // =================================================================================
        // LÓGICA DE INPUT E COMPRA (CORRIGIDA)
        // =================================================================================
        
        char ch = _getch();

        // 1. DETECÇÃO DE SETAS (Prioridade)
        if (ch == -32 || ch == 224) {
            char arrow = _getch(); // Consome o segundo código IMEDIATAMENTE
            
            if (arrow == 77) { // Direita
                if(pagina < 3) {
                    pagina++;
                    Beep(700, 30);
                }
            }
            if (arrow == 75) { // Esquerda
                if(pagina > 1) {
                    pagina--;
                    Beep(700, 30);
                }
            }
            continue; // Redesenha a tela
        }

        // 2. DETECÇÃO DE SAÍDA
        if (ch == 27 || ch == '0') {
            Beep(300, 50);
            break; 
        }

        // 3. LÓGICA DE COMPRA
        // Compra Pág 1
        if (pagina == 1 && ch >= '1' && ch <= '5') {
            int id = ch - '0';
            if (perfil.paginas_desbloqueadas[id] == 0) {
                if (perfil.moedas_caronte >= PRECO_PAGINA) {
                    perfil.moedas_caronte -= PRECO_PAGINA;
                    Beep(1000, 100); Beep(1500, 150);
                    perfil.paginas_desbloqueadas[id] = 1;
                    salvar_perfil(&perfil);
                    print_centralizado("COMPRADO!", GRN); Sleep(800);
                } else { print_centralizado("ALMAS INSUFICIENTES.", RED); Sleep(500); Beep(200, 300); }
            }
        }

        // Compra Pág 2
        else if (pagina == 2 && ch >= '1' && ch <= '3') {
            int id = ch - '0';
            if (perfil.upgrades_classe[id] == 0) {
                if (perfil.moedas_caronte >= PRECO_UPGRADE) {
                    perfil.moedas_caronte -= PRECO_UPGRADE;
                    Beep(1000, 100); Beep(1500, 150);
                    perfil.upgrades_classe[id] = 1;
                    salvar_perfil(&perfil);
                    print_centralizado("COMPRADO!", GRN); Sleep(800);
                } else { print_centralizado("ALMAS INSUFICIENTES.", RED); Sleep(500); Beep(200, 300); }
            }
        }

        // Compra Pág 3
        else if (pagina == 3 && ch >= '1' && ch <= '4') {
            int id = ch - '0';
            if (perfil.evolucoes_lacaios[id] == 0) {
                if (perfil.moedas_caronte >= PRECO_EVOLUCAO) {
                    perfil.moedas_caronte -= PRECO_EVOLUCAO;
                    Beep(1000, 100); Beep(1500, 150);
                    perfil.evolucoes_lacaios[id] = 1;
                    salvar_perfil(&perfil);
                    print_centralizado("COMPRADO!", GRN); Sleep(800);
                } else { print_centralizado("ALMAS INSUFICIENTES.", RED); Sleep(500); Beep(200, 300); }
            }
        }
    }
    limparTela();
}

int main() {
    srand(time(NULL));
    configurarSistema(); // Garante tela 160x45 e cores

    // --- LIGAR O SOM ---
    tocar_musica_fundo();
    
    while(1) {
        limparTela();
        
        // Verifica save
        FILE *f = fopen("savegame.dat", "rb");
        int tem_save = 0;
        if (f) {
            tem_save = 1;
            fclose(f);
        }

        printf("\n\n\n");

        // Espaçamento para centralizar (Tela 160 - Arte ~75) / 2 = ~42 espaços
        char* pad = "                                          "; 

        // --- GALERIA DE LACAIOS (LINHA 1) ---
        // Esqueleto | Zumbi | Carniçal | Abominação | Sombra
        
        printf("%s%s   .---.      %s  _______    %s     \\ /       %s   _____     %s    . .    \n", pad, WHT, CYN, RED, YEL, MAG);
        printf("%s%s  /  _  \\     %s /  o o  \\   %s    --X--      %s  /  O  \\    %s    ~ ~    \n", pad, WHT, CYN, RED, YEL, MAG);
        printf("%s%s | (X_X) |    %s |   _   |    %s    / \\       %s  (  _  )    %s   ( ? )   \n", pad, WHT, CYN, RED, YEL, MAG);
        printf("%s%s  \\  ^  /     %s |  (_)  |    %s    `v'       %s  / / \\ \\    %s    ) (    \n", pad, WHT, CYN, RED, YEL, MAG);
        printf("%s%s   `---'      %s  \\_____/     %s              %s (_/   \\_)   %s   `---'   \n", pad, WHT, CYN, RED, YEL, MAG);
        
        printf("\n"); // Espaço entre as linhas de lacaios

        // --- GALERIA DE LACAIOS (LINHA 2) ---
        // Legião | Come-Mana | Devorador | Lich | Sifão
        
        printf("%s%s  (o) (o)     %s    /\\        %s   \\|||/      %s    /|\\       %s    {o}    \n", pad, RED, CYN, GRN, MAG, RED);
        printf("%s%s (o) (o) (o)  %s   <  >       %s   (o o)      %s   ( @ )      %s   /   \\   \n", pad, RED, CYN, GRN, MAG, RED);
        printf("%s%s  (o) (o)     %s    \\/        %s    (O)       %s    | |       %s   | @ |   \n", pad, RED, CYN, GRN, MAG, RED);
        printf("%s%s (o) (o) (o)  %s    ||        %s   /   \\      %s   /   \\      %s   \\___/   \n", pad, RED, CYN, GRN, MAG, RED);
        printf("%s%s   \\ | /      %s   (++)       %s  (     )     %s  [MMMMM]     %s    V V    \n", pad, RED, CYN, GRN, MAG, RED);

        printf("\n\n");
        
        print_centralizado("===========================================================================================================", WHT);
        printf("\n");
        
        // --- TÍTULO ---
        print_centralizado("N E C R O N Q U I S T A", MAG); 
        printf("\n");
        print_centralizado("A  Q U E D A  D E  V I T A L H I A", RED); 
        
        printf("\n");
        print_centralizado("===========================================================================================================", WHT);
        printf("\n\n");

        // --- MENU OPÇÕES ---
        if (tem_save) {
            print_centralizado("[ 1 ]  CONTINUAR JOGO", GRN);
            printf("\n");
            print_centralizado("[ 2 ]  NOVO JOGO (Apaga Save)", RED);
        } else {
            print_centralizado("[ 1 ]  INICIAR A CONQUISTA (Novo Jogo)", GRN);
            printf("\n");
        }

        printf("\n");
        print_centralizado("[ 3 ]  CEMITERIO", CYN);
        printf("\n");
        print_centralizado("[ 4 ]  TEMPLO DE THANECROTHOS", YEL);
        printf("\n");
        print_centralizado("[ 0 ]  SAIR", WHT);
        
        printf("\n\n");
        print_centralizado("Escolha o seu destino:", WHT);
        printf("                                                                                "); 

        char ch = _getch();
        
        if (ch == '1') {
            if (tem_save) {
                Beep(600, 100);
                executar_run(1); // Carregar
            } else {
                Beep(600, 100);
                executar_run(0); // Novo
            }
        }
        else if (ch == '2') {
            if (tem_save) {
                Beep(500, 100);
                printf("\n\n");
                print_centralizado("Tem certeza? O progresso sera perdido (s/n): ", RED);
                char conf = _getch();
                if (conf == 's' || conf == 'S') {
                    Beep(800, 100);
                    deletar_save();
                    executar_run(0); 
                }else{
                    Beep(300, 50);
                }
            }
        }
        else if (ch == '3') {
            Beep(600, 100);
            menu_cemiterio();
        }
        else if (ch == '4') {
            Beep(600, 100);
            menu_loja();
        }
        else if (ch == '0' || ch == '4') { 
            Beep(200, 300);
            limparTela();
            printf("\n\n\n");
            print_centralizado("Vitalhia respira aliviada... por enquanto.", WHT);
            Sleep(1500);
            break; 
        }
    }
    
    return 0;
}
