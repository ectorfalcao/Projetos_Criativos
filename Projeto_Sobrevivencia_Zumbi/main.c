#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#endif

// --- DEFINIÇÕES ---
#define MAX_VIDA_PLAYER 100
#define MAX_ENERGIA_PLAYER 100
#define MAX_SEGURANCA 100
#define MAX_STATUS_LEO 50 

#define LARGURA_LOG 58
#define LARGURA_DIARIO 30 
#define MAX_HISTORICO 2000 
#define MAX_LINHAS_DIARIO 500
#define TAMANHO_HISTORIA_BUFFER 8192 
#define ALTURA_FIXA_HISTORIA 8 
#define ALTURA_HUD_TOTAL 22 

#define ITEM_VAZIO 0
#define ITEM_COLT 1
#define ITEM_FACAO 2
#define ITEM_MEDKIT 3
#define ITEM_LANTERNA 4
#define ITEM_TABUAS 5
#define ITEM_SINALIZADOR 6

#define HORARIO_MANHA 0
#define HORARIO_TARDE 1
#define HORARIO_NOITE 2

// Códigos de Fim de Jogo
#define FIM_JOGANDO 0
#define FIM_MORTE_PLAYER 1 
#define FIM_MORTE_FILHO 2  
#define FIM_DESISTENCIA 3

// Locais
#define LOC_BASE 0
#define LOC_CIDADE 1
#define LOC_COMERCIANTE 2
#define LOC_PLANALTA 3
#define LOC_SAMAMBA 4
#define LOC_CEI 5

// --- DEFINIÇÕES DA HORTA ---
#define PLANTA_VAZIA 0
#define PLANTA_ALFACE 1
#define PLANTA_TOMATE 2
#define PLANTA_CENOURA 3

#define ESTAGIO_VAZIO 0
#define ESTAGIO_SEMENTE 1
#define ESTAGIO_BROTO 2
#define ESTAGIO_MADURO 3
#define ESTAGIO_MORTO 4

// IDs dos NPCs para o sistema de tarefas
#define NPC_AUGUSTO 0
#define NPC_ANA 1
#define NPC_MARCOS 2
#define NPC_MIGUEL 3
#define NPC_CLAUDIO 4
#define NPC_ELISA 5
#define NPC_NEIDE 6
#define NPC_KEVIN 7
#define NPC_SOMBRA 8
#define NPC_KALEB 9
#define NPC_BETO 10
#define NPC_LUCIA 11
#define NPC_ZERO 12
#define NPC_TALES 13
#define NPC_ALBERTO 14
#define NPC_PEDRO 15
#define NPC_BIANCA 16
#define NPC_IVO 17
#define NPC_LARA 18
#define NPC_TOM 19
#define TOTAL_NPCS 20 // Total de NPCs possíveis no jogo


const char *nomesItens[] = {
    "(Vazio)",
    "Revolver Colt",
    "Facao",
    "Kit Medico",
    "Lanterna",
    "Tabuas",
    "Sinalizador"
};

typedef struct {
    int estagio;      // Fase de vida
    int tipo;         // Qual vegetal
    int hidratacao;   // 0-100%
    int adubado;      // 0 ou 1
} Canteiro;

// --- ESTRUTURAS ---
typedef struct {
    int vida;      
    int energia;   
} Sobrevivente;

typedef struct {
    int dias;
    int horario;
    int acoesNoTurno; // CONTADOR DE AÇÕES DO TURNO
    int vida;
    int energia;
    int comida;
    int dinheiro;
    int seguranca;
    int municao;
    int pilhas;
    int geracao;
    int gasolina;
 
    int aliados;

    // --- REGISTROS DE NPC ---
    //NPCS DE SOBRAL
    int npcAugusto; // O Engenheiro
    int npcAna;     // A Coletora
    int npcMarcos;  // O Guarda
    // NPCs de Planalta
    int npcMiguel;  // O Padre
    int npcClaudio; // O Sargento
    int npcElisa;   // A Cientista
    int npcAlberto;
    //NPCS DE CEI
    int statusNeide; //a matriarca
    int statusKevin; //o tecnico
    int statusSombra; //o vigia
    int npcKaleb; //o tecnico de som
    int npcBeto; //o eletricista
    int npcLucia; // Química
    int npcZero;  // Robótica
    int npcTales; // Biblioteca
    // NPCs Errantes (Encontrados na opção "Procurar Recursos" da Base)
    int npcPedro;
    int npcBianca;
    int npcIvo;
    int npcLara;
    int npcTom;
    
    // Flags de Ação Diária
    int brincouHoje; 
    int treinouHoje; 
    int fezFogueiraHoje;
    int cacouHoje;
    int fezRondaHoje;
    int exercitouHoje;
    int ouviuRadioTarde;
    int ouviuRadioNoite;
    int cuidouHortaHoje; 
    //cidades
    int visitouMercadoHoje;
    int visitouFarmaciaHoje;
    int visitouPredioHoje;
    int desbloqueouSobral;
    int desbloqueouPlanalta;
    int desbloqueouSamamba;
    int desbloqueouCei;
    int visitouHospitalHoje;
    int visitouRodoviariaHoje;
    int visitouPracaHoje;
    int visitouEscolaHoje;
    int visitouFeiraHoje;
    int visitouPredioSamamba;
    int visitouMetroHoje;
    int visitouCassinoHoje;
    int visitouRadioHoje;
    int visitouParedaoHoje;
    int visitouGatosHoje;
    int visitouIFHoje;
    int descobriuTunel;
    
    // FRAGMENTOS DO CARTÓGRAFO
    int ouviuLendaCartografo; // Se já leu a intro
    int fragEscola;           // 1/3
    int fragRodoviaria;       // 2/3
    int fragPraca;

        // PROGRESSO PERMANENTE EM SOBRAL 
    // 0 = Não visitou, 1 = Já visitou
    int checkSobralExplorar;
    int checkSobralMercado;
    int checkSobralFarmacia;
    int checkSobralPredio;
    int checkSobralRic;
    int checkEscolaCofre;
    
    // PROGRESSO INTERNO DA CASA
    // 0 = Ainda tem loot, 1 = Já pegou tudo
    int checkCasaSala;
    int checkCasaCozinha;
    int checkCasaEscritorio;

    //ESTOQUE DO COMERCIANTE (Reinicia todo dia)
    // 1 = Tem, 0 = Esgotado
    int stkColt;
    int stkFacao;
    int stkMedkit;
    int stkLanterna;
    int stkTabuas;
    int stkSinalizador;
    int stkGasolina;
    // Quantidade (0 a 3)
    int stkComida;
    int stkSementes;
    int stkMunicao;
    int stkPilhas;
    
    // Novas Flags Diárias
    int bebeuCafeNoTurno;
    int procurouSementesNoTurno;
    int procurouAliadosNoTurno;
    
    // Flags de Ação por Turno (Resetam a cada horário)
    int explorouNoTurno;
    int comeuNoTurno;
    int alimentouNoTurno;

    // Local Atual
    int localAtual;
    
    // Flags de Conhecimento (Diário)
    int dicaExploracao;
    int dicaRonda;
    int dicaRadio;
    int dicaSono;
    int dicaCaca;
    int dicaBrincar;
    int dicaFogueira;
    int dicaHortaMorte;
    int dicaHortaGeral;
    int dicaFortificar;
    int dicaExercitar;
    int dicaMedkit;
    int dicaTreino;
    
    int tipoExploracao;

    char nomeJogador[50];
    char nomeFilho[50];
    Sobrevivente filho;
    int inventario[3];
    char textoHistoria[TAMANHO_HISTORIA_BUFFER];
    int historiaAtualizadaHoje;

    //HORTA
    int semAlface;
    int semTomate;
    int semCenoura;
    Canteiro canteiros[3]; // 3 Slots
    int estoqueAdubo;

    // Controle de tarefas diárias (0 = Disponível, 1 = Já trabalhou)
    int tarefasAliados[TOTAL_NPCS];
    int ordensDadasHoje;

    int nivelHistoria;

    int leoNivel; // 0 a 4
    int leoXP;    // 0 a 100


} GameState;

// Variáveis Globais de Log e Diário
char logs[5][LARGURA_LOG + 2]; 
char historicoGlobal[MAX_HISTORICO][LARGURA_LOG + 2];
int totalHistorico = 0;

char linhasDiario[MAX_LINHAS_DIARIO][LARGURA_DIARIO + 5]; 
int totalLinhasDiario = 0;

// --- PROTOTIPOS ---
void renderizarHUD(GameState *gs);
void atualizarCena(GameState *gs);
int lerInput();
void adicionarNotaDiario(char *texto);
void logMsg(char *novaMensagem);
void novoMomento();
void fecharMomento(GameState *gs);
void oferecerItem(GameState *gs, int itemID);
void modificarStatus(GameState *gs, int *variavel, int valor, int max, char *nomeStatus);
void oferecerItem(GameState *gs, int itemID);
void executarEventoBaixaSeguranca(GameState *gs, int *eventoAconteceu); // <--- NOVO
void modificarStatus(GameState *gs, int *variavel, int valor, int max, char *nomeStatus);
void checarFragmentosSamamba(GameState *gs);
void ganharSementes(GameState *gs, int quantidade);
void resolverHortaDinamica(GameState *gs);
void resolverAliadosVisual(GameState *gs);
void atualizarHistoria(GameState *gs, char *titulo, char *texto);
// --- CONFIGURAÇÃO DA JANELA DO CONSOLE ---
void configurarJanela() {
    #ifdef _WIN32
    // Pega o identificador da janela do console
    HWND consoleWindow = GetConsoleWindow(); 
    
    // Comando para maximizar a janela (Ocupar a tela toda)
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    
    #endif
}

// --- FUNÇÕES AUXILIARES ---

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lerInput() {
    int valor;
    if (scanf("%d", &valor) == 1) {
        limparBuffer(); 
        return valor;
    } else {
        limparBuffer();
        return -1;
    }
}

void pausar() {
    printf("\n[Pressione ENTER para continuar...]");
    int c = getchar();
    if (c != '\n' && c != EOF) {
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

void pausarMenu() {
    printf("\n[Pressione ENTER para voltar...]");
    getchar();
}

void limparLogs() {
    for(int i=0; i<5; i++) strcpy(logs[i], "");
}

void iniciarLogs() {
    limparLogs();
    totalHistorico = 0;
}

// --- SISTEMA DE DIÁRIO ---
void adicionarNotaDiario(char *texto) {
    char bufferCompleto[1024];
    sprintf(bufferCompleto, "- %s", texto);
    
    int len = strlen(bufferCompleto);
    int start = 0;
    
    while (start < len) {
        if (totalLinhasDiario >= MAX_LINHAS_DIARIO) break;

        int indent = (start == 0) ? 0 : 2;
        int maxWidth = LARGURA_DIARIO - indent; 
        
        int remaining = len - start;
        int count = (remaining > maxWidth) ? maxWidth : remaining;
        
        if (remaining > maxWidth) {
            if (bufferCompleto[start + count] != ' ') {
                int k;
                int encontrouEspaco = 0;
                for (k = count; k > 0; k--) {
                    if (bufferCompleto[start + k] == ' ') {
                        count = k;
                        encontrouEspaco = 1;
                        break;
                    }
                }
                if (!encontrouEspaco) {
                    count = maxWidth;
                }
            }
        }
        
        strcpy(linhasDiario[totalLinhasDiario], "");
        for(int i=0; i<indent; i++) strcat(linhasDiario[totalLinhasDiario], " ");
        strncat(linhasDiario[totalLinhasDiario], bufferCompleto + start, count);
        
        totalLinhasDiario++;
        start += count;
        
        if (start < len && bufferCompleto[start] == ' ') {
            start++;
        }
    }
    
    if (totalLinhasDiario < MAX_LINHAS_DIARIO) {
        strcpy(linhasDiario[totalLinhasDiario], "");
        totalLinhasDiario++;
    }
}

char* getLinhaVisualDiario(int linhaVisualIndex) {
    int inicio = 0;
    if (totalLinhasDiario > ALTURA_HUD_TOTAL) {
        inicio = totalLinhasDiario - ALTURA_HUD_TOTAL;
    }
    
    int indiceReal = inicio + linhaVisualIndex;
    
    if (indiceReal >= 0 && indiceReal < totalLinhasDiario) {
        return linhasDiario[indiceReal];
    }
    return "";
}

void exibirDiarioCompleto() {
    limparTela();
    printf("=== DIARIO DO SOBREVIVENTE ===\n\n");
    if (totalLinhasDiario == 0) printf("O diario esta vazio.\n");
    else {
        int linhasPorPag = 20;
        for(int i=0; i<totalLinhasDiario; i++) {
            printf("%s\n", linhasDiario[i]);

            if ((i+1) % linhasPorPag == 0 && i != totalLinhasDiario-1) {
                printf("\n[ENTER para virar a pagina...]");
                getchar();
                limparTela();
                printf("=== DIARIO (Cont.) ===\n\n");
            }
        }
    }
    printf("\n[ENTER para voltar]");
    getchar();
}

void exibirHistoricoLogs() {
    limparTela();
    printf("=== HISTORICO DE EVENTOS ===\n\n");
    if (totalHistorico == 0) printf("Nada aconteceu ainda.\n");
    else {
        int linhasPorPag = 20;
        for(int i=0; i<totalHistorico; i++) {
            printf("%s\n", historicoGlobal[i]);

            if ((i+1) % linhasPorPag == 0 && i != totalHistorico-1) {
                printf("\n[ENTER para ver mais...]");
                getchar();
                limparTela();
                printf("=== HISTORICO (Cont.) ===\n\n");
            }
        }
    }
    printf("\n[ENTER para voltar]");
    getchar();
}

// --- SISTEMA DE LOG EM LOTE ---

void logMsg(char *novaMensagem) {
    for (int i = 0; i < 4; i++) {
        strcpy(logs[i], logs[i+1]);
    }
    snprintf(logs[4], LARGURA_LOG + 1, "%s", novaMensagem);

    if (totalHistorico < MAX_HISTORICO) {
        snprintf(historicoGlobal[totalHistorico], LARGURA_LOG, "%s", novaMensagem);
        totalHistorico++;
    }
}

void novoMomento() {
    limparLogs();
}

void fecharMomento(GameState *gs) {
    renderizarHUD(gs);
    pausar();
    // NÃO limpa os logs aqui, para permitir leitura sequencial
}

void modificarStatus(GameState *gs, int *variavel, int valor, int max, char *nomeStatus) {
    *variavel += valor;
    
    if (max > 0 && *variavel > max) {
        *variavel = max;
    }
    if (*variavel < 0) {
        *variavel = 0;
    }
    
    char bufferLog[LARGURA_LOG];
    if (valor != 0) {
        sprintf(bufferLog, "[STATUS] %s: %+d", nomeStatus, valor);
        logMsg(bufferLog);
    }
}

// --- SISTEMA DE HORTA ---
void resolverHortaDinamica(GameState *gs) {
    int sair = 0;
    char feedback[100] = "Bem-vindo a area da horta."; // Mensagem de feedback local

    while (!sair) {
        limparTela();
        
        // 1. ASCII ART (Recuperada e Adaptada)
        printf("======================================================\n");
        printf("                 AREA DA HORTA / ESTUFA               \n");
        printf("======================================================\n");
        printf("        /'''''\\      __   __   __   __                \n");
        printf("       / v v v \\    /  \\ /  \\ /  \\ /  \\               \n");
        printf("      |  | | |  |  | v || v || v || v |               \n");
        printf("      |---------|  |___||___||___||___|               \n");
        printf("      | v v v v |  |___||___||___||___|               \n");
        printf("      '---------' /____________________\\              \n");
        printf("======================================================\n\n");

        // 2. STATUS GERAL
        printf(" [ ENERGIA: %d ]    [ ADUBO: %d ]\n", gs->energia, gs->estoqueAdubo);
        printf(" ESTOQUE DE SEMENTES:\n");
        printf(" [ Alface: %d ]  [ Tomate: %d ]  [ Cenoura: %d ]\n\n", 
               gs->semAlface, gs->semTomate, gs->semCenoura);

        printf(" ULTIMA ACAO: %s\n", feedback);
        printf("------------------------------------------------------\n");

        // 3. DESENHO DOS CANTEIROS
        for (int i = 0; i < 3; i++) {
            char nomePlanta[15] = "";
            char desenho[20];
            char status[40];
            
            // Define nome
            if (gs->canteiros[i].tipo == PLANTA_ALFACE) strcpy(nomePlanta, "Alface");
            else if (gs->canteiros[i].tipo == PLANTA_TOMATE) strcpy(nomePlanta, "Tomate");
            else if (gs->canteiros[i].tipo == PLANTA_CENOURA) strcpy(nomePlanta, "Cenoura");
            
            // Define Estado Visual
            if (gs->canteiros[i].estagio == ESTAGIO_VAZIO) {
                strcpy(desenho, "[ ___ ]");
                sprintf(status, "Vazio (Pronto p/ Plantio)");
            } 
            else if (gs->canteiros[i].estagio == ESTAGIO_SEMENTE) {
                strcpy(desenho, "[  .  ]");
                sprintf(status, "%s (Semente) Agua:%d%%", nomePlanta, gs->canteiros[i].hidratacao);
            }
            else if (gs->canteiros[i].estagio == ESTAGIO_BROTO) {
                strcpy(desenho, "[ \\|/ ]");
                sprintf(status, "%s (Broto) Agua:%d%%", nomePlanta, gs->canteiros[i].hidratacao);
            }
            else if (gs->canteiros[i].estagio == ESTAGIO_MADURO) {
                strcpy(desenho, "[ {@} ]");
                sprintf(status, "%s (MADURO - COLHER!)", nomePlanta);
            }
            else { 
                strcpy(desenho, "[  X  ]");
                sprintf(status, "Planta Morta (Limpar)");
            }
            
            printf(" CANTEIRO %d: %s  -> %s %s\n", 
                i+1, desenho, status, gs->canteiros[i].adubado ? "[+ADUBO]" : "");
        }
        printf("------------------------------------------------------\n");

        // 4. MENU DE AÇÕES
        printf(" 1 - Plantar (Escolher Semente + 5 Energia)\n");
        printf(" 2 - Regar Todos (Gasta 10 Energia)\n");
        printf(" 3 - Adubar (Gasta 1 Adubo)\n");
        printf(" 4 - Colher ou Limpar (Gasta 5 Energia)\n");
        printf(" 5 - Compostagem (1 Comida -> 2 Adubos)\n");
        printf(" 0 - Voltar para Base\n");
        printf(" Escolha: ");

        int esc = lerInput();

        // LÓGICA (Atualiza a string 'feedback' para mostrar na tela e 'logMsg' para o histórico)
        if (esc == 0) {
            sair = 1;
        }
        else if (esc == 1) { // PLANTAR
            if (gs->energia >= 5) {
                int slot = -1;
                for(int i=0; i<3; i++) if(gs->canteiros[i].estagio == ESTAGIO_VAZIO) { slot=i; break; }
                
                if (slot != -1) {
                    printf("\n Plantar no Slot %d:\n", slot+1);
                    printf(" 1-Alface (Rapido)\n 2-Tomate (Medio)\n 3-Cenoura (Lento)\n 0-Cancelar\n Escolha: ");
                    int s = lerInput();
                    
                    int plantou = 0;
                    if (s == 1 && gs->semAlface > 0) { 
                        gs->semAlface--; gs->canteiros[slot].tipo = PLANTA_ALFACE; plantou=1; 
                    }
                    else if (s == 2 && gs->semTomate > 0) { 
                        gs->semTomate--; gs->canteiros[slot].tipo = PLANTA_TOMATE; plantou=1; 
                    }
                    else if (s == 3 && gs->semCenoura > 0) { 
                        gs->semCenoura--; gs->canteiros[slot].tipo = PLANTA_CENOURA; plantou=1; 
                    }
                    else if (s > 0) { strcpy(feedback, "Voce nao tem essa semente."); }

                    if (plantou) {
                        modificarStatus(gs, &gs->energia, -5, 100, "Energia");
                        gs->canteiros[slot].estagio = ESTAGIO_SEMENTE;
                        gs->canteiros[slot].hidratacao = 50; 
                        strcpy(feedback, "Semente plantada com sucesso.");
                        logMsg("Horta: Nova semente plantada.");
                    }
                } else { strcpy(feedback, "Nao ha canteiros vazios."); }
            } else { strcpy(feedback, "Sem energia suficiente."); }
        }
        else if (esc == 2) { // REGAR
            if (gs->energia >= 10) {
                modificarStatus(gs, &gs->energia, -10, 100, "Energia");
                int regou = 0;
                for(int i=0; i<3; i++) {
                    if (gs->canteiros[i].estagio > ESTAGIO_VAZIO && gs->canteiros[i].estagio < ESTAGIO_MADURO) {
                        gs->canteiros[i].hidratacao = 100;
                        regou = 1;
                    }
                }
                if (regou) {
                    strcpy(feedback, "Plantas regadas.");
                    logMsg("Horta: Plantas regadas.");
                } else strcpy(feedback, "Nada para regar.");
            } else strcpy(feedback, "Sem energia suficiente.");
        }
        else if (esc == 3) { // ADUBAR
            if (gs->estoqueAdubo > 0) {
                printf(" Slot (1-3): "); int c = lerInput()-1;
                if(c>=0 && c<3 && gs->canteiros[c].estagio > 0 && gs->canteiros[c].estagio < 3) {
                    if (!gs->canteiros[c].adubado) {
                        gs->estoqueAdubo--; gs->canteiros[c].adubado = 1; 
                        strcpy(feedback, "Canteiro adubado.");
                        logMsg("Horta: Adubo aplicado.");
                    } else strcpy(feedback, "Ja esta adubado hoje.");
                } else strcpy(feedback, "Slot invalido ou planta nao precisa.");
            } else strcpy(feedback, "Sem estoque de adubo.");
        }
        else if (esc == 4) { // COLHER / LIMPAR
            if (gs->energia >= 5) {
                printf(" Slot (1-3): "); int c = lerInput()-1;
                if (c >= 0 && c < 3) {
                    if (gs->canteiros[c].estagio == ESTAGIO_MADURO) {
                        modificarStatus(gs, &gs->energia, -5, 100, "Energia");
                        
                        // 1. GERA COMIDA
                        int qtd = 0;
                        if (gs->canteiros[c].tipo == PLANTA_ALFACE) qtd = 2;
                        else if (gs->canteiros[c].tipo == PLANTA_TOMATE) qtd = 4;
                        else qtd = 6;
                        
                        // Bônus de comida
                        if (rand() % 100 < 30) qtd++;

                        modificarStatus(gs, &gs->comida, qtd, 999, "Comida");
                        sprintf(feedback, "Colheita realizada: +%d Comida.", qtd);
                        
                        char msgLog[50]; sprintf(msgLog, "Horta: Colheu %d comidas.", qtd);
                        logMsg(msgLog);
                        
                        // 2. RECUPERA SEMENTE (100% GARANTIDO DO MESMO TIPO)
                        if (gs->canteiros[c].tipo == PLANTA_ALFACE) {
                            gs->semAlface++;
                            strcat(feedback, " (+1 Sem. Alface)");
                            logMsg("Horta: Recuperou semente de Alface.");
                        }
                        else if (gs->canteiros[c].tipo == PLANTA_TOMATE) {
                            gs->semTomate++;
                            strcat(feedback, " (+1 Sem. Tomate)");
                            logMsg("Horta: Recuperou semente de Tomate.");
                        }
                        else if (gs->canteiros[c].tipo == PLANTA_CENOURA) {
                            gs->semCenoura++;
                            strcat(feedback, " (+1 Sem. Cenoura)");
                            logMsg("Horta: Recuperou semente de Cenoura.");
                        }

                        // 3. RESET DO SLOT
                        gs->canteiros[c].estagio = ESTAGIO_VAZIO;
                        gs->canteiros[c].tipo = PLANTA_VAZIA;
                        gs->canteiros[c].hidratacao = 0;
                        gs->canteiros[c].adubado = 0;
                        
                    } else if (gs->canteiros[c].estagio == ESTAGIO_MORTO) {
                        modificarStatus(gs, &gs->energia, -5, 100, "Energia");
                        gs->canteiros[c].estagio = ESTAGIO_VAZIO;
                        gs->canteiros[c].tipo = PLANTA_VAZIA;
                        strcpy(feedback, "Planta morta removida. Semente perdida.");
                        logMsg("Horta: Limpeza realizada.");
                    } else {
                        strcpy(feedback, "A planta ainda nao esta pronta.");
                    }
                } else strcpy(feedback, "Slot invalido.");
            } else strcpy(feedback, "Sem energia suficiente.");
        }
        else if (esc == 5) { // COMPOSTAGEM
            if (gs->comida > 0) {
                gs->comida--; gs->estoqueAdubo += 2;
                strcpy(feedback, "Comida transformada em Adubo (+2).");
                logMsg("Horta: Compostagem feita.");
            } else strcpy(feedback, "Voce precisa de comida para compostar.");
        }
        else {
            strcpy(feedback, "Opcao invalida.");
        }
    }
}

// --- SISTEMA DE ITENS ---

void oferecerItem(GameState *gs, int itemID) {
    char msgTemp[100];
    
    sprintf(msgTemp, "Voce encontrou: %s!", nomesItens[itemID]);
    logMsg(msgTemp);
    renderizarHUD(gs);

    printf("\nDeseja pegar %s?\n1 - Sim\n2 - Nao\nEscolha: ", nomesItens[itemID]);
    int escolhaItem = lerInput();

    if (escolhaItem == 1) {
        int slotVazio = -1;
        for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_VAZIO) { slotVazio = i; break; }
        
        if(slotVazio != -1) {
            gs->inventario[slotVazio] = itemID;
            logMsg("Item guardado na mochila.");
        } else {
            novoMomento(); // Só limpa se precisar da tela de troca
            logMsg("Sua mochila esta cheia!");
            renderizarHUD(gs);
            
            printf("\nSubstituir qual item?\n1 - %s\n2 - %s\n3 - %s\n4 - Cancelar\nEscolha: ", 
                nomesItens[gs->inventario[0]], 
                nomesItens[gs->inventario[1]], 
                nomesItens[gs->inventario[2]]);
            
            int tr = lerInput();
            if(tr >= 1 && tr <= 3) {
                logMsg("Item substituido.");
                gs->inventario[tr-1] = itemID;
            } else {
                logMsg("Voce deixou o item para tras.");
            }
        }
    } else {
        logMsg("Voce decidiu deixar o item.");
    }
}

//COMBATE COM ZUMBIS PADRÃO
void resolverEncontroZumbi(GameState *gs, char *descricaoSituacao) {
    novoMomento();
    logMsg(descricaoSituacao);
    renderizarHUD(gs);

    // 1. Verifica Inventário
    int temColt = 0; 
    for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_COLT) temColt = 1;
    
    int temFacao = 0; 
    for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_FACAO) temFacao = 1;

    int podeAtirar = (temColt && gs->municao > 0);

    // 2. Monta o Menu Dinâmico
    printf("Opcoes:\n");
    // Correr: 40% chance de sucesso -> Média/Baixa. Vamos classificar como Media para diferenciar de Lutar.
    printf("1 - Correr (Media)\n"); 
    // Lutar: 25% chance de sucesso -> Baixa.
    printf("2 - Lutar corpo-a-corpo (Baixa)\n");
    // Nada: 100% Dano -> Caótico.
    printf("3 - Fazer nada (Caotico)\n");

    int opAtirar = 0;
    int opFacao = 0;
    int proximaOp = 4;

    if (podeAtirar) {
        printf("%d - Atirar (Alta)\n", proximaOp); // 100% Sucesso
        opAtirar = proximaOp++;
    }

    if (temFacao) {
        // Facão: 60% chance -> Média/Alta. Vamos de Media.
        printf("%d - Usar Facao (Media)\n", proximaOp); 
        opFacao = proximaOp++;
    }

    printf("Escolha: ");
    int escolha = lerInput();
    novoMomento();

    // 3. Resolve a Escolha
    if (escolha == 1) { // CORRER (40%)
        if (rand() % 100 < 40) {
            logMsg("Voce correu desesperadamente e escapou.");
            modificarStatus(gs, &gs->energia, -10, 100, "Energia");
        } else {
            logMsg("Voce tropecou na fuga e foi mordido.");
            modificarStatus(gs, &gs->vida, -15, 100, "Vida");
            modificarStatus(gs, &gs->energia, -10, 100, "Energia");
        }
    }
    else if (escolha == 2) { // CORPO-A-CORPO (25%)
        if (rand() % 100 < 25) {
            logMsg("Na base da forca bruta, voce derrubou o zumbi.");
            modificarStatus(gs, &gs->vida, -5, 100, "Vida (Esforco)");
            modificarStatus(gs, &gs->energia, -15, 100, "Energia");
        } else {
            logMsg("Lutar sem armas foi um erro. Voce foi ferido gravemente.");
            modificarStatus(gs, &gs->vida, -25, 100, "Vida");
        }
    }
    else if (escolha == 3) { // FAZER NADA
        logMsg("Voce paralisou de medo.");
        logMsg("O zumbi te atacou livremente.");
        modificarStatus(gs, &gs->vida, -30, 100, "Vida");
    }
    else if (podeAtirar && escolha == opAtirar) { // ATIRAR
        gs->municao--;
        logMsg("Um tiro preciso na cabeca.");
        modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca");
    }
    else if (temFacao && escolha == opFacao) { // FACÃO (60%)
        if (rand() % 100 < 60) {
            logMsg("Voce decapitou o zumbi com o facao.");
            modificarStatus(gs, &gs->energia, -5, 100, "Energia");
        } else {
            logMsg("O zumbi agarrou seu braco armado e te mordeu.");
            modificarStatus(gs, &gs->vida, -15, 100, "Vida");
        }
    }
    else {
        logMsg("Voce hesitou e o zumbi atacou!");
        modificarStatus(gs, &gs->vida, -15, 100, "Vida");
    }
}

// --- EVENTOS DE RONDA ---
void executarEventoRonda(GameState *gs) {
    int r = rand() % 100;
    int evento;
    char msgTemp[100]; 
    
    if (r < 20) evento = 90; // Sementes
    else if (r < 40) evento = 91; // Aliados
    else evento = rand() % 9; // Eventos normais

    if (!gs->dicaRonda) {
        adicionarNotaDiario("E importante tomar as decisoes certas na ronda para garantir a seguranca.");
        gs->dicaRonda = 1;
    }

    novoMomento(); 
    logMsg("Voce patrulhou o perimetro e reforcou as defesas.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");
    modificarStatus(gs, &gs->seguranca, 5, MAX_SEGURANCA, "Seguranca");
    
    if (rand() % 100 < 25) {
        int rLoot = rand() % 4; // Agora são 4 possibilidades
        
        if (rLoot == 0) { 
            logMsg("Achou uma bala perdida na cerca."); 
            modificarStatus(gs, &gs->municao, 1, 999, "Municao"); 
        }
        else if (rLoot == 1) { 
            logMsg("Achou pilhas velhas no chao."); 
            modificarStatus(gs, &gs->pilhas, 1, 999, "Pilhas"); 
        }
        else if (rLoot == 2) { 
            logMsg("Achou dinheiro no bolso de um casaco velho."); 
            modificarStatus(gs, &gs->dinheiro, 10, 9999, "Dinheiro"); 
        }
        else { 
            logMsg("Encontrou um pouco de gasolina numa garrafa."); 
            modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina"); 
        }
    }
    
    renderizarHUD(gs);
    pausar(); 
    novoMomento();
    
    int escolha;
    
    if (evento == 91) { // ALIADOS
        logMsg("EVENTO: Cachorro faminto apareceu.");
        renderizarHUD(gs);
        printf("O que fazer?\n1 - Dar Comida (Media)\n2 - Espantar (Alta)\n3 - Ignorar (Alta)\nEscolha: ");
        escolha = lerInput();
        if(escolha==1) {
            if(gs->comida > 0) {
                gs->comida--;
                logMsg("Ele comeu e virou seu amigo!"); 
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else { logMsg("Voce nao tem comida para dar."); }
        } else if(escolha==2) { logMsg("O cachorro fugiu assustado."); }
        else { logMsg("O cachorro foi embora."); }
    }
    else {
        switch(evento) {
            case 0: // RASTRO
                logMsg("EVENTO: Rastro de sangue fresco.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Seguir (Caotico)\n2 - Ignorar e voltar (Alta)\n3 - Montar armadilha (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Era uma emboscada!"); modificarStatus(gs, &gs->vida, -15, 100, "Vida"); }
                else if(escolha==2) { logMsg("Voce voltou em seguranca."); }
                else { logMsg("Voce pegou um lobo!"); modificarStatus(gs, &gs->comida, 3, 999, "Comida"); modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca"); }
                break;
            case 1: // ZUMBI
                resolverEncontroZumbi(gs, "EVENTO: Um zumbi esta preso na cerca, se debatendo.");
                if (gs->vida < 100) { 
                     logMsg("A cerca foi danificada na confusao.");
                     modificarStatus(gs, &gs->seguranca, -5, 100, "Seguranca");
                }
                break;
            case 2: // MOCHILA
                logMsg("EVENTO: Mochila presa em arvore alta.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Subir (Caotico)\n2 - Derrubar com pedras (Caotico)\n3 - Cortar galho (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Voce caiu!"); modificarStatus(gs, &gs->vida, -10, 100, "Vida"); }
                else if(escolha==2) { logMsg("Caiu e quebrou o que tinha dentro."); }
                else { logMsg("Sucesso! O galho caiu devagar."); oferecerItem(gs, ITEM_MEDKIT); }
                break;
            case 3: // FOGUEIRA
                logMsg("EVENTO: Fogueira recente de estranhos.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Apagar e fugir (Caotico)\n2 - Rastrear eles (Caotico)\n3 - Roubar sobras (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Voce fugiu assustado."); modificarStatus(gs, &gs->energia, -5, 100, "Energia"); }
                else if(escolha==2) { logMsg("Eles te viram!"); modificarStatus(gs, &gs->vida, -10, 100, "Vida"); }
                else { logMsg("Voce pegou restos uteis."); modificarStatus(gs, &gs->comida, 1, 999, "Comida"); modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca"); }
                break;
            case 4: // BURACO
                logMsg("EVENTO: Buraco na grade dos fundos.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Tapar com lixo (Caotico)\n2 - Ignorar por hoje (Caotico)\n3 - Soldar/Amarrar bem (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("O lixo caiu na noite."); modificarStatus(gs, &gs->seguranca, -5, 100, "Seguranca"); }
                else if(escolha==2) { logMsg("Entraram ratos."); modificarStatus(gs, &gs->comida, -1, 999, "Comida"); }
                else { logMsg("Grade reforcada."); modificarStatus(gs, &gs->seguranca, 15, 100, "Seguranca"); modificarStatus(gs, &gs->energia, -5, 100, "Energia"); }
                break;
            case 5: // GERADOR
                logMsg("EVENTO: Gerador velho encontrado.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Tentar ligar agora (Caotico)\n2 - Desmontar (Caotico)\n3 - Consertar com calma (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Explodiu na sua cara!"); modificarStatus(gs, &gs->vida, -15, 100, "Vida"); }
                else if(escolha==2) { logMsg("Pecas inuteis."); }
                else { logMsg("Luz e conforto!"); modificarStatus(gs, &gs->seguranca, 10, 100, "Seguranca"); sprintf(msgTemp, "Energia (%s)", gs->nomeFilho); modificarStatus(gs, &gs->filho.energia, 10, MAX_STATUS_LEO, msgTemp); }
                break;
            case 6: // CHUVA
                logMsg("EVENTO: Chuva acida repentina.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Correr para dentro (Caotico)\n2 - Se cobrir com lona (Caotico)\n3 - Coletar agua filtrada (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Voce escorregou."); modificarStatus(gs, &gs->vida, -5, 100, "Vida"); }
                else if(escolha==2) { logMsg("A lona rasgou."); modificarStatus(gs, &gs->energia, -5, 100, "Energia"); }
                else { logMsg("Agua potavel coletada."); modificarStatus(gs, &gs->vida, 5, 100, "Vida"); modificarStatus(gs, &gs->energia, 5, 100, "Energia"); }
                break;
            case 7: // CORVO
                logMsg("EVENTO: Corvo mensageiro?");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Atirar (Alta)\n2 - Espantar (Alta)\n3 - Seguir o corvo (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { 
                    int temColt = 0; for(int i=0;i<3;i++) if(gs->inventario[i]==ITEM_COLT) temColt=1;
                    if (temColt && gs->municao > 0) {
                        gs->municao--;
                        logMsg("Voce acertou, mas era carne podre."); 
                        modificarStatus(gs, &gs->vida, -5, 100, "Vida (Infeccao)"); 
                        modificarStatus(gs, &gs->comida, 1, 999, "Comida"); 
                    } else {
                        logMsg("Voce nao pode atirar sem arma ou balas. O corvo riu.");
                        modificarStatus(gs, &gs->energia, -5, 100, "Energia");
                    }
                }
                else if(escolha==2) { logMsg("Ele voou."); }
                else { logMsg("Ele levou a um corpo com itens."); modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas"); modificarStatus(gs, &gs->municao, 2, 999, "Municao"); }
                break;
            case 8: // INVASOR
                logMsg("EVENTO: Invasor solitario.");
                renderizarHUD(gs);
                printf("O que fazer?\n1 - Lutar corpo a corpo (Media)\n2 - Gritar para sair (Caotico)\n3 - Render e interrogar (Alta)\nEscolha: ");
                escolha = lerInput();
                if(escolha==1) { logMsg("Voce venceu, mas se feriu."); modificarStatus(gs, &gs->vida, -10, 100, "Vida"); modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca"); }
                else if(escolha==2) { logMsg("Ele riu e voltou depois."); modificarStatus(gs, &gs->seguranca, -10, 100, "Seguranca"); }
                else { logMsg("Ele deu informacoes valiosas."); modificarStatus(gs, &gs->seguranca, 10, 100, "Seguranca"); modificarStatus(gs, &gs->energia, 5, 100, "Energia"); }
                break;
        }
    }
}

// --- TELA DE FIM DE JOGO ---
void telaFimDeJogo(int motivo, int dias, char *nomeJogador, char *nomeFilho) {
    limparTela();
    printf("======================================================\n");
    printf("                   FIM DE JOGO                        \n");
    printf("======================================================\n\n");
    
    switch(motivo) {
        case FIM_MORTE_FILHO:
            printf("   %s morreu...\n", nomeFilho);
            printf("   Voce (%s) falhou em proteger o futuro.\n", nomeJogador);
            break;
        case FIM_DESISTENCIA:
            printf("   Voce desistiu de lutar...\n");
            printf("   %s nao conseguiu sobreviver sozinho sem voce.\n", nomeFilho);
            break;
    }

    printf("\n   --------------------------------------------\n");
    printf("   Dias Sobrevividos (Total): %d\n", dias);
    printf("   --------------------------------------------\n\n");
    
    printf("Pressione ENTER para voltar ao Menu Principal...");
    getchar();
}

void telaRegistros() {
    limparTela();
    printf("=== REGISTROS DE SOBREVIVENCIA ===\n\n");
    printf("[!] Funcionalidade de persistencia ainda nao implementada.\n");
    pausarMenu();
}

void renderizarMenuPrincipal() {
    limparTela();
    // Cabeçalho Largo
    printf("====================================================================================================\n");
    printf("                                     SOBREVIVA E PROTEJA v1.0                                       \n");
    printf("====================================================================================================\n");
    
    // Layout em Colunas: Esquerda (Arte/Menu) | Direita (Manual Completo)
    printf("          O            o             |  [ ! ] REGRAS DE SOBREVIVENCIA                             \n");
    printf("         /|\\          /|\\            |  1. STATUS VITAIS:                                         \n");
    printf("         / \\          / \\            |     - Vida/Energia zerada = Morte.                         \n");
    printf("                                     |     - Se o Pai morrer, o Filho (Leo) assume anos depois.   \n");
    printf("   -------------------------------   |     - Leo precisa comer e dormir tambem.                   \n");
    printf("                                     |                                                            \n");
    printf("       1 - INICIAR JORNADA           |  2. GERENCIAMENTO DA BASE:                                 \n");
    printf("                                     |     - SEGURANCA: Se < 50%%, sua base sera invadida a noite.\n");
    printf("       2 - REGISTROS                 |       Use Tabuas e faca Rondas para aumentar.              \n");
    printf("                                     |     - HORTA: Regue TODO dia. Se a agua secar, a planta morre.\n");
    printf("       3 - SAIR                      |       Use Adubo para acelerar a colheita.                  \n");
    printf("                                     |                                                            \n");
    printf("   -------------------------------   |  3. GUIA DE EXPLORACAO (Requer Gasolina):                  \n");
    printf("                                     |     > SOBRAL (Cidade): Loot basico. Visite o Comerciante.  \n");
    printf("                                     |     > PLANALTA (Serra): Otimos para ARMAS e KIT MEDICOS.   \n");
    printf("                                     |       Cuidado com o Hospital e a Escola.                   \n");
    printf("                                     |     > SAMAMBA (Metropole): Alto risco, mas MUITA comida.   \n");
    printf("                                     |       Explore os predios verticais e o metro.              \n");
    printf("                                     |     > CEI (Industrial): Area final. Requer explosivos.     \n");
    printf("                                     |       Resolva os puzzles para encontrar o Eden.            \n");
    printf("                                     |                                                            \n");
    printf("   CONTROLES: Use os numeros do      |  4. ALIADOS:                                               \n");
    printf("   teclado e pressione ENTER.        |     - Existem 20 sobreviventes escondidos no mundo.        \n");
    printf("                                     |     - Recrute-os para ganhar bonus passivos e tarefas.     \n");
    printf("====================================================================================================\n");
}

// --- HUD RENDER (INTERFACE ESTÁTICA) ---
void renderizarHUD(GameState *gs) {
    limparTela();
    int linhaHUD = 0; 
    
    // CORREÇÃO 1: Loop agora vai até <= LARGURA_LOG para gerar 59 iguais (Total 61 chars com os +)
    // Isso alinha com o conteúdo que usa "| %-58s|" (que tem 61 chars de largura total)
    char borderLog[LARGURA_LOG + 5];
    strcpy(borderLog, "+");
    for(int i=0; i<=LARGURA_LOG; i++) strcat(borderLog, "="); 
    strcat(borderLog, "+");

    char descHorario[10];
    char asciiTempo[3][8]; 

    if (gs->horario == HORARIO_NOITE) {
        strcpy(descHorario, "NOITE");
        strcpy(asciiTempo[0], "   .  ");
        strcpy(asciiTempo[1], "  ( ) ");
        strcpy(asciiTempo[2], "   '  ");
    } 
    else if (gs->horario == HORARIO_MANHA) {
        strcpy(descHorario, "MANHA");
        strcpy(asciiTempo[0], "  \\./ ");
        strcpy(asciiTempo[1], " - O -");
        strcpy(asciiTempo[2], "  /'\\ ");
    } 
    else { 
        strcpy(descHorario, "TARDE");
        strcpy(asciiTempo[0], "  \\./ ");
        strcpy(asciiTempo[1], " - O -");
        strcpy(asciiTempo[2], "      "); 
    }

    // --- PREPARAÇÃO DA CAIXA DE LOCALIZAÇÃO ---
    char boxLocal[10][60]; 
    char nomeLocal[50];
    char asciiArt[5][60];
    
    for(int i=0; i<10; i++) sprintf(boxLocal[i], "%-54s", "");
    for(int i=0; i<5; i++) { memset(asciiArt[i], ' ', 59); asciiArt[i][59] = '\0'; }

    switch(gs->localAtual) {
        case LOC_CIDADE:
            strcpy(nomeLocal, "RUINAS DE SOBRAL");
            strcpy(asciiArt[0], "      _   __   _   _      _   ");
            strcpy(asciiArt[1], "     | | |  | | | | |    | |  ");
            strcpy(asciiArt[2], "     |_| |__| |_| | |   _|_|_ ");
            strcpy(asciiArt[3], "     | | |__| | | |_|  |     |");
            strcpy(asciiArt[4], "     |_| |  | |_| | |  |_____|");
            break;
        case LOC_COMERCIANTE:
            strcpy(nomeLocal, "COMERCIANTE VIAJANTE");
            strcpy(asciiArt[0], "          ,---.               ");
            strcpy(asciiArt[1], "         /_____\\   ( $ )      ");
            strcpy(asciiArt[2], "        |       |   /|\\       ");
            strcpy(asciiArt[3], "        | TROCA |    |        ");
            strcpy(asciiArt[4], "       -O-------O-  / \\       ");
            break;
        case LOC_BASE:
        default:
            strcpy(nomeLocal, "ARREDORES DA BASE");
            strcpy(asciiArt[0], "            .                  ");
            strcpy(asciiArt[1], "           / \\       ( )       ");
            strcpy(asciiArt[2], "          /___\\       |        ");
            strcpy(asciiArt[3], "         | [ ] |    __|__      ");
            strcpy(asciiArt[4], "         |_[_]_|   |_____|     ");
            break;
        case LOC_PLANALTA:
            strcpy(nomeLocal, "RUINAS DE PLANALTA");
            // ASCII: Montanhas ao fundo
            strcpy(asciiArt[0], "          _      _   _            ");
            strcpy(asciiArt[1], "        _/ \\____/ \\_/ \\_          ");
            strcpy(asciiArt[2], "       /                \\         ");
            strcpy(asciiArt[3], "      /  _   _   _   _   \\        ");
            strcpy(asciiArt[4], "     /__|_|_|_|_|_|_|_|___\\       ");
            break;
        case LOC_SAMAMBA:
        strcpy(nomeLocal, "RUINAS DE SAMAMBA");
        // ASCII: Prédios altos e vegetação tomando conta
        strcpy(asciiArt[0], "      | |  _      | |   ( )       ");
        strcpy(asciiArt[1], "     _| |_| |  _  | |  _|_|_      ");
        strcpy(asciiArt[2], "    |o|o|o|o| | | |o| | | | |     ");
        strcpy(asciiArt[3], "    |o|o|o|o|_|_|_|o|_| | | |     ");
        strcpy(asciiArt[4], "   /vvvvvvvvvvvvvvvvvvvvvvvvv\\    ");
        break;
        case LOC_CEI:
            strcpy(nomeLocal, "RUINAS DE CEI");
            // ASCII: Grandes chaminés industriais ou prédios quebrados
            strcpy(asciiArt[0], "       _  _      _______      ");
            strcpy(asciiArt[1], "      | || |    |  ___  |     ");
            strcpy(asciiArt[2], "     _| || |_   | |___| |     ");
            strcpy(asciiArt[3], "    |_______|   |_______|     ");
            strcpy(asciiArt[4], "    /_______\\   /_______\\     ");
            break;    
    }

    // CORREÇÃO 2: Ajuste de largura da caixa da esquerda para caber em 54 chars
    // Largura total reduzida em 1 char para não empurrar a caixa da direita
    sprintf(boxLocal[0], "   +=================================================+"); // 49 iguais
    sprintf(boxLocal[1], "   | LOCAL: %-40s |", nomeLocal);
    sprintf(boxLocal[2], "   |-------------------------------------------------|");
    sprintf(boxLocal[3], "   | %-47s |", asciiArt[0]);
    sprintf(boxLocal[4], "   | %-47s |", asciiArt[1]);
    sprintf(boxLocal[5], "   | %-47s |", asciiArt[2]);
    sprintf(boxLocal[6], "   | %-47s |", asciiArt[3]);
    sprintf(boxLocal[7], "   | %-47s |", asciiArt[4]);
    sprintf(boxLocal[8], "   +=================================================+");


    // --- INICIO DA RENDERIZAÇÃO ---

    char bufferEsq[100];
    char bufferInv[LARGURA_LOG + 10]; 
    char bufferLogHeader[LARGURA_LOG + 1]; 

    memset(bufferLogHeader, ' ', LARGURA_LOG);
    bufferLogHeader[LARGURA_LOG] = '\0';
    strncpy(bufferLogHeader + 1, descHorario, strlen(descHorario));
    strncpy(bufferLogHeader + 12, "(Acoes: ", 8);
    char bufAcoes[5]; sprintf(bufAcoes, "%d/2)", gs->acoesNoTurno);
    strncpy(bufferLogHeader + 20, bufAcoes, strlen(bufAcoes));
    
    char resInfo[50];
    sprintf(resInfo, "Municao:%-3d Pilhas:%-3d", gs->municao, gs->pilhas);
    int posInfo = 57 - strlen(resInfo);
    if (posInfo < 0) posInfo = 35;
    strncpy(bufferLogHeader + posInfo, resInfo, strlen(resInfo));

    // LINHA 0
    sprintf(bufferEsq, "--- DIA %d ", gs->dias);
    printf("%-54s        %s   === DIARIO ===\n", bufferEsq, borderLog);
    linhaHUD++;

    // LINHA 1
    char nomes[60];
    char nomeBoxJog[30];
    char nomeBoxFilho[30];
    sprintf(nomeBoxJog, "[ %s ]", gs->nomeJogador);
    sprintf(nomeBoxFilho, "[ %s ]", gs->nomeFilho);
    int espacos = 48 - strlen(nomeBoxJog) - strlen(nomeBoxFilho); 
    if (espacos < 1) espacos = 1;
    sprintf(nomes, "   %s%*s%s", nomeBoxJog, espacos, "", nomeBoxFilho);
    
    // CORREÇÃO 3: Adicionado espaço em "| %s|" para alinhar com os outros "| %-58s|"
    printf("%-54s        | %s|   %s\n", nomes, bufferLogHeader, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 2
    char tituloLog[LARGURA_LOG + 1];
    memset(tituloLog, ' ', LARGURA_LOG);
    tituloLog[LARGURA_LOG] = '\0';
    const char *txtLog = "=== LOG ===";
    int posLog = (LARGURA_LOG - strlen(txtLog)) / 2;
    strncpy(tituloLog + posLog, txtLog, strlen(txtLog));
    
    char nomeStatusFilho[30];
    sprintf(nomeStatusFilho, "Vida (%s):", gs->nomeFilho);
    sprintf(bufferEsq, "   Vida:    %3d/%-3d       %-18s %2d/%-2d", gs->vida, MAX_VIDA_PLAYER, nomeStatusFilho, gs->filho.vida, MAX_STATUS_LEO);
    printf("%-54s  %s| %s|   %s\n", bufferEsq, asciiTempo[0], tituloLog, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 3: VIDA
    sprintf(nomeStatusFilho, "Energia (%s):", gs->nomeFilho);
    sprintf(bufferEsq, "   Energia: %3d/%-3d       %-18s %2d/%-2d", gs->energia, MAX_ENERGIA_PLAYER, nomeStatusFilho, gs->filho.energia, MAX_STATUS_LEO);
    printf("%-54s  %s| %-58s|   %s\n", bufferEsq, asciiTempo[1], logs[0], getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 4: SEGURANÇA
    sprintf(bufferEsq, "   Seguranca: %-3d%%", gs->seguranca);
    printf("%-54s  %s| %-58s|   %s\n", bufferEsq, asciiTempo[2], logs[1], getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 5, 6, 7 (Logs)
    printf("%-54s        | %-58s|   %s\n", "", logs[2], getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;
    printf("%-54s        | %-58s|   %s\n", "", logs[3], getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;
    printf("%-54s        | %-58s|   %s\n", "", logs[4], getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 8: Header Inv
    printf("%-54s        | %-58s|   %s\n", "", "            --- INVENTARIO ---", getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;
    
// LINHA 9: Inv 1 + COMIDA
    char txtItem[50], txtStat[30];
    
    sprintf(txtItem, "[ 1 ] %s", nomesItens[gs->inventario[0]]);
    sprintf(txtStat, "Comida: %-3d", gs->comida);
    sprintf(bufferInv, "%-42s%-16s", txtItem, txtStat);
    printf("%-54s        | %-58s|   %s\n", "          O                           o", bufferInv, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 10: Inv 2 + DINHEIRO
    sprintf(txtItem, "[ 2 ] %s", nomesItens[gs->inventario[1]]);
    sprintf(txtStat, "Dinheiro: $%d", gs->dinheiro); 
    sprintf(bufferInv, "%-42s%-16s", txtItem, txtStat);
    printf("%-54s        | %-58s|   %s\n", "         /|\\                         /|\\", bufferInv, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 11: Inv 3 + GASOLINA (NOVO)
    sprintf(txtItem, "[ 3 ] %s", nomesItens[gs->inventario[2]]); 
    sprintf(txtStat, "Gasolina: %-3d", gs->gasolina); // <--- ADICIONADO AQUI
    sprintf(bufferInv, "%-42s%-16s", txtItem, txtStat);
    printf("%-54s        | %-58s|   %s\n", "         / \\                         / \\", bufferInv, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA 12: Borda Intermediária
    printf("------------------------------------------------------        %s   %s\n", borderLog, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;
    
    // --- LÓGICA DE HISTÓRIA ---
    char linhasVisiveis[ALTURA_FIXA_HISTORIA][60];
    int linhasPreenchidas = 0;
    char *textoPtr = gs->textoHistoria;
    int len = strlen(gs->textoHistoria);
    int pos = 0;
    while(pos < len) {
        char bufferLinha[60];
        int charsParaCopiar = 58;
        char *pNovaLinha = strchr(textoPtr + pos, '\n');
        int distNovaLinha = pNovaLinha ? (int)(pNovaLinha - (textoPtr + pos)) : -1;
        if (distNovaLinha != -1 && distNovaLinha < charsParaCopiar) {
            charsParaCopiar = distNovaLinha;
            strncpy(bufferLinha, textoPtr + pos, charsParaCopiar);
            bufferLinha[charsParaCopiar] = '\0';
            pos += charsParaCopiar + 1; 
        } else if (pos + charsParaCopiar >= len) {
            charsParaCopiar = len - pos;
            strncpy(bufferLinha, textoPtr + pos, charsParaCopiar);
            bufferLinha[charsParaCopiar] = '\0';
            pos += charsParaCopiar;
        } else {
            int k = charsParaCopiar;
            while (k > 0 && (textoPtr + pos)[k] != ' ') k--;
            if (k > 40) { 
                charsParaCopiar = k;
                strncpy(bufferLinha, textoPtr + pos, charsParaCopiar);
                bufferLinha[charsParaCopiar] = '\0';
                pos += charsParaCopiar + 1; 
            } else {
                strncpy(bufferLinha, textoPtr + pos, 58);
                bufferLinha[58] = '\0';
                pos += 58;
            }
        }
        if (linhasPreenchidas < ALTURA_FIXA_HISTORIA) {
            strcpy(linhasVisiveis[linhasPreenchidas], bufferLinha);
            linhasPreenchidas++;
        } else {
            for(int i=0; i<ALTURA_FIXA_HISTORIA-1; i++) strcpy(linhasVisiveis[i], linhasVisiveis[i+1]);
            strcpy(linhasVisiveis[ALTURA_FIXA_HISTORIA-1], bufferLinha);
        }
    }
    
    // LINHA 13: Header História + Topo da Caixa de Local
    printf("%-54s        | %-58s|   %s\n", boxLocal[0], "             === HISTORIA ===", getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHAS 14 a 21: História
    for(int i=0; i<ALTURA_FIXA_HISTORIA; i++) {
        char *conteudoEsq = (i+1 < 9) ? boxLocal[i+1] : ""; 
        
        if (i < linhasPreenchidas) 
            printf("%-54s        | %-58s|   %s\n", conteudoEsq, linhasVisiveis[i], getLinhaVisualDiario(linhaHUD - 1));
        else 
            printf("%-54s        | %-58s|   %s\n", conteudoEsq, "", getLinhaVisualDiario(linhaHUD - 1));
        linhaHUD++;
    }

    // LINHA 22: Fim do Log
    printf("%-54s        %s   %s\n", "", borderLog, getLinhaVisualDiario(linhaHUD - 1));
    linhaHUD++;

    // LINHA FINAL: Status Horta/Aliados
    printf("   [ ALIADOS: %-2d ]     [ ADUBO: %-2d ]\n", gs->aliados, gs->estoqueAdubo);
}

int verificarAtualizacaoHistoria(GameState *gs) {
//vou deixar vazio no momento
    return 0;
}

//EVENTOS PARA QUANDO SEGURANÇA ESTÁ BAIXA
void executarEventoBaixaSeguranca(GameState *gs, int *eventoAconteceu) {
    int chanceBase = 0;
    if (gs->seguranca < 30) chanceBase = 100; 
    else {
        chanceBase = (100 - gs->seguranca) / 2;
        if (gs->horario == HORARIO_NOITE) chanceBase += 15;
    }

    int roll = rand() % 100;

    if (roll < chanceBase) {
        *eventoAconteceu = 1; 
        
        int tipoAmeaca = rand() % 4; 
        int escolha;
        char msgTemp[100];

        novoMomento();

        switch (tipoAmeaca) {
            case 0: // HORDA NA PORTA
                logMsg("[ALERTA] A horda esta invadindo!");
                renderizarHUD(gs);
                printf("1 - Segurar porta (Media)\n"); // 60% chance
                
                int temArma = 0; for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_COLT) temArma = 1;
                if (temArma && gs->municao > 0) printf("2 - Atirar (Alta)\n"); // 100% Sucesso
                else printf("2 - Atirar (Caotico)\n"); // Falha certa sem item
                
                printf("3 - Recuar (Caotico)\n"); // Perde segurança garantido
                printf("Escolha: ");
                escolha = lerInput();
                
                novoMomento();
                if (escolha == 1) {
                    if (gs->energia > 20 && rand() % 100 < 60) {
                        logMsg("Voce segurou a porta."); modificarStatus(gs, &gs->energia, -30, 100, "Energia");
                    } else {
                        logMsg("Eles arrombaram a porta!"); modificarStatus(gs, &gs->vida, -40, 100, "Vida");
                    }
                } 
                else if (escolha == 2) {
                    if (temArma && gs->municao > 0) {
                        logMsg("BANG! BANG! Voce conteve a invasao na bala.");
                        gs->municao = 0; 
                        modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca");
                    } else {
                        if (!temArma) logMsg("Voce levou a mao ao coldre... mas esta vazio!");
                        else logMsg("CLIQUE... CLIQUE... Sem municao!");
                        logMsg("Os invasores te espancaram enquanto voce falhava.");
                        modificarStatus(gs, &gs->vida, -50, 100, "Vida (Massivo)");
                    }
                } 
                else {
                    logMsg("Voce recuou e trancou a porta interna.");
                    logMsg("Eles destruiram a sala principal.");
                    modificarStatus(gs, &gs->seguranca, -30, 100, "Seguranca");
                }
                break;

            case 1: // SAQUEADORES
                logMsg("[ALERTA] Saqueadores armados!");
                renderizarHUD(gs);
                
                int temFacao = 0; for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_FACAO) temFacao = 1;
                int temColt = 0; for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_COLT) temColt = 1;
                int podeAtirar = (temColt && gs->municao > 0);

                printf("1 - Dar comida (Caotico)\n"); // Perde tudo
                printf("2 - Emboscada com facao (Alta)\n"); // Sucesso com item
                printf("3 - Blefar (Baixa)\n"); // 40% chance
                if (podeAtirar) printf("4 - Atirar (Alta)\n");
                
                printf("Escolha: ");
                escolha = lerInput();

                novoMomento();
                if (escolha == 1) { 
                    if (gs->comida > 0) { gs->comida = 0; logMsg("Voce entregou toda a comida."); } 
                    else { logMsg("'Sem comida? Entao vamos levar sangue.'"); modificarStatus(gs, &gs->vida, -25, 100, "Vida"); }
                } 
                else if (escolha == 2) {
                    if (temFacao) {
                        logMsg("Com o facao, voce eliminou a ameaca.");
                        modificarStatus(gs, &gs->vida, -10, 100, "Vida"); 
                    } else {
                        logMsg("Voce pulou neles de maos vazias? Erro fatal.");
                        modificarStatus(gs, &gs->vida, -60, 100, "Vida (Critico)");
                    }
                } 
                else if (escolha == 3) {
                    if (rand() % 100 < 40) logMsg("Seu blefe funcionou. Eles recuaram.");
                    else {
                        logMsg("Eles nao acreditaram e atiraram.");
                        modificarStatus(gs, &gs->vida, -30, 100, "Vida");
                        sprintf(msgTemp, "Vida (%s)", gs->nomeFilho); modificarStatus(gs, &gs->filho.vida, -10, MAX_STATUS_LEO, msgTemp);
                    }
                }
                else if (escolha == 4) {
                    if (podeAtirar) {
                        gs->municao--;
                        logMsg("Voce atirou no lider. O resto fugiu.");
                        modificarStatus(gs, &gs->seguranca, 15, 100, "Seguranca");
                    } else {
                        logMsg("Voce tentou sacar uma arma que nao tem!");
                        modificarStatus(gs, &gs->vida, -50, 100, "Vida");
                    }
                }
                break;

            case 2: // INFILTRADOR
                logMsg("[ALERTA] Barulho na cozinha escura.");
                renderizarHUD(gs);
                printf("1 - Usar Lanterna (Alta)\n");
                printf("2 - Ir no escuro (Baixa)\n");
                printf("3 - Trancar (Caotico)\n"); // Perde recursos
                printf("Escolha: ");
                escolha = lerInput();

                novoMomento();
                if (escolha == 1) {
                    int temLanterna = 0; for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_LANTERNA) temLanterna = 1;
                    if (temLanterna && gs->pilhas > 0) {
                        gs->pilhas--;
                        logMsg("O feixe de luz cegou o zumbi. Eliminado.");
                        modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca");
                    } else {
                        logMsg("Sem luz, o zumbi pulou em voce!");
                        modificarStatus(gs, &gs->vida, -40, 100, "Vida");
                    }
                } else if (escolha == 2) { 
                    logMsg("Voce lutou no escuro e se feriu."); modificarStatus(gs, &gs->vida, -20, 100, "Vida"); 
                } else { 
                    logMsg("Voce trancou a cozinha e perdeu suprimentos.");
                    modificarStatus(gs, &gs->seguranca, -10, 100, "Seguranca"); 
                    if(gs->comida > 0) gs->comida--;
                }
                break;

            case 3: // COLAPSO
                logMsg("[ALERTA] O teto esta caindo!");
                renderizarHUD(gs);
                printf("1 - Escorar com TABUAS (Alta)\n");
                printf("2 - Segurar (Caotico)\n"); // Perde vida garantido
                printf("3 - Correr (Caotico)\n"); // Perde segurança garantido
                printf("Escolha: ");
                escolha = lerInput();

                novoMomento();
                if (escolha == 1) {
                    int temTabua = 0; int s = -1; for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_TABUAS) {temTabua=1; s=i;}
                    if (temTabua) { gs->inventario[s] = ITEM_VAZIO; logMsg("As tabuas seguraram o teto."); } 
                    else { logMsg("Voce nao tem tabuas! O teto desabou."); modificarStatus(gs, &gs->vida, -30, 100, "Vida"); modificarStatus(gs, &gs->seguranca, -20, 100, "Seguranca"); }
                } else if (escolha == 2) { modificarStatus(gs, &gs->vida, -30, 100, "Vida"); }
                else { modificarStatus(gs, &gs->seguranca, -30, 100, "Seguranca"); }
                break;
        }
        fecharMomento(gs);
    }
}

//EXPLORAÇÃO NA CIDADE OPÇÃO 1
void resolverExploracaoCidade(GameState *gs) {
    novoMomento();
    logMsg("Voce entra nas ruinas silenciosas de Sobral...");
    modificarStatus(gs, &gs->energia, -15, MAX_ENERGIA_PLAYER, "Energia");
    
    // --- FASE 1: RECOMPENSA OU NPC ---
    int encontrouNPC = 0;
    int r = rand() % 100;

    // Tenta encontrar NPC (40% de chance) SE houver alguém para recrutar
    if (r < 40 && (!gs->npcAugusto || !gs->npcAna || !gs->npcMarcos)) {
        int npcSorteado = -1;
        while (npcSorteado == -1) {
            int k = rand() % 3;
            if (k == 0 && !gs->npcAugusto) npcSorteado = 0;
            else if (k == 1 && !gs->npcAna) npcSorteado = 1;
            else if (k == 2 && !gs->npcMarcos) npcSorteado = 2;
        }

        // --- NPC 1: AUGUSTO ---
        if (npcSorteado == 0) {
            logMsg("Encontrou um senhor mexendo num radio: 'Sou Augusto.'");
            renderizarHUD(gs); // Atualiza Log na tela
            
            printf("1 - Voce parece habilidoso. Venha para nossa base.\n");
            printf("2 - Tem alguma pilha para trocar?\n");
            printf("3 - Melhor continuarmos sos.\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) {
                logMsg("Augusto: 'Um teto seguro? Aceito. Posso consertar coisas.'");
                gs->npcAugusto = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 2) {
                logMsg("Augusto: 'Tome, tenho sobrando.'");
                modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
            } else {
                logMsg("Voces se cumprimentam e se afastam.");
            }
        }
        // --- NPC 2: ANA ---
        else if (npcSorteado == 1) {
            logMsg("Uma jovem que parece cansada: 'Sou Ana, busco suprimentos.'");
            renderizarHUD(gs);
            
            printf("1 - Achou algo interessante?\n");
            printf("2 - Cuidado por ai.\n");
            printf("3 - Precisamos de gente agil. Junte-se a nos.\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 3) {
                logMsg("Ana: 'Sozinha e dificil... Vou dar uma chance.'");
                gs->npcAna = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 1) {
                logMsg("Ana: 'Vi comida naquele mercado ali.'");
                modificarStatus(gs, &gs->comida, 2, 999, "Comida");
            } else {
                logMsg("Ela desaparece nas sombras.");
            }
        }
        // --- NPC 3: MARCOS ---
        else if (npcSorteado == 2) {
            logMsg("Um homem armado bloqueia o caminho: 'Sou Marcos.'");
            renderizarHUD(gs);
            
            printf("1 - Calma, so estamos passando.\n");
            printf("2 - Voce parece forte. Precisamos de protecao.\n");
            printf("3 - Saia da frente!\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 2) {
                logMsg("Marcos: 'Proteger algo real... sinto falta disso.'");
                gs->npcMarcos = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 3) {
                logMsg("Ele recua e voces passam tensos.");
            } else {
                logMsg("Voce passa com cuidado.");
            }
        }
        encontrouNPC = 1;
    } 
    else {
        // Recurso simples (Logs curtos)
        int tipo = rand() % 5;
        if (tipo == 0) { logMsg("Achou uma carteira velha no chao."); modificarStatus(gs, &gs->dinheiro, (rand()%20)+10, 9999, "Dinheiro"); }
        else if (tipo == 1) { logMsg("Achou um pacote de sementes."); ganharSementes(gs, 2); }
        else if (tipo == 2) { logMsg("Achou municao num carro."); modificarStatus(gs, &gs->municao, 2, 999, "Municao"); }
        else if (tipo == 3) { logMsg("Achou enlatados numa lixeira."); modificarStatus(gs, &gs->comida, 2, 999, "Comida"); }
        else { logMsg("Sifonou gasolina de uma moto abandonada."); modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina"); }
    }
    
    // Pausa para ler o resultado do NPC/Recurso antes do próximo evento
    renderizarHUD(gs);
    pausar(); 

    // --- FASE 2: O EVENTO (Carro Forte) ---
    novoMomento();
    logMsg("EVENTO: Um carro forte tombado e semi-aberto a frente.");
    logMsg("Ha dinheiro e caixas, mas a estrutura parece instavel.");
    renderizarHUD(gs); // Mostra o log do evento
    
    printf("1 - Pegar apenas o que caiu para fora\n");
    printf("2 - Entrar e pegar o que der\n");
    printf("3 - Tentar abrir o cofre trancado\n");
    printf("Escolha: ");
    
    int eventoEsc = lerInput();
    novoMomento(); // Limpa menu para mostrar resultado

    if (eventoEsc == 1) {
        logMsg("Voce pegou algumas notas e saiu em seguranca.");
        modificarStatus(gs, &gs->dinheiro, 15, 9999, "Dinheiro");
    }
    else if (eventoEsc == 2) {
        logMsg("Entrou e achou otimos itens, mas se cortou ao sair.");
        modificarStatus(gs, &gs->comida, 3, 999, "Comida");
        modificarStatus(gs, &gs->dinheiro, 50, 9999, "Dinheiro");
        modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida");
    }
    else if (eventoEsc == 3) {
        if (rand() % 100 < 80) { 
            logMsg("Sucesso! O cofre tinha municao da escolta.");
            modificarStatus(gs, &gs->municao, 4, 999, "Municao");
            modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
        } else {
            logMsg("A fechadura estava emperrada. Perdeu muito tempo.");
            modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");
        }
    }
    else {
        logMsg("Decidiu nao arriscar e seguiu caminho.");
    }
        // BÔNUS LEO NÍVEL 2
    if (gs->leoNivel >= 2 && rand() % 100 < 40) { // 40% de chance
        logMsg("Leo: 'Pai, olha o que eu achei!'");
        int r = rand() % 3;
        if (r==0) modificarStatus(gs, &gs->comida, 1, 999, "Comida (Leo)");
        else if (r==1) modificarStatus(gs, &gs->pilhas, 1, 999, "Pilhas (Leo)");
        else modificarStatus(gs, &gs->municao, 1, 999, "Municao (Leo)");
    }
    
    gs->checkSobralExplorar = 1;
    // mesmo erro de sempre
    //fecharMomento(gs);
}

//EXPLORAÇÃO NA CIDADE OPÇÃO 2 - MERCADO
void resolverMercado(GameState *gs) {
    novoMomento();
    logMsg("Voce entra no Mercado abandonado. O cheiro e forte.");
    logMsg("Ha 5 prateleiras ainda de pe no fundo.");
    renderizarHUD(gs);

    // Exibe as opções na mesma linha conforme solicitado
    printf("Escolha uma prateleira para vasculhar:\n");
    printf("[1] Prat.1   [2] Prat.2   [3] Prat.3   [4] Prat.4   [5] Prat.5\n");
    printf("Escolha: ");

    int esc = lerInput();
    
    if (esc >= 1 && esc <= 5) {
        novoMomento();
        char msg[100];
        sprintf(msg, "Voce revirou a Prateleira %d...", esc);
        logMsg(msg);
        
        // Custo de Energia
        modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

        // 1. RECURSO FIXO POR PRATELEIRA
        switch (esc) {
            case 1: // Comida
                logMsg("Encontrou latas de conserva escondidas.");
                modificarStatus(gs, &gs->comida, 3, 999, "Comida");
                break;
            case 2: // Sementes
                logMsg("Encontrou pacotes de sementes intactos.");
                ganharSementes(gs, 2);
                break;
            case 3: // Dinheiro
                logMsg("Encontrou um caixa registradora emperrada.");
                modificarStatus(gs, &gs->dinheiro, 40, 9999, "Dinheiro");
                break;
            case 4: // Munição
                logMsg("Encontrou uma caixa de balas debaixo de trapos.");
                modificarStatus(gs, &gs->municao, 3, 999, "Municao");
                break;
            case 5: // Pilhas
                logMsg("Encontrou cartelas de pilhas no balcao.");
                modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas");
                break;
        }

        // 2. ITEM ALEATÓRIO (Item do jogo)
        // Gera um ID de item entre 1 e 6 (Colt a Sinalizador)
        int itemAleatorio = (rand() % 6) + 1;
        
        // Pausa para ler o log do recurso antes de oferecer o item
        renderizarHUD(gs); 
        pausar();
        
        // Função existente que oferece o item (Log + Menu de pegar/trocar)
        oferecerItem(gs, itemAleatorio);
        
        // Marca que visitou para sumir a opção
        gs->visitouMercadoHoje = 1;
        gs->checkSobralMercado = 1;

    } else {
        logMsg("Voce hesitou e nao vasculhou nada.");
    }
}

//EXPLORAÇÃO NA CIDADE OPÇÃO 3 - FARMÁCIA
void resolverFarmacia(GameState *gs) {
    novoMomento();
    logMsg("Voce entra na Farmacia. O cheiro de alcool e mofo e forte.");
    logMsg("Muitas prateleiras estao caidas, mas 5 parecem acessiveis.");
    renderizarHUD(gs);

    printf("Escolha uma prateleira para vasculhar:\n");
    printf("[1] Fundos   [2] Balcao   [3] Esquerda   [4] Direita   [5] Vitrine\n");
    printf("Escolha: ");

    int esc = lerInput();
    
    if (esc >= 1 && esc <= 5) {
        novoMomento();
        modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

        int evento = rand() % 10;
        int ganharMedkit = 0;

        // --- LISTA DE 10 EVENTOS ALEATÓRIOS ---
        
        if (evento == 0) { // GAVETA TRANCADA
            logMsg("A prateleira tem uma gaveta de narcoticos trancada.");
            renderizarHUD(gs);
            printf("1 - Arrombar na forca bruta\n");
            printf("2 - Tentar abrir com um clipe (Habilidade)\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                logMsg("Voce se machucou e fez barulho, mas abriu.");
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                ganharMedkit = 1; // Sucesso com dor
            } else {
                if (rand()%100 < 50) { logMsg("Voce conseguiu abrir sem danos!"); ganharMedkit = 1; }
                else { logMsg("A fechadura quebrou. Nada feito."); }
            }
        }
        else if (evento == 1) { // FRASCOS SEM RÓTULO
            logMsg("Varios frascos sem rotulo estao espalhados.");
            renderizarHUD(gs);
            printf("1 - Tomar um comprimido branco\n");
            printf("2 - Cheirar o liquido azul\n");
            printf("3 - Ignorar tudo\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                if (rand()%2==0) { logMsg("Era um antibiotico potente!"); modificarStatus(gs, &gs->vida, 15, MAX_VIDA_PLAYER, "Vida"); }
                else { logMsg("Era veneno de rato..."); modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida"); }
            } else if (e == 2) { logMsg("Cheiro de alcool puro. Serve para limpar feridas."); modificarStatus(gs, &gs->vida, 5, MAX_VIDA_PLAYER, "Vida"); }
            else { logMsg("Melhor nao arriscar."); }
        }
        else if (evento == 2) { // CORPO E MOCHILA
            logMsg("Ha um corpo abracado a uma mochila medica.");
            renderizarHUD(gs);
            printf("1 - Puxar a mochila rapido\n");
            printf("2 - Verificar se ha armadilhas\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) { logMsg("O corpo estava minado com vidro!"); modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida"); }
            else { logMsg("Voce cortou as alcas com cuidado."); ganharMedkit = 1; }
        }
        else if (evento == 3) { // ESTANTE INSTÁVEL
            logMsg("A estante ameaca cair sobre voce ao mexer.");
            renderizarHUD(gs);
            printf("1 - Segurar e pegar o item (Forca)\n");
            printf("2 - Pular para tras (Agilidade)\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                if (gs->energia > 30) { logMsg("Voce segurou o peso e pegou o item."); ganharMedkit = 1; modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia"); }
                else { logMsg("Faltou forca! A estante caiu em voce."); modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida"); }
            } else { logMsg("Voce escapou ileso, mas a estante quebrou os itens."); }
        }
        else if (evento == 4) { // RATOS
            logMsg("Um ninho de ratos protege uma caixa de primeiros socorros.");
            renderizarHUD(gs);
            printf("1 - Espantar com as maos\n");
            printf("2 - Jogar algo pesado\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) { logMsg("Eles morderam voce!"); modificarStatus(gs, &gs->vida, -5, MAX_VIDA_PLAYER, "Vida"); ganharMedkit = 1; }
            else { logMsg("O barulho assustou eles, mas quebrou parte dos itens."); modificarStatus(gs, &gs->vida, 5, MAX_VIDA_PLAYER, "Vida (Bandagens)"); } // Ganha vida direta, não o item
        }
        else if (evento == 5) { // CAIXA NO ALTO
            logMsg("Uma caixa intacta esta no topo, fora de alcance.");
            renderizarHUD(gs);
            printf("1 - Escalar as prateleiras\n");
            printf("2 - Derrubar com um cabo de vassoura\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                if (rand()%100 < 60) { logMsg("Voce alcancou!"); ganharMedkit = 1; }
                else { logMsg("A prateleira cedeu e voce caiu."); modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida"); }
            } else { logMsg("A caixa caiu e se abriu. Voce pegou o que sobrou."); modificarStatus(gs, &gs->vida, 10, MAX_VIDA_PLAYER, "Vida"); }
        }
        else if (evento == 6) { // ALARME FALSO
            logMsg("Ao abrir o armario, um brinquedo a pilha comeca a tocar alto.");
            renderizarHUD(gs);
            printf("1 - Destruir o brinquedo (Barulho)\n");
            printf("2 - Tentar desligar (Tecnica)\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) { logMsg("Voce esmagou o brinquedo. O stress aumentou."); modificarStatus(gs, &gs->energia, -5, MAX_ENERGIA_PLAYER, "Energia"); }
            else { logMsg("Conseguiu desligar e aproveitar as pilhas."); modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas"); }
        }
        else if (evento == 7) { // POÇA QUÍMICA
            logMsg("Uma poca de liquido corrosivo bloqueia o caminho.");
            renderizarHUD(gs);
            printf("1 - Atravessar correndo\n");
            printf("2 - Jogar panos por cima\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) { logMsg("O vapor queimou seus pulmoes."); modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida"); ganharMedkit = 1; }
            else { logMsg("Os panos absorveram o pior. Caminho seguro."); modificarStatus(gs, &gs->vida, 5, MAX_VIDA_PLAYER, "Vida"); }
        }
        else if (evento == 8) { // ESCONDIDO
            logMsg("Voce ve um fundo falso no armario.");
            renderizarHUD(gs);
            printf("1 - Abrir devagar\n");
            printf("2 - Ignorar (Pode ser armadilha)\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) { logMsg("Sorte! Estava cheio de suprimentos medicos."); ganharMedkit = 1; }
            else { logMsg("Melhor prevenir do que remediar."); }
        }
        else { // Evento 9: VAZIO
            logMsg("A prateleira estava vazia, apenas lixo.");
            logMsg("Voce perdeu tempo vasculhando.");
        }

        // --- CONCLUSÃO ---
        if (ganharMedkit) {
            //renderizarHUD(gs);
            //pausar();
            oferecerItem(gs, ITEM_MEDKIT);
        } else {
            // Se não ganhou item, apenas pausa para ler o log do evento
            //renderizarHUD(gs);
            //pausar();
        }

        gs->visitouFarmaciaHoje = 1;
        gs->checkSobralFarmacia = 1;

    } else {
        logMsg("Voce decidiu nao mexer em nada.");
    }
}

//EXPLORAÇÃO NA CIDADE OPÇÃO 4 - PRÉDIO ABANDONADO
void resolverPredioAbandonado(GameState *gs) {
    novoMomento();
    logMsg("Voce força a porta de entrada de um residencial antigo.");
    logMsg("Subindo as escadas, voce vasculha os apartamentos abertos.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia"); // Custo
    
    // Sorteia o tipo de recurso (0 a 5)
    int tipoRecurso = rand() % 7;
    
    renderizarHUD(gs);
    pausar(); // Pausa dramática para o jogador ler a entrada
    novoMomento();

    switch (tipoRecurso) {
        case 0: // DINHEIRO
            logMsg("Num escritorio no 3o andar, voce arrombou uma gaveta.");
            logMsg("Havia um envelope com dinheiro esquecido.");
            modificarStatus(gs, &gs->dinheiro, (rand() % 40) + 20, 9999, "Dinheiro");
            break;

        case 1: // COMIDA
            logMsg("A despensa do apartamento 402 ainda tinha latas validas.");
            logMsg("Um achado de sorte na cozinha.");
            modificarStatus(gs, &gs->comida, 3, 999, "Comida");
            break;

        case 2: // PILHAS
            logMsg("No quarto de uma crianca, brinquedos eletronicos.");
            logMsg("Voce retirou as pilhas que ainda tinham carga.");
            modificarStatus(gs, &gs->pilhas, 4, 999, "Pilhas");
            break;

        case 3: // MUNIÇÃO
            logMsg("Encontrou o corpo de um policial num corredor.");
            logMsg("A arma sumiu, mas o cinto de municao ficou.");
            modificarStatus(gs, &gs->municao, 3, 999, "Municao");
            break;

        case 4: // SEMENTES (Horta)
            logMsg("Alguem tentou cultivar na varanda antes de fugir.");
            logMsg("Voce recolheu pacotes de sementes fechados.");
            ganharSementes(gs, 2);
            break;

        case 5: // ALIADOS
            logMsg("Voce ouviu um barulho no armario do corredor.");
            logMsg("Era um sobrevivente assustado que decidiu te seguir.");
            modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            break;
        case 6: // GASOLINA (NOVO)
            logMsg("Na garagem do subsolo, voce achou galoes esquecidos.");
            logMsg("O cheiro e forte, mas o combustivel e util.");
            modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina");
            break;
    }

    gs->visitouPredioHoje = 1;
    gs->checkSobralPredio = 1;
    
    //isso aqui causava um pressionar enter extra, cometi esse erro 300 vezes
    //renderizarHUD(gs);
    //pausar();
}

//VERIFICAÇÃO DE EVENTOS SCRIPTADOS
void verificarEventosScriptados(GameState *gs) {
    // Evento: Encontrar Mapa de Sobral (Dia 2, Manhã)
    if (gs->dias == 2 && gs->horario == HORARIO_MANHA && gs->desbloqueouSobral == 0) {
        novoMomento(); // Prepara para nova renderização

        // Insere a narrativa diretamente no Log
        logMsg("EVENTO: Voce tropecou numa mochila velha na entrada.");
        logMsg("Alguem passou por aqui. Nao ha comida, apenas um MAPA.");
        logMsg("O mapa indica um caminho seguro para as Ruinas de Sobral.");
        logMsg("[!] LOCAL DESBLOQUEADO: Ruinas de Sobral");
        
        // Atualiza o estado
        gs->desbloqueouSobral = 1;
        adicionarNotaDiario("Achei um mapa. Agora sei o caminho para a cidade.");
        
        // Mostra na tela e espera o jogador ler
        renderizarHUD(gs);
        pausar();
    }
}

//EXPLORAÇÃO NA CASA EM RUÍNAS
void resolverDestrocosCasa(GameState *gs) {
    novoMomento();
    logMsg("Voce entra nos destrocos da casa parcialmente desabada.");
    
    // Só gasta energia se realmente fizer algo? 
    // Vamos cobrar energia pela entrada para evitar spam, ou cobrar por cômodo.
    // Aqui cobraremos pela entrada na exploração:
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    printf("Locais disponiveis para vasculhar:\n");
    
    // Mostra (Vazio) se já pegou
    printf("[ 1 - Sala %s ]   ", gs->checkCasaSala ? "(Vazio)" : "");
    printf("[ 2 - Cozinha %s ]   ", gs->checkCasaCozinha ? "(Vazio)" : "");
    printf("[ 3 - Escritorio %s ]\n", gs->checkCasaEscritorio ? "(Vazio)" : "");
    
    printf("Escolha: ");
    int sub = lerInput();
    novoMomento();

    if (sub == 1) { // SALA
        if (!gs->checkCasaSala) {
            logMsg("No sofa rasgado, voce encontrou moedas e balas.");
            modificarStatus(gs, &gs->dinheiro, 15, 9999, "Dinheiro");
            modificarStatus(gs, &gs->municao, 2, 999, "Municao");
            gs->checkCasaSala = 1; // Marca como feito
        } else {
            logMsg("Voce ja revirou a sala. Nao ha mais nada aqui.");
        }
    }
    else if (sub == 2) { // COZINHA
        if (!gs->checkCasaCozinha) {
            logMsg("Nos armarios, achou algumas latas amassadas.");
            modificarStatus(gs, &gs->comida, 2, 999, "Comida");
            gs->checkCasaCozinha = 1; // Marca como feito
        } else {
            logMsg("A cozinha esta vazia. Voce ja pegou tudo.");
        }
    }
    else if (sub == 3) { // ESCRITÓRIO
        if (!gs->checkCasaEscritorio) {
            logMsg("Voce revirou papeis e mapas antigos na mesa.");
            logMsg("BINGO! Um mapa detalhado levando as montanhas.");
            logMsg("[ ! ] NOVO LOCAL DESBLOQUEADO: Ruinas de Planalta");
            
            gs->desbloqueouPlanalta = 1;
            adicionarNotaDiario("Descobri o caminho para Planalta na casa em ruinas.");
            //página 2 da história
            if (gs->nivelHistoria == 0) {
                atualizarHistoria(gs, 
                    "PAGINA 2: A QUEDA DE SOBRAL",
                    "'O exercito explodiu as pontes. Estamos presos.\n"
                    "Ouvi no radio que montaram um campo de triagem nas\n"
                    "montanhas de Planalta. Tenho que subir a serra.'");
                gs->nivelHistoria = 1;
            }
            
            gs->checkCasaEscritorio = 1; 
        } else {
            logMsg("O escritorio esta revirado. O mapa ja esta com voce.");
        }
    }
    else {
        logMsg("Voce hesitou e nao vasculhou nada.");
    }
    
    // Não chamamos renderizarHUD/pausar aqui para evitar "duplo enter" no loop principal
}

//EXPLORAÇÃO NA SERRA DE PLANALTA
void resolverExploracaoPlanalta(GameState *gs) {
    novoMomento();
    logMsg("Voce caminha pela neblina fria de Planalta."); // Encurtado
    modificarStatus(gs, &gs->energia, -15, MAX_ENERGIA_PLAYER, "Energia");

    // --- FASE 1: RECOMPENSA OU NPC ---
    int r = rand() % 100;

    if (r < 40 && (!gs->npcMiguel || !gs->npcClaudio || !gs->npcElisa)) {
        int npcSorteado = -1;
        while (npcSorteado == -1) {
            int k = rand() % 3;
            if (k == 0 && !gs->npcMiguel) npcSorteado = 0;
            else if (k == 1 && !gs->npcClaudio) npcSorteado = 1;
            else if (k == 2 && !gs->npcElisa) npcSorteado = 2;
        }

        if (npcSorteado == 0) { // MIGUEL
            logMsg("Um padre enterra um corpo: 'Orem por ele.'"); // Encurtado
            renderizarHUD(gs);
            printf("1 - Deus nos abandonou. Venha ou morra.\n");
            printf("2 - Estamos construindo um santuario seguro.\n");
            printf("3 - Tome esta comida e fique bem.\n");
            printf("Escolha: ");
            int esc = lerInput();
            if (esc == 2) {
                logMsg("Miguel: 'Um rebanho precisa de pastor... Eu irei.'");
                gs->npcMiguel = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 3) {
                logMsg("Miguel: 'Agradeco. Que a paz esteja convosco.'");
                modificarStatus(gs, &gs->energia, 10, MAX_ENERGIA_PLAYER, "Energia");
            } else { logMsg("Ele continua rezando, ignorando voce."); }
        }
        else if (npcSorteado == 1) { // CLÁUDIO
            logMsg("Um militar limpa o fuzil: 'Disciplina e tudo.'"); // Encurtado
            renderizarHUD(gs);
            printf("1 - Voce parece exausto. Deixe-me ajudar.\n");
            printf("2 - Temos suprimentos, mas falta ordem. Venha.\n");
            printf("3 - Bela arma. Troca por pilhas?\n");
            printf("Escolha: ");
            int esc = lerInput();
            if (esc == 2) {
                logMsg("Claudio: 'Civis nao tem organizacao. Assumo a seguranca.'");
                gs->npcClaudio = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 3) {
                logMsg("Claudio: 'Negativo. Mas tenho municao sobrando.'");
                modificarStatus(gs, &gs->municao, 3, 999, "Municao");
            } else { logMsg("Claudio: 'Nao preciso de pena. Circulando.'"); }
        }
        else if (npcSorteado == 2) { // ELISA
            logMsg("Uma mulher coleta amostras de um zumbi morto.");
            renderizarHUD(gs);
            printf("1 - Isso e perigoso! Saia dai!\n");
            printf("2 - Achou a cura?\n");
            printf("3 - Temos energia e laboratorio seguro. Sem riscos.\n");
            printf("Escolha: ");
            int esc = lerInput();
            if (esc == 3) {
                logMsg("Elisa: 'Estrutura estavel? Levarei minhas amostras.'");
                gs->npcElisa = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } else if (esc == 2) {
                logMsg("Elisa: 'Nao seja ingenuo. Mas achei sementes raras.'");
                ganharSementes(gs, 2);
            } else { logMsg("Ela revira os olhos: 'Nao me atrapalhe.'"); }
        }
    } 
    else {
        // Loot
        int tipo = rand() % 4;
        if (tipo == 0) { logMsg("Achou casacos de frio em bom estado."); modificarStatus(gs, &gs->energia, 10, MAX_ENERGIA_PLAYER, "Energia"); }
        else if (tipo == 1) { logMsg("Encontrou ferramentas numa oficina."); modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca"); }
        else if (tipo == 2) { logMsg("Achou um galao de diesel."); modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina"); }
        else { logMsg("Encontrou enlatados militares."); modificarStatus(gs, &gs->comida, 3, 999, "Comida"); }
    }
    
    renderizarHUD(gs);
    pausar();

    // --- FASE 2: EVENTO ALEATÓRIO (Encurtado) ---
    novoMomento();
    int idEvento = rand() % 4;

    if (idEvento == 0) { // DESLIZAMENTO
        logMsg("EVENTO: Um caminhao soterrado por um deslizamento.");
        logMsg("A porta esta acessivel, mas a terra estala acima."); // Encurtado
        renderizarHUD(gs);
        printf("1 - Entrar rapido e pegar o que der\n");
        printf("2 - Usar tabuas para escorar a terra antes\n");
        printf("3 - Ignorar e seguir\n");
        printf("Escolha: ");
        int esc = lerInput();
        novoMomento();

        if (esc == 1) {
            if (rand() % 100 < 50) {
                logMsg("A terra cedeu! Voce escapou, mas perdeu itens."); // Encurtado
                if(gs->comida > 0) gs->comida--;
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
            } else {
                logMsg("Arriscado, mas valeu a pena. Achou suprimentos.");
                modificarStatus(gs, &gs->comida, 2, 999, "Comida");
                modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
            }
        }
        else if (esc == 2) {
            int temTabuas = -1;
            for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_TABUAS) temTabuas = i;
            if (temTabuas != -1) {
                gs->inventario[temTabuas] = ITEM_VAZIO;
                logMsg("Voce escorou a terra. Achou um estoque medico!");
                oferecerItem(gs, ITEM_MEDKIT); 
            } else {
                logMsg("Sem tabuas! Tentou improvisar e se machucou.");
                modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else { logMsg("Voce seguiu caminho."); }
    }
    
    else if (idEvento == 1) { // PONTE
        logMsg("EVENTO: Uma ponte de cordas sobre um abismo."); // Encurtado
        logMsg("Uma mochila de acampamento esta presa no meio."); // Encurtado
        renderizarHUD(gs);
        printf("1 - Ir engatinhando devagar buscar a mochila\n");
        printf("2 - Correr, pegar e voltar\n");
        printf("3 - Nao vale o risco\n");
        printf("Escolha: ");
        int esc = lerInput();
        novoMomento();

        if (esc == 1) {
            logMsg("Levou horas, tremendo de frio. Mas conseguiu."); // Encurtado
            modificarStatus(gs, &gs->energia, -20, MAX_ENERGIA_PLAYER, "Energia");
            modificarStatus(gs, &gs->comida, 3, 999, "Comida");
            if(rand()%2==0) oferecerItem(gs, ITEM_LANTERNA);
        }
        else if (esc == 2) {
            if (rand() % 100 < 30) {
                logMsg("Voce pisou numa tabua podre e quase caiu!");
                logMsg("Perdeu itens na correria para voltar.");
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                if(gs->pilhas > 0) gs->pilhas--;
            } else {
                logMsg("Sorte insana! Foi e voltou num folego so.");
                modificarStatus(gs, &gs->municao, 4, 999, "Municao");
            }
        }
        else { logMsg("Olhar para baixo deu vertigem. Voce recuou."); }
    }

    else if (idEvento == 2) { // PREDADOR
        logMsg("EVENTO: Rosnados vem de uma garagem escura."); // Encurtado
        logMsg("Olhos brilhantes guardam caixas de suprimento."); // Encurtado
        renderizarHUD(gs);
        printf("1 - Atirar na escuridao\n");
        printf("2 - Jogar carne para distrair\n");
        printf("3 - Gritar e fazer barulho\n");
        printf("Escolha: ");
        int esc = lerInput();
        novoMomento();

        if (esc == 1) {
            int temColt = 0;
            for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_COLT) temColt=1;
            if (temColt && gs->municao > 0) {
                gs->municao--;
                logMsg("Ouviu-se um ganido. O animal fugiu. Achou gasolina."); // Encurtado
                modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina");
            } else {
                logMsg("Sem arma carregada? O lobo atacou voce!");
                modificarStatus(gs, &gs->vida, -25, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else if (esc == 2) {
            if (gs->comida > 0) {
                gs->comida--;
                logMsg("O animal pegou a comida. Voce pegou pilhas."); // Encurtado
                modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas");
            } else {
                logMsg("Voce nao tem comida! O animal avancou.");
                modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else {
            if (rand() % 100 < 50) {
                logMsg("O barulho assustou o bicho. Voce pegou um item.");
                oferecerItem(gs, ITEM_SINALIZADOR);
            } else {
                logMsg("O bicho se sentiu desafiado e atacou!");
                modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
            }
        }
    }

    else if (idEvento == 3) { // MINA
        logMsg("EVENTO: Um fio quase invisivel cruza a trilha.");
        logMsg("Parece uma armadilha protegendo um bau."); // Encurtado
        renderizarHUD(gs);
        printf("1 - Tentar desarmar o mecanismo\n");
        printf("2 - Pular por cima\n");
        printf("3 - Dar a volta\n");
        printf("Escolha: ");
        int esc = lerInput();
        novoMomento();

        if (esc == 1) {
            if (gs->energia > 30) {
                logMsg("Com maos firmes, voce desativou e pegou o bau."); // Encurtado
                modificarStatus(gs, &gs->dinheiro, 50, 9999, "Dinheiro");
                modificarStatus(gs, &gs->municao, 3, 999, "Municao");
            } else {
                logMsg("Suas maos tremeram. BOOM!");
                modificarStatus(gs, &gs->vida, -30, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else if (esc == 2) {
            if (rand() % 100 < 40) {
                logMsg("Voce tropecou na aterrissagem e ativou a carga.");
                modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
            } else {
                logMsg("Salto perfeito. Mas nao conseguiu pegar o bau.");
            }
        }
        else { logMsg("Seguro morreu de velho. Voce ignorou o bau."); }
    }
        // BÔNUS LEO NÍVEL 2
    if (gs->leoNivel >= 2 && rand() % 100 < 40) { // 40% de chance
        logMsg("Leo: 'Pai, olha o que eu achei!'");
        int r = rand() % 3;
        if (r==0) modificarStatus(gs, &gs->comida, 1, 999, "Comida (Leo)");
        else if (r==1) modificarStatus(gs, &gs->pilhas, 1, 999, "Pilhas (Leo)");
        else modificarStatus(gs, &gs->municao, 1, 999, "Municao (Leo)");
    }
}

//EXPLORAÇÃO NO HOSPITAL DE PLANALTA
void resolverHospital(GameState *gs) {
    novoMomento();
    logMsg("O Hospital de Planalta cheira a formol e sangue seco.");
    logMsg("O gerador de emergencia zune baixo em algum lugar.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    printf("Qual ala voce deseja vasculhar?\n");
    printf("[1] Emergencia  [2] Cirurgia  [3] Maternidade  [4] Necroterio\n");
    printf("Escolha: ");

    int esc = lerInput();
    
    if (esc >= 1 && esc <= 4) {
        novoMomento();
        int evento = rand() % 8; // 8 Eventos possíveis
        int ganharMedkit = 0;

        // --- EVENTOS DO HOSPITAL ---

        if (evento == 0) { // ARMÁRIO DE VIDRO
            logMsg("Um armario de vidro trancado exibe suprimentos.");
            renderizarHUD(gs);
            printf("1 - Quebrar o vidro\n");
            printf("2 - Tentar abrir a fechadura\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                logMsg("O barulho ecoou e os cacos te cortaram.");
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                ganharMedkit = 1;
            } else {
                if (rand()%100 < 40) { logMsg("Click. Abriu silenciosamente."); ganharMedkit = 1; }
                else { logMsg("A fechadura emperrou de vez."); }
            }
        }
        else if (evento == 1) { // ENFERMEIRA ZUMBI
            logMsg("Uma enfermeira zumbi esta de costas, bloqueando o corredor.");
            renderizarHUD(gs);
            printf("1 - Atacar de surpresa\n");
            printf("2 - Jogar algo para atrair ela para longe\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                logMsg("Ela resistiu mais do que devia e te arranhou.");
                modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida");
                ganharMedkit = 1; // Ela tinha itens no bolso
            } else {
                logMsg("Ela seguiu o som. O caminho ficou livre.");
                logMsg("Voce achou pilhas no balcao.");
                modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
            }
        }
        else if (evento == 2) { // CILINDRO DE OXIGÊNIO
            logMsg("Um cilindro de gas instavel bloqueia uma porta.");
            renderizarHUD(gs);
            printf("1 - Mover o cilindro\n");
            printf("2 - Passar se espremendo\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                logMsg("A valvula quebrou! O gas explodiu.");
                modificarStatus(gs, &gs->vida, -25, MAX_VIDA_PLAYER, "Vida");
            } else {
                logMsg("Voce passou raspando. Achou remedios na sala.");
                modificarStatus(gs, &gs->vida, 10, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else if (evento == 3) { // MACA COBERTA
            logMsg("Ha um corpo numa maca coberto por um lencol limpo.");
            renderizarHUD(gs);
            printf("1 - Revistar o corpo\n");
            printf("2 - Ignorar\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                if (rand()%2==0) { 
                    logMsg("O 'corpo' acordou e tentou te morder!"); 
                    modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida"); 
                } else { 
                    logMsg("Era apenas um corpo. Tinha curativos no bolso."); 
                    modificarStatus(gs, &gs->vida, 5, MAX_VIDA_PLAYER, "Vida");
                }
            } else { logMsg("Melhor nao perturbar os mortos."); }
        }
        else if (evento == 4) { // LUZ PISCANDO
            logMsg("A luz de uma sala de cirurgia pisca. Ha algo na mesa.");
            renderizarHUD(gs);
            printf("1 - Usar lanterna e investigar\n");
            printf("2 - Ir no escuro mesmo\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                int temLanterna = 0; for(int i=0;i<3;i++) if(gs->inventario[i]==ITEM_LANTERNA) temLanterna=1;
                if(temLanterna && gs->pilhas > 0) {
                    gs->pilhas--;
                    logMsg("A luz revelou buracos no chao. Voce evitou cair.");
                    ganharMedkit = 1;
                } else {
                    logMsg("Sem lanterna funcional. Voce tropecou feio.");
                    modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                }
            } else {
                logMsg("Voce caiu numa armadilha de chao quebrada!");
                modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
            }
        }
        else if (evento == 5) { // ARQUIVO MEDICO
            logMsg("Fichas medicas espalhadas. Uma sala parece intacta.");
            renderizarHUD(gs);
            printf("1 - Ler as fichas primeiro\n");
            printf("2 - Entrar na sala direto\n");
            printf("Escolha: ");
            int e = lerInput();
            novoMomento();
            if (e == 1) {
                logMsg("As fichas avisavam sobre 'Paciente Zero' na sala.");
                logMsg("Voce evitou entrar e se contaminar.");
                modificarStatus(gs, &gs->energia, 5, MAX_ENERGIA_PLAYER, "Energia");
            } else {
                logMsg("A sala estava contaminada com esporos!");
                modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida (Tosse)");
            }
        }
        else if (evento == 6) { // CAIXA FORTE
            logMsg("Uma caixa de remedios controlados.");
            ganharMedkit = 1; // Sorte pura
        }
        else { // NADA
            logMsg("A ala estava completamente saqueada e vazia.");
        }

        // --- CONCLUSÃO ---
        if (ganharMedkit) {
            // Log já foi exibido, agora oferece o item
            oferecerItem(gs, ITEM_MEDKIT);
        }

        gs->visitouHospitalHoje = 1;

    } else {
        logMsg("O ambiente hospitalar te assustou. Voce saiu.");
    }
}

//EXPLORAÇÃO NA RODOVIÁRIA DE PLANALTA
void resolverRodoviaria(GameState *gs) {
    novoMomento();
    // Textos ajustados para caber no Log (< 58 chars)
    logMsg("Um engavetamento de onibus bloqueia a via."); 
    logMsg("Eles formam um tunel de metal escuro e enferrujado.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    // Acumuladores temporários
    int tempDinheiro = 0;
    int tempComida = 0;
    int tempMunicao = 0;
    int tempGasolina = 0;
    
    int onibusAtual = 1;
    int continuar = 1;
    int perdeuTudo = 0;

    // Loop do Comboio
    while (continuar && onibusAtual <= 5) {
        renderizarHUD(gs);
        printf("=== ONIBUS %d/5 ===\n", onibusAtual);
        
        // 1. CÁLCULO DE RISCO
        int chanceAtaque = (onibusAtual - 1) * 20; 
        int roll = rand() % 100;

        // 2. VERIFICAÇÃO DE SORTE
        if (roll < chanceAtaque) {
            // FALHA
            novoMomento();
            logMsg("BARULHO! Uma horda dormia nos bancos!");
            
            // Frase longa dividida em duas para não cortar:
            logMsg("Voce fugiu por uma janela quebrada..."); 
            logMsg("...deixando todo o saque para tras.");
            
            int dano = 10 * onibusAtual;
            modificarStatus(gs, &gs->vida, -dano, MAX_VIDA_PLAYER, "Vida");
            
            perdeuTudo = 1;
            continuar = 0;
        } 
        else {
            // SUCESSO: GERAR LOOT
            int tipoLoot = rand() % 4;
            printf("Loot encontrado neste veiculo:\n");
            
            if (tipoLoot == 0) {
                int qtd = (rand() % 3) + 1;
                tempComida += qtd;
                printf("- Comida: +%d\n", qtd);
            } 
            else if (tipoLoot == 1) {
                int qtd = (rand() % 20) + 10;
                tempDinheiro += qtd;
                printf("- Dinheiro: +$%d\n", qtd);
            }
            else if (tipoLoot == 2) {
                int qtd = (rand() % 3) + 1;
                tempMunicao += qtd;
                printf("- Municao: +%d\n", qtd);
            }
            else {
                int qtd = 1;
                tempGasolina += qtd;
                printf("- Gasolina: +%d\n", qtd);
            }

            // 3. DECISÃO
            printf("\n--- SUA MOCHILA TEMPORARIA ---\n");
            printf("Dinheiro: $%d | Comida: %d | Municao: %d | Gasolina: %d\n\n", 
                   tempDinheiro, tempComida, tempMunicao, tempGasolina);
            
            if (onibusAtual < 5) {
                printf("O proximo onibus parece mais perigoso...\n");
                printf("1 - Avancar para o proximo onibus\n");
                printf("2 - Sair agora e garantir o saque\n");
                printf("Escolha: ");
                
                int esc = lerInput();
                if (esc == 2) {
                    continuar = 0;
                } else {
                    onibusAtual++; 
                    novoMomento();
                    logMsg("Voce forca a porta e entra no proximo...");
                }
            } else {
                printf("Este foi o ultimo onibus da fila.\n");
                printf("1 - Sair vitorioso\n");
                lerInput(); 
                continuar = 0;
            }
        }
    }

    // CONSOLIDAÇÃO DO SAQUE
    if (!perdeuTudo) {
        if (onibusAtual >= 4 && !gs->fragRodoviaria) {
            logMsg("O motorista morto segurava um papel rasgado (2/3).");
            gs->fragRodoviaria = 1;
            checarFragmentosSamamba(gs);
        }
        novoMomento();
        logMsg("Voce saiu da rodoviaria com a mochila cheia.");
        
        // Correção: usando modificarStatus corretamente
        if (tempDinheiro > 0) modificarStatus(gs, &gs->dinheiro, tempDinheiro, 9999, "Dinheiro");
        if (tempComida > 0) modificarStatus(gs, &gs->comida, tempComida, 999, "Comida");
        if (tempMunicao > 0) modificarStatus(gs, &gs->municao, tempMunicao, 999, "Municao");
        if (tempGasolina > 0) modificarStatus(gs, &gs->gasolina, tempGasolina, 999, "Gasolina");
        
        // Chance de item raro
        if (onibusAtual >= 4 && rand() % 100 < 50) {
            logMsg("No fundo da mochila, achou algo extra!");
            oferecerItem(gs, ITEM_MEDKIT); 
        }
    }

    gs->visitouRodoviariaHoje = 1;
    
    // Comentado para evitar duplo enter no loop principal
    // renderizarHUD(gs);
    // pausar();
}

//EXPLORAÇÃO NA PRAÇA CENTRAL DE PLANALTA
void resolverPraca(GameState *gs) {
    novoMomento();
    // Textos ajustados para não cortar
    logMsg("A neblina na Praca e espessa.");
    logMsg("Vultos estao parados. Zumbis em hibernacao.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    int limiteRuido = 60 + (rand() % 41); 
    int ruidoAtual = 0;
    
    int lootPlayground = 0;
    int lootFonte = 0;
    int lootCoreto = 0;
    int lootCorpos = 0;
    
    int sair = 0;
    int acordouHorda = 0;

    while (!sair && !acordouHorda) {
        renderizarHUD(gs);
        
        // Feedback visual do ambiente (sem números)
        if (ruidoAtual > 0 && ruidoAtual < 40) printf("[!] O silencio foi quebrado. Alguns vultos se mexem.\n");
        else if (ruidoAtual >= 40) printf("[!!!] PERIGO: Zumbis inquietos! Estalos por toda parte.\n");
        else printf("[ ] O ambiente esta mortalmente silencioso.\n");
        
        printf("\nLocais para vasculhar:\n");
        
        // --- MENU DINÂMICO ---
        int opNum = 1;
        int opPlay = 0;
        int opFonte = 0;
        int opCoreto = 0;
        int opCorpos = 0;

        if (!lootPlayground) {
            printf("%d - O Playground (Chao de terra batida)\n", opNum);
            opPlay = opNum++;
        }
        if (!lootFonte) {
            printf("%d - A Fonte Seca (Piso de ladrilho)\n", opNum);
            opFonte = opNum++;
        }
        if (!lootCoreto) {
            printf("%d - O Coreto de Madeira (Estrutura velha)\n", opNum);
            opCoreto = opNum++;
        }
        if (!lootCorpos) {
            printf("%d - Saquear corpos no caminho\n", opNum);
            opCorpos = opNum++;
        }
        
        printf("0 - Sair da Praca (Seguranca)\n");
        printf("Escolha: ");

        int esc = lerInput();
        novoMomento();
        
        int gerouRuido = 0;

        if (esc == 0) {
            logMsg("Voce saiu da area de perigo na ponta dos pes.");
            sair = 1;
        }
        else if (opPlay != 0 && esc == opPlay) { // PLAYGROUND
            logMsg("A areia abafou seus passos. Achou itens.");
            modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
            
            char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs->nomeFilho);
            modificarStatus(gs, &gs->filho.energia, 5, MAX_STATUS_LEO, msgTemp);
            
            gerouRuido = 10 + (rand() % 10); 
            lootPlayground = 1;
        }
        else if (opFonte != 0 && esc == opFonte) { // FONTE
            logMsg("Botas estalaram no piso. Havia moedas.");
            modificarStatus(gs, &gs->dinheiro, 25, 9999, "Dinheiro");
            
            gerouRuido = 25 + (rand() % 10); 
            lootFonte = 1;
        }
        else if (opCoreto != 0 && esc == opCoreto) { // CORETO
            logMsg("A madeira rangeu alto! Achou estoque militar.");
            modificarStatus(gs, &gs->municao, 4, 999, "Municao");
            modificarStatus(gs, &gs->comida, 2, 999, "Comida");
                if (!gs->fragPraca) {
                logMsg("Pregado na viga do teto, um papel rasgado (3/3).");
                gs->fragPraca = 1;
                checarFragmentosSamamba(gs); 
                }
            gerouRuido = 40 + (rand() % 15); 
            lootCoreto = 1;
        }
        else if (opCorpos != 0 && esc == opCorpos) { // CORPOS
            logMsg("Voce revistou os bolsos dos mortos.");
            int rLoot = rand() % 3;
            if (rLoot == 0) modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina");
            else if (rLoot == 1) modificarStatus(gs, &gs->dinheiro, 15, 9999, "Dinheiro");
            else oferecerItem(gs, ITEM_FACAO); 
            
            gerouRuido = 15 + (rand() % 20); 
            lootCorpos = 1;
        }
        else {
            // Opção inválida
        }

        // VERIFICAÇÃO DE RUIDO
        if (gerouRuido > 0) {
            ruidoAtual += gerouRuido;
            
            if (ruidoAtual > limiteRuido) {
                acordouHorda = 1;
                novoMomento();
                logMsg("CRACK! O barulho foi alto demais.");
                logMsg("Todos os vultos olharam para voce."); // Encurtado
                renderizarHUD(gs);
                pausar();
                
                logMsg("A horda acordou! Voce correu desesperado.");
                logMsg("Eles te arranharam e morderam na fuga.");
                modificarStatus(gs, &gs->vida, -40, MAX_VIDA_PLAYER, "Vida");
                modificarStatus(gs, &gs->energia, -20, MAX_ENERGIA_PLAYER, "Energia");
            }
        }
        
        // Saída automática se pegou tudo
        if (lootPlayground && lootFonte && lootCoreto && lootCorpos && !acordouHorda) {
            logMsg("Nao ha mais nada aqui. Voce saiu em silencio.");
            sair = 1;
        }
    }

    gs->visitouPracaHoje = 1;
}

//EXPLORAÇÃO NA ESCOLA DE PLANALTA
void resolverEscola(GameState *gs) {
    novoMomento();
    logMsg("A Escola esta silenciosa. Voce vai a diretoria.");
    logMsg("A sala do Diretor e blindada. Ha um teclado na porta.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    // Configuração do Minigame
    int senhaCorreta = rand() % 101; // 0 a 100
    int tentativas = 5;
    int abriu = 0;
    int desistiu = 0;

    printf("\n[ SISTEMA DE SEGURANCA ATIVO ]\n");
    printf("Adivinhe a senha numerica (0 a 100).\n");

    while (tentativas > 0 && !abriu && !desistiu) {
        printf("\nTentativas restantes: %d\n", tentativas);
        printf("Digite a senha (-1 para desistir): ");
        
        int chute = lerInput();
        
        if (chute == -1) {
            desistiu = 1;
            logMsg("Voce desistiu de tentar abrir a porta.");
        }
        else if (chute == senhaCorreta) {
                if (!gs->fragEscola) {
            logMsg("No fundo do cofre, um pedaco de papel rasgado (1/3).");
            gs->fragEscola = 1;
            checarFragmentosSamamba(gs); // Verifica se completou
        }
            abriu = 1;
            novoMomento();
            logMsg("SENHA CORRETA. A tranca magnetica se solta.");
            renderizarHUD(gs);
            pausar();
        }
        else {
            novoMomento();
            // Feedback do Minigame
            if (chute > senhaCorreta) logMsg("[ERRO] A senha e MENOR.");
            else logMsg("[ERRO] A senha e MAIOR.");
            
            // Penalidade por erro
            tentativas--;
            
            if (tentativas == 0) {
                logMsg("ALARMES DISPARADOS! O sistema bloqueou.");
                logMsg("O barulho atraiu zumbis. Voce fugiu.");
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
            } else {
                renderizarHUD(gs); // Mostra o log de erro e pede novo input
            }
        }
    }

    // --- SUCESSO AO ABRIR ---
    if (abriu) {
        novoMomento();
            if (gs->nivelHistoria == 1) {
            atualizarHistoria(gs, 
                "PAGINA 3: O FRIO DA SERRA",
                "'Planalta caiu. Até a escola virou um matadouro.\n"
                "Um professor disse que a pesquisa foi levada para Samamba.\n"
                "A cidade grande me assusta, mas nao tenho escolha.'");
            gs->nivelHistoria = 2;
            }
        // PRIMEIRO ENCONTRO (NPC ALBERTO)
        if (!gs->checkEscolaCofre) {
            logMsg("Dentro da sala, um homem de terno aponta uma arma.");
            logMsg("Diretor Alberto: 'Como descobriu a senha?!'");
            renderizarHUD(gs);
            
            printf("1 - 'Foi sorte. Mas temos um lugar seguro na base.'\n");
            printf("2 - 'Sou bom com numeros. Voce precisa de gestao.'\n");
            printf("3 - 'Abaixe a arma ou eu atiro.'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            novoMomento();
            
            if (esc == 2) {
                logMsg("Alberto: 'Gestao... faz tempo que nao ouco isso.'");
                logMsg("Alberto: 'Irei com voce. Alguem precisa por ordem.'");
                gs->npcAlberto = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados");
            } 
            else if (esc == 1) {
                logMsg("Alberto: 'Sorte nao mantem ninguem vivo. Saia.'");
                logMsg("Ele te expulsou, mas voce pegou suprimentos antes.");
                modificarStatus(gs, &gs->comida, 2, 999, "Comida");
            } 
            else {
                logMsg("Ele disparou! Voce revidou e ele fugiu ferido.");
                modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                logMsg("Pelo menos o estoque ficou para tras.");
                modificarStatus(gs, &gs->municao, 2, 999, "Municao");
            }
            
            gs->checkEscolaCofre = 1; // Marca que o evento já ocorreu
        } 
        // VISITAS FUTURAS (LOOT APENAS)
        else {
            logMsg("A sala do diretor esta aberta.");
            logMsg("Voce vasculhou o estoque de confiscados.");
            
            int r = rand() % 3;
            if (r == 0) {
                modificarStatus(gs, &gs->comida, 3, 999, "Comida (Merenda)");
            } else if (r == 1) {
                modificarStatus(gs, &gs->municao, 3, 999, "Municao (Confiscada)");
            } else {
                logMsg("Encontrou um item raro na gaveta!");
                oferecerItem(gs, ITEM_LANTERNA);
            }
        }
    }

    gs->visitouEscolaHoje = 1;
}

//fragmentos do mapa checagem
void checarFragmentosSamamba(GameState *gs) {
    if (gs->fragEscola && gs->fragRodoviaria && gs->fragPraca && !gs->desbloqueouSamamba) {
        novoMomento();
        logMsg("EVENTO: Voce juntou os tres pedacos de papel rasgado.");
        logMsg("Com fita adesiva, o mapa se revela.");
        logMsg("[ ! ] NOVO LOCAL DESBLOQUEADO: Ruinas de Samamba");
        
        gs->desbloqueouSamamba = 1;
        adicionarNotaDiario("Reuni o mapa do Cartografo. O destino e Samamba.");
        
        renderizarHUD(gs);
        pausar();
    }
}

//resolver feira
void resolverFeira(GameState *gs) {
    novoMomento();
    logMsg("A Feira e um labirinto de lonas rasgadas e cheiro de podre.");
    logMsg("Voce observa as barracas tentando ver o que ha dentro.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    // 0 = Vazio, 1 = Loot, 2 = Zumbi
    int grid[9];
    
    // Geração do Tabuleiro
    for (int i = 0; i < 9; i++) {
        int r = rand() % 100;
        if (r < 30) grid[i] = 2;      // 30% Zumbi [Z]
        else if (r < 70) grid[i] = 1; // 40% Loot  [$]
        else grid[i] = 0;             // 30% Vazio [.]
    }

    // --- ETAPA 1: MEMORIZAÇÃO ---
    printf("\n--- MEMORIZE AS BARRACAS ---\n\n");
    
    for (int i = 0; i < 9; i++) {
        char conteudo[5];
        if (grid[i] == 2) strcpy(conteudo, "[Z]"); // Zumbi
        else if (grid[i] == 1) strcpy(conteudo, "[$]"); // Loot
        else strcpy(conteudo, "[.]"); // Vazio
        
        printf("%d:%s  ", i + 1, conteudo);
        
        // Quebra de linha a cada 3 itens
        if ((i + 1) % 3 == 0) printf("\n\n");
    }

    printf("\nDigite 0 e pressione ENTER para esconder o tabuleiro e responder: ");
    int temp = lerInput(); // O jogo vai parar aqui e esperar você digitar
    // --- ETAPA 2: ESCOLHA ---
    limparTela(); // O SEGREDO DO JOGO: Limpar a tela
    renderizarHUD(gs); // Redesenha o HUD, mas sem o grid revelado
    
    printf("Onde estava o suprimento seguro?\n\n");
    printf("[ 1 ]  [ 2 ]  [ 3 ]\n");
    printf("[ 4 ]  [ 5 ]  [ 6 ]\n");
    printf("[ 7 ]  [ 8 ]  [ 9 ]\n\n");
    
    printf("Escolha o numero da barraca: ");
    int escolha = lerInput();
    novoMomento();

    if (escolha >= 1 && escolha <= 9) {
        int indice = escolha - 1;
        int conteudo = grid[indice];

        if (conteudo == 1) { // LOOT [$]
            logMsg("Memoria boa! Voce entrou na barraca certa.");
            int tipo = rand() % 3;
            if (tipo == 0) {
                logMsg("Encontrou vegetais que ainda serviam.");
                modificarStatus(gs, &gs->comida, 3, 999, "Comida");
            } else if (tipo == 1) {
                logMsg("Havia uma caixa de ferramentas.");
                modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas");
            } else {
                logMsg("Alguem escondeu dinheiro aqui.");
                modificarStatus(gs, &gs->dinheiro, 40, 9999, "Dinheiro");
            }
        } 
        else if (conteudo == 2) { // ZUMBI [Z]
            logMsg("ERRO FATAL! Havia um infectado escondido!");
            logMsg("Voce abriu a lona e ele pulou no seu pescoco.");
            modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
        } 
        else { // VAZIO [.]
            logMsg("A barraca estava vazia. Voce perdeu tempo.");
        }
    } else {
        logMsg("Voce hesitou e perdeu a oportunidade.");
    }

    gs->visitouFeiraHoje = 1;
}

//resolver predio samamba
void resolverPredioSamamba(GameState *gs) {
    novoMomento();
    logMsg("O Edificio Jardins Suspensos e alto e coberto de mato.");
    logMsg("O elevador caiu. A unica via e a escadaria de incendio externa.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    int andarAtual = 0;
    int andarMaximo = 10;
    int folego = 100;
    int desistiu = 0;
    int caiu = 0;

    while (andarAtual < andarMaximo && !desistiu && !caiu) {
        renderizarHUD(gs);
        
        printf("\n=== ESCADARIA: ANDAR %d / %d ===\n", andarAtual, andarMaximo);
        printf("[ FOLEGO: %d%% ]\n\n", folego);
        
        if (folego <= 20) printf("[!] ATENCAO: Voce esta tonto de exaustao!\n");

        printf("1 - Subir Rapido (+2 Andares, -25 Folego, Risco Zumbi)\n");
        printf("2 - Subir Seguro (+1 Andar,   -15 Folego, Sem Risco)\n");
        printf("3 - Descansar    (+30 Folego,  Alto Risco de Zumbi)\n");
        printf("4 - Desistir e descer (Sai com a vida, sem itens)\n");
        printf("Escolha: ");

        int esc = lerInput();
        novoMomento();

        if (esc == 1) { // RÁPIDO
            folego -= 25;
            if (folego > 0) {
                andarAtual += 2;
                if (andarAtual > andarMaximo) andarAtual = andarMaximo;
                
                logMsg("Voce pulou degraus correndo.");
                // 20% de chance de Zumbi
                if (rand() % 100 < 20) {
                    logMsg("Um zumbi tentou agarrar seu pe!");
                    modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
                }
            } else { caiu = 1; }
        }
        else if (esc == 2) { // SEGURO
            folego -= 15;
            if (folego > 0) {
                andarAtual += 1;
                logMsg("Voce subiu com cautela, checando os degraus.");
            } else { caiu = 1; }
        }
        else if (esc == 3) { // DESCANSAR
            folego += 30;
            if (folego > 100) folego = 100;
            logMsg("Voce parou para respirar...");
            
            // 50% de chance de Zumbi (Alto risco pois está parado)
            if (rand() % 100 < 50) {
                logMsg("PERIGO! Um infectado saiu de uma janela quebrada!");
                logMsg("Voce teve que lutar na escada estreita.");
                modificarStatus(gs, &gs->vida, -15, MAX_VIDA_PLAYER, "Vida");
            } else {
                logMsg("O vento soprou forte, mas nada apareceu.");
            }
        }
        else if (esc == 4) { // DESISTIR
            logMsg("A vertigem foi forte demais. Voce desceu em seguranca.");
            desistiu = 1;
        }
        else {
            // Inválido
        }
        
        // VERIFICAÇÃO DE QUEDA
        if (caiu) {
            logMsg("SEU CORPO FALHOU! A exaustao te fez apagar.");
            logMsg("Voce rolou escada abaixo por varios lances.");
            modificarStatus(gs, &gs->vida, -40, MAX_VIDA_PLAYER, "Vida (Queda)");
            // Zera o progresso
            desistiu = 1; 
        }
    }

    // --- VITÓRIA ---
    if (andarAtual >= andarMaximo) {
        novoMomento();
        logMsg("TOPO DO MUNDO! Voce chegou a cobertura.");
        logMsg("A vista e triste, mas o saque e otimo.");
        
        // Recompensa de Alto Nível
        modificarStatus(gs, &gs->comida, 4, 999, "Comida (Estoque)");
        modificarStatus(gs, &gs->municao, 5, 999, "Municao (Militar)");
        
        // Chance de item raro
        if (rand() % 100 < 50) {
            logMsg("Achou um equipamento tatico!");
            oferecerItem(gs, ITEM_SINALIZADOR); 
        }
        //página 4 da história
        if (gs->nivelHistoria == 2) {
            atualizarHistoria(gs, 
                "PAGINA 4: A SELVA DE PEDRA",
                "'Samamba e o inferno na terra. Predios sao tumulos.\n"
                "Vi luzes na zona industrial de Cei.\n"
                "Dizem que o Instituto Federal de la ainda tem energia.'");
            gs->nivelHistoria = 3;
        }
        //------------
        gs->visitouPredioSamamba = 1;
    }
    
    // Sem pausa final para não travar loop
}

//EXPLORAÇÃO NO METRÔ DE SAMAMBA
void resolverMetro(GameState *gs) {
    novoMomento();
    logMsg("A Estacao Central esta alagada. Agua na cintura.");
    logMsg("Para acessar o deposito, voce deve ligar as bombas.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    printf("\n[ PAINEL DE CONTROLE DE DRENAGEM ]\n");
    printf("O sistema exibe uma sequencia. Voce deve repeti-la.\n");
    printf("Se errar, o sistema de defesa da um choque.\n");
    pausar();

    int niveis = 3;
    int errou = 0;

    for (int nivel = 1; nivel <= niveis && !errou; nivel++) {
        novoMomento();
        printf("=== NIVEL %d/%d ===\n\n", nivel, niveis);
        
        // Define o tamanho da sequência (3, 4 e 5 números)
        int qtdNumeros = 2 + nivel; 
        int sequencia[5];

        // Gera e mostra a sequência
        printf("MEMORIZE A SEQUENCIA:\n\n   ");
        for (int i = 0; i < qtdNumeros; i++) {
            sequencia[i] = (rand() % 4) + 1; // Números de 1 a 4
            printf("[ %d ]  ", sequencia[i]);
        }
        printf("\n\n");

        printf("Pressione ENTER para digitar...");
        char c; while ((c = getchar()) != '\n' && c != EOF); // Espera
        
        limparTela();
        renderizarHUD(gs); // Redesenha HUD para esconder a cola
        
        // Jogador digita
        printf("Digite a sequencia (Um numero por vez):\n");
        for (int i = 0; i < qtdNumeros; i++) {
            printf("%do Numero: ", i + 1);
            int input = lerInput();
            
            if (input != sequencia[i]) {
                errou = 1;
                break; // Sai do loop de input
            }
        }

        if (errou) {
            novoMomento();
            logMsg("ERRO DE SISTEMA! O painel soltou uma faisca forte.");
            logMsg("Voce foi eletrocutado e caiu na agua suja.");
            modificarStatus(gs, &gs->vida, -15 * nivel, MAX_VIDA_PLAYER, "Vida (Choque)");
        } else {
            novoMomento();
            logMsg("SEQUENCIA ACEITA. Drenagem parcial concluida.");
            
            // Recompensas por nível
            if (nivel == 1) {
                logMsg("Nivel 1: Achou suprimentos boiando.");
                modificarStatus(gs, &gs->comida, 2, 999, "Comida");
            } 
            else if (nivel == 2) {
                logMsg("Nivel 2: Acesso a sala de maquinas.");
                modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina");
                modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
            } 
            else if (nivel == 3) {
                logMsg("Nivel 3: Cofre da estacao aberto!");
                modificarStatus(gs, &gs->dinheiro, 60, 9999, "Dinheiro");
                modificarStatus(gs, &gs->municao, 4, 999, "Municao");

                                if (!gs->descobriuTunel) {
                    logMsg("EVENTO: Com a agua baixa, voce viu os trilhos antigos.");
                    logMsg("O mapa na parede indica: 'Proxima Estacao: CEI'.");
                    logMsg("O tunel esta bloqueado por escombros, mas pode ser explodido.");
                    gs->descobriuTunel = 1;
                    adicionarNotaDiario("O metro leva a Cei. Preciso explodir a passagem.");
                }
            }
            
            if (nivel < 3) {
                printf("\nContinuar para o proximo nivel?\n1 - Sim\n2 - Nao (Sair com o que tem)\nEscolha: ");
                int cont = lerInput();
                if (cont != 1) break; // Sai com o lucro
            }
        }
    }

    gs->visitouMetroHoje = 1;
}

// Função auxiliar para puxar carta (valor 1 a 11)
int puxarCarta() {
    int r = (rand() % 13) + 1;
    if (r > 10) return 10; // J, Q, K valem 10
    if (r == 1) return 11; // Ás vale 11
    return r;
}

void resolverCassino(GameState *gs) {
    novoMomento();
    // TEXTOS ENCURTADOS PARA NÃO CORTAR NO LOG:
    logMsg("Voce entra nos fundos de uma barbearia.");
    logMsg("O 'Bicheiro' louco te convida para jogar.");
    
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    if (gs->dinheiro <= 0) {
        logMsg("Bicheiro: 'Sem grana, sem jogo. Saia.'");
        renderizarHUD(gs);
        pausar();
        return;
    }

    int jogando = 1;

    while (jogando && gs->dinheiro > 0) {
        renderizarHUD(gs);
        printf("=== CASSINO DO APOCALIPSE (BLACKJACK) ===\n");
        printf("Seu Dinheiro: $%d\n", gs->dinheiro);
        printf("Quanto quer apostar?\n");
        printf("1 - $10\n2 - $50\n3 - $100\n4 - Tudo o que tenho\n0 - Sair\n");
        printf("Escolha: ");
        
        int apostaEsc = lerInput();
        int aposta = 0;
        
        if (apostaEsc == 0) { jogando = 0; break; }
        else if (apostaEsc == 1) aposta = 10;
        else if (apostaEsc == 2) aposta = 50;
        else if (apostaEsc == 3) aposta = 100;
        else if (apostaEsc == 4) aposta = gs->dinheiro;
        
        if (aposta > gs->dinheiro) {
            novoMomento();
            printf("Voce nao tem esse dinheiro todo!\n");
            pausar();
            continue;
        }

        novoMomento();
        int playerSoma = puxarCarta() + puxarCarta();
        int dealerSoma = puxarCarta(); 
        
        int turnoPlayer = 1;
        
        while (turnoPlayer) {
            printf("--- MESA ---\n");
            printf("Dealer mostra: %d + ?\n", dealerSoma);
            printf("Sua mao: %d\n\n", playerSoma);
            
            if (playerSoma > 21) {
                printf("ESTOUROU! Voce passou de 21.\n");
                turnoPlayer = 0;
            } else {
                printf("1 - Pedir Carta (Hit)\n");
                printf("2 - Parar (Stand)\n");
                printf("Escolha: ");
                int acao = lerInput();
                
                if (acao == 1) {
                    int nova = puxarCarta();
                    printf("\nVoce puxou um %d.\n", nova);
                    playerSoma += nova;
                } else {
                    turnoPlayer = 0;
                }
                novoMomento();
            }
        }

        // Turno do Dealer
        if (playerSoma <= 21) {
            printf("Dealer revela a carta oculta...\n");
            dealerSoma += puxarCarta();
            
            while (dealerSoma < 17) {
                int nova = puxarCarta();
                printf("Dealer puxou um %d. Soma: %d\n", nova, dealerSoma);
                dealerSoma += nova;
            }
        }

        // Resultado Final
        printf("\n=== RESULTADO ===\n");
        printf("Voce: %d  vs  Dealer: %d\n", playerSoma, dealerSoma);
        
        // Logs encurtados e diretos
        if (playerSoma > 21) {
            logMsg("Voce estourou e perdeu a aposta.");
            gs->dinheiro -= aposta;
        }
        else if (dealerSoma > 21) {
            logMsg("Dealer estourou! Voce venceu.");
            gs->dinheiro += aposta;
        }
        else if (playerSoma > dealerSoma) {
            logMsg("Sua mao e maior! Voce venceu.");
            gs->dinheiro += aposta;
        }
        else if (playerSoma < dealerSoma) {
            logMsg("A casa venceu.");
            gs->dinheiro -= aposta;
        }
        else {
            logMsg("Empate. O dinheiro volta.");
        }
        
        if (gs->dinheiro <= 0) {
            printf("\nVoce perdeu tudo.\n");
            jogando = 0;
        } else {
            printf("\nJogar novamente? 1-Sim 2-Nao: ");
            if (lerInput() != 1) jogando = 0;
        }
        
        novoMomento();
    }

    logMsg("Voce saiu do cassino.");
    gs->visitouCassinoHoje = 1;
}

//resolver explosão
void resolverExplosaoTunel(GameState *gs) {
    novoMomento();
    logMsg("Voce caminha ate o bloqueio nos trilhos do metro.");
    logMsg("Precisa de uma explosao controlada para passar.");
    renderizarHUD(gs);

    // Verifica Itens Necessários: 1 Gasolina (Status) + 1 Sinalizador (Inventário)
    int temSinalizador = -1;
    for(int i=0; i<3; i++) if(gs->inventario[i] == ITEM_SINALIZADOR) temSinalizador = i;
    
    int temGasolina = (gs->gasolina >= 1);

    printf("Requisitos: 1 Galao de Gasolina + 1 Sinalizador\n");
    printf("Voce tem: %s + %s\n\n", temGasolina ? "[OK]" : "[X]", temSinalizador != -1 ? "[OK]" : "[X]");
    
    printf("1 - Armar explosivos e detonar\n");
    printf("0 - Voltar\n");
    printf("Escolha: ");
    
    int esc = lerInput();
    novoMomento();

    if (esc == 1) {
        if (temGasolina && temSinalizador != -1) {
            // Consome recursos
            modificarStatus(gs, &gs->gasolina, -1, 999, "Gasolina");
            gs->inventario[temSinalizador] = ITEM_VAZIO;
            
            logMsg("KABOOM! A explosao tremeu a terra.");
            logMsg("A poeira baixou. O caminho para Cei esta aberto!");
            logMsg("[ ! ] NOVO LOCAL DESBLOQUEADO: Ruinas de Cei");
            
            gs->desbloqueouCei = 1;
            modificarStatus(gs, &gs->seguranca, -5, 100, "Seguranca (Barulho)"); // Barulho atrai perigo
        } else {
            logMsg("Voce nao tem os materiais necessarios para a detonacao.");
        }
    } else {
        logMsg("Voce decidiu nao explodir nada agora.");
    }
    
    // Sem renderizarHUD/pausar final
}

//resolver radio
void resolverRadioCei(GameState *gs) {
    novoMomento();
    logMsg("Voce sobe na caixa d'agua. O vento e forte.");
    logMsg("O radio amador chia, ligado a uma bateria velha.");
    
    if (gs->pilhas < 1) {
        logMsg("O radio precisa de 1 Pilha para funcionar.");
        renderizarHUD(gs);
        pausar();
        return;
    }

    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");
    modificarStatus(gs, &gs->pilhas, -1, 999, "Pilhas");

    // Configuração do Puzzle (Simplificado para focar no NPC)
    float frequenciaAtual = 20.0 + (rand() % 60);
    float frequenciaAlvo = 100.0;
    int tentativas = 5;
    int sintonizou = 0;

    printf("\n[ SISTEMA DE TRANSMISSAO PIRATA ]\n");
    printf("Objetivo: %.1f MHz | Bateria: %d tentativas\n", frequenciaAlvo, tentativas);
    pausar();

    while (tentativas > 0 && !sintonizou) {
        novoMomento();
        renderizarHUD(gs);
        
        printf(" FREQUENCIA: [ %.1f MHz ]  >>  ALVO: [ %.1f MHz ]\n\n", frequenciaAtual, frequenciaAlvo);
        printf("1 - Ajuste Grosso (+25.0)\n");
        printf("2 - Ajuste Medio  (-12.0)\n");
        printf("3 - Ajuste Fino   (+4.0)\n");
        printf("4 - Micro Ajuste  (-1.0)\n");
        printf("Escolha: ");

        int esc = lerInput();
        if (esc == 1) frequenciaAtual += 25.0;
        else if (esc == 2) frequenciaAtual -= 12.0;
        else if (esc == 3) frequenciaAtual += 4.0;
        else if (esc == 4) frequenciaAtual -= 1.0;

        if (frequenciaAtual >= 98.0 && frequenciaAtual <= 102.0) sintonizou = 1;
        else {
            tentativas--;
            if (frequenciaAtual > 150 || frequenciaAtual < 0) {
                logMsg("Sinal perdido. Frequencia instavel.");
                tentativas = 0;
            }
        }
    }

    novoMomento();
    if (sintonizou) {
        logMsg("SINAL CLARO! Coordenadas recebidas.");
        logMsg("Voce achou o pacote de suprimentos no telhado.");
        modificarStatus(gs, &gs->comida, 2, 999, "Comida");
        modificarStatus(gs, &gs->municao, 3, 999, "Municao");
        
        renderizarHUD(gs);
        pausar(); // Pausa para ver o loot antes do NPC

        // --- SISTEMA DE NPCs (SEQUENCIAL) ---
        novoMomento();
        
        // 1. DONA NEIDE (A Matriarca)
        if (gs->statusNeide == 0) {
            logMsg("Uma senhora sai da sombra da caixa d'agua.");
            logMsg("Neide: 'Eu vi voce sintonizando. Voce nao e daqui.'");
            renderizarHUD(gs);
            
            printf("1 - Venha conosco. Temos uma base segura.\n");
            printf("2 - So quero suprimentos. Tem mais ai?\n");
            printf("3 - Nao chegue perto, senhora.\n");
            printf("Escolha: ");
            int esc = lerInput();
            
            if (esc == 1) {
                logMsg("Neide: 'Cuidar de gente... e o que eu faco. Eu vou.'");
                gs->statusNeide = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Neide)");
            } else if (esc == 2) {
                logMsg("Neide: 'Egoista... Tome isso e suma.'");
                modificarStatus(gs, &gs->comida, 2, 999, "Comida");
                gs->statusNeide = 2; // Foi embora
            } else {
                logMsg("Ela recua e some nas escadas.");
                gs->statusNeide = 2;
            }
        }
        // 2. KEVIN (O Técnico)
        else if (gs->statusKevin == 0) {
            logMsg("Um jovem com fones de ouvido aparece.");
            logMsg("Kevin: 'Ei! Esse radio fui eu que consertei.'");
            renderizarHUD(gs);
            
            printf("1 - Voce tem talento. Junte-se a nos.\n");
            printf("2 - Consertou mal. Quase gastei minha pilha.\n");
            printf("3 - Passa tudo o que voce tem.\n");
            printf("Escolha: ");
            int esc = lerInput();
            
            if (esc == 1) {
                logMsg("Kevin: 'Serio? Cansei de ficar sozinho. Bora.'");
                gs->statusKevin = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Kevin)");
            } else if (esc == 2) {
                logMsg("Kevin: 'Faz melhor entao, otario.' Ele foge.");
                gs->statusKevin = 2;
            } else {
                logMsg("Ele joga uma bateria velha e corre.");
                modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas");
                gs->statusKevin = 2;
            }
        }
        // 3. SOMBRA (O Vigia)
        else if (gs->statusSombra == 0) {
            logMsg("Uma figura encapuzada surge atras de voce.");
            logMsg("Sombra: 'Voce faz muito barulho, forasteiro.'");
            renderizarHUD(gs);
            
            printf("1 - Preciso de alguem silencioso como voce.\n");
            printf("2 - Quem e voce? Saia da minha frente!\n");
            printf("3 - (Atacar primeiro)\n");
            printf("Escolha: ");
            int esc = lerInput();
            
            if (esc == 1) {
                logMsg("Sombra: 'Um proposito... interessante. Vou observar voces.'");
                gs->statusSombra = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Sombra)");
            } else if (esc == 2) {
                logMsg("Sombra: 'Voce nao duraria um dia na minha quebrada.'");
                gs->statusSombra = 2;
            } else {
                logMsg("Voce socou o ar. Ele ja tinha sumido.");
                gs->statusSombra = 2;
            }
        }
        else {
            // Se já encontrou os 3, apenas mensagem genérica
            logMsg("O radio continua transmitindo coordenadas, mas ninguem apareceu.");
        }

    } else {
        logMsg("A bateria morreu. So restou o chiado.");
    }

    gs->visitouRadioHoje = 1;
}

//resolver paredão
void resolverParedao(GameState *gs) {
    novoMomento();
    logMsg("Uma muralha de caixas de som bloqueia a rua.");
    logMsg("Ha um cofre com timer automatico no centro.");
    
    // Custo de entrada
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    printf("\n[ SISTEMA DE DEFESA SONORA ]\n");
    printf("Objetivo: Manter a horda longe do cofre por 3 turnos.\n");
    printf("Se a bateria ou as caixas falharem, voce morre.\n");
    pausar();

    // Status do Minigame
    int bateria = 100;
    int integridadeCaixas = 100;
    int distanciaHorda = 50; // 0 = No Cofre (Derrota), 100 = Longe (Seguro)
    
    int turno = 1;
    int gameover = 0;
    int vitoria = 0;

    while (turno <= 3 && !gameover) {
        novoMomento();
        renderizarHUD(gs);
        
        printf("=== TURNO %d/3 ===\n\n", turno);
        printf("BATERIA:     [%d%%]\n", bateria);
        printf("CAIXAS SOM:  [%d%%]\n", integridadeCaixas);
        printf("DIST. HORDA: [%d m] (0m = Morte)\n\n", distanciaHorda);
        
        if (distanciaHorda < 30) printf("[!] ALERTA: A horda esta muito perto do cofre!\n");
        
        printf("1 - Detonar o Grave (Empurrao Sonico)\n");
        printf("2 - Frequencia Aguda (Atordoar)\n");
        printf("3 - Diminuir Volume (Economizar)\n");
        printf("Escolha: ");
        
        int acao = lerInput();
        novoMomento();

        if (acao == 1) { // GRAVE (Gasta Bateria, Afasta muito)
            logMsg("BOOOOM! O grave fez o chao tremer.");
            logMsg("Os zumbis recuaram com o impacto.");
            bateria -= 30;
            integridadeCaixas -= 10;
            distanciaHorda += 25;
        }
        else if (acao == 2) { // AGUDO (Dano nas Caixas, Para a Horda)
            logMsg("SCREEECH! Um som estridente rasga o ar.");
            logMsg("A horda parou, tapando os ouvidos.");
            bateria -= 15;
            integridadeCaixas -= 25;
            distanciaHorda += 5; // Avanço mínimo
        }
        else if (acao == 3) { // ECONOMIZAR (Horda Avança muito)
            logMsg("Voce baixou o volume para poupar energia.");
            logMsg("A horda aproveitou e correu para o cofre!");
            bateria -= 5;
            integridadeCaixas += 0; // Sem dano extra
            distanciaHorda -= 30;
        }
        else {
            logMsg("Voce hesitou nos controles! A horda avancou.");
            distanciaHorda -= 20;
        }

        // Limites
        if (distanciaHorda > 100) distanciaHorda = 100;

        // Checagem de Derrota
        if (bateria <= 0) {
            logMsg("SILENCIO TOTAL. A bateria morreu.");
            logMsg("A horda te cercou na escuridao.");
            modificarStatus(gs, &gs->vida, -40, MAX_VIDA_PLAYER, "Vida");
            gameover = 1;
        }
        else if (integridadeCaixas <= 0) {
            logMsg("ESTOUROU! As caixas de som explodiram.");
            logMsg("Os zumbis invadiram pelo buraco.");
            modificarStatus(gs, &gs->vida, -30, MAX_VIDA_PLAYER, "Vida");
            gameover = 1;
        }
        else if (distanciaHorda <= 0) {
            logMsg("FALHA! A horda alcancou o cofre e voce.");
            modificarStatus(gs, &gs->vida, -50, MAX_VIDA_PLAYER, "Vida (Massivo)");
            gameover = 1;
        }
        
        // Checagem de Vitória
        if (!gameover && turno == 3) {
            vitoria = 1;
        }
        
        turno++;
    }

if (vitoria) {
        novoMomento();
        logMsg("CLACK. O timer zerou e o cofre abriu.");
        logMsg("A musica parou e voce pegou o saque.");
        
        // Loot
        modificarStatus(gs, &gs->municao, 5, 999, "Municao");
        modificarStatus(gs, &gs->dinheiro, 50, 9999, "Dinheiro");
        
        if (rand() % 100 < 40) {
            logMsg("Havia um sinalizador militar no fundo!");
            oferecerItem(gs, ITEM_SINALIZADOR);
        }

        // --- ENCONTRO COM NPC: KALEB ---
        if (!gs->npcKaleb) {
            renderizarHUD(gs);
            pausar(); // Pausa para ler o loot
            
            novoMomento();
            logMsg("Um homem sai de uma escotilha atras das caixas.");
            logMsg("Kaleb: 'Mandou bem na mixagem. Eu sou o criador disso.'");
            renderizarHUD(gs);
            
            printf("1 - Isso e apenas barulho irritante. Sai da frente.\n");
            printf("2 - Voce tem talento. Precisamos dessa engenharia na base.\n");
            printf("3 - Onde estao os outros? Voce esta sozinho?\n");
            printf("Escolha: ");
            
            int escNPC = lerInput();
            novoMomento();
            
            if (escNPC == 2) {
                logMsg("Kaleb: 'Minha arte salvando vidas... Gostei. Eu vou.'");
                gs->npcKaleb = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Kaleb)");
            } 
            else if (escNPC == 1) {
                logMsg("Kaleb: 'Ignorante. O som e a unica defesa aqui.'");
                logMsg("Ele entra na escotilha e tranca a porta.");
                // Kaleb não é recrutado, mas não some para sempre (pode tentar outro dia se quiser resetar a lógica, 
                // mas no código atual npcKaleb=0 permite tentar de novo se vencer o jogo outra vez)
                // diferente dos outros NPCs que somem permanentemente
            } 
            else {
                logMsg("Kaleb: 'Todos partiram ou viraram estatua. So restou o som.'");
                logMsg("Ele parece deprimido e nao quer conversar mais.");
            }
        }
        // -------------------------------
    }

    gs->visitouParedaoHoje = 1;
    // Sem pausa final
}
//resolver central de gatos
void resolverCentralGatos(GameState *gs) {
    novoMomento();
    logMsg("Um emaranhado de fios desce do poste ate uma porta de aco.");
    logMsg("O painel exige uma voltagem exata para abrir.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    // 1. Gerar Voltagens dos Fios
    int v1 = (rand() % 10) + 5;  // 5 a 14v
    int v2 = (rand() % 15) + 10; // 10 a 24v
    int v3 = (rand() % 20) + 15; // 15 a 34v
    
    // 2. Definir Meta (Garante que é solucionável)
    int meta = 0;
    int solucao = rand() % 4; 
    // 0 = v1+v2, 1 = v2+v3, 2 = v1+v3, 3 = v1+v2+v3
    if (solucao == 0) meta = v1 + v2;
    else if (solucao == 1) meta = v2 + v3;
    else if (solucao == 2) meta = v1 + v3;
    else meta = v1 + v2 + v3;

    // Estado dos Interruptores (0 = Desligado, 1 = Ligado)
    int s1 = 0, s2 = 0, s3 = 0;
    int tentativaFeita = 0;
    int sucesso = 0;

    while (!tentativaFeita) {
        int cargaAtual = (s1 * v1) + (s2 * v2) + (s3 * v3);
        
        novoMomento();
        renderizarHUD(gs);
        
        printf("\n[ PAINEL DE CONTROLE DE ENERGIA ]\n");
        printf("META EXIGIDA: %d Volts\n", meta);
        printf("CARGA ATUAL:  %d Volts\n\n", cargaAtual);
        
        printf("1 - Fio Azul      (%d v) [%s]\n", v1, s1 ? "LIGADO" : "DESL.");
        printf("2 - Fio Vermelho  (%d v) [%s]\n", v2, s2 ? "LIGADO" : "DESL.");
        printf("3 - Fio Amarelo   (%d v) [%s]\n", v3, s3 ? "LIGADO" : "DESL.");
        printf("4 - [ ! ] TESTAR CARGA (Finalizar)\n");
        printf("0 - Desistir (Sair)\n");
        printf("Escolha: ");
        
        int esc = lerInput();
        
        if (esc == 1) s1 = !s1; // Alterna 0/1
        else if (esc == 2) s2 = !s2;
        else if (esc == 3) s3 = !s3;
        else if (esc == 0) {
            logMsg("Voce ficou com medo do choque e saiu.");
            tentativaFeita = 1;
        }
        else if (esc == 4) {
            tentativaFeita = 1;
            novoMomento();
            
            if (cargaAtual == meta) {
                logMsg("SUCESSO! A voltagem estabilizou e a porta abriu.");
                sucesso = 1;
            } 
            else if (cargaAtual > meta) {
                logMsg("SOBRECARGA! O painel explodiu na sua cara.");
                modificarStatus(gs, &gs->vida, -25, MAX_VIDA_PLAYER, "Vida (Choque)");
            } 
            else {
                logMsg("CARGA INSUFICIENTE. O sistema travou por seguranca.");
            }
        }
    }

    // --- RECOMPENSA E NPC ---
    if (sucesso) {
        // Loot
        modificarStatus(gs, &gs->pilhas, 4, 999, "Pilhas (Alta Cap.)");
        if (rand() % 100 < 40) {
            logMsg("Achou ferramentas de precisao.");
            oferecerItem(gs, ITEM_LANTERNA); // Exemplo
        }
        
        renderizarHUD(gs);
        pausar();

        // NPC BETO
        if (!gs->npcBeto) {
            novoMomento();
            logMsg("Um homem sai de tras dos geradores, assustado.");
            logMsg("Beto: 'Voce nao fritou o sistema? Impressionante.'");
            renderizarHUD(gs);
            
            printf("1 - 'Quase fritei. Quem e voce?'\n");
            printf("2 - 'Entendo de eletrica. Voce seria util na base.'\n");
            printf("3 - 'Saia da frente antes que eu te de um choque.'\n");
            printf("Escolha: ");
            
            int escNPC = lerInput();
            novoMomento();
            
            if (escNPC == 2) {
                logMsg("Beto: 'Uma base com energia estavel? Sonho meu. Eu vou.'");
                gs->npcBeto = 1;
                modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Beto)");
            } else if (escNPC == 1) {
                logMsg("Beto: 'Sou apenas o cara que mantem as luzes acesas.'");
                logMsg("Ele volta a mexer nos fios, ignorando voce.");
            } else {
                logMsg("Beto correu para a escuridao dos becos.");
            }
        }
    }

    gs->visitouGatosHoje = 1;
}

//resolver IF
void resolverIFCei(GameState *gs) {
    novoMomento();
    logMsg("Voce entra no campus do Instituto Federal.");
    logMsg("Salas de aula viraram barricadas e laboratorios.");
    modificarStatus(gs, &gs->energia, -10, MAX_ENERGIA_PLAYER, "Energia");

    printf("Escolha o setor para investigar:\n");
    printf("[ 1 - Lab. Quimica ]  [ 2 - Oficina Robotica ]  [ 3 - Biblioteca ]\n");
    printf("Escolha: ");
    
    int setor = lerInput();
    
    // --- SALA 1: LABORATÓRIO DE QUÍMICA ---
    if (setor == 1) {
        novoMomento();
        logMsg("Um destilador caseiro complexo borbulha na bancada.");
        logMsg("Objetivo: Estabilizar a pressao em exatos 50 PSI.");
        renderizarHUD(gs);
        pausar();

        int pressao = rand() % 30; 
        int alvo = 50;
        int tentativas = 5;
        int sucesso = 0;

        while (tentativas > 0 && !sucesso) {
            novoMomento();
            printf("PRESSAO ATUAL: [ %d PSI ]  |  ALVO: [ 50 PSI ]\n", pressao);
            printf("Tentativas restantes: %d\n\n", tentativas);
            
            printf("1 - Adicionar Reagente Azul (+12 PSI)\n");
            printf("2 - Adicionar Reagente Vermelho (+7 PSI)\n");
            printf("3 - Liberar Valvula (-5 PSI)\n");
            printf("4 - Resfriar Sistema (-10 PSI)\n");
            printf("Escolha: ");
            
            int acao = lerInput();
            
            if (acao == 1) pressao += 12;
            else if (acao == 2) pressao += 7;
            else if (acao == 3) pressao -= 5;
            else if (acao == 4) pressao -= 10;
            
            if (pressao == alvo) sucesso = 1;
            else {
                tentativas--;
                if (pressao > 80) {
                    logMsg("PERIGO! Pressao critica! O tanque explodiu.");
                    modificarStatus(gs, &gs->vida, -20, MAX_VIDA_PLAYER, "Vida");
                    tentativas = 0;
                }
            }
        }

        if (sucesso) {
            logMsg("Estavel! Voce extraiu combustivel sintetico.");
            modificarStatus(gs, &gs->gasolina, 2, 999, "Gasolina");
            
            if (!gs->npcLucia) {
                renderizarHUD(gs); pausar(); novoMomento();
                logMsg("Uma mulher de jaleco surge: 'Bela sintese.'");
                logMsg("Prof. Lucia: 'Poucos entendem essa ciencia hoje.'");
                renderizarHUD(gs);
                
                printf("1 - Venha conosco. Precisamos de remedios e combustivel.\n");
                printf("2 - Ciencia nao salva ninguem. Adeus.\n");
                printf("3 - 'Nao preciso de voce, so da formula. Escreva aqui.'\n");
                printf("Escolha: ");
                
                int esc = lerInput();
                if (esc == 1) {
                    logMsg("Lucia: 'O campus ja era. Levarei meu conhecimento.'");
                    gs->npcLucia = 1;
                    modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Lucia)");
                } 
                else if (esc == 3) {
                    logMsg("Lucia: 'Grosso... Mas tome. Nao se exploda.'");
                    logMsg("Ela anotou uma receita de explosivos para voce.");
                    // Ganha um item extra mas perde o NPC
                    oferecerItem(gs, ITEM_SINALIZADOR); 
                    gs->npcLucia = 2; 
                }
                else {
                    logMsg("Ela volta para suas anotacoes, decepcionada.");
                    gs->npcLucia = 2; 
                }
            }
        }
    }
    
    // --- SALA 2: OFICINA DE ROBÓTICA ---
    else if (setor == 2) {
        novoMomento();
        logMsg("Drones desmontados e fios por todo lado.");
        logMsg("Um drone de carga esta apitando um erro.");
        
        int problema = rand() % 3; 
        
        renderizarHUD(gs);
        printf("\n[ MANUAL TECNICO ]\n");
        printf("- Luz VERMELHA: Trocar Bateria.\n");
        printf("- Luz AMARELA: Limpar Sensor.\n");
        printf("- Luz PISCANDO RAPIDO: Resfriar Motor.\n\n");
        
        printf("DIAGNOSTICO DO DRONE: ");
        if (problema == 0) printf("[ LUZ PISCANDO RAPIDO ]\n");
        if (problema == 1) printf("[ LUZ VERMELHA ]\n");
        if (problema == 2) printf("[ LUZ AMARELA ]\n");
        
        printf("\nO que fazer?\n");
        printf("1 - Trocar a Bateria\n");
        printf("2 - Limpar o Sensor Otico\n");
        printf("3 - Aplicar Pasta Termica (Resfriar)\n");
        printf("Escolha: ");
        
        int esc = lerInput();
        novoMomento();
        
        int acertou = 0;
        if (problema == 0 && esc == 3) acertou = 1;
        else if (problema == 1 && esc == 1) acertou = 1;
        else if (problema == 2 && esc == 2) acertou = 1;
        
        if (acertou) {
            logMsg("O drone estabilizou e soltou a carga.");
            modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas");
            
            if (!gs->npcZero) {
                renderizarHUD(gs); pausar(); novoMomento();
                logMsg("Um jovem sai de baixo da mesa: 'Deu certo!'");
                logMsg("Zero: 'Eu programei ele, mas nao tinha as pecas.'");
                renderizarHUD(gs);
                
                printf("1 - Voce e um genio. Junte-se a nos.\n");
                printf("2 - Peguei o que queria. Adeus.\n");
                printf("3 - 'Brinquedos nao matam zumbis. Aprenda a atirar.' \n");
                printf("Escolha: ");
                
                int npcEsc = lerInput();
                if (npcEsc == 1) {
                    logMsg("Zero: 'Hackear o apocalipse? To dentro.'");
                    gs->npcZero = 1;
                    modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Zero)");
                } 
                else if (npcEsc == 3) {
                    logMsg("Zero: 'Voce soa como meu pai... que morreu.'");
                    logMsg("Ele se escondeu de novo, ofendido.");
                    gs->npcZero = 2;
                }
                else {
                    gs->npcZero = 2;
                }
            }
        } else {
            logMsg("ERRO CRITICO! O drone deu um curto-circuito.");
            logMsg("Voce levou um choque e a peca queimou.");
            modificarStatus(gs, &gs->vida, -10, MAX_VIDA_PLAYER, "Vida");
        }
    }
    
    // --- SALA 3: BIBLIOTECA ---
    else if (setor == 3) {
        novoMomento();
        logMsg("Estantes caidas formam um labirinto de livros.");
        logMsg("Um cofre de arquivo exige uma senha verbal (Charada).");
        renderizarHUD(gs);
        
        int enigma = rand() % 2;
        if (enigma == 0) {
            printf("\nCHARADA: 'Tenho cidades, mas nao casas.\n");
            printf("Tenho montanhas, mas nao arvores. O que sou?'\n");
            printf("1 - O Sonho\n2 - O Mapa\n3 - O Deserto\n");
        } else {
            printf("\nCHARADA: 'O que sobe mas nunca desce?'\n");
            printf("1 - A Idade\n2 - O Balao\n3 - A Mare\n");
        }
        printf("Escolha: ");
        int resp = lerInput();
        novoMomento();
        
        int acertou = 0;
        if (enigma == 0 && resp == 2) acertou = 1;
        if (enigma == 1 && resp == 1) acertou = 1;
        
        if (acertou) {
            logMsg("O cofre destravou. Havia manuais de sobrevivencia.");
            ganharSementes(gs, 2);
            // --- ATUALIZAÇÃO DA HISTÓRIA FINAL ---
            if (gs->nivelHistoria == 3) {
                atualizarHistoria(gs, 
                    "PAGINA 5: A ULTIMA ESPERANCA",
                    "'O IF de Cei era quase vazio, mas os dados estao aqui.\n"
                    "O Projeto Eden e real. Nao e uma cidade...\n"
                    "E um bunker. As coordenadas apontam para o Norte.\n"
                    "Esta sera minha ultima viagem.'");
                
                gs->nivelHistoria = 4; // História concluída (por enquanto)

                // --- RECOMPENSA MÁXIMA ---
                renderizarHUD(gs);
                pausar(); // Pausa para ler a história primeiro
                
                novoMomento();
                logMsg("Junto com a nota, voce achou a chave do Estoque Mestre!");
                logMsg("Voce abriu o bunker do Viajante.");
                logMsg("RECOMPENSA FINAL: Todos os recursos no maximo!");

                // Adiciona valores altos para topar os limites (Max 999 ou 9999)
                modificarStatus(gs, &gs->municao, 999, 999, "Municao (MAX)");
                modificarStatus(gs, &gs->pilhas, 999, 999, "Pilhas (MAX)");
                modificarStatus(gs, &gs->comida, 999, 999, "Comida (MAX)");
                modificarStatus(gs, &gs->gasolina, 999, 999, "Gasolina (MAX)");
                modificarStatus(gs, &gs->dinheiro, 9999, 9999, "Dinheiro (MAX)");
                
                // Opcional: Cura total também
                gs->vida = MAX_VIDA_PLAYER;
                gs->energia = MAX_ENERGIA_PLAYER;
                logMsg("Sua Vida e Energia foram restauradas.");
                
                renderizarHUD(gs);
                pausar();
            }
            if (!gs->npcTales) {
                renderizarHUD(gs); pausar(); novoMomento();
                logMsg("Um homem de oculos surge: 'Bravo. Cultura vive.'");
                logMsg("Tales: 'Sou o guardiao desta historia.'");
                renderizarHUD(gs);
                
                printf("1 - Precisamos preservar a historia. Venha.\n");
                printf("2 - Livros nao param zumbis. Fique ai.\n");
                printf("3 - 'Papel serve para fogueira. Nada mais.'\n");
                printf("Escolha: ");
                
                int npcEsc = lerInput();
                if (npcEsc == 1) {
                    logMsg("Tales: 'Serei o escriba da sua jornada.'");
                    gs->npcTales = 1;
                    modificarStatus(gs, &gs->aliados, 1, 10, "Aliados (+Tales)");
                } 
                else if (npcEsc == 3) {
                    logMsg("Tales: 'Barbaro! Saia da minha biblioteca!'");
                    logMsg("Ele te expulsou aos gritos.");
                    gs->npcTales = 2;
                }
                else {
                    gs->npcTales = 2;
                }
            }
        } else {
            logMsg("Resposta incorreta. O mecanismo travou.");
        }
    }
    
    gs->visitouIFHoje = 1;
}

//FUNÇÕES DA HORTA
void ganharSementes(GameState *gs, int quantidade) {
    char msg[60];
    for (int i = 0; i < quantidade; i++) {
        int r = rand() % 3;
        if (r == 0) {
            gs->semAlface++;
            logMsg("Encontrou sementes de Alface.");
        } else if (r == 1) {
            gs->semTomate++;
            logMsg("Encontrou sementes de Tomate.");
        } else {
            gs->semCenoura++;
            logMsg("Encontrou sementes de Cenoura.");
        }
    }
}

//resolver aliados
void resolverAliadosVisual(GameState *gs) {
    int sair = 0;
    
    // Histórico local de ações (5 linhas de texto)
    char relatorio[5][50];
    for(int i=0; i<5; i++) strcpy(relatorio[i], ""); // Limpa inicio

    while (!sair) {
        // 1. MAPEAMENTO DE PRESENÇA
        int presentes[TOTAL_NPCS];
        for(int i=0; i<TOTAL_NPCS; i++) presentes[i] = 0;

        if (gs->npcAugusto) presentes[NPC_AUGUSTO] = 1;
        if (gs->npcAna) presentes[NPC_ANA] = 1;
        if (gs->npcMarcos) presentes[NPC_MARCOS] = 1;
        if (gs->npcMiguel) presentes[NPC_MIGUEL] = 1;
        if (gs->npcClaudio) presentes[NPC_CLAUDIO] = 1;
        if (gs->npcElisa) presentes[NPC_ELISA] = 1;
        if (gs->statusNeide == 1) presentes[NPC_NEIDE] = 1;
        if (gs->statusKevin == 1) presentes[NPC_KEVIN] = 1;
        if (gs->statusSombra == 1) presentes[NPC_SOMBRA] = 1;
        if (gs->npcKaleb) presentes[NPC_KALEB] = 1;
        if (gs->npcBeto) presentes[NPC_BETO] = 1;
        if (gs->npcLucia) presentes[NPC_LUCIA] = 1;
        if (gs->npcZero) presentes[NPC_ZERO] = 1;
        if (gs->npcTales) presentes[NPC_TALES] = 1;
        if (gs->npcAlberto) presentes[NPC_ALBERTO] = 1;
        if (gs->npcPedro) presentes[NPC_PEDRO] = 1;
        if (gs->npcBianca) presentes[NPC_BIANCA] = 1;
        if (gs->npcIvo) presentes[NPC_IVO] = 1;
        if (gs->npcLara) presentes[NPC_LARA] = 1;
        if (gs->npcTom) presentes[NPC_TOM] = 1;

        limparTela();
        printf("==========================================================================\n");
        printf("                     ACAMPAMENTO DOS SOBREVIVENTES                        \n");
        printf("==========================================================================\n");

        // 2. DESENHO DOS BONECOS (Centralizado ou Esquerda)
        int desenhados = 0;
        int totalRecrutados = gs->aliados;
        
        // Desenha bonecos (mantendo lógica de quebra de linha se tiver muitos)
        while (desenhados < totalRecrutados) {
            printf("  ");
            for (int i = 0; i < 10 && (desenhados + i) < totalRecrutados; i++) printf(" O     ");
            printf("\n  ");
            for (int i = 0; i < 10 && (desenhados + i) < totalRecrutados; i++) printf("/|\\    ");
            printf("\n  ");
            for (int i = 0; i < 10 && (desenhados + i) < totalRecrutados; i++) printf("/ \\    ");
            printf("\n\n");
            desenhados += 10;
        }
        if (totalRecrutados == 0) printf("  (Ninguem aqui ainda...)\n\n");

        // 3. PAINEL DE CONTROLE (Layout Lado a Lado)
        // Cálculos
        int ociosos = gs->aliados - gs->ordensDadasHoje;
        int bonusSeg = ociosos * 2;

        printf(" +-------------------------------------+   +------------------------------------+\n");
        printf(" | RESUMO DA TROPA                     |   | RELATORIO DE ORDENS                |\n");
        printf(" |-------------------------------------|   |------------------------------------|\n");
        
        // Linha 1
        printf(" | Total de Aliados: %-17d |   | %-34s |\n", gs->aliados, relatorio[0]);
        
        // Linha 2
        char bufOrdens[30]; sprintf(bufOrdens, "%d / 5", gs->ordensDadasHoje);
        printf(" | Ordens Usadas:    %-17s |   | %-34s |\n", bufOrdens, relatorio[1]);
        
        // Linha 3
        char bufOciosos[30]; sprintf(bufOciosos, "%d", ociosos);
        printf(" | Aliados Ociosos:  %-17s |   | %-34s |\n", bufOciosos, relatorio[2]);
        
        // Linha 4
        char bufBonus[30]; sprintf(bufBonus, "+%d%% (Fim do dia)", bonusSeg);
        printf(" | Bonus Seguranca:  %-17s |   | %-34s |\n", bufBonus, relatorio[3]);
        
        // Linha 5 (Vazia na esq, Log na dir)
        printf(" |                                     |   | %-34s |\n", relatorio[4]);
        
        printf(" +-------------------------------------+   +------------------------------------+\n\n");

        // 4. MENU DE TAREFAS
        int opcoes[20];
        int contador = 1;

        if (presentes[NPC_AUGUSTO]) { printf(" %d - Augusto (Engenheiro): Reciclar Pilhas %s\n", contador, gs->tarefasAliados[NPC_AUGUSTO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_AUGUSTO; }
        if (presentes[NPC_ANA]) { printf(" %d - Ana (Coletora): Buscar Sementes %s\n", contador, gs->tarefasAliados[NPC_ANA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_ANA; }
        if (presentes[NPC_MARCOS]) { printf(" %d - Marcos (Seguranca): Patrulha %s\n", contador, gs->tarefasAliados[NPC_MARCOS] ? "[FEITO]" : ""); opcoes[contador++] = NPC_MARCOS; }
        if (presentes[NPC_MIGUEL]) { printf(" %d - Miguel (Padre): Acalmar o Filho %s\n", contador, gs->tarefasAliados[NPC_MIGUEL] ? "[FEITO]" : ""); opcoes[contador++] = NPC_MIGUEL; }
        if (presentes[NPC_CLAUDIO]) { printf(" %d - Claudio (Militar): Treino Tatico %s\n", contador, gs->tarefasAliados[NPC_CLAUDIO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_CLAUDIO; }
        if (presentes[NPC_ELISA]) { printf(" %d - Elisa (Cientista): Criar Adubo %s\n", contador, gs->tarefasAliados[NPC_ELISA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_ELISA; }
        if (presentes[NPC_ALBERTO]) { printf(" %d - Alberto (Gestor): Economia %s\n", contador, gs->tarefasAliados[NPC_ALBERTO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_ALBERTO; }
        if (presentes[NPC_NEIDE]) { printf(" %d - Neide (Cozinheira): Banquete %s\n", contador, gs->tarefasAliados[NPC_NEIDE] ? "[FEITO]" : ""); opcoes[contador++] = NPC_NEIDE; }
        if (presentes[NPC_KEVIN]) { printf(" %d - Kevin (Tecnico): Conserto %s\n", contador, gs->tarefasAliados[NPC_KEVIN] ? "[FEITO]" : ""); opcoes[contador++] = NPC_KEVIN; }
        if (presentes[NPC_SOMBRA]) { printf(" %d - Sombra (Vigia): Reconhecimento %s\n", contador, gs->tarefasAliados[NPC_SOMBRA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_SOMBRA; }
        if (presentes[NPC_KALEB]) { printf(" %d - Kaleb (Eng. Som): Alarmes %s\n", contador, gs->tarefasAliados[NPC_KALEB] ? "[FEITO]" : ""); opcoes[contador++] = NPC_KALEB; }
        if (presentes[NPC_BETO]) { printf(" %d - Beto (Eletricista): Gerador %s\n", contador, gs->tarefasAliados[NPC_BETO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_BETO; }
        if (presentes[NPC_LUCIA]) { printf(" %d - Lucia (Quimica): Sintetizar Gasolina %s\n", contador, gs->tarefasAliados[NPC_LUCIA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_LUCIA; }
        if (presentes[NPC_ZERO]) { printf(" %d - Zero (Hacker): Drone de Carga %s\n", contador, gs->tarefasAliados[NPC_ZERO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_ZERO; }
        if (presentes[NPC_TALES]) { printf(" %d - Tales (Historiador): Mapear %s\n", contador, gs->tarefasAliados[NPC_TALES] ? "[FEITO]" : ""); opcoes[contador++] = NPC_TALES; }
        if (presentes[NPC_PEDRO]) { printf(" %d - Pedro (Carpinteiro): Reforcar Base %s\n", contador, gs->tarefasAliados[NPC_PEDRO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_PEDRO; }
        if (presentes[NPC_BIANCA]) { printf(" %d - Bianca (Veterinaria): Tratar Feridas %s\n", contador, gs->tarefasAliados[NPC_BIANCA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_BIANCA; }
        if (presentes[NPC_IVO]) { printf(" %d - Ivo (Mecanico): Sifonar Carros %s\n", contador, gs->tarefasAliados[NPC_IVO] ? "[FEITO]" : ""); opcoes[contador++] = NPC_IVO; }
        if (presentes[NPC_LARA]) { printf(" %d - Lara (Agricultora): Cuidar da Horta %s\n", contador, gs->tarefasAliados[NPC_LARA] ? "[FEITO]" : ""); opcoes[contador++] = NPC_LARA; }
        if (presentes[NPC_TOM]) { printf(" %d - Tom (Musico): Tocar Violao %s\n", contador, gs->tarefasAliados[NPC_TOM] ? "[FEITO]" : ""); opcoes[contador++] = NPC_TOM; }
        printf(" 0 - Voltar para Base\n");
        printf(" Escolha: ");
        int esc = lerInput();

        // Variável auxiliar para a nova mensagem de log
        char acaoMsg[50] = "";

        if (esc == 0) {
            sair = 1;
        }
        else if (esc > 0 && esc < contador) {
            int npcID = opcoes[esc];

            if (gs->tarefasAliados[npcID] == 1) {
                // Apenas feedback visual, não muda o log da caixa
            } 
            else if (gs->ordensDadasHoje >= 5) {
                // Limite atingido
            }
            else {
                gs->tarefasAliados[npcID] = 1;
                gs->ordensDadasHoje++;
                
                // LÓGICA COM FEEDBACK ESPECÍFICO PARA A CAIXA
                if (npcID == NPC_AUGUSTO) { modificarStatus(gs, &gs->pilhas, 2, 999, "Pilhas"); strcpy(acaoMsg, "Augusto: [+2 Pilhas]"); }
                else if (npcID == NPC_ANA) { ganharSementes(gs, 2); strcpy(acaoMsg, "Ana: [+2 Sementes]"); }
                else if (npcID == NPC_MARCOS) { modificarStatus(gs, &gs->seguranca, 15, 100, "Seguranca"); strcpy(acaoMsg, "Marcos: [+15 Seguranca]"); }
                else if (npcID == NPC_MIGUEL) { 
                    char msg[30]; sprintf(msg, "Energia (%s)", gs->nomeFilho);
                    modificarStatus(gs, &gs->filho.energia, 20, MAX_STATUS_LEO, msg);
                    strcpy(acaoMsg, "Miguel: [+20 Energia Filho]");
                }
                else if (npcID == NPC_CLAUDIO) {
                    if (gs->municao > 0) { gs->municao--; modificarStatus(gs, &gs->seguranca, 25, 100, "Seguranca"); strcpy(acaoMsg, "Claudio: [+25 Seguranca]"); }
                    else { modificarStatus(gs, &gs->seguranca, 10, 100, "Seguranca"); strcpy(acaoMsg, "Claudio: [+10 Seguranca]"); }
                }
                else if (npcID == NPC_ELISA) { gs->estoqueAdubo += 2; strcpy(acaoMsg, "Elisa: [+2 Adubos]"); }
                else if (npcID == NPC_ALBERTO) { modificarStatus(gs, &gs->dinheiro, 30, 9999, "Dinheiro"); strcpy(acaoMsg, "Alberto: [+$30 Dinheiro]"); }
                else if (npcID == NPC_KALEB) { modificarStatus(gs, &gs->seguranca, 10, 100, "Seguranca"); strcpy(acaoMsg, "Kaleb: [+10 Seguranca]"); }
                else if (npcID == NPC_BETO) { modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas"); strcpy(acaoMsg, "Beto: [+3 Pilhas]"); }
                else if (npcID == NPC_NEIDE) {
                    if (gs->comida > 0) {
                        gs->comida--; modificarStatus(gs, &gs->vida, 30, MAX_VIDA_PLAYER, "Vida"); 
                        char msg[30]; sprintf(msg, "Vida (%s)", gs->nomeFilho); modificarStatus(gs, &gs->filho.vida, 20, MAX_STATUS_LEO, msg);
                        strcpy(acaoMsg, "Neide: [+Vida Player/Filho]");
                    } else { modificarStatus(gs, &gs->seguranca, 5, 100, "Seguranca"); strcpy(acaoMsg, "Neide: [+5 Seguranca]"); }
                }
                else if (npcID == NPC_KEVIN) {
                    if (rand() % 100 < 30) { oferecerItem(gs, ITEM_LANTERNA); strcpy(acaoMsg, "Kevin: [Achou Item!]"); }
                    else { strcpy(acaoMsg, "Kevin: [Organizacao]"); }
                }
                else if (npcID == NPC_SOMBRA) { modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina"); strcpy(acaoMsg, "Sombra: [+1 Gasolina]"); }
                else if (npcID == NPC_LUCIA) { modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina"); strcpy(acaoMsg, "Lucia: [+1 Gasolina]"); }
                else if (npcID == NPC_ZERO) {
                    int r = rand() % 3;
                    if (r==0) { modificarStatus(gs, &gs->comida, 3, 999, "Comida"); strcpy(acaoMsg, "Zero: [+3 Comida]"); }
                    else if (r==1) { modificarStatus(gs, &gs->municao, 3, 999, "Municao"); strcpy(acaoMsg, "Zero: [+3 Municao]"); }
                    else { modificarStatus(gs, &gs->pilhas, 3, 999, "Pilhas"); strcpy(acaoMsg, "Zero: [+3 Pilhas]"); }
                }
                else if (npcID == NPC_TALES) {
                    int r = rand() % 4;
                    if(r==0) { gs->semAlface++; strcpy(acaoMsg, "Tales: [+1 Sem. Alface]"); }
                    else if(r==1) { gs->semTomate++; strcpy(acaoMsg, "Tales: [+1 Sem. Tomate]"); }
                    else if(r==2) { gs->semCenoura++; strcpy(acaoMsg, "Tales: [+1 Sem. Cenoura]"); }
                    else { modificarStatus(gs, &gs->dinheiro, 20, 9999, "Dinheiro"); strcpy(acaoMsg, "Tales: [+$20 Dinheiro]"); }
                }
                else if (npcID == NPC_PEDRO) { modificarStatus(gs, &gs->seguranca, 20, 100, "Seguranca"); strcpy(acaoMsg, "Pedro: [+20 Seguranca]"); }
                else if (npcID == NPC_BIANCA) { 
                    modificarStatus(gs, &gs->vida, 15, MAX_VIDA_PLAYER, "Vida"); 
                    char msg[30]; sprintf(msg, "Vida (%s)", gs->nomeFilho); modificarStatus(gs, &gs->filho.vida, 10, MAX_STATUS_LEO, msg);
                    strcpy(acaoMsg, "Bianca: [+Vida Global]"); 
                }
                else if (npcID == NPC_IVO) { modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina"); strcpy(acaoMsg, "Ivo: [+1 Gasolina]"); }
                else if (npcID == NPC_LARA) { 
                    ganharSementes(gs, 2); 
                    gs->estoqueAdubo++; 
                    strcpy(acaoMsg, "Lara: [+Sementes +Adubo]"); 
                }
                else if (npcID == NPC_TOM) { 
                    modificarStatus(gs, &gs->energia, 20, MAX_ENERGIA_PLAYER, "Energia");
                    char msg[30]; sprintf(msg, "Energia (%s)", gs->nomeFilho); modificarStatus(gs, &gs->filho.energia, 20, MAX_STATUS_LEO, msg);
                    strcpy(acaoMsg, "Tom: [+Energia Global]"); 
                }
                
                // ATUALIZA O LOG DA CAIXA À DIREITA
                // Move tudo para baixo e insere no topo
                for(int i=4; i>0; i--) strcpy(relatorio[i], relatorio[i-1]);
                strcpy(relatorio[0], acaoMsg);
                
                // Também grava no log geral do jogo para histórico
                char logGeral[60]; sprintf(logGeral, "Aliados: %s", acaoMsg); logMsg(logGeral);
            }
        }
    }
}

//ATUALIZADOR DA HISTÓRIA
void atualizarHistoria(GameState *gs, char *titulo, char *texto) {
    // Limpa o buffer da história para remover o texto antigo
    memset(gs->textoHistoria, 0, TAMANHO_HISTORIA_BUFFER);
    
    //  Escreve o Título (Sobrescrevendo o anterior)
    strcpy(gs->textoHistoria, titulo);
    strcat(gs->textoHistoria, "\n\n"); // Pula uma linha
    
    // Adiciona o Corpo do texto
    strcat(gs->textoHistoria, texto);
    
    // Feedback visual no Log de eventos
    char logH[60];
    sprintf(logH, "[HISTORIA: %s]", titulo); // Log curto para caber
    logMsg(logH);
    
    // Adiciona nota no Diário (Menu 1) para ter o registro permanente
    // Assim, a HUD mostra o "Agora", e o Diário guarda o "Histórico"
    char notaDiario[100];
    sprintf(notaDiario, "Descobri: %s", titulo);
    adicionarNotaDiario(notaDiario);
}
//resolver npcs errantes
void resolverEncontroErrante(GameState *gs) {
    novoMomento();
    logMsg("Voce vasculha as matas e estradas proximas a base.");
    modificarStatus(gs, &gs->energia, -15, MAX_ENERGIA_PLAYER, "Energia");

    // Verifica se ainda tem alguém para encontrar
    if (gs->npcPedro && gs->npcBianca && gs->npcIvo && gs->npcLara && gs->npcTom) {
        logMsg("Nao encontrou ninguem novo. A area parece deserta.");
        return;
    }

    // Sorteia até achar um não recrutado
    int npcSorteado = -1;
    int tentativas = 0;
    while (npcSorteado == -1 && tentativas < 20) {
        int r = rand() % 5;
        if (r == 0 && !gs->npcPedro) npcSorteado = 0;
        else if (r == 1 && !gs->npcBianca) npcSorteado = 1;
        else if (r == 2 && !gs->npcIvo) npcSorteado = 2;
        else if (r == 3 && !gs->npcLara) npcSorteado = 3;
        else if (r == 4 && !gs->npcTom) npcSorteado = 4;
        tentativas++;
    }

    if (npcSorteado != -1) {
        
        // --- PEDRO (Carpinteiro) ---
        if (npcSorteado == 0) {
            logMsg("Encontrou um homem carregando tabuas.");
            logMsg("Pedro: 'Construir e melhor que destruir.'");
            renderizarHUD(gs); // Atualiza a tela com os textos acima
            
            printf("1 - 'Precisamos de reparos na base. Venha.'\n");
            printf("2 - 'Deixe as tabuas e suma.'\n");
            printf("3 - 'Boa sorte na caminhada.'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) { logMsg("Pedro: 'Maos a obra.'"); gs->npcPedro=1; modificarStatus(gs, &gs->aliados, 1, 99, "Aliados"); }
            else if (esc == 2) { logMsg("Ele largou as tabuas e fugiu."); oferecerItem(gs, ITEM_TABUAS); gs->npcPedro=2; }
            else { logMsg("Ele seguiu seu caminho."); gs->npcPedro=2; }
        }
        
        // --- BIANCA (Veterinária) ---
        else if (npcSorteado == 1) {
            logMsg("Viu uma mulher cuidando de um passaro ferido.");
            logMsg("Bianca: 'Toda vida importa, ate agora.'");
            renderizarHUD(gs);
            
            printf("1 - 'Cuidamos uns dos outros. Junte-se a nos.'\n");
            printf("2 - 'Animais sao comida. Nada mais.'\n");
            printf("3 - 'Tem algum remedio para gente?'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) { logMsg("Bianca: 'Espero que tenham espaco.'"); gs->npcBianca=1; modificarStatus(gs, &gs->aliados, 1, 99, "Aliados"); }
            else if (esc == 2) { logMsg("Ela te olhou com nojo e saiu."); gs->npcBianca=2; }
            else { logMsg("Ela te deu uma atadura e saiu."); modificarStatus(gs, &gs->vida, 10, 100, "Vida"); gs->npcBianca=2; }
        }
        
        // --- IVO (Mecânico) ---
        else if (npcSorteado == 2) {
            logMsg("Um homem sujo de graxa mexe num motor velho.");
            logMsg("Ivo: 'Essa belezinha so precisa de carinho.'");
            renderizarHUD(gs);
            
            printf("1 - 'Voce entende de maquinas? Vem com a gente.'\n");
            printf("2 - 'Pare de fazer barulho!'\n");
            printf("3 - 'Achou alguma coisa util ai?'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) { logMsg("Ivo: 'Se tiver ferramentas, eu vou.'"); gs->npcIvo=1; modificarStatus(gs, &gs->aliados, 1, 99, "Aliados"); }
            else if (esc == 3) { logMsg("Ivo: 'So essa gasolina velha.'"); modificarStatus(gs, &gs->gasolina, 1, 999, "Gasolina"); gs->npcIvo=2; }
            else { logMsg("Ele resmungou e saiu."); gs->npcIvo=2; }
        }
        
        // --- LARA (Agricultora) ---
        else if (npcSorteado == 3) {
            logMsg("Uma jovem coleta amostras de solo.");
            logMsg("Lara: 'A terra ainda e fertil, so precisa de amor.'");
            renderizarHUD(gs);
            
            printf("1 - 'Temos uma horta precisando de ajuda.'\n");
            printf("2 - 'Tem comida nessa mochila?'\n");
            printf("3 - 'Cuidado com os zumbis.'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) { logMsg("Lara: 'Comida fresca? Estou dentro.'"); gs->npcLara=1; modificarStatus(gs, &gs->aliados, 1, 99, "Aliados"); }
            else if (esc == 2) { logMsg("Ela jogou sementes e correu."); ganharSementes(gs, 2); gs->npcLara=2; }
            else { logMsg("Ela agradeceu e saiu."); gs->npcLara=2; }
        }
        
        // --- TOM (Músico) ---
        else if (npcSorteado == 4) {
            logMsg("Um rapaz afina um violao surrado.");
            logMsg("Tom: 'O silencio desse mundo me enlouquece.'");
            renderizarHUD(gs);
            
            printf("1 - 'Toque na nossa base. Precisamos de alegria.'\n");
            printf("2 - 'Violao nao mata zumbi. Largue isso.'\n");
            printf("3 - 'Faca silencio ou morra.'\n");
            printf("Escolha: ");
            
            int esc = lerInput();
            if (esc == 1) { logMsg("Tom: 'Musica para meus ouvidos.'"); gs->npcTom=1; modificarStatus(gs, &gs->aliados, 1, 99, "Aliados"); }
            else if (esc == 2) { logMsg("Tom: 'A alma morre antes do corpo.' Saiu."); gs->npcTom=2; }
            else { logMsg("Ele fugiu assustado."); gs->npcTom=2; }
        }
    } else {
        logMsg("Ninguem por perto hoje.");
    }
}

//função apra treinar Leo
void treinarLeo(GameState *gs) {
    if (gs->leoNivel >= 4) {
        novoMomento();
        logMsg("Leo ja aprendeu tudo o que voce podia ensinar.");
        logMsg("Ele e um sobrevivente completo agora.");
        pausar();
        return;
    }

    novoMomento();
    printf("=== TREINAMENTO DE SOBREVIVENCIA ===\n");
    printf("Nivel Atual: %d / 4\n", gs->leoNivel);
    printf("Progresso (XP): [%d / 100]\n\n", gs->leoXP);
    
    printf("O que voce quer ensinar hoje?\n");
    printf("1 - Furtividade e Silencio (Seguranca)\n");
    printf("2 - Identificar Recursos (Coleta)\n");
    printf("3 - Primeiros Socorros (Medicina)\n");
    printf("0 - Voltar\n");
    printf("Escolha: ");
    
    int esc = lerInput();
    novoMomento();

    if (esc >= 1 && esc <= 3) {
        if (gs->energia >= 20) {
            modificarStatus(gs, &gs->energia, -20, MAX_ENERGIA_PLAYER, "Energia");
            
            // Ganho de XP (Varia um pouco para ser dinâmico)
            int ganhoXP = 25 + (rand() % 15); // 25 a 40 XP
            gs->leoXP += ganhoXP;
            
            // Narrativa baseada na escolha
            if (esc == 1) logMsg("Voces praticaram andar sem fazer barulho.");
            else if (esc == 2) logMsg("Voce ensinou a diferenciar plantas e sucatas.");
            else logMsg("Ensinou como limpar feridas e fazer torniquetes.");
            
            // LEVEL UP
            if (gs->leoXP >= 100) {
                gs->leoNivel++;
                gs->leoXP = 0; // Reseta barra (ou carrega sobra: gs->leoXP -= 100)
                
                printf("\n========================================\n");
                printf(" [ ! ] LEO SUBIU PARA O NIVEL %d!\n", gs->leoNivel);
                printf("========================================\n");
                
                if (gs->leoNivel == 1) logMsg("NOVA HABILIDADE: Sombra (Aumenta seguranca diaria).");
                else if (gs->leoNivel == 2) logMsg("NOVA HABILIDADE: Olheiro (Chance de itens extras).");
                else if (gs->leoNivel == 3) logMsg("NOVA HABILIDADE: Jardineiro (Cuida da horta).");
                else if (gs->leoNivel == 4) logMsg("NOVA HABILIDADE: Socorrista (Salva sua vida).");
                
                // Recupera status do garoto ao evoluir (motivação)
                char msg[30]; sprintf(msg, "Energia (%s)", gs->nomeFilho);
                modificarStatus(gs, &gs->filho.energia, 50, MAX_STATUS_LEO, msg);
            } else {
                printf("Leo esta aprendendo rapido. (+%d XP)\n", ganhoXP);
            }
            
            renderizarHUD(gs); // Para ver o log
            pausar();
        } else {
            logMsg("Voce esta cansado demais para dar aula.");
        }
    }
}

// --- LÓGICA DO JOGO ---
void jogar(char *nomeInicial) {
    GameState gs;
    gs.dias = 1;
    gs.horario = HORARIO_MANHA;
    gs.acoesNoTurno = 0;
    gs.vida = 100;
    gs.energia = 100;
    gs.comida = 3;
    gs.dinheiro = 0;
    gs.gasolina = 8;
    gs.seguranca = 50;
    
    // Início com 1 Mun e 1 Pilha 
    gs.municao = 3;
    gs.pilhas = 2;
    
    gs.geracao = 0;
    
    //INICIO NA BASE
    gs.localAtual = LOC_BASE;

    // Início de Status Especiais 
    gs.aliados = 0;
    //diárias cidade
    gs.visitouMercadoHoje = 0;
    gs.visitouFarmaciaHoje = 0;
    gs.visitouPredioHoje = 0;
    gs.visitouHospitalHoje = 0;
    gs.visitouRodoviariaHoje = 0;
    gs.visitouPracaHoje = 0;
    gs.visitouEscolaHoje = 0;
    gs.visitouFeiraHoje = 0;
    gs.visitouPredioSamamba = 0;
    gs.visitouMetroHoje = 0;
    gs.visitouRadioHoje = 0;
    gs.visitouParedaoHoje = 0;
    gs.visitouGatosHoje = 0;
    gs.visitouIFHoje = 0;

    //cidades
    gs.desbloqueouSobral = 0;
    gs.desbloqueouPlanalta = 0;
    gs.desbloqueouSamamba = 0;
    gs.desbloqueouCei = 0;
    gs.descobriuTunel = 0;
    
    // PROGRESSO CARTOGRAFO
    gs.ouviuLendaCartografo = 0;
    gs.fragEscola = 0;
    gs.fragRodoviaria = 0;
    gs.fragPraca = 0;

    // PROGRESSO SOBRAL 
    gs.checkSobralExplorar = 0;
    gs.checkSobralMercado = 0;
    gs.checkSobralFarmacia = 0;
    gs.checkSobralPredio = 0;
    gs.checkSobralRic = 0;

    //progresso planalta
    gs.npcAlberto = 0;
    gs.checkEscolaCofre = 0;
    //npcs errantes
    gs.npcPedro = 0;
    gs.npcBianca = 0;
    gs.npcIvo = 0;
    gs.npcLara = 0;
    gs.npcTom = 0;
    
    // PROGRESSO CASA 
    gs.checkCasaSala = 0;
    gs.checkCasaCozinha = 0;
    gs.checkCasaEscritorio = 0;

    //NPCS
    //CEI
    gs.statusNeide = 0;
    gs.statusKevin = 0;
    gs.statusSombra = 0;
    gs.npcKaleb = 0;
    gs.npcBeto = 0;
    gs.npcLucia = 0;
    gs.npcZero = 0;
    gs.npcTales = 0;

    // ESTOQUE INICIAL DO RIC
    gs.stkColt = 1; gs.stkFacao = 1; gs.stkMedkit = 1;
    gs.stkLanterna = 1; gs.stkTabuas = 1; gs.stkSinalizador = 1;
    gs.stkComida = 3; gs.stkSementes = 3; gs.stkMunicao = 3; gs.stkPilhas = 3;
    gs.stkGasolina = 3;

    //NPCS
    gs.npcAugusto = 0;
    gs.npcAna = 0;
    gs.npcMarcos = 0;
        // Inicialização Planalta
    gs.npcMiguel = 0;
    gs.npcClaudio = 0;
    gs.npcElisa = 0;
    
    // Flags Reset
    gs.brincouHoje = 0;
    gs.treinouHoje = 0;
    gs.fezFogueiraHoje = 0;
    gs.cacouHoje = 0;
    gs.fezRondaHoje = 0;
    gs.exercitouHoje = 0;
    gs.ouviuRadioTarde = 0;
    gs.ouviuRadioNoite = 0;
    gs.cuidouHortaHoje = 0;
    
    // Novas Flags Diárias
    gs.bebeuCafeNoTurno = 0;
    gs.procurouSementesNoTurno = 0;
    gs.procurouAliadosNoTurno = 0;

    // Flags de Turno
    gs.explorouNoTurno = 0;
    gs.comeuNoTurno = 0;
    gs.alimentouNoTurno = 0;
    
    // Flags Dicas Reset
    gs.dicaExploracao = 0;
    gs.dicaRonda = 0;
    gs.dicaRadio = 0;
    gs.dicaSono = 0;
    gs.dicaCaca = 0;
    gs.dicaBrincar = 0;
    gs.dicaFogueira = 0;
    gs.dicaHortaMorte = 0;
    gs.dicaHortaGeral = 0;
    gs.dicaFortificar = 0;
    gs.dicaExercitar = 0;
    gs.dicaMedkit = 0;
    gs.dicaTreino = 0;

    //leo
    gs.leoNivel = 0;
    gs.leoXP = 0;
    
    gs.tipoExploracao = 0; 
    int turnoInicial = 1; 
    int pularEventos = 0;
    
    strcpy(gs.nomeJogador, nomeInicial);
    strcpy(gs.nomeFilho, "Leo");
    gs.filho.vida = MAX_STATUS_LEO;
    gs.filho.energia = MAX_STATUS_LEO;
    gs.inventario[0] = ITEM_VAZIO;
    gs.inventario[1] = ITEM_VAZIO;
    gs.inventario[2] = ITEM_VAZIO;
    gs.historiaAtualizadaHoje = 0;
    
    memset(gs.textoHistoria, 0, TAMANHO_HISTORIA_BUFFER);
    strcpy(gs.textoHistoria, "Voces tentam sobreviver neste mundo hostil...");

    iniciarLogs();
    totalLinhasDiario = 0; 
    
    // LOG INICIAL DO DIA 1
    logMsg("=== DIA 1 ===");
    
    int motivoFim = FIM_JOGANDO;
    char msgTemp[100]; 
    int escolha;

    renderizarHUD(&gs);

    // INICIALIZAÇÃO DA NOVA HORTA
gs.semAlface = 2; // Começa com algumas
gs.semTomate = 1;
gs.semCenoura = 1;
gs.estoqueAdubo = 1;

//INICIALIZAÇÃO DAS TAREFAS DOS ALIADOS
for(int i=0; i<TOTAL_NPCS; i++) {
        gs.tarefasAliados[i] = 0;
}
gs.ordensDadasHoje = 0;

for(int i=0; i<3; i++) {
    gs.canteiros[i].estagio = ESTAGIO_VAZIO;
    gs.canteiros[i].tipo = PLANTA_VAZIA;
    gs.canteiros[i].hidratacao = 0;
    gs.canteiros[i].adubado = 0;
}

//inicialização da história
    gs.nivelHistoria = 0;
    // Texto Inicial (História 0)
    strcpy(gs.textoHistoria, "--- DIARIO DO VIAJANTE: PAGINA 1 ---\n");
    strcat(gs.textoHistoria, "'O silencio da base seria ensurdecer sem meu filho.\n");
    strcat(gs.textoHistoria, "Dizem que ha um lugar seguro chamado Eden.\n");
    strcat(gs.textoHistoria, "Preciso seguir para Sobral. A resposta deve estar la.'\n\n");

    while (motivoFim == FIM_JOGANDO) {

        // --- BÔNUS LEO NÍVEL 4 (VIDA EXTRA) ---
        if (gs.vida <= 0 && gs.leoNivel >= 4) {
            // Verifica se tem Medkit
            int slotMed = -1;
            for(int i=0; i<3; i++) if(gs.inventario[i] == ITEM_MEDKIT) slotMed = i;
            
            if (slotMed != -1) {
                gs.inventario[slotMed] = ITEM_VAZIO;
                gs.vida = 50; // Revive com metade
                novoMomento();
                logMsg("VOCE CAIU... Mas Leo usou o Kit Medico em voce!");
                logMsg("Leo: 'Nao vou deixar voce morrer!'");
                renderizarHUD(&gs);
                pausar();
                continue; // Segue o jogo
            }
        }
        
        // --- MORTE E SUCESSÃO ---
        if (gs.vida <= 0) {
            novoMomento();
            logMsg("Voce sucumbiu aos ferimentos...");
            fecharMomento(&gs); 

            novoMomento();
            if (gs.geracao == 0) {
                sprintf(msgTemp, "%s chora ao ver voce cair...", gs.nomeFilho);
                logMsg(msgTemp);
                strcat(gs.textoHistoria, "\n\nVoce lutou bravamente, mas nao resistiu.");
                strcat(gs.textoHistoria, "\n[PASSAGEM DE TEMPO] Leo pegou seus itens e jurou sobreviver.");
                strcat(gs.textoHistoria, "\nAnos depois, ele encontrou uma crianca perdida e decidiu protege-la.");
                fecharMomento(&gs); 

                memset(gs.textoHistoria, 0, TAMANHO_HISTORIA_BUFFER);
                strcpy(gs.textoHistoria, "Uma nova geracao se inicia. Proteja a crianca a todo custo.");
                strcpy(gs.nomeJogador, "Leo");
                strcpy(gs.nomeFilho, "Crianca");
            } else {
                logMsg("O ciclo de sacrificio se repete...");
                strcat(gs.textoHistoria, "\n\nMais um protetor se vai.");
                strcat(gs.textoHistoria, "\n[PASSAGEM DE TEMPO] A crianca cresceu e agora assume o manto.");
                fecharMomento(&gs); 

                memset(gs.textoHistoria, 0, TAMANHO_HISTORIA_BUFFER);
                strcpy(gs.textoHistoria, "O ciclo continua. Sobreviva.");
                strcpy(gs.nomeJogador, "Leo"); 
                strcpy(gs.nomeFilho, "Crianca");
            }

            gs.vida = MAX_VIDA_PLAYER;
            gs.energia = MAX_ENERGIA_PLAYER;
            gs.filho.vida = MAX_STATUS_LEO;
            gs.filho.energia = MAX_STATUS_LEO;
            gs.geracao++;
            // Reset flags
            gs.brincouHoje = 0; gs.treinouHoje = 0; gs.fezFogueiraHoje = 0; 
            gs.cacouHoje = 0; gs.fezRondaHoje = 0; gs.exercitouHoje = 0;
            gs.ouviuRadioTarde = 0; gs.ouviuRadioNoite = 0; gs.cuidouHortaHoje = 0;
            // Reset de flags por Turno
            gs.bebeuCafeNoTurno = 0; gs.procurouSementesNoTurno = 0; gs.procurouAliadosNoTurno = 0;
            gs.explorouNoTurno = 0; gs.comeuNoTurno = 0; gs.alimentouNoTurno = 0;
            
            turnoInicial = 1; 
            
            novoMomento();
            logMsg("Uma nova geracao se inicia.");
            fecharMomento(&gs); 
            continue; 
        }

        if (gs.filho.vida <= 0) {
            motivoFim = FIM_MORTE_FILHO;
            break;
        }

        verificarAtualizacaoHistoria(&gs);

        verificarEventosScriptados(&gs); 

        int recuperouVida = 0;
        int recuperouEnergia = 0;
        int recuperouVidaFilho = 0;
        int recuperouEnergiaFilho = 0;

// --- SISTEMA DE EVENTOS (ATUALIZADO) ---
        int eventoOcorreu = 0;
        
        if (!pularEventos) {
        // ALTERAÇÃO: Eventos só ocorrem se o jogador estiver na BASE
        if (gs.localAtual == LOC_BASE) {
            
            // 1. Verificar Eventos de Baixa Segurança (Prioridade Alta)
            if (!turnoInicial) {
                executarEventoBaixaSeguranca(&gs, &eventoOcorreu);
            }

            // 2. Se não houve evento de segurança, tenta Eventos Aleatórios Normais
            int vaiTerEvento = 0;
            // Ajuste para não acumular chances excessivas
            if (!eventoOcorreu && !turnoInicial && rand() % 100 < 30) vaiTerEvento = 1;
            
            // Apenas executa se NÃO ocorreu evento de segurança antes
            if (vaiTerEvento && !eventoOcorreu) {
                int tipoEvento = rand() % 27; 
                
                if (tipoEvento < 10) {
                    novoMomento();
                    char msgTemp[100]; // Requerido para os sprintf abaixo
                    switch(tipoEvento) {
                        case 0: logMsg("Um vento gelado entrou na base."); modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia"); 
                                sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, -5, MAX_STATUS_LEO, msgTemp); break;
                        case 1: logMsg("Tiros proximos. O medo aumenta."); modificarStatus(&gs, &gs.seguranca, -5, MAX_SEGURANCA, "Seguranca"); break;
                        case 2: sprintf(msgTemp, "%s tropecou e se machucou.", gs.nomeFilho); logMsg(msgTemp); 
                                sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, -3, MAX_STATUS_LEO, msgTemp); break;
                        case 3: logMsg("Goteiras molharam a base."); modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia"); break;
                        case 4: logMsg("Voce sentiu uma fraqueza repentina."); modificarStatus(&gs, &gs.vida, -5, MAX_VIDA_PLAYER, "Vida"); break;
                        case 5: logMsg("Encontrou um pacote de biscoitos."); modificarStatus(&gs, &gs.comida, 1, 999, "Comida"); break;
                        case 6: logMsg("Encontrou pilhas velhas no chao."); modificarStatus(&gs, &gs.pilhas, 2, 999, "Pilhas"); break;
                        case 7: sprintf(msgTemp, "%s ajudou a reforcar a porta.", gs.nomeFilho); logMsg(msgTemp); modificarStatus(&gs, &gs.seguranca, 5, MAX_SEGURANCA, "Seguranca"); break;
                        case 8: logMsg("Encontrou uma bala perdida."); modificarStatus(&gs, &gs.municao, 1, 999, "Municao"); break; 
                        case 9: logMsg("Uma brisa fresca acalmou os animos."); sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, 5, MAX_STATUS_LEO, msgTemp); break;
                    }
                    fecharMomento(&gs); 
                }
                else if (tipoEvento < 17) { 
                    int idRecorrente = tipoEvento - 10;
                    char msgTemp[100]; 
                    novoMomento();
                    switch(idRecorrente) {
                        case 0: // TEMPESTADE
                            logMsg("EVENTO: Tempestade vindo!");
                            renderizarHUD(&gs); 
                            printf("O que fazer?\n1 - Usar TABUAS (Alta)\n2 - Improvisar (Caotico)\n3 - Ignorar (Caotico)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if (escolha == 1) {
                                 int tem = 0; int s=-1; for(int i=0;i<3;i++) if(gs.inventario[i]==ITEM_TABUAS) {tem=1;s=i;}
                                 if(tem) { gs.inventario[s]=0; logMsg("A base esta segura."); modificarStatus(&gs, &gs.seguranca, 10, 100, "Seguranca"); }
                                 else { logMsg("Voce nao achou tabuas! O vento destruiu a protecao."); modificarStatus(&gs, &gs.seguranca, -20, 100, "Seguranca"); modificarStatus(&gs, &gs.vida, -5, 100, "Vida"); }
                            } else if (escolha == 2) { modificarStatus(&gs, &gs.seguranca, -15, 100, "Seguranca"); }
                            else { modificarStatus(&gs, &gs.energia, -5, 100, "Energia"); }
                            break;
                        case 1: // CORTE
                            sprintf(msgTemp, "EVENTO: %s se cortou.", gs.nomeFilho); logMsg(msgTemp);
                            renderizarHUD(&gs);
                            printf("O que fazer?\n1 - Usar KIT MEDICO (Alta)\n2 - Lavar (Caotico)\n3 - Nada (Caotico)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if (escolha == 1) {
                                int tem=0; int s=-1; for(int i=0;i<3;i++) if(gs.inventario[i]==ITEM_MEDKIT) {tem=1;s=i;}
                                if(tem) { gs.inventario[s]=0; sprintf(msgTemp, "%s foi curado.", gs.nomeFilho); logMsg(msgTemp); 
                                          sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, 25, MAX_STATUS_LEO, msgTemp); recuperouVidaFilho = 1;}
                                else { logMsg("Voce nao tinha o Kit! Infeccionou."); sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, -25, MAX_STATUS_LEO, msgTemp); }
                            } else if (escolha == 2) { sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, -10, MAX_STATUS_LEO, msgTemp); }
                            else { modificarStatus(&gs, &gs.energia, -5, 100, "Energia"); }
                            break;
                        case 2: // ESCURO
                            logMsg("EVENTO: Barulhos no escuro.");
                            renderizarHUD(&gs);
                            printf("O que fazer?\n1 - Usar LANTERNA (Alta)\n2 - Esperar (Caotico)\n3 - Ignorar (Caotico)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if(escolha==1) {
                                int temLanterna = 0; for(int i=0; i<3; i++) if(gs.inventario[i] == ITEM_LANTERNA) temLanterna = 1;
                                if(temLanterna && gs.pilhas > 0) { gs.pilhas--; logMsg("A luz espantou os ratos."); modificarStatus(&gs, &gs.comida, 1, 999, "Comida (Salva)"); }
                                else { logMsg("Sem lanterna ou pilhas! Voce caiu."); modificarStatus(&gs, &gs.vida, -15, 100, "Vida"); }
                            } else if (escolha==2) { modificarStatus(&gs, &gs.vida, -10, 100, "Vida"); }
                            else { modificarStatus(&gs, &gs.energia, -5, 100, "Energia"); }
                            break;
                        case 3: // SAQUEADOR
                            logMsg("EVENTO: Saqueador avistado!");
                            renderizarHUD(&gs);
                            printf("O que fazer?\n1 - Atirar (Alta)\n2 - Esconder (Caotico)\n3 - Fugir (Caotico)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if(escolha==1) {
                                int temColt = 0; for(int i=0;i<3;i++) if(gs.inventario[i]==ITEM_COLT) temColt=1;
                                if(temColt && gs.municao > 0) { gs.municao--; logMsg("Voce o acertou e ele fugiu."); modificarStatus(&gs, &gs.seguranca, 15, 100, "Seguranca"); }
                                else { logMsg("Sem arma ou municao! Ele te atacou."); modificarStatus(&gs, &gs.vida, -25, 100, "Vida"); modificarStatus(&gs, &gs.seguranca, -10, 100, "Seguranca"); }
                            } else if (escolha==2) { logMsg("O saqueador roubou a base."); modificarStatus(&gs, &gs.seguranca, -20, 100, "Seguranca"); }
                            else { modificarStatus(&gs, &gs.energia, -10, 100, "Energia"); }
                            break;
                        case 4: // HELICOPTERO
                            logMsg("EVENTO: Helicoptero sobrevoando.");
                            renderizarHUD(&gs);
                            printf("O que fazer?\n1 - SINALIZADOR (Alta)\n2 - Gritar (Caotico)\n3 - Observar (Caotico)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if(escolha==1) { 
                                 int tem=0; int s=-1; for(int i=0;i<3;i++) if(gs.inventario[i]==ITEM_SINALIZADOR) {tem=1;s=i;}
                                 if(tem) { gs.inventario[s]=0; logMsg("Eles jogaram suprimentos!"); modificarStatus(&gs, &gs.comida, 6, 999, "Comida"); }
                                 else { logMsg("Sem sinalizador! Eles foram embora."); modificarStatus(&gs, &gs.energia, -20, 100, "Energia"); }
                            } else { modificarStatus(&gs, &gs.energia, -5, 100, "Energia"); }
                            break;
                        case 6: // EVENTO ALIADOS (Cachorro)
                            logMsg("EVENTO: Cachorro faminto apareceu.");
                            renderizarHUD(&gs);
                            printf("O que fazer?\n1 - Dar Comida (Media)\n2 - Espantar (Alta)\n3 - Ignorar (Alta)\nEscolha: ");
                            escolha = lerInput();
                            novoMomento();
                            if(escolha==1) {
                                if(gs.comida > 0) {
                                    gs.comida--;
                                    if(rand() % 2 == 0) { logMsg("Ele comeu e ficou por perto!"); modificarStatus(&gs, &gs.aliados, 1, 10, "Aliados"); } 
                                    else { logMsg("Ele comeu e fugiu."); }
                                } else { logMsg("Voce nao tem comida para dar."); }
                            } else if(escolha==2) { logMsg("O cachorro fugiu assustado."); }
                            else { logMsg("O cachorro foi embora."); }
                            break;
                    }
                }
                else {
                    executarEventoRonda(&gs);
                }
            }
        }// <--- FECHAMENTO DO IF (gs.localAtual == LOC_BASE)
     } // <--- FECHAMENTO DO IF (!pularEventos)
        pularEventos = 0; 

        if (gs.vida <= 0) continue; 

        renderizarHUD(&gs); 

        int temMedkit = 0;
        int temTabuas = 0;
        int temColt = 0;
        for(int i=0; i<3; i++) {
            if(gs.inventario[i] == ITEM_MEDKIT) temMedkit = 1;
            if(gs.inventario[i] == ITEM_TABUAS) temTabuas = 1;
            if(gs.inventario[i] == ITEM_COLT) temColt = 1;
        }

        // Variáveis de Menu Dinâmicas
        int opNum = 1;
        int opViajar = 0;
        int opExplorar = 0;
        int opMenuComida = 0; // Agrupado
        int opCafe = 0;
        int opMenuProcurar = 0; // Agrupado
        int opMedkit = 0;
        int opFortificar = 0;
        int opBrincar = 0;
        int opCacar = 0;
        int opTreinar = 0;
        int opFogueira = 0;
        int opRonda = 0;
        int opExercitar = 0;
        int opRadioTarde = 0;
        int opRadioNoite = 0;
        int opGuarda = 0;
        int opDormir = 0;
        int opCuidarHorta = 0;
        int opAliados = 0;
        int passouTempo = 0; // Começa como 0. Só vira 1 se uma ação real ocorrer.
        int escolha = 0;

// --- BLOCO DE MENU DA CIDADE (SOBRAL) ---
        if (gs.localAtual == LOC_CIDADE) {
            int opNum = 1;
            int opExplorar = 0;
            int opMercado = 0;
            int opFarmacia = 0;
            int opPredio = 0;
            int opRic = 0;
            int opCasa = 0;
            int opStatus = 0; 

            // 1. RENDERIZAÇÃO DAS OPÇÕES
            printf("%d - Explorar Arredores\n", opNum);
            opExplorar = opNum++;

            if (!gs.visitouMercadoHoje) {
                printf("%d - Mercado (Recursos + Item)\n", opNum);
                opMercado = opNum++;
            }

            if (!gs.visitouFarmaciaHoje) {
                printf("%d - Farmacia (Eventos)\n", opNum);
                opFarmacia = opNum++;
            }

            if (!gs.visitouPredioHoje) {
                printf("%d - Predio abandonado (Recursos)\n", opNum);
                opPredio = opNum++;
            }

            printf("%d - Ric (O Comerciante)\n", opNum);
            opRic = opNum++;

            // Lógica da Casa
            int visitouTudoSobral = (gs.checkSobralExplorar && gs.checkSobralMercado && 
                                     gs.checkSobralFarmacia && gs.checkSobralPredio && 
                                     gs.checkSobralRic);
            int casaAindaTemLoot = (!gs.checkCasaSala || !gs.checkCasaCozinha || !gs.checkCasaEscritorio);

            if (visitouTudoSobral && casaAindaTemLoot) {
                printf("%d - Destrocos de casa (Novo Local)\n", opNum);
                opCasa = opNum++;
            }
            
            // Opção de Status
            printf("%d - Gerenciar Status (Comida/Descanso)\n", opNum);
            opStatus = opNum++;

            printf("0 - Viajar \n");
            
            // 2. LEITURA E LÓGICA
            printf("Escolha: ");
            escolha = lerInput();

            novoMomento(); 

            if (escolha == 0) { // VIAJAR (Menu Aglutinado)
                printf("=== VIAGEM ===\n");
                printf("[ 1 - Voltar para Base ]");
                
                // Só mostra Planalta se já tiver desbloqueado
                if (gs.desbloqueouPlanalta) printf("  [ 2 - Ruinas de Planalta ]");
                //só mostra Samamba se já tiver desbloqueado
                if (gs.desbloqueouSamamba)  printf("  [ 3 - Samamba ]");
                //cei só mostra Cei se já tiver desbloqueado
                if (gs.desbloqueouCei) printf(" [ 4 - Ruinas de Cei ]");
                printf("  [ 0 - Cancelar ]\n");
                printf("Escolha: ");
                
                int sub = lerInput();
                novoMomento();

                if (sub == 1) { // PARA BASE
                    logMsg("Voce fez a viagem de volta para a base.");
                    modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia (Viagem)");
                    gs.localAtual = LOC_BASE;
                    passouTempo = 0; 
                    pularEventos = 1; // Chega seguro
                }
                else if (sub == 2 && gs.desbloqueouPlanalta) { // PARA PLANALTA
                    if (gs.gasolina >= 2) {
                        logMsg("Voce subiu a serra em direcao a Planalta.");
                        modificarStatus(&gs, &gs.gasolina, -2, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_PLANALTA;
                        
                        // Passagem de tempo e turno
                        passouTempo = 1;
                        gs.acoesNoTurno = 2;
                        pularEventos = 1;
                    } else {
                        logMsg("Gasolina insuficiente (Requer 2).");
                        passouTempo = 0;
                    }
                }
                else if (sub == 3 && gs.desbloqueouSamamba) {
                    if (gs.gasolina >= 3) { // Samamba é longe! Custa 3 Gasolina
                        logMsg("Voce viajou longas horas ate as Ruinas de Samamba.");
                        modificarStatus(&gs, &gs.gasolina, -3, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_SAMAMBA;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else {
                        logMsg("Gasolina insuficiente (Requer 3).");
                        passouTempo = 0;
                    }
                }
                else if (sub == 4 && gs.desbloqueouCei) {
                    if (gs.gasolina >= 1) { // Perto (via túnel)
                        logMsg("Voce atravessou o tunel escuro ate Cei.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CEI;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente."); passouTempo = 0; }
                }
                else {
                    passouTempo = 0;
                }
            }
            else if (escolha == opExplorar) { 
                if (gs.energia >= 15) {
                    resolverExploracaoCidade(&gs);
                    passouTempo = 1; 
                } else {
                    logMsg("Voce esta cansado demais para explorar.");
                }
            }
            else if (opMercado != 0 && escolha == opMercado) {
                if (gs.energia >= 10) {
                    resolverMercado(&gs);
                    if (gs.visitouMercadoHoje) passouTempo = 1;
                } else {
                    logMsg("Energia insuficiente.");
                }
            }
            else if (opFarmacia != 0 && escolha == opFarmacia) {
                if (gs.energia >= 10) {
                    resolverFarmacia(&gs);
                    if (gs.visitouFarmaciaHoje) passouTempo = 1;
                } else {
                    logMsg("Energia insuficiente.");
                }
            }
            else if (opPredio != 0 && escolha == opPredio) {
                if (gs.energia >= 10) {
                    resolverPredioAbandonado(&gs);
                    if (gs.visitouPredioHoje) passouTempo = 1;
                } else {
                    logMsg("Energia insuficiente.");
                }
            }
            else if (escolha == opRic) {
                logMsg("Voce vai ate o Ric.");
                gs.localAtual = LOC_COMERCIANTE;
                gs.checkSobralRic = 1;
                passouTempo = 0; 
            }
            else if (opCasa != 0 && escolha == opCasa) {
                if (gs.energia >= 10) {
                    resolverDestrocosCasa(&gs);
                    passouTempo = 1;
                } else {
                    logMsg("Voce esta cansado demais.");
                }
            }
            
            // --- LÓGICA DO SUBMENU STATUS (TURNO) ---
            else if (opStatus != 0 && escolha == opStatus) {
                printf("=== GERENCIAR STATUS ===\n");
                
                // Mostra se o café está disponível neste TURNO
                printf("[ 1 - Comer ]  [ 2 - Beber Cafe %s ]", gs.bebeuCafeNoTurno ? "(Ja bebeu)" : "");
                
                if (gs.horario == HORARIO_NOITE) {
                    printf("  [ 3 - Dormir (Perigoso) ]  [ 4 - Ficar de Guarda ]");
                }
                
                printf("  [ 0 - Voltar ]\n");
                printf("Escolha: ");
                
                int sub = lerInput();
                novoMomento();
                
                if (sub == 1) { // COMER
                    if (gs.comida > 0) {
                        gs.comida--;
                        logMsg("Voce comeu uma refeicao nas ruinas.");
                        modificarStatus(&gs, &gs.vida, 20, MAX_VIDA_PLAYER, "Vida");
                        passouTempo = 1;
                    } else {
                        logMsg("Sem comida na mochila.");
                        passouTempo = 0;
                    }
                }
                else if (sub == 2) { // CAFÉ (1x por Turno)
                    if (!gs.bebeuCafeNoTurno) {
                        logMsg("Voce preparou um cafe improvisado.");
                        modificarStatus(&gs, &gs.energia, 15, MAX_ENERGIA_PLAYER, "Energia");
                        
                        // Marca flag de turno
                        gs.bebeuCafeNoTurno = 1;
                        
                        passouTempo = 0; 
                    } else {
                        logMsg("Voce ja tomou cafe neste periodo.");
                        passouTempo = 0;
                    }
                }
                else if (sub == 3 && gs.horario == HORARIO_NOITE) { // DORMIR
                    int eventoRuim = 0;
                    // 40% Chance Ruim
                    if (rand() % 100 < 40) {
                        eventoRuim = 1;
                        logMsg("PERIGO: Voces foram surpreendidos enquanto dormiam!");
                        logMsg("Saqueadores levaram coisas e machucaram voces.");
                        modificarStatus(&gs, &gs.vida, -15, MAX_VIDA_PLAYER, "Vida (Ataque)");
                        if(gs.dinheiro > 0) modificarStatus(&gs, &gs.dinheiro, -10, 9999, "Dinheiro (Roubo)");
                        modificarStatus(&gs, &gs.energia, 10, MAX_ENERGIA_PLAYER, "Energia");
                    } else {
                        logMsg("Por sorte, a noite foi tranquila.");
                        modificarStatus(&gs, &gs.energia, 30, MAX_ENERGIA_PLAYER, "Energia");
                        modificarStatus(&gs, &gs.vida, 5, MAX_VIDA_PLAYER, "Vida");
                    }

                    sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); 
                    modificarStatus(&gs, &gs.filho.energia, 20, MAX_STATUS_LEO, msgTemp);
                    
                    passouTempo = 1;
                    gs.acoesNoTurno = 2; 
                }
                else if (sub == 4 && gs.horario == HORARIO_NOITE) { // GUARDA
                    if (gs.energia >= 10) {
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        char logA[60]; sprintf(logA, "Voce vigiou a noite toda.");
                        logMsg(logA);
                        
                        sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                        modificarStatus(&gs, &gs.filho.energia, 10, MAX_STATUS_LEO, msgTemp);
                        
                        passouTempo = 1;
                        gs.acoesNoTurno = 2;
                    } else {
                        logMsg("Cansado demais para vigiar.");
                        passouTempo = 0;
                    }
                }
                else {
                    passouTempo = 0;
                }
            }
            // --------------------------------
            
            else {
                // Inválido
            }
        }
         // --- BLOCO DO COMERCIANTE ---
        else if (gs.localAtual == LOC_COMERCIANTE) {
            printf("Ric: 'Olhe a vontade, mas so aceito dinheiro.'\n");
            printf("SEU DINHEIRO: $%d\n\n", gs.dinheiro);
            
            // ITENS (Estoque 1) - Preços balanceados
            printf("1 - Colt [$150] [%s]\n", gs.stkColt ? "Disp." : "Esgotado");
            printf("2 - Facao [$80] [%s]\n", gs.stkFacao ? "Disp." : "Esgotado");
            printf("3 - Kit Medico [$100] [%s]\n", gs.stkMedkit ? "Disp." : "Esgotado");
            printf("4 - Lanterna [$50] [%s]\n", gs.stkLanterna ? "Disp." : "Esgotado");
            printf("5 - Tabuas [$30] [%s]\n", gs.stkTabuas ? "Disp." : "Esgotado");
            printf("6 - Sinalizador [$40] [%s]\n", gs.stkSinalizador ? "Disp." : "Esgotado");
            
            printf("--------------------------------\n");
            
            // RECURSOS (Estoque 3)
            printf("7 - Comida [$25] [Restam: %d]\n", gs.stkComida);
            printf("8 - Sementes [$20] [Restam: %d]\n", gs.stkSementes);
            printf("9 - Municao [$30] [Restam: %d]\n", gs.stkMunicao);
            printf("10- Pilhas [$20] [Restam: %d]\n", gs.stkPilhas);
            printf("11- Gasolina [$30] [Restam: %d]\n", gs.stkGasolina);
            
            printf("\n12 - Voltar para a Cidade\n");
            printf("13 - Voltar para a Base (Viagem)\n");
            
            printf("Escolha: ");
            escolha = lerInput();
            novoMomento();

            // LÓGICA DE COMPRA
            int itemID = -1;
            int custo = 0;
            int *estoqueItem = NULL; // Ponteiro para controlar estoque de itens únicos
            int tipoRecurso = -1;    // 0=Comida, 1=Sementes, 2=Municao, 3=Pilhas
            int *estoqueRecurso = NULL;

            // Mapeamento das escolhas
            if (escolha == 1) { itemID = ITEM_COLT; custo = 150; estoqueItem = &gs.stkColt; }
            else if (escolha == 2) { itemID = ITEM_FACAO; custo = 80; estoqueItem = &gs.stkFacao; }
            else if (escolha == 3) { itemID = ITEM_MEDKIT; custo = 100; estoqueItem = &gs.stkMedkit; }
            else if (escolha == 4) { itemID = ITEM_LANTERNA; custo = 50; estoqueItem = &gs.stkLanterna; }
            else if (escolha == 5) { itemID = ITEM_TABUAS; custo = 30; estoqueItem = &gs.stkTabuas; }
            else if (escolha == 6) { itemID = ITEM_SINALIZADOR; custo = 40; estoqueItem = &gs.stkSinalizador; }
            
            else if (escolha == 7) { tipoRecurso = 0; custo = 25; estoqueRecurso = &gs.stkComida; }
            else if (escolha == 8) { tipoRecurso = 1; custo = 20; estoqueRecurso = &gs.stkSementes; }
            else if (escolha == 9) { tipoRecurso = 2; custo = 30; estoqueRecurso = &gs.stkMunicao; }
            else if (escolha == 10){ tipoRecurso = 3; custo = 20; estoqueRecurso = &gs.stkPilhas; }
            else if (escolha == 11){ tipoRecurso = 4; custo = 30; estoqueRecurso = &gs.stkGasolina; }

            // PROCESSAMENTO
            if (escolha >= 1 && escolha <= 6) { // COMPRA DE ITENS
                if (*estoqueItem > 0) {
                    if (gs.dinheiro >= custo) {
                        gs.dinheiro -= custo;
                        *estoqueItem = 0; // Esgota
                        oferecerItem(&gs, itemID); // Usa a função padrão de inventário
                        // A função oferecerItem gera log, então não precisamos duplicar muito texto
                    } else {
                        logMsg("Ric: 'Sem dinheiro, sem mercadoria.'");
                    }
                } else {
                    logMsg("Ric: 'Ja vendi tudo desse hoje.'");
                }
            }
            else if (escolha >= 7 && escolha <= 11) { // COMPRA DE RECURSOS
                if (*estoqueRecurso > 0) {
                    if (gs.dinheiro >= custo) {
                        gs.dinheiro -= custo;
                        (*estoqueRecurso)--; // Reduz estoque
                        
                        if (tipoRecurso == 0) { logMsg("Comprou Comida."); modificarStatus(&gs, &gs.comida, 1, 999, "Comida"); }
                        else if (tipoRecurso == 1) { logMsg("Comprou Sementes."); ganharSementes(&gs, 2); }
                        else if (tipoRecurso == 2) { logMsg("Comprou Municao."); modificarStatus(&gs, &gs.municao, 2, 999, "Municao (+2)"); }
                        else if (tipoRecurso == 3) { logMsg("Comprou Pilhas."); modificarStatus(&gs, &gs.pilhas, 2, 999, "Pilhas (+2)"); }
                        
                    } else {
                        logMsg("Ric: 'Isso custa dinheiro, amigo.'");
                    }
                } else {
                    logMsg("Ric: 'Volte amanha, acabou.'");
                }
            }
            else if (escolha == 12) { // VOLTAR CIDADE
                logMsg("Voce se afasta da tenda.");
                gs.localAtual = LOC_CIDADE;
            }
            else if (escolha == 13) { // VOLTAR BASE
                logMsg("Voce viaja de volta para a base.");
                modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia (Viagem)");
                gs.localAtual = LOC_BASE;
                pularEventos = 1; // Pula eventos na chegada
            }
            else {
                logMsg("Ric te olha confuso.");
            }
            
            // IMPORTANTE: Não alteramos 'passouTempo' aqui (mantém 0), 
            // permitindo comprar várias vezes sem gastar horas do dia.
        }
        // --- BLOCO DE PLANALTA ---
// --- BLOCO DE PLANALTA (MENU DINÂMICO COMPLETO) ---
        else if (gs.localAtual == LOC_PLANALTA) {
            int opNum = 1;
                if (!gs.ouviuLendaCartografo) {
                    logMsg("Voce ve um pichacao recente na placa da cidade:");
                    logMsg("'O Cartografo enlouqueceu. Rasgou o mapa em tres.'");
                    logMsg("'Escola, Rodoviaria, Praca... so os loucos acharao a saida.'");
                    gs.ouviuLendaCartografo = 1;
                    adicionarNotaDiario("Lenda local: O mapa de saida esta dividido em 3 partes.");
                }
            // Variáveis de controle das opções
            int opExplorar = 0;
            int opHospital = 0;
            int opEscola = 0;
            int opRodoviaria = 0;
            int opPraca = 0;
            int opStatus = 0;
            
            // 1. RENDERIZAÇÃO
            
            // Opção 1: Sempre Explorar
            printf("%d - Explorar Arredores\n", opNum);
            opExplorar = opNum++;
            
            // Opção 2: Hospital (Se não visitou hoje)
            if (!gs.visitouHospitalHoje) {
                printf("%d - Hospital (Eventos + Medkit)\n", opNum);
                opHospital = opNum++;
            }
            //escola
            if (!gs.visitouEscolaHoje) {
            printf("%d - Escola (Minigame da Senha)\n", opNum);
            opEscola = opNum++;
            }
            
                    if (!gs.visitouRodoviariaHoje) {
            printf("%d - Rodoviaria (Comboio de Saque)\n", opNum);
            opRodoviaria = opNum++;
                    }
            
                    if (!gs.visitouPracaHoje) {
            printf("%d - Praca em ruinas (Stealth/Risco)\n", opNum);
            opPraca = opNum++;
                    }
            
            // Opções Fixas de Gameplay (Sempre no final)
            printf("%d - Gerenciar Status (Comida/Descanso)\n", opNum);
            opStatus = opNum++;
            
            printf("0 - Viajar...\n");
            
            // 2. LEITURA E LÓGICA
            printf("Escolha: ");
            escolha = lerInput();
            novoMomento();

            if (escolha == 0) { // VIAJAR (Funcional)
                printf("=== VIAGEM ===\n");
                printf("[ 1 - Voltar para Base ]  [ 2 - Ruinas de Sobral ]");
                if (gs.desbloqueouSamamba)  printf("  [ 3 - Samamba ]");
                if (gs.desbloqueouCei) printf(" [ 4 - Ruinas de Cei ]");
                printf("[ 0 - Cancelar ]\n");
                printf("Escolha: ");
                int sub = lerInput();
                novoMomento();

                if (sub == 1) { // Base
                    logMsg("Voce desce a serra voltando para a base.");
                    modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                    gs.localAtual = LOC_BASE;
                    passouTempo = 0; pularEventos = 1;
                }
                else if (sub == 2) { // Sobral
                    if (gs.gasolina >= 1) {
                        logMsg("Voce desceu a serra ate Sobral.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CIDADE;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else {
                        logMsg("Gasolina insuficiente (Requer 1).");
                        passouTempo = 0;
                    }
                }
                else if (sub == 3 && gs.desbloqueouSamamba) {
                    if (gs.gasolina >= 3) { // Samamba é longe! Custa 3 Gasolina
                        logMsg("Voce viajou longas horas ate as Ruinas de Samamba.");
                        modificarStatus(&gs, &gs.gasolina, -3, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_SAMAMBA;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else {
                        logMsg("Gasolina insuficiente (Requer 3).");
                        passouTempo = 0;
                    }
                }
                else if (sub == 4 && gs.desbloqueouCei) {
                    if (gs.gasolina >= 1) { // Perto (via túnel)
                        logMsg("Voce atravessou o tunel escuro ate Cei.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CEI;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente."); passouTempo = 0; }
                }
                 else { passouTempo = 0; }
            }
            else if (escolha == opExplorar) { // EXPLORAR 
                if (gs.energia >= 15) {
                    resolverExploracaoPlanalta(&gs);
                    passouTempo = 1;
                } else {
                    logMsg("Voce esta exausto demais.");
                }
            }
            else if (opHospital != 0 && escolha == opHospital) { // HOSPITAL 
                if (gs.energia >= 10) {
                    resolverHospital(&gs);
                    if (gs.visitouHospitalHoje) passouTempo = 1;
                } else {
                    logMsg("Energia insuficiente para vasculhar o hospital.");
                }
            }
            // escola
            else if (opEscola != 0 && escolha == opEscola) {
            if (gs.energia >= 10) {
                resolverEscola(&gs);
                if (gs.visitouEscolaHoje) passouTempo = 1;
            } else {
                logMsg("Voce esta cansado demais para raciocinar.");
            }
            }
            else if (opRodoviaria != 0 && escolha == opRodoviaria) {
            if (gs.energia >= 10) {
                resolverRodoviaria(&gs);
                if (gs.visitouRodoviariaHoje) passouTempo = 1;
            } else {
                logMsg("Voce precisa de energia para enfrentar o comboio.");
            }
            }
            else if (opPraca != 0 && escolha == opPraca) {
            if (gs.energia >= 10) {
                resolverPraca(&gs);
                if (gs.visitouPracaHoje) passouTempo = 1;
            } else {
                logMsg("Voce precisa de energia para se mover em silencio.");
            }
                }
            
            // --- GERENCIAR STATUS (Funcional) ---
            else if (opStatus != 0 && escolha == opStatus) {
                printf("=== GERENCIAR STATUS ===\n");
                printf("[ 1 - Comer ]  [ 2 - Beber Cafe %s ]", gs.bebeuCafeNoTurno ? "(Ja bebeu)" : "");
                if (gs.horario == HORARIO_NOITE) printf("  [ 3 - Dormir (Perigoso) ]  [ 4 - Ficar de Guarda ]");
                printf("  [ 0 - Voltar ]\n");
                printf("Escolha: ");
                int sub = lerInput();
                novoMomento();
                
                if (sub == 1) { // Comer
                    if (gs.comida > 0) {
                        gs.comida--;
                        logMsg("Refeicao consumida.");
                        modificarStatus(&gs, &gs.vida, 20, MAX_VIDA_PLAYER, "Vida");
                        passouTempo = 1;
                    } else { logMsg("Sem comida."); passouTempo = 0; }
                }
                else if (sub == 2) { // Café
                    if (!gs.bebeuCafeNoTurno) {
                        logMsg("Cafe consumido.");
                        modificarStatus(&gs, &gs.energia, 15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.bebeuCafeNoTurno = 1; passouTempo = 0;
                    } else { logMsg("Ja bebeu cafe neste turno."); passouTempo = 0; }
                }
                else if (sub == 3 && gs.horario == HORARIO_NOITE) { // Dormir
                    if (rand() % 100 < 40) {
                        logMsg("PERIGO: Emboscada!");
                        modificarStatus(&gs, &gs.vida, -15, MAX_VIDA_PLAYER, "Vida");
                        modificarStatus(&gs, &gs.energia, 10, MAX_ENERGIA_PLAYER, "Energia");
                    } else {
                        logMsg("Noite segura.");
                        modificarStatus(&gs, &gs.energia, 30, MAX_ENERGIA_PLAYER, "Energia");
                        modificarStatus(&gs, &gs.vida, 5, MAX_VIDA_PLAYER, "Vida");
                    }
                    char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); 
                    modificarStatus(&gs, &gs.filho.energia, 20, MAX_STATUS_LEO, msgTemp);
                    passouTempo = 1; gs.acoesNoTurno = 2;
                }
                else if (sub == 4 && gs.horario == HORARIO_NOITE) { // Guarda
                     if (gs.energia >= 10) {
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        logMsg("Voce vigiou a noite toda.");
                        char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                        modificarStatus(&gs, &gs.filho.energia, 10, MAX_STATUS_LEO, msgTemp);
                        passouTempo = 1; gs.acoesNoTurno = 2;
                    } else { logMsg("Cansado demais."); passouTempo = 0; }
                }
                else { passouTempo = 0; }
            }
            else {
                // Inválido
            }
        }
// --- BLOCO DE SAMAMBA (MENU INICIAL) ---
        else if (gs.localAtual == LOC_SAMAMBA) {
            int opNum = 1;
            int opPredio = 0;
            int opFeira = 0;
            int opMetro = 0;
            int opStatus = 0;
            int opCassino = 0;
            int opTunel = 0;
            // 1. RENDERIZAÇÃO DAS OPÇÕES
                    if (!gs.visitouFeiraHoje) {
            printf("%d - Feira (Jogo da Memoria)\n", opNum);
            opFeira = opNum++;
                }
                    if (!gs.visitouPredioSamamba) {
            printf("%d - Predio 'Jardins Suspensos' (Escalada)\n", opNum);
            opPredio = opNum++;
              }
                    if (!gs.visitouMetroHoje) {
            printf("%d - Estacao de Metro (Puzzle de Memoria)\n", opNum);
            opMetro = opNum++;
            }
                    if (!gs.visitouCassinoHoje) {
            printf("%d - Cassino Clandestino (Apostas)\n", opNum);
            opCassino = opNum++;
            }
                    if (gs.descobriuTunel && !gs.desbloqueouCei) {
            printf("%d - [MISSAO] Explodir Tunel para Cei\n", opNum);
            opTunel = opNum++;
        }

            // 2. OPÇÕES FUNCIONAIS
            printf("%d - Gerenciar Status (Comida/Descanso)\n", opNum);
            opStatus = opNum++;

            printf("0 - Viajar...\n");
            
            // 3. LEITURA E LÓGICA
            printf("Escolha: ");
            escolha = lerInput();
            novoMomento();

            if (escolha == 0) { // VIAJAR
                printf("=== VIAGEM ===\n");
                printf("[ 1 - Voltar p/ Base ] [ 2 - Sobral ] [ 3 - Planalta ]");
                if (gs.desbloqueouCei) printf(" [ 4 - Ruinas de Cei ]");
                printf(" [ 0 - Cancelar ]\n");
                printf("Escolha: ");
                
                int sub = lerInput();
                novoMomento();

                if (sub == 1) { // Base (Muito longe)
                    if (gs.gasolina >= 3) {
                        logMsg("Voce viajou a longa distancia de volta para a base.");
                        modificarStatus(&gs, &gs.gasolina, -3, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_BASE;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente (Requer 3)."); passouTempo = 0; }
                }
                else if (sub == 2) { // Sobral (Longe)
                    if (gs.gasolina >= 2) {
                        logMsg("Voce viajou pelas estradas velhas ate Sobral.");
                        modificarStatus(&gs, &gs.gasolina, -2, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CIDADE;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente (Requer 2)."); passouTempo = 0; }
                }
                else if (sub == 3) { // Planalta (Perto/Serra)
                    if (gs.gasolina >= 1) {
                        logMsg("Voce subiu a serra de volta para Planalta.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_PLANALTA;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente (Requer 1)."); passouTempo = 0; }
                }
                else if (sub == 4 && gs.desbloqueouCei) {
                    if (gs.gasolina >= 1) { // Perto (via túnel)
                        logMsg("Voce atravessou o tunel escuro ate Cei.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CEI;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente."); passouTempo = 0; }
                }
                else { passouTempo = 0; }
            }
            
            // --- GERENCIAR STATUS (Funcional) ---
            else if (opStatus != 0 && escolha == opStatus) {
                printf("=== GERENCIAR STATUS ===\n");
                printf("[ 1 - Comer ]  [ 2 - Beber Cafe %s ]", gs.bebeuCafeNoTurno ? "(Ja bebeu)" : "");
                if (gs.horario == HORARIO_NOITE) printf("  [ 3 - Dormir (Perigoso) ]  [ 4 - Ficar de Guarda ]");
                printf("  [ 0 - Voltar ]\n");
                printf("Escolha: ");
                int sub = lerInput();
                novoMomento();
                
                if (sub == 1) { // Comer
                    if (gs.comida > 0) {
                        gs.comida--;
                        logMsg("Voce comeu entre as ruinas de concreto.");
                        modificarStatus(&gs, &gs.vida, 20, MAX_VIDA_PLAYER, "Vida");
                        passouTempo = 1;
                    } else { logMsg("Sem comida."); passouTempo = 0; }
                }
                else if (sub == 2) { // Café
                    if (!gs.bebeuCafeNoTurno) {
                        logMsg("Cafe consumido.");
                        modificarStatus(&gs, &gs.energia, 15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.bebeuCafeNoTurno = 1; passouTempo = 0;
                    } else { logMsg("Ja bebeu cafe neste turno."); passouTempo = 0; }
                }
                else if (sub == 3 && gs.horario == HORARIO_NOITE) { // Dormir
                    if (rand() % 100 < 40) { // 40% chance de ataque
                        logMsg("PERIGO: Horda urbana atacou o acampamento!");
                        modificarStatus(&gs, &gs.vida, -15, MAX_VIDA_PLAYER, "Vida");
                        modificarStatus(&gs, &gs.energia, 10, MAX_ENERGIA_PLAYER, "Energia");
                    } else {
                        logMsg("Voces se esconderam bem num predio alto.");
                        modificarStatus(&gs, &gs.energia, 30, MAX_ENERGIA_PLAYER, "Energia");
                        modificarStatus(&gs, &gs.vida, 5, MAX_VIDA_PLAYER, "Vida");
                    }
                    char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); 
                    modificarStatus(&gs, &gs.filho.energia, 20, MAX_STATUS_LEO, msgTemp);
                    passouTempo = 1; gs.acoesNoTurno = 2;
                }
                else if (sub == 4 && gs.horario == HORARIO_NOITE) { // Guarda
                     if (gs.energia >= 10) {
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        logMsg("Voce vigiou a selva de pedra a noite toda.");
                        char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                        modificarStatus(&gs, &gs.filho.energia, 10, MAX_STATUS_LEO, msgTemp);
                        passouTempo = 1; gs.acoesNoTurno = 2;
                    } else { logMsg("Cansado demais."); passouTempo = 0; }
                }
                else { passouTempo = 0; }
            }
                    else if (opFeira != 0 && escolha == opFeira) { // FEIRA
                        if (gs.energia >= 10) {
                            resolverFeira(&gs);
                            if (gs.visitouFeiraHoje) passouTempo = 1;
                        } else {
                            logMsg("Voce esta exausto demais.");
                        }
                    }
                    else if (opPredio != 0 && escolha == opPredio) { // PRÉDIO
                    if (gs.energia >= 10) {
                        resolverPredioSamamba(&gs);
                        if (gs.visitouPredioSamamba) passouTempo = 1;
                    } else {
                        logMsg("Voce esta cansado demais para escalar.");
                    }
                    }
                    else if (opMetro != 0 && escolha == opMetro) { // METRÔ
                    if (gs.energia >= 10) {
                        resolverMetro(&gs);
                        if (gs.visitouMetroHoje) passouTempo = 1;
                    } else {
                        logMsg("Voce esta cansado demais para se concentrar.");
                    }
                    }
                    else if (opCassino != 0 && escolha == opCassino) { // CASSINO
                    if (gs.energia >= 10) {
                        resolverCassino(&gs);
                        if (gs.visitouCassinoHoje) passouTempo = 1;
                    } else {
                        logMsg("Voce precisa de energia para lidar com o Bicheiro.");
                    }
                    }
                    else if (opTunel != 0 && escolha == opTunel) {
                        resolverExplosaoTunel(&gs);
                        // Não passa tempo obrigatoriamente, é uma ação de crafting/evento
                    }
                    else {
                        // Opção inválida
                    }
        }
// --- BLOCO DE CEI (RUINAS DE CEI) ---
        else if (gs.localAtual == LOC_CEI) {
            int opNum = 1;
            
            // Variáveis de Opções (Futuras)
            int opRadio = 0;
            int opParedao = 0;
            int opGatos = 0;
            int opIF = 0;
            int opStatus = 0;
            if (!gs.visitouRadioHoje) {
            printf("%d - Torre da Radio 'Voz da Laje' (Sintonia)\n", opNum);
            opRadio = opNum++;
            }
            if (!gs.visitouParedaoHoje) {
            printf("%d - Paredao de som (Controle de Horda)\n", opNum);
            opParedao = opNum++;
            }
            if (!gs.visitouGatosHoje) {
            printf("%d - Central de Gatos (Puzzle Eletrico)\n", opNum);
            opGatos = opNum++;
            }
            if (!gs.visitouIFHoje) {
            printf("%d - IF Cei (Laboratorio/Crafting)\n", opNum);
            opIF = opNum++;
            }

            printf("%d - Gerenciar Status (Comida/Descanso)\n", opNum);
            opStatus = opNum++;

            printf("0 - Viajar...\n");
            
            // 3. LEITURA
            printf("Escolha: ");
            escolha = lerInput();
            novoMomento();

            // 4. LÓGICA
            if (escolha == 0) { // VIAJAR (Funcional)
                printf("=== VIAGEM ===\n");
                printf("[ 1 - Samamba (Tunel) ]  [ 2 - Base ]  [ 0 - Cancelar ]\n");
                printf("Escolha: ");
                int sub = lerInput();
                novoMomento();

                if (sub == 1) { // Para Samamba
                    if (gs.gasolina >= 1) {
                        logMsg("Voce voltou pelo tunel para Samamba.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_SAMAMBA;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Sem gasolina (Requer 1)."); passouTempo = 0; }
                }
                else if (sub == 2) { // Para Base
                    if (gs.gasolina >= 4) {
                        logMsg("A viagem de volta para a base foi longa.");
                        modificarStatus(&gs, &gs.gasolina, -4, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -20, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_BASE;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente (Requer 4)."); passouTempo = 0; }
                }
                else { passouTempo = 0; }
            }
            
            else if (opStatus != 0 && escolha == opStatus) { // STATUS (Funcional)
                printf("=== GERENCIAR STATUS ===\n");
                printf("[ 1 - Comer ]  [ 2 - Beber Cafe %s ]", gs.bebeuCafeNoTurno ? "(Ja bebeu)" : "");
                if (gs.horario == HORARIO_NOITE) printf("  [ 3 - Dormir (Perigoso) ]  [ 4 - Ficar de Guarda ]");
                printf("  [ 0 - Voltar ]\n");
                printf("Escolha: ");
                
                int sub = lerInput();
                novoMomento();
                
                if (sub == 1) { // Comer
                    if (gs.comida > 0) {
                        gs.comida--;
                        logMsg("Voce comeu entre as barricadas.");
                        modificarStatus(&gs, &gs.vida, 20, MAX_VIDA_PLAYER, "Vida");
                        passouTempo = 1;
                    } else { logMsg("Sem comida."); passouTempo = 0; }
                }
                else if (sub == 2) { // Café
                    if (!gs.bebeuCafeNoTurno) {
                        logMsg("Cafe consumido.");
                        modificarStatus(&gs, &gs.energia, 15, MAX_ENERGIA_PLAYER, "Energia");
                        gs.bebeuCafeNoTurno = 1; passouTempo = 0;
                    } else { logMsg("Ja bebeu cafe neste turno."); passouTempo = 0; }
                }
                else if (sub == 3 && gs.horario == HORARIO_NOITE) { // Dormir
                    if (rand() % 100 < 40) {
                        logMsg("PERIGO: Gangues locais tentaram invadir!");
                        modificarStatus(&gs, &gs.vida, -15, MAX_VIDA_PLAYER, "Vida");
                        modificarStatus(&gs, &gs.energia, 10, MAX_ENERGIA_PLAYER, "Energia");
                    } else {
                        logMsg("Noite segura em uma laje alta.");
                        modificarStatus(&gs, &gs.energia, 30, MAX_ENERGIA_PLAYER, "Energia");
                        modificarStatus(&gs, &gs.vida, 5, MAX_VIDA_PLAYER, "Vida");
                    }
                    char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); 
                    modificarStatus(&gs, &gs.filho.energia, 20, MAX_STATUS_LEO, msgTemp);
                    passouTempo = 1; gs.acoesNoTurno = 2;
                }
                else if (sub == 4 && gs.horario == HORARIO_NOITE) { // Guarda
                     if (gs.energia >= 10) {
                        modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                        logMsg("Voce vigiou os becos a noite toda.");
                        char msgTemp[30]; sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                        modificarStatus(&gs, &gs.filho.energia, 10, MAX_STATUS_LEO, msgTemp);
                        passouTempo = 1; gs.acoesNoTurno = 2;
                    } else { logMsg("Cansado demais."); passouTempo = 0; }
                }
                else { passouTempo = 0; }
            }
            else if (opRadio != 0 && escolha == opRadio) { // RÁDIO
            if (gs.energia >= 10) {
                resolverRadioCei(&gs);
                if (gs.visitouRadioHoje) passouTempo = 1;
            } else {
                logMsg("Voce esta exausto demais para subir a torre.");
            }
            }
            else if (opParedao != 0 && escolha == opParedao) { // PAREDÃO
            if (gs.energia >= 10) {
                resolverParedao(&gs);
                if (gs.visitouParedaoHoje) passouTempo = 1;
            } else {
                logMsg("Voce precisa de energia para operar o som.");
            }
            }
            else if (opGatos != 0 && escolha == opGatos) { // GATOS
            if (gs.energia >= 10) {
                resolverCentralGatos(&gs);
                if (gs.visitouGatosHoje) passouTempo = 1;
            } else {
                logMsg("Voce precisa de energia para raciocinar.");
            }
            }
            else if (opIF != 0 && escolha == opIF) { // IF CEÍ
            if (gs.energia >= 10) {
                resolverIFCei(&gs);
                if (gs.visitouIFHoje) passouTempo = 1;
            } else {
                logMsg("Voce precisa de energia para estudar.");
            }
            }
            else {
                // Opção inválida
            }
        }
        else {

        printf("O que fazer?\n");
        
        if (!gs.explorouNoTurno) {
            printf("%d - Explorar arredores\n", opNum);
            opExplorar = opNum++;

        if (gs.desbloqueouSobral) {
            printf("%d - Viajar (Requer Gasolina)\n", opNum);
            opViajar = opNum++;
        }
        }

        // Opção agrupada COMIDA (só aparece se nenhuma das sub-opções foi feita)
        if (!gs.alimentouNoTurno && !gs.comeuNoTurno) {
            printf("%d - Gerenciar Comida\n", opNum);
            opMenuComida = opNum++;
        }

        if (!gs.bebeuCafeNoTurno) {
            printf("%d - Beber cafe\n", opNum);
            opCafe = opNum++;
        }

        // Opção agrupada PROCURAR (só aparece se nenhuma das sub-opções foi feita)
        if (!gs.procurouSementesNoTurno && !gs.procurouAliadosNoTurno) {
            printf("%d - Procurar Recursos\n", opNum);
            opMenuProcurar = opNum++;
        }
        
        if (gs.horario == HORARIO_NOITE) {
            printf("%d - Ficar de guarda\n", opNum);
            opGuarda = opNum++;
            
            if (gs.seguranca >= 50) printf("%d - Dormir juntos\n", opNum);
            else printf("%d - [PERIGOSO] Dormir juntos (< 50%% Seguranca)\n", opNum);
            opDormir = opNum++;
        }

        // Opção da Horta (Sempre visível na Base)
        printf("%d - Cuidar da Horta (Gerenciar Canteiros)\n", opNum);
        opCuidarHorta = opNum++;

        if (gs.aliados > 0) {
            printf("%d - Gerenciar Aliados (Atribuir Tarefas)\n", opNum);
            // Reutilize uma variável ou crie int opAliados = 0; no início do bloco
            opAliados = opNum++; 
        }

        if (temMedkit) {
            printf("%d - Usar Kit Medico\n", opNum);
            opMedkit = opNum++;
        }
        if (temTabuas) {
            printf("%d - Fortificar base\n", opNum);
            opFortificar = opNum++;
        }
        
        printf("%d - Treinar/Ensinar Leo (Nivel %d)\n", opNum, gs.leoNivel);
        int opTreinarLeo = opNum++;
        
        if (gs.horario == HORARIO_MANHA && !gs.cacouHoje && temColt && gs.municao > 0) {
            printf("%d - Sair para cacar\n", opNum);
            opCacar = opNum++;
        }

        if (gs.horario == HORARIO_MANHA && !gs.exercitouHoje) {
            printf("%d - Se exercitar\n", opNum);
            opExercitar = opNum++;
        }
        
        if (gs.horario == HORARIO_TARDE && !gs.treinouHoje && temColt && gs.municao > 0) {
            printf("%d - Treinar tiro com o garoto\n", opNum);
            opTreinar = opNum++;
        }

        if (gs.horario == HORARIO_TARDE && !gs.ouviuRadioTarde && gs.pilhas > 0) {
            printf("%d - Ouvir radio (Tarde)\n", opNum);
            opRadioTarde = opNum++;
        }

        if (gs.horario == HORARIO_NOITE && !gs.fezFogueiraHoje && temTabuas) {
            printf("%d - Contar historias na fogueira\n", opNum);
            opFogueira = opNum++;
        }

        if (gs.horario == HORARIO_NOITE && !gs.ouviuRadioNoite && gs.pilhas > 0) {
            printf("%d - Ouvir radio (Noite)\n", opNum);
            opRadioNoite = opNum++;
        }

        if (!gs.fezRondaHoje) {
            printf("%d - Fazer ronda\n", opNum);
            opRonda = opNum++;
        }

        printf("0 - MENU\n"); 

        printf("Escolha: ");
        escolha = lerInput();

        passouTempo = 1;

if (escolha == opExplorar && opExplorar != 0) { // EXPLORAR ARREDORES
            // Dica Diário
            if (!gs.dicaExploracao) {
                adicionarNotaDiario("A vegetacao e densa. Um facao ajudaria na exploracao.");
                gs.dicaExploracao = 1;
            }

            // REMOVIDO: gs.tipoExploracao = !gs.tipoExploracao; (Não alternamos mais)
            gs.explorouNoTurno = 1;

            if (gs.energia >= 20 && gs.filho.energia >= 10) {
                novoMomento();
                
                logMsg("Voce saiu para explorar os arredores...");
                
                modificarStatus(&gs, &gs.energia, -20, MAX_ENERGIA_PLAYER, "Energia");
                sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                modificarStatus(&gs, &gs.filho.energia, -10, MAX_STATUS_LEO, msgTemp);
                
                int r = rand() % 100;
                
                // A lógica de probabilidade permanece a mesma, pois funciona para qualquer ambiente
                if(r < 60) { 
                     int tipo = rand() % 5;
                     if(tipo == 0) { logMsg("Encontrou comida."); modificarStatus(&gs, &gs.comida, 1, 999, "Comida"); }
                     else if(tipo == 1) { logMsg("Encontrou municao."); modificarStatus(&gs, &gs.municao, 2, 999, "Municao"); }
                     else if (tipo == 2) { logMsg("Encontrou pilhas."); modificarStatus(&gs, &gs.pilhas, 2, 999, "Pilhas"); }
                    else if(tipo == 3) { 
                             logMsg("Achou uma carteira perdida na mata."); 
                             modificarStatus(&gs, &gs.dinheiro, (rand() % 15) + 5, 9999, "Dinheiro"); 
                         }
                         else { 
                             logMsg("Sifonou gasolina de um trator abandonado."); 
                             modificarStatus(&gs, &gs.gasolina, 1, 999, "Gasolina"); 
                         }
                    }

                else if (r < 80) { 
                    resolverEncontroZumbi(&gs, "Voce encontrou um zumbi vagando entre as arvores!");
                }
                else { 
                    int temFacao = 0;
                    for(int i=0; i<3; i++) if(gs.inventario[i] == ITEM_FACAO) temFacao = 1;
                    
                    if(temFacao) {
                        logMsg("Usou o facao para cortar a vegetacao.");
                        logMsg("Achou um atalho seguro.");
                    } else {
                        logMsg("Vegetacao densa bloqueou o caminho.");
                        logMsg("Voce se arranhou tentando passar.");
                        modificarStatus(&gs, &gs.vida, -5, MAX_VIDA_PLAYER, "Vida");
                    }
                }
            } else {
                novoMomento();
                logMsg("Voces estao muito cansados para sair.");
                fecharMomento(&gs);
                passouTempo = 0;
            }
        }
            else if (escolha == opViajar && opViajar != 0) {
            // EXIBE SUBMENU
            printf("[ 1 - Ruinas de Sobral ]");
            // Futuro: printf("  [ 2 - Fortaleza ]");
            if (gs.desbloqueouPlanalta) printf("  [ 2 - Ruinas de Planalta ]");
            if (gs.desbloqueouSamamba)  printf("  [ 3 - Samamba ]");
            if (gs.desbloqueouCei) printf(" [ 4 - Ruinas de Cei ]"); 
            printf("  [ 0 - Cancelar ]\n");
            printf("Escolha: ");
            
            
            int sub = lerInput();
            
            if (sub == 1) { // SOBRAL
                if (gs.desbloqueouSobral) {
                    if (gs.gasolina >= 1) {
                        novoMomento();
                        logMsg("Voce abasteceu e viajou por horas ate Sobral.");
                        
                        // Custos
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        
                        gs.localAtual = LOC_CIDADE;
                        
                        // --- CORREÇÃO 1: AVANÇAR HORÁRIO ---
                        passouTempo = 1; 
                        // Força o contador de ações para o máximo, garantindo 
                        // que o horário mude (Manhã->Tarde) imediatamente após a viagem.
                        gs.acoesNoTurno = 2; 

                        // --- CORREÇÃO 2: EVITAR INTERRUPÇÃO ---
                        pularEventos = 1; // Impede eventos na chegada (segurança na transição)

                    } else {
                        novoMomento();
                        logMsg("Tanque vazio! Voce precisa de Gasolina.");
                        fecharMomento(&gs); // Pausa para ler
                        passouTempo = 0;
                        
                        // Impede que um evento aleatório ocorra só porque você errou a gasolina
                        pularEventos = 1; 
                    }
                }
            }
                    else if (sub == 2 && gs.desbloqueouPlanalta) { // PLANALTA
            if (gs.gasolina >= 2) { // Custo maior: 2 Gasolina
                novoMomento();
                logMsg("Voce viajou pelas estradas da serra ate Planalta.");
                
                modificarStatus(&gs, &gs.gasolina, -2, 999, "Gasolina");
                modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                
                gs.localAtual = LOC_PLANALTA;
                
                // Configurações de tempo e segurança
                passouTempo = 1; 
                gs.acoesNoTurno = 2; // Força virada de horário
                pularEventos = 1; 
            } else {
                novoMomento();
                logMsg("Gasolina insuficiente! Planalta requer 2 unidades.");
                passouTempo = 0;
                pularEventos = 1;
            }
        }
                    else if (sub == 3 && gs.desbloqueouSamamba) {
                        if (gs.gasolina >= 3) { // Samamba é longe! Custa 3 Gasolina
                            logMsg("Voce viajou longas horas ate as Ruinas de Samamba.");
                            modificarStatus(&gs, &gs.gasolina, -3, 999, "Gasolina");
                            modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                            gs.localAtual = LOC_SAMAMBA;
                            passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                        } else {
                            logMsg("Gasolina insuficiente (Requer 3).");
                            passouTempo = 0;
                        }
                    }
                    else if (sub == 4 && gs.desbloqueouCei) {
                    if (gs.gasolina >= 1) { // Perto (via túnel)
                        logMsg("Voce atravessou o tunel escuro ate Cei.");
                        modificarStatus(&gs, &gs.gasolina, -1, 999, "Gasolina");
                        modificarStatus(&gs, &gs.energia, -5, MAX_ENERGIA_PLAYER, "Energia");
                        gs.localAtual = LOC_CEI;
                        passouTempo = 1; gs.acoesNoTurno = 2; pularEventos = 1;
                    } else { logMsg("Gasolina insuficiente."); passouTempo = 0; }
                }
                        else {
                // Cancelou
                passouTempo = 0;
                // Impede que um evento ocorra só porque você abriu o menu e desistiu
                pularEventos = 1; 
            }
        }
        else if (escolha == opCafe && opCafe != 0) { // CAFE
            novoMomento();
            logMsg("Voce renovou um pouco de sua energia.");
            modificarStatus(&gs, &gs.energia, 15, MAX_ENERGIA_PLAYER, "Energia");
            gs.bebeuCafeNoTurno = 1;
        }
        // SUBMENU PROCURAR
        else if (escolha == opMenuProcurar && opMenuProcurar != 0) {
            // EXIBE EM UMA LINHA NA TELA
            printf("[ 1 - Procurar Sementes ]  [ 2 - Procurar Aliados ]  [ 0 - Cancelar ]\n");
            printf("Escolha: ");
            int sub = lerInput();
            
            if (sub == 1) { // SEMENTES
                novoMomento();
                if (!gs.procurouSementesNoTurno) {
                    modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                    if (rand() % 100 < 50) {
                        logMsg("Voce vasculhou a vegetacao...");
                        ganharSementes(&gs, (rand() % 2) + 1); // 1 ou 2 sementes aleatórias
                    } else {
                        logMsg("Nao encontrou sementes uteis hoje.");
                    }
                    gs.procurouSementesNoTurno = 1;
                } else {
                    logMsg("Voce ja procurou sementes neste turno.");
                }
            }
            else if (sub == 2) { // ALIADOS (Errantes)
                if (!gs.procurouAliadosNoTurno) {
                    resolverEncontroErrante(&gs);
                    gs.procurouAliadosNoTurno = 1;
                } else {
                    novoMomento();
                    logMsg("Voce ja vasculhou a area por pessoas.");
                }
            }
            else {
                passouTempo = 0;
            }
        }
        // SUBMENU COMIDA
        else if (escolha == opMenuComida && opMenuComida != 0) {
            // EXIBE EM UMA LINHA NA TELA
            printf("[ 1 - Comer ]  [ 2 - Alimentar %s ]  [ 0 - Cancelar ]\n", gs.nomeFilho);
            printf("Escolha: ");
            int sub = lerInput();

            if (sub == 1) {
                if (gs.comida > 0) {
                    gs.comida--;
                    gs.comeuNoTurno = 1;
                    novoMomento();
                    logMsg("Voce comeu.");
                    modificarStatus(&gs, &gs.vida, 20, MAX_VIDA_PLAYER, "Vida");
                } else {
                    novoMomento();
                    logMsg("Nao ha comida suficiente!");
                    fecharMomento(&gs);
                    passouTempo = 0;
                }
            }
            else if (sub == 2) {
                if (gs.comida > 0) {
                    gs.comida--;
                    gs.alimentouNoTurno = 1;
                    novoMomento();
                    char logA[30]; sprintf(logA, "%s foi alimentado(a).", gs.nomeFilho);
                    logMsg(logA);
                    sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, 15, MAX_STATUS_LEO, msgTemp);
                    sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, 15, MAX_STATUS_LEO, msgTemp);
                } else {
                    novoMomento();
                    logMsg("Nao ha comida suficiente!");
                    fecharMomento(&gs);
                    passouTempo = 0;
                }
            }
            else {
                passouTempo = 0;
            }
        }
        else if (escolha == opGuarda && opGuarda != 0) { // VIGIAR
            if (gs.energia >= 10) {
                novoMomento();
                modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                char logA[40]; sprintf(logA, "Voce vigiou enquanto %s descansava.", gs.nomeFilho);
                logMsg(logA);
                sprintf(msgTemp, "Energia (%s)", gs.nomeFilho);
                modificarStatus(&gs, &gs.filho.energia, 10, MAX_STATUS_LEO, msgTemp);
                passouTempo = 1;
                gs.acoesNoTurno = 2;
            } else {
                novoMomento();
                logMsg("Voce esta exausto demais para vigiar.");
                fecharMomento(&gs);
                passouTempo = 0;
            }
        }
        else if (escolha == opDormir && opDormir != 0) { // DORMIR
            if (gs.seguranca < 50 && !gs.dicaSono) {
                adicionarNotaDiario("Dormir com pouca seguranca atrai ratos e pesadelos.");
                gs.dicaSono = 1;
            }

            novoMomento();
            logMsg("Voces dormiram...");
            // Lógica de dormir...
            if (gs.seguranca < 50) {
                int r = rand() % 3;
                if (r == 0) {
                    logMsg("[POUCA SEGURANCA] Ratos invadiram a base.");
                    modificarStatus(&gs, &gs.energia, -10, MAX_ENERGIA_PLAYER, "Energia");
                } else if (r == 1) {
                    logMsg("[POUCA SEGURANCA] Voces tiveram pesadelos.");
                    sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, -10, MAX_STATUS_LEO, msgTemp);
                } else {
                    logMsg("[POUCA SEGURANCA] Houve um roubo durante a noite.");
                    if(gs.comida > 0) modificarStatus(&gs, &gs.comida, -1, 999, "Comida");
                }
            } 

            logMsg("Voces tiveram uma noite de sono.");
            logMsg("As energias foram renovadas.");
            modificarStatus(&gs, &gs.energia, 40, MAX_ENERGIA_PLAYER, "Energia");
            modificarStatus(&gs, &gs.vida, 5, MAX_VIDA_PLAYER, "Vida");
            sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, 30, MAX_STATUS_LEO, msgTemp);
            recuperouEnergia = 1; recuperouVida = 1; recuperouEnergiaFilho = 1; recuperouVidaFilho = 1;
            gs.acoesNoTurno = 0; // Dormir reseta ações para forçar passagem de tempo
        }
        else if (escolha == opCuidarHorta && opCuidarHorta != 0) {
                resolverHortaDinamica(&gs);
                // A horta tem gasto interno de energia, não avança o turno automaticamente
                passouTempo = 0; 
        }
        else if (opAliados != 0 && escolha == opAliados) {
            resolverAliadosVisual(&gs);
            // Usar aliados é uma ação administrativa, não gasta turno global (free action),
            // pois cada aliado já tem seu limite de 1x por dia.
            passouTempo = 0; 
        }
        else if (escolha == opFortificar && opFortificar != 0) { 
            // Dica Diário
            if (!gs.dicaFortificar) {
                adicionarNotaDiario("Tabuas sao essenciais para reforcar a seguranca da base.");
                gs.dicaFortificar = 1;
            }

            int slotTabuas = -1;
            for (int i = 0; i < 3; i++) { if (gs.inventario[i] == ITEM_TABUAS) { slotTabuas = i; break; } }
            if (slotTabuas != -1 && gs.energia >= 15) {
                novoMomento();
                modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                gs.inventario[slotTabuas] = ITEM_VAZIO; 
                logMsg("A base foi fortificada.");
                modificarStatus(&gs, &gs.seguranca, 25, MAX_SEGURANCA, "Seguranca");
            } else {
                novoMomento();
                logMsg("Voce precisa de energia e tabuas.");
                fecharMomento(&gs);
                passouTempo = 0;
            }
        }
        else if (escolha == opMedkit && opMedkit != 0) {
            // Dica Diário
            if (!gs.dicaMedkit) {
                adicionarNotaDiario("Kit Medicos sao raros. Use-os com sabedoria.");
                gs.dicaMedkit = 1;
            }

            printf("\nUsar Kit Medico em quem?\n1 - Em mim\n2 - Em %s\n3 - Cancelar\nEscolha: ", gs.nomeFilho);
            int escMed = lerInput();
            int used = 0;
            if (escMed == 1) {
                novoMomento();
                gs.vida = MAX_VIDA_PLAYER;
                logMsg("Voce usou o Kit. Vida restaurada.");
                used = 1;
            } else if (escMed == 2) {
                novoMomento();
                gs.filho.vida = MAX_STATUS_LEO;
                sprintf(msgTemp, "Voce curou %s. Vida restaurada.", gs.nomeFilho);
                logMsg(msgTemp);
                used = 1;
            }
            if (used) {
                for(int i=0; i<3; i++) {
                    if(gs.inventario[i] == ITEM_MEDKIT) { gs.inventario[i] = ITEM_VAZIO; break; }
                }
            } else {
                passouTempo = 0;
            }
        }
        else if (escolha == opTreinarLeo) {
            treinarLeo(&gs);
            passouTempo = 1; // Treinar gasta tempo
        }
        else if (escolha == opCacar && opCacar != 0) {
            // Dica Diário
            if (!gs.dicaCaca) {
                adicionarNotaDiario("Cacar gasta energia e municao, mas fornece carne fresca.");
                gs.dicaCaca = 1;
            }

            if (gs.energia >= 15) {
                novoMomento();
                modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                modificarStatus(&gs, &gs.municao, -1, 999, "Municao"); // Cost Ammo
                int r = rand() % 3;
                char *animal;
                int comidaGanho;
                if(r==0) { animal="coelhos"; comidaGanho=2; }
                else if(r==1) { animal="javalis"; comidaGanho=3; }
                else { animal="um cervo"; comidaGanho=5; }
                
                char buff[60];
                sprintf(buff, "Encontraram %s.", animal);
                logMsg(buff);
                modificarStatus(&gs, &gs.comida, comidaGanho, 999, "Comida");
                
                if (rand() % 100 < 30) {
                    if (rand() % 2 == 0) { 
                        logMsg("Achou municao perto da presa."); 
                        modificarStatus(&gs, &gs.municao, 2, 999, "Municao"); 
                    } else { 
                        logMsg("Achou pilhas perdidas no mato."); 
                        modificarStatus(&gs, &gs.pilhas, 2, 999, "Pilhas"); 
                    }
                }
                
                gs.cacouHoje = 1;
            } else {
                novoMomento();
                logMsg("Voce esta muito cansado para cacar.");
                fecharMomento(&gs);
                passouTempo = 0;
            }
        }
        else if (escolha == opExercitar && opExercitar != 0) {
            // Dica Diário
            if (!gs.dicaExercitar) {
                adicionarNotaDiario("Exercicios fisicos melhoram a saude, mas consomem energia.");
                gs.dicaExercitar = 1;
            }

            if (gs.energia >= 15) {
                novoMomento();
                modificarStatus(&gs, &gs.energia, -15, MAX_ENERGIA_PLAYER, "Energia");
                if (rand() % 2 == 0) logMsg("Voces levantaram pesos improvisados.");
                else logMsg("Fizeram exercicio aerobico.");
                
                modificarStatus(&gs, &gs.vida, 10, MAX_VIDA_PLAYER, "Vida");
                sprintf(msgTemp, "Vida (%s)", gs.nomeFilho);
                modificarStatus(&gs, &gs.filho.vida, 10, MAX_STATUS_LEO, msgTemp);
                
                if (rand() % 100 < 20) {
                     logMsg("Ao mover coisas, achou algo util!");
                     if(rand()%2==0) { modificarStatus(&gs, &gs.comida, 1, 999, "Comida"); }
                     else { modificarStatus(&gs, &gs.pilhas, 1, 999, "Pilhas"); }
                }
                
                gs.exercitouHoje = 1;
            } else {
                novoMomento(); logMsg("Sem energia para exercicio."); fecharMomento(&gs); passouTempo = 0;
            }
        }
        else if (escolha == opTreinar && opTreinar != 0) {
            // Dica Diário
            if (!gs.dicaTreino) {
                adicionarNotaDiario("Treinar o garoto gasta municao, mas aumenta a seguranca a longo prazo.");
                gs.dicaTreino = 1;
            }

            novoMomento();
            modificarStatus(&gs, &gs.municao, -1, 999, "Municao"); // Cost Ammo
            logMsg("Ele esta pegando o jeito.");
            modificarStatus(&gs, &gs.seguranca, 5, MAX_SEGURANCA, "Seguranca");
            gs.treinouHoje = 1;
        }
        else if (escolha == opRadioTarde && opRadioTarde != 0) {
            if (!gs.dicaRadio) {
                adicionarNotaDiario("Pessoas diferentes pedem ajuda no radio.");
                gs.dicaRadio = 1;
            }
            novoMomento();
            modificarStatus(&gs, &gs.pilhas, -1, 999, "Pilhas"); // Cost Battery
            if (rand() % 2 == 0) {
                logMsg("Voces ouviram um chamado e foram receber ajuda.");
                modificarStatus(&gs, &gs.comida, 4, 999, "Comida");
            } else {
                logMsg("Voces ouviram um chamado e cairam em uma armadilha.");
                modificarStatus(&gs, &gs.vida, -4, MAX_VIDA_PLAYER, "Vida");
                sprintf(msgTemp, "Vida (%s)", gs.nomeFilho);
                modificarStatus(&gs, &gs.filho.vida, -4, MAX_STATUS_LEO, msgTemp);
            }
            gs.ouviuRadioTarde = 1;
        }
        else if (escolha == opFogueira && opFogueira != 0) {
            // Dica Diário
            if (!gs.dicaFogueira) {
                adicionarNotaDiario("Fogueiras aumentam a seguranca, mas consomem madeira.");
                gs.dicaFogueira = 1;
            }

            novoMomento();
            for(int i=0; i<3; i++) {
                if(gs.inventario[i] == ITEM_TABUAS) {
                    gs.inventario[i] = ITEM_VAZIO;
                    logMsg("Usou tabuas para a fogueira.");
                    break;
                }
            }
            logMsg("Viraram a noite contando historias.");
            modificarStatus(&gs, &gs.seguranca, 5, MAX_SEGURANCA, "Seguranca");
            gs.fezFogueiraHoje = 1;
        }
        else if (escolha == opRadioNoite && opRadioNoite != 0) {
            if (!gs.dicaRadio) {
                adicionarNotaDiario("Pessoas diferentes pedem ajuda no radio.");
                gs.dicaRadio = 1;
            }
            novoMomento();
            modificarStatus(&gs, &gs.pilhas, -1, 999, "Pilhas"); // Cost Battery
            if (rand() % 2 == 0) {
                logMsg("Uma voz feminina pediu ajuda. Ela ajudou na base.");
                modificarStatus(&gs, &gs.seguranca, 15, MAX_SEGURANCA, "Seguranca");
            } else {
                logMsg("Uma voz masculina pediu ajuda. Era uma armadilha.");
                modificarStatus(&gs, &gs.vida, -9, MAX_VIDA_PLAYER, "Vida");
                sprintf(msgTemp, "Vida (%s)", gs.nomeFilho);
                modificarStatus(&gs, &gs.filho.vida, -9, MAX_STATUS_LEO, msgTemp);
            }
            gs.ouviuRadioNoite = 1;
        }
        else if (escolha == opRonda && opRonda != 0) {
            if (gs.energia >= 10) {
                gs.fezRondaHoje = 1;
                // A função executarEventoRonda já faz o tratamento de pausa correto
                executarEventoRonda(&gs);
            } else {
                novoMomento();
                logMsg("Voce esta muito cansado para fazer a ronda.");
                fecharMomento(&gs);
                passouTempo = 0;
            }
        }
        else if (escolha == 0) { 
            limparTela();
            printf("=== MENU ===\n\n");
            printf("1 - Diario (Ver registros)\n");
            printf("2 - Historico (Ver todos os Logs)\n");
            printf("3 - Historia\n");
            printf("4 - Desistir (Fim de Jogo)\n");
            printf("0 - Voltar\n\n");
            printf("Escolha: ");
            int subMenu = lerInput();

            if (subMenu == 1) {
                exibirDiarioCompleto();
                passouTempo = 0;
                pularEventos = 1;
            } else if (subMenu == 2) {
                exibirHistoricoLogs();
                passouTempo = 0;
                pularEventos = 1;
            } else if (subMenu == 4) {
                gs.filho.vida = 0;
                motivoFim = FIM_DESISTENCIA; 
                passouTempo = 0;
                
            }
            else if (subMenu == 3) { 
                limparTela();
                printf("=== A SAGA DO VIAJANTE ===\n\n");
                printf("%s\n", gs.textoHistoria);
                pausarMenu();
            }
             else {
                passouTempo = 0;
                pularEventos = 1;
            }
        }
// --- CHEAT CODE (ATUALIZADO: MAPA + ALIADOS) ---
        else if (escolha == 23032001) {
            novoMomento();
            logMsg("[CHEAT] GOD MODE: Mapas e Aliados desbloqueados.");
            
            // 1. Desbloquear Cidades
            gs.desbloqueouSobral = 1;
            gs.desbloqueouPlanalta = 1;
            gs.desbloqueouSamamba = 1;
            gs.desbloqueouCei = 1;
            gs.descobriuTunel = 1; // Atalho Samamba-Ceí
            
            // 2. Recrutar NPCs de Sobral
            gs.npcAugusto = 1;
            gs.npcAna = 1;
            gs.npcMarcos = 1;

            // 3. Recrutar NPCs de Planalta
            gs.npcMiguel = 1;
            gs.npcClaudio = 1;
            gs.npcElisa = 1;
            gs.npcAlberto = 1; // Diretor

            // 4. Recrutar NPCs de Ceí
            gs.statusNeide = 1; // 1 = Recrutado
            gs.statusKevin = 1;
            gs.statusSombra = 1;
            gs.npcKaleb = 1;
            gs.npcBeto = 1;
            gs.npcLucia = 1;
            gs.npcZero = 1;
            gs.npcTales = 1;

            // 5. Atualizar Contador e Recursos para Teste
            gs.aliados = 15; // Total de NPCs únicos
            gs.dinheiro += 1000; // Ajuda a testar o Cassino/Ric
            gs.gasolina += 10;   // Ajuda a testar viagens
            
            passouTempo = 0;
            pularEventos = 1;
        }
        // ---------------------------------------------- 
        else {
            passouTempo = 0;
            pularEventos = 1;
        }

        }

        if (gs.vida <= 0) continue;

        if (passouTempo) {
            
            if (escolha != opDormir) { // Dormir já zera ações e força tempo
                if (escolha != opCuidarHorta) { // Horta já incrementou antes
                     gs.acoesNoTurno++;
                }

                if (gs.acoesNoTurno < 2) {
                    fecharMomento(&gs); // AQUI pausa para mostrar o log da ação anterior se não passou o tempo
                    continue; 
                } else {
                    gs.acoesNoTurno = 0; 
                }
            }

            int decaimento = (rand() % 10) + 1;
            
            // Pausa ANTES de limpar a tela para o tempo passar
            // Permite ler o resultado da ação que completou o turno
            fecharMomento(&gs);
            
            // Limpa e mostra o tempo passando
            novoMomento();
            logMsg("O tempo passou...");
            
            if (!recuperouVida) modificarStatus(&gs, &gs.vida, -decaimento, MAX_VIDA_PLAYER, "Vida");
            if (!recuperouEnergia) modificarStatus(&gs, &gs.energia, -decaimento, MAX_ENERGIA_PLAYER, "Energia");
            if (!recuperouVidaFilho) { sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, -decaimento, MAX_STATUS_LEO, msgTemp); }
            if (!recuperouEnergiaFilho) { sprintf(msgTemp, "Energia (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.energia, -decaimento, MAX_STATUS_LEO, msgTemp); }
            
            // --- LÓGICA DE TEMPO CORRIGIDA ---
            // Manhã -> Tarde -> Noite -> Manhã (Novo Dia)
            if (gs.horario == HORARIO_MANHA) {
                gs.horario = HORARIO_TARDE;
            } else if (gs.horario == HORARIO_TARDE) {
                gs.horario = HORARIO_NOITE;
            } else {
                gs.horario = HORARIO_MANHA;
                gs.dias++;
            // Renova a energia dos aliados
            for(int i=0; i<TOTAL_NPCS; i++) {
                gs.tarefasAliados[i] = 0;
            }
            gs.ordensDadasHoje = 0;

                //resets diários
                //sobral
                gs.visitouMercadoHoje = 0; 
                gs.visitouFarmaciaHoje = 0;
                gs.visitouPredioHoje = 0;
                //planalta
                gs.visitouHospitalHoje = 0;
                gs.visitouRodoviariaHoje = 0;
                gs.visitouPracaHoje = 0;
                gs.visitouEscolaHoje = 0;
                //SAMAMBA
                gs.visitouFeiraHoje = 0;
                gs.visitouPredioSamamba = 0;
                gs.visitouMetroHoje = 0;
                gs.visitouCassinoHoje = 0;
                //cei
                gs.visitouRadioHoje = 0;
                gs.visitouParedaoHoje = 0;
                gs.visitouGatosHoje = 0;
                gs.visitouIFHoje = 0;

                // REABASTECER COMERCIANTE
                gs.stkColt = 1; gs.stkFacao = 1; gs.stkMedkit = 1;
                gs.stkLanterna = 1; gs.stkTabuas = 1; gs.stkSinalizador = 1;
                gs.stkComida = 3; gs.stkSementes = 3; gs.stkMunicao = 3; gs.stkPilhas = 3;
                gs.stkGasolina = 3;
                
                // Separador de dia no log
                char bufferDia[50];
                sprintf(bufferDia, "=== DIA %d ===", gs.dias);
                logMsg(bufferDia);
                
                // Reset flags diárias
                gs.brincouHoje = 0; gs.treinouHoje = 0; gs.fezFogueiraHoje = 0; 
                gs.cacouHoje = 0; gs.fezRondaHoje = 0; gs.exercitouHoje = 0;
                gs.ouviuRadioTarde = 0; gs.ouviuRadioNoite = 0;
                gs.cuidouHortaHoje = 0;

                // Segurança cai a cada 3 dias
                if (gs.dias % 3 == 0) {
                    logMsg("A seguranca da base caiu com o tempo.");
                    modificarStatus(&gs, &gs.seguranca, -15, MAX_SEGURANCA, "Seguranca");
                    
                    if(gs.aliados > 0) {
                        logMsg("Seus aliados ajudaram na seguranca.");
                        modificarStatus(&gs, &gs.seguranca, gs.aliados * 10, 100, "Seguranca (Aliados)");
                    }
                }

                // --- SIMULAÇÃO DIÁRIA DA HORTA  ---
            for (int i = 0; i < 3; i++) {
                // Só processa se tiver planta viva crescendo
                if (gs.canteiros[i].estagio > ESTAGIO_VAZIO && gs.canteiros[i].estagio < ESTAGIO_MADURO) {
                    
                    // 1. Consumo de Água
                    int consumo = 30;
                    if (gs.canteiros[i].tipo == PLANTA_ALFACE) consumo = 20; // Bebe pouco
                    if (gs.canteiros[i].tipo == PLANTA_TOMATE) consumo = 40; // Bebe muito
                    
                    // BÔNUS LEO NÍVEL 3 (Rega automática de emergência)
                    if (gs.leoNivel >= 3 && gs.canteiros[i].hidratacao < 30) {
                        if (rand() % 100 < 50) { // 50% de chance se estiver seco
                            gs.canteiros[i].hidratacao += 40;
                            logMsg("Leo regou uma planta que estava secando.");
                        }
                    }

                    gs.canteiros[i].hidratacao -= consumo;
                    
                    // 2. Verifica Morte (Sede)
                    if (gs.canteiros[i].hidratacao <= 0) {
                        gs.canteiros[i].estagio = ESTAGIO_MORTO;
                        gs.canteiros[i].hidratacao = 0;
                        logMsg("Uma das suas plantas morreu de sede.");
                        
                        // Mantendo a lógica da Dica no Diário
                        if (!gs.dicaHortaMorte) {
                            adicionarNotaDiario("Sem agua diaria, as plantas morrem.");
                            gs.dicaHortaMorte = 1;
                        }
                    } 
                    else {
                        // 3. Chance de Crescimento
                        int chance = 40; // Base
                        if (gs.canteiros[i].tipo == PLANTA_ALFACE) chance = 60; // Rápido
                        if (gs.canteiros[i].tipo == PLANTA_CENOURA) chance = 25; // Lento
                        
                        if (gs.canteiros[i].adubado) chance += 40; // Adubo ajuda muito
                        
                        if (rand() % 100 < chance) {
                            gs.canteiros[i].estagio++; // Cresceu um estágio
                        }
                    }
                    
                    // O adubo é consumido na virada do dia
                    gs.canteiros[i].adubado = 0; 
                }
            }
            // --- SISTEMA DE GUARNIÇÃO AUTOMÁTICA ---
            // Calcula quantos aliados não trabalharam hoje
            int aliadosOciosos = gs.aliados - gs.ordensDadasHoje;
            
            if (aliadosOciosos > 0) {
                // Bônus: 2% de segurança por aliado livre
                int bonusSeguranca = aliadosOciosos * 2;
                
                // Aplica o bônus
                gs.seguranca += bonusSeguranca;
                if (gs.seguranca > 100) gs.seguranca = 100;
                
                // Feedback no Log (apenas se houve ganho real ou manutenção)
                if (gs.seguranca < 100 || bonusSeguranca > 0) {
                    char msgG[60];
                    sprintf(msgG, "%d aliados vigiaram a base (+%d Seguranca).", aliadosOciosos, bonusSeguranca);
                    logMsg(msgG);
                }
            }
            // ----------------------------------------------

            // Renova a energia dos aliados (Reset das flags)
            for(int i=0; i<TOTAL_NPCS; i++) {
                gs.tarefasAliados[i] = 0;
            }
            //habilidade 1 leo
            if (gs.leoNivel >= 1) {
                gs.seguranca += 5; // +5 Fixo por dia
                if (gs.seguranca > 100) gs.seguranca = 100;
                // Não precisa de log para não poluir, ou coloque um log discreto
            }
            // ----------------------------------------------------------------------
            }
            
            // Reset Flags de Turno a cada mudança de horário
            gs.explorouNoTurno = 0; gs.comeuNoTurno = 0; gs.alimentouNoTurno = 0;
            gs.bebeuCafeNoTurno = 0; gs.procurouSementesNoTurno = 0; gs.procurouAliadosNoTurno = 0;

            if (gs.vida <= 0) continue;

            if (rand() % 100 < 50) { 
                int novoItem;
                int temNoInv;
                int tentativas = 0;
                do {
                    novoItem = (rand() % 6) + 1;
                    temNoInv = 0;
                    for(int k=0; k<3; k++) if(gs.inventario[k] == novoItem) temNoInv = 1;
                    tentativas++;
                } while(temNoInv && tentativas < 20);

                if (!temNoInv) {
                    fecharMomento(&gs); // Se achar item, precisa pausar antes para ver o log do tempo passando
                    oferecerItem(&gs, novoItem);
                }
            }

            if (gs.filho.energia <= 0) { 
                sprintf(msgTemp, "%s desmaiou de exaustao!", gs.nomeFilho);
                logMsg(msgTemp);
                gs.filho.energia = 0; 
                sprintf(msgTemp, "Vida (%s)", gs.nomeFilho); modificarStatus(&gs, &gs.filho.vida, -2, MAX_STATUS_LEO, msgTemp);
            }
            
            // PAUSE FINAL DA PASSAGEM DE TEMPO
            // Garante que o jogador leia "O tempo passou..." e os logs das ações anteriores
            fecharMomento(&gs);
        }
        else if (passouTempo == 0 && escolha != 0) {
            // ...
        }
        turnoInicial = 0;
    }

    telaFimDeJogo(motivoFim, gs.dias, gs.nomeJogador, gs.nomeFilho);
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    configurarJanela();

    int opcaoMenu;
    char nomeJogador[50];
    int rodando = 1;

    while (rodando) {
        renderizarMenuPrincipal();
        opcaoMenu = lerInput();

        switch (opcaoMenu) {
            case 1:
                limparTela();
                printf("=== CRIACAO DE PERSONAGEM ===\n\n");
                printf("Digite o nome para o seu personagem: ");
                if(scanf(" %49[^\n]s", nomeJogador) == 1) {
                    limparBuffer(); 
                    jogar(nomeJogador);
                } else {
                    limparBuffer();
                }
                break;
            case 2:
                telaRegistros();
                break;
            case 3:
                printf("\nObrigado por jogar. Ate logo!\n");
                rodando = 0;
                break;
            default:
                break;
        }
    }
    return 0;
}
