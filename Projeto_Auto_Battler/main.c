#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Para a função sleep() no Linux/Mac. No Windows use <windows.h> e Sleep()

// --- Configurações e Structs ---

typedef enum { GUERREIRO, LADINO, TANQUE } Classe;

typedef struct {
    char nome[30];
    Classe classe;
    int hp_atual;
    int hp_max;
    int ataque;
    int defesa;
    int agilidade;   // Define chance de esquiva
    int critico;     // Chance de dano duplo (%)
} Gladiador;

// Protótipos das funções
int rolarDado(int min, int max);
Gladiador criarGladiador(char* nome, Classe classe);
void desenharBarraVida(Gladiador g);
void salvarVencedor(Gladiador g);
void logBatalha(char* mensagem);
void executarTurno(Gladiador *atacante, Gladiador *defensor);

// --- Função Principal ---

int main() {
    // Semente para números aleatórios baseada no tempo
    srand(time(NULL));

    printf("========================================\n");
    printf("       ⚔️  ARENA CODE: AUTO-BATTLER ⚔️\n");
    printf("========================================\n\n");

    // Criação dos Lutadores (Hardcoded para teste rápido, mas poderia ser via input)
    Gladiador p1 = criarGladiador("Kratos", GUERREIRO);
    Gladiador p2 = criarGladiador("Altair", LADINO);

    printf("COMBATE INICIADO: %s (HP:%d) vs %s (HP:%d)\n\n", 
            p1.nome, p1.hp_max, p2.nome, p2.hp_max);
    
    // Loop da Batalha
    int turno = 1;
    while (p1.hp_atual > 0 && p2.hp_atual > 0) {
        printf("--- TURNO %d ---\n", turno);
        
        // Quem é mais rápido ataca primeiro? (Mecânica simples de iniciativa)
        // Aqui alternaremos para simplificar, mas a agilidade poderia decidir.
        
        // P1 Ataca P2
        executarTurno(&p1, &p2);
        if (p2.hp_atual <= 0) break; // Se P2 morreu, para o loop

        // P2 Ataca P1
        executarTurno(&p2, &p1);
        
        // Feedback Visual
        desenharBarraVida(p1);
        desenharBarraVida(p2);
        printf("\n");

        turno++;
        sleep(1); // Pausa de 1 segundo para dar emoção (No Windows: Sleep(1000))
    }

    // Fim de Jogo
    printf("========================================\n");
    if (p1.hp_atual > 0) {
        printf("🏆 O VENCEDOR FOI: %s!\n", p1.nome);
        salvarVencedor(p1);
    } else {
        printf("🏆 O VENCEDOR FOI: %s!\n", p2.nome);
        salvarVencedor(p2);
    }
    printf("========================================\n");

    return 0;
}

// --- Implementação das Funções ---

// Gera um número aleatório entre min e max
int rolarDado(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Factory para criar personagens balanceados
Gladiador criarGladiador(char* nome, Classe classe) {
    Gladiador g;
    strcpy(g.nome, nome);
    g.classe = classe;

    switch (classe) {
        case GUERREIRO: // Equilibrado
            g.hp_max = 100; g.ataque = 15; g.defesa = 5; g.agilidade = 10; g.critico = 10;
            break;
        case LADINO: // Papel (pouca vida, muito dano e esquiva)
            g.hp_max = 70; g.ataque = 20; g.defesa = 2; g.agilidade = 30; g.critico = 25;
            break;
        case TANQUE: // Esponja de dano
            g.hp_max = 150; g.ataque = 10; g.defesa = 10; g.agilidade = 0; g.critico = 5;
            break;
    }
    g.hp_atual = g.hp_max;
    return g;
}

// Lógica de um ataque único
void executarTurno(Gladiador *atacante, Gladiador *defensor) {
    char buffer[100];

    // 1. Tentar Esquivar
    // Se rolar um dado 0-100 e for menor que a agilidade, esquiva.
    if (rolarDado(0, 100) < defensor->agilidade) {
        sprintf(buffer, "💨 %s tentou atacar, mas %s ESQUIVOU!", atacante->nome, defensor->nome);
        logBatalha(buffer);
        return;
    }

    // 2. Calcular Dano Base (Variação de +/- 20% do ataque)
    int variacao = rolarDado(-2, 2); 
    int dano = atacante->ataque + variacao;

    // 3. Verificar Crítico
    if (rolarDado(0, 100) < atacante->critico) {
        dano *= 2;
        printf("🔥 ACERTO CRÍTICO! ");
    }

    // 4. Aplicar Redução de Defesa (Defesa bloqueia valor fixo)
    int danoFinal = dano - defensor->defesa;
    if (danoFinal < 0) danoFinal = 0; // Não cura se defesa for alta

    // 5. Aplicar Dano na Vida
    defensor->hp_atual -= danoFinal;
    if (defensor->hp_atual < 0) defensor->hp_atual = 0;

    sprintf(buffer, "⚔️  %s atacou %s causando %d de dano.", atacante->nome, defensor->nome, danoFinal);
    logBatalha(buffer);
}

// Mostra uma barra de vida visual: [||||||....]
void desenharBarraVida(Gladiador g) {
    printf("%-10s [", g.nome);
    
    // Calcula quantos tracinhos desenhar (base 10)
    int barras = (g.hp_atual * 10) / g.hp_max;
    
    for (int i = 0; i < 10; i++) {
        if (i < barras) printf("|");
        else printf(".");
    }
    printf("] %d/%d HP\n", g.hp_atual, g.hp_max);
}

// Salva o resultado em CSV
void salvarVencedor(Gladiador g) {
    FILE *f = fopen("hall_da_fama.csv", "a"); // "a" para append (adicionar ao final)
    
    if (f == NULL) {
        printf("Erro ao salvar arquivo de ranking.\n");
        return;
    }

    // Pega data/hora atual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    // Formato: Data, Nome, HP_Restante
    fprintf(f, "%02d/%02d/%d, %s, %d\n", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 
            g.nome, g.hp_atual);
            
    fclose(f);
    printf("💾 Resultado salvo no Hall da Fama!\n");
}

void logBatalha(char* mensagem) {
    printf("%s\n", mensagem);
}
