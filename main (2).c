/*
 * ╔══════════════════════════════════════════════════════════════╗
 *  SISTEMA DE GERENCIAMENTO DE CONTATOS  —  v3.0 FINAL
 *  Disciplina : Algoritmos e Linguagem C
 *  Linguagem  : C  (compativel com Dev-C++ / MinGW / GCC)
 * ╚══════════════════════════════════════════════════════════════╝
 *
 *  Funcionalidades:
 *   1. Cadastrar contato  (com validacao de e-mail e telefone)
 *   2. Alterar contato
 *   3. Apagar contato     (com confirmacao)
 *   4. Procurar contato   (busca parcial por nome)
 *   5. Listar contatos    (ordem alfabetica + paginacao)
 *   6. Estatisticas       (totais por tipo, barra de capacidade)
 *   7. Sair
 *
 *  Persistencia:
 *   - contatos.dat   : dados binarios dos contatos
 *   - contatos_log.txt: registro cronologico de todas as operacoes
 *
 *  Requisitos atendidos:
 *   [X] Struct / vetor de ate 100 contatos
 *   [X] Funcoes para cada funcionalidade
 *   [X] Manipulacao de arquivos (leitura e gravacao)
 *   [X] Estruturas de repeticao
 *   [X] Estruturas de decisao
 *   [X] Modularizacao adequada
 *   [X] Tratamento de erros e mensagens claras
 *   [X] Codigo indentado e comentado
 */

/* ── includes ─────────────────────────────────────────────────── */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>       /* data/hora no rodape e no log           */
#include <ctype.h>      /* isdigit(), tolower()                   */

/* ── cores ANSI ───────────────────────────────────────────────── */
/* Funcionam no Windows 10+ (cmd/PowerShell), Linux e macOS.      */
#define RST   "\033[0m"
#define BOLD  "\033[1m"
#define DIM   "\033[2m"

#define CRED  "\033[31m"    /* vermelho normal  */
#define CGRN  "\033[32m"    /* verde normal     */
#define CYLW  "\033[33m"    /* amarelo normal   */
#define CBLU  "\033[34m"    /* azul normal      */
#define CMAG  "\033[35m"    /* magenta          */
#define CCYN  "\033[36m"    /* ciano normal     */

#define BRED  "\033[91m"    /* vermelho brilhante */
#define BGRN  "\033[92m"    /* verde brilhante    */
#define BYLW  "\033[93m"    /* amarelo brilhante  */
#define BBLU  "\033[94m"    /* azul brilhante     */
#define BMAG  "\033[95m"    /* magenta brilhante  */
#define BCYN  "\033[96m"    /* ciano brilhante    */
#define BWHT  "\033[97m"    /* branco brilhante   */

/* atalhos semanticos */
#define C_OK    BGRN BOLD   /* sucesso            */
#define C_ERR   BRED BOLD   /* erro               */
#define C_WARN  BYLW        /* aviso              */
#define C_HEAD  BCYN BOLD   /* cabecalhos         */
#define C_LBL   BBLU        /* rotulos de campo   */
#define C_VAL   BWHT        /* valores            */
#define C_ITEM  BYLW BOLD   /* numeros de menu    */
#define C_LINE  DIM CBLU    /* linhas / bordas    */
#define C_IN    BGRN        /* prompt de entrada  */
#define C_DIM   DIM         /* texto secundario   */

/* ── constantes ───────────────────────────────────────────────── */
#define MAX_CONTATOS   100
#define MAX_NOME        80
#define MAX_EMAIL       80
#define MAX_TELEFONE    20
#define MAX_TIPO        20
#define PAG_TAMANHO      5          /* contatos por pagina na listagem */
#define ARQ_DADOS    "contatos.dat"
#define ARQ_LOG      "contatos_log.txt"

/* ── estrutura ────────────────────────────────────────────────── */
typedef struct {
    char nome[MAX_NOME];
    char email[MAX_EMAIL];
    char telefone[MAX_TELEFONE];
    char tipo[MAX_TIPO];   /* Pessoal | Trabalho | Familia | Cliente | Outro */
} Contato;

/* ── variaveis globais ────────────────────────────────────────── */
Contato agenda[MAX_CONTATOS];   /* vetor principal                 */
int totalContatos = 0;          /* quantidade atual no vetor       */

/* ── prototipos ───────────────────────────────────────────────── */
/* persistencia */
void carregarArquivo(void);
void salvarArquivo(void);
void gravarLog(const char *acao, const char *detalhe);

/* tela / layout */
void splash(void);
void exibirMenu(void);
void cabecalho(const char *titulo);
void rodape(void);
void barraCapacidade(void);
void pausar(void);
void limparBuffer(void);

/* auxiliares */
int  buscarPorNome(const char *busca);
void corTipo(const char *tipo);
int  validarEmail(const char *email);
int  validarTelefone(const char *tel);
int  comparaNome(const void *a, const void *b);
void ordenarContatos(void);
void obterDataHora(char *buf, int tam);
int  selecionarTipo(void);
void exibirContato(int i);
void estatisticas(void);

/* funcionalidades do menu */
void cadastrarContato(void);
void alterarContato(void);
void apagarContato(void);
void procurarContato(void);
void listarContatos(void);

/* ══════════════════════════════════════════════════════════════
 *  FUNCAO PRINCIPAL
 * ══════════════════════════════════════════════════════════════ */
int main(void) {
    int opcao;

    /* Habilita UTF-8 e cores ANSI no Windows */
    #ifdef _WIN32
        system("chcp 65001 > nul");
        system("cls");
    #endif

    splash();           /* tela de abertura                        */
    carregarArquivo();  /* carrega dados persistidos               */

    do {
        exibirMenu();
        printf(C_IN "\n  > Opcao: " RST);

        /* leitura robusta: ignora entrada nao-numerica */
        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
        }
        limparBuffer();

        switch (opcao) {
            case 1: cadastrarContato(); break;
            case 2: alterarContato();   break;
            case 3: apagarContato();    break;
            case 4: procurarContato();  break;
            case 5: listarContatos();   break;
            case 6: estatisticas();     break;
            case 7:
                salvarArquivo();
                gravarLog("SISTEMA", "Programa encerrado pelo usuario");
                system("cls || clear");
                printf(C_HEAD
"\n  +================================================+\n"
"  |         ATE LOGO! DADOS SALVOS COM SUCESSO.    |\n"
"  +================================================+\n\n" RST);
                break;
            default:
                printf(C_ERR "\n  [ERRO] Opcao invalida. Digite um numero de 1 a 7.\n" RST);
                pausar();
        }
    } while (opcao != 7);

    return 0;
}

/* ══════════════════════════════════════════════════════════════
 *  SPLASH — tela de abertura
 * ══════════════════════════════════════════════════════════════ */
void splash(void) {
    system("cls || clear");
    printf("\n\n");
    printf(BCYN BOLD
"        ╔══════════════════════════════════════════╗\n"
"        ║                                          ║\n"
"        ║     AGENDA DE CONTATOS  v3.0             ║\n"
"        ║     Algoritmos e Linguagem C             ║\n"
"        ║                                          ║\n"
"        ╠══════════════════════════════════════════╣\n"
"        ║  Capacidade : 100 contatos               ║\n"
"        ║  Arquivo    : contatos.dat               ║\n"
"        ║  Log        : contatos_log.txt           ║\n"
"        ╚══════════════════════════════════════════╝\n\n"
    RST);
    printf(C_DIM "        Carregando dados..." RST);
    /* pequena pausa visual (portavel) */
    {
        volatile long i;
        for (i = 0; i < 80000000L; i++);
    }
    printf(C_OK " OK\n\n" RST);
    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  OBTÉM DATA E HORA ATUAL COMO STRING
 * ══════════════════════════════════════════════════════════════ */
void obterDataHora(char *buf, int tam) {
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);
    /* formato: DD/MM/AAAA HH:MM:SS */
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", t);
}

/* ══════════════════════════════════════════════════════════════
 *  GRAVAR ENTRADA NO LOG (arquivo texto)
 * ══════════════════════════════════════════════════════════════ */
void gravarLog(const char *acao, const char *detalhe) {
    FILE *fp = fopen(ARQ_LOG, "a");   /* "a" = append — nunca apaga o historico */
    if (fp == NULL) return;

    char dt[30];
    obterDataHora(dt, sizeof(dt));
    fprintf(fp, "[%s] %-15s | %s\n", dt, acao, detalhe);
    fclose(fp);
}

/* ══════════════════════════════════════════════════════════════
 *  BARRA DE CAPACIDADE
 * ══════════════════════════════════════════════════════════════ */
void barraCapacidade(void) {
    int i;
    int cheio   = (totalContatos * 30) / MAX_CONTATOS;
    int vazio   = 30 - cheio;
    int pct     = (totalContatos * 100) / MAX_CONTATOS;

    /* cor da barra muda conforme ocupacao */
    const char *cor = (pct < 50) ? BGRN : (pct < 80) ? BYLW : BRED;

    printf(C_DIM "  [" RST);
    printf("%s", cor);
    for (i = 0; i < cheio;  i++) printf("|");
    printf(C_DIM);
    for (i = 0; i < vazio; i++) printf(".");
    printf(RST C_DIM "]" RST);
    printf(" %s%d/%d" RST C_DIM " (%d%%) contatos\n" RST,
           cor, totalContatos, MAX_CONTATOS, pct);
}

/* ══════════════════════════════════════════════════════════════
 *  RODAPE COM DATA/HORA
 * ══════════════════════════════════════════════════════════════ */
void rodape(void) {
    char dt[30];
    obterDataHora(dt, sizeof(dt));
    printf(C_LINE
"  +--------------------------------------------------+\n"
    RST);
    printf(C_DIM "   %s\n" RST, dt);
}

/* ══════════════════════════════════════════════════════════════
 *  MENU PRINCIPAL
 * ══════════════════════════════════════════════════════════════ */
void exibirMenu(void) {
    system("cls || clear");

    printf(C_HEAD
"\n"
"  ╔══════════════════════════════════════════════════╗\n"
"  ║          SISTEMA DE GERENCIAMENTO DE             ║\n"
"  ║               CONTATOS  v3.0                     ║\n"
"  ╠══════════════════════════════════════════════════╣\n"
    RST);

    /* ── barra de capacidade dentro das bordas do quadro ──────────
     * Largura interna do quadro: 50 chars (entre os dois      ║)
     * Layout: "  Cap: [████░░░░░░░░░░░░░░░]  NN/100 (NNN%)  "
     * A barra tem tamanho fixo de 20 chars; o resto e texto fixo.
     * Total visivel: 2 + 5 + 1 + 20 + 1 + 2 + 7 + 8 + 4 = 50
     * ─────────────────────────────────────────────────────────── */
    {
        int cheio  = (totalContatos * 20) / MAX_CONTATOS;
        int vazio  = 20 - cheio;
        int pct    = (totalContatos * 100) / MAX_CONTATOS;
        int k;
        /* sufixo: "  NN/100 (NNN%)  " — sempre 16 chars visiveis  */
        char sufixo[32];
        sprintf(sufixo, "  %3d/100 (%3d%%)", totalContatos, pct);

        const char *cor = (pct < 50) ? BGRN : (pct < 80) ? BYLW : BRED;

        printf(C_HEAD "   ║" RST);             /*  ║                    */
        printf("  Cap: ");                   /*  2 + 5 + 1 = 8 chars  */
        printf(C_DIM "[" RST);
        printf("%s", cor);
        for (k = 0; k < cheio; k++) printf("|");
        printf(C_DIM);
        for (k = 0; k < vazio; k++) printf(".");
        printf(RST C_DIM "]" RST);
        printf("%s%s   " RST, cor, sufixo); /* sufixo + 3 espacos = 50 chars internos */
        printf(C_HEAD "║\n" RST);          /* fecha a borda direita */
    }
    printf(C_HEAD "  ╠══════════════════════════════════════════════════╣\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [1]" RST " Cadastrar contato" C_HEAD "                           ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [2]" RST " Alterar contato" C_HEAD "                             ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [3]" RST " Apagar contato" C_HEAD "                              ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [4]" RST " Procurar contato" C_HEAD "                            ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [5]" RST " Listar contatos (ord. alfabetica)" C_HEAD "           ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [6]" RST " Estatisticas por tipo" C_HEAD "                       ║\n" RST);
    printf(C_HEAD "  ║" RST C_ITEM "  [7]" RST " Sair" C_HEAD "                                        ║\n" RST);
    printf(C_HEAD "  ╚══════════════════════════════════════════════════╝\n" RST);
    rodape();
}

/* ══════════════════════════════════════════════════════════════
 *  CABECALHO DE SECAO
 * ══════════════════════════════════════════════════════════════ */
void cabecalho(const char *titulo) {
    system("cls || clear");
    printf(C_HEAD
"\n  ╔══════════════════════════════════════════════════╗\n"
    RST);
    printf(C_HEAD "  ║  %-49s║\n" RST, titulo);
    printf(C_HEAD
"  ╚══════════════════════════════════════════════════╝\n\n"
    RST);
}

/* ══════════════════════════════════════════════════════════════
 *  PAUSA — aguarda ENTER
 * ══════════════════════════════════════════════════════════════ */
void pausar(void) {
    printf(C_DIM "\n  Pressione ENTER para continuar..." RST);
    getchar();
}

/* ══════════════════════════════════════════════════════════════
 *  LIMPA CARACTERES RESIDUAIS DO BUFFER
 * ══════════════════════════════════════════════════════════════ */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ══════════════════════════════════════════════════════════════
 *  IMPRIME A COR DO TIPO DO CONTATO
 * ══════════════════════════════════════════════════════════════ */
void corTipo(const char *tipo) {
    if      (strcmp(tipo, "Pessoal")  == 0) printf(BGRN  BOLD "%s" RST, tipo);
    else if (strcmp(tipo, "Trabalho") == 0) printf(BBLU  BOLD "%s" RST, tipo);
    else if (strcmp(tipo, "Familia")  == 0) printf(BMAG  BOLD "%s" RST, tipo);
    else if (strcmp(tipo, "Cliente")  == 0) printf(BYLW  BOLD "%s" RST, tipo);
    else                                    printf(C_DIM       "%s" RST, tipo);
}

/* ══════════════════════════════════════════════════════════════
 *  EXIBE CARD DE UM CONTATO
 * ══════════════════════════════════════════════════════════════ */
void exibirContato(int i) {
    printf(C_LINE "  ┌──────────────────────────────────────────────┐\n" RST);
    printf(C_LINE "  │ " RST C_ITEM "#%-3d " RST C_VAL BOLD "%-42s" RST C_LINE " │\n" RST,
           i + 1, agenda[i].nome);
    printf(C_LINE "  ├──────────────────────────────────────────────┤\n" RST);
    printf(C_LINE "  │ " RST C_LBL "Email    : " RST "%-36s" C_LINE " │\n" RST,
           agenda[i].email);
    printf(C_LINE "  │ " RST C_LBL "Telefone : " RST "%-36s" C_LINE " │\n" RST,
           agenda[i].telefone);
    printf(C_LINE "  │ " RST C_LBL "Tipo     : " RST);
    corTipo(agenda[i].tipo);
    printf("\n");
    printf(C_LINE "  └──────────────────────────────────────────────┘\n" RST);
}

/* ══════════════════════════════════════════════════════════════
 *  VALIDACAO DE E-MAIL
 *  Regra basica: deve conter '@' e '.' apos o '@'
 * ══════════════════════════════════════════════════════════════ */
int validarEmail(const char *email) {
    const char *arroba = strchr(email, '@');
    if (arroba == NULL)         return 0;   /* sem '@'            */
    if (arroba == email)        return 0;   /* '@' no inicio      */
    if (strchr(arroba, '.') == NULL) return 0; /* sem '.' apos '@' */
    return 1;
}

/* ══════════════════════════════════════════════════════════════
 *  VALIDACAO DE TELEFONE
 *  Aceita: digitos, espacos, '(', ')', '-', '+'
 * ══════════════════════════════════════════════════════════════ */
int validarTelefone(const char *tel) {
    int i, digitos = 0;
    if (strlen(tel) == 0) return 0;
    for (i = 0; tel[i] != '\0'; i++) {
        if (isdigit((unsigned char)tel[i])) {
            digitos++;
        } else if (tel[i] != ' ' && tel[i] != '(' &&
                   tel[i] != ')' && tel[i] != '-' && tel[i] != '+') {
            return 0;   /* caractere invalido */
        }
    }
    return (digitos >= 7);  /* minimo de 7 digitos */
}

/* ══════════════════════════════════════════════════════════════
 *  FUNCAO DE COMPARACAO PARA qsort (ordena por nome)
 * ══════════════════════════════════════════════════════════════ */
int comparaNome(const void *a, const void *b) {
    const Contato *ca = (const Contato *)a;
    const Contato *cb = (const Contato *)b;
    return strcmp(ca->nome, cb->nome);
}

/* ══════════════════════════════════════════════════════════════
 *  ORDENA O VETOR ALFABETICAMENTE USANDO qsort
 * ══════════════════════════════════════════════════════════════ */
void ordenarContatos(void) {
    qsort(agenda, totalContatos, sizeof(Contato), comparaNome);
}

/* ══════════════════════════════════════════════════════════════
 *  BUSCA PARCIAL POR NOME (retorna primeiro indice ou -1)
 * ══════════════════════════════════════════════════════════════ */
int buscarPorNome(const char *busca) {
    int i;
    for (i = 0; i < totalContatos; i++) {
        if (strstr(agenda[i].nome, busca) != NULL)
            return i;
    }
    return -1;
}

/* ══════════════════════════════════════════════════════════════
 *  SUBMENU DE SELECAO DE TIPO
 * ══════════════════════════════════════════════════════════════ */
int selecionarTipo(void) {
    int t;
    printf(C_LBL "\n  Tipo do contato:\n" RST);
    printf(BGRN  "   [1]" RST " Pessoal\n");
    printf(BBLU  "   [2]" RST " Trabalho\n");
    printf(BMAG  "   [3]" RST " Familia\n");
    printf(BYLW  "   [4]" RST " Cliente\n");
    printf(C_DIM "   [5]" RST " Outro\n");
    printf(C_IN  "   > Escolha: " RST);
    if (scanf("%d", &t) != 1) t = 5;
    limparBuffer();
    return t;
}

/* ══════════════════════════════════════════════════════════════
 *  1. CADASTRAR CONTATO
 * ══════════════════════════════════════════════════════════════ */
void cadastrarContato(void) {
    cabecalho("[ 1 ]  CADASTRAR CONTATO");

    /* verifica capacidade */
    if (totalContatos >= MAX_CONTATOS) {
        printf(C_ERR "  [ERRO] Capacidade maxima (%d) atingida.\n" RST, MAX_CONTATOS);
        gravarLog("CADASTRO", "Falha: capacidade maxima atingida");
        pausar();
        return;
    }

    Contato novo;
    memset(&novo, 0, sizeof(Contato));   /* zera a struct              */

    /* ── Nome ── */
    printf(C_LBL "  Nome     : " RST);
    fgets(novo.nome, MAX_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';
    if (strlen(novo.nome) == 0) {
        printf(C_ERR "\n  [ERRO] O nome nao pode ser vazio.\n" RST);
        pausar();
        return;
    }

    /* ── E-mail (com validacao) ── */
    do {
        printf(C_LBL "  E-mail   : " RST);
        fgets(novo.email, MAX_EMAIL, stdin);
        novo.email[strcspn(novo.email, "\n")] = '\0';
        if (!validarEmail(novo.email)) {
            printf(C_WARN "  [AVISO] E-mail invalido. Deve conter '@' e '.' (ex: nome@dominio.com)\n" RST);
        }
    } while (!validarEmail(novo.email));

    /* ── Telefone (com validacao) ── */
    do {
        printf(C_LBL "  Telefone : " RST);
        fgets(novo.telefone, MAX_TELEFONE, stdin);
        novo.telefone[strcspn(novo.telefone, "\n")] = '\0';
        if (!validarTelefone(novo.telefone)) {
            printf(C_WARN "  [AVISO] Telefone invalido. Use apenas digitos, espacos, '()', '-', '+'. Minimo 7 digitos.\n" RST);
        }
    } while (!validarTelefone(novo.telefone));

    /* ── Tipo ── */
    int t = selecionarTipo();
    switch (t) {
        case 1: strcpy(novo.tipo, "Pessoal");  break;
        case 2: strcpy(novo.tipo, "Trabalho"); break;
        case 3: strcpy(novo.tipo, "Familia");  break;
        case 4: strcpy(novo.tipo, "Cliente");  break;
        default: strcpy(novo.tipo, "Outro");   break;
    }

    /* ── Insere no vetor, ordena e salva ── */
    agenda[totalContatos++] = novo;
    ordenarContatos();          /* mantém a lista sempre ordenada   */
    salvarArquivo();            /* autosalva apos cada operacao     */

    /* log da operacao */
    {
        char detalhe[120];
        sprintf(detalhe, "Nome: %s | Tipo: %s", novo.nome, novo.tipo);
        gravarLog("CADASTRO", detalhe);
    }

    printf(C_OK "\n  [OK] Contato '" RST C_VAL "%s" RST C_OK "' cadastrado e salvo!\n" RST,
           novo.nome);
    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  2. ALTERAR CONTATO
 * ══════════════════════════════════════════════════════════════ */
void alterarContato(void) {
    cabecalho("[ 2 ]  ALTERAR CONTATO");

    if (totalContatos == 0) {
        printf(C_WARN "  [AVISO] Nenhum contato cadastrado.\n" RST);
        pausar();
        return;
    }

    char busca[MAX_NOME];
    printf(C_IN "  > Nome do contato a alterar: " RST);
    fgets(busca, MAX_NOME, stdin);
    busca[strcspn(busca, "\n")] = '\0';

    int idx = buscarPorNome(busca);
    if (idx == -1) {
        printf(C_ERR "\n  [ERRO] Nenhum contato encontrado com '%s'.\n" RST, busca);
        pausar();
        return;
    }

    printf(C_HEAD "\n  Contato localizado:\n" RST);
    exibirContato(idx);
    printf(C_DIM "\n  (Deixe em branco + ENTER para manter o valor atual)\n\n" RST);

    char temp[MAX_NOME];

    /* ── Nome ── */
    printf(C_LBL "  Novo nome     " C_DIM "[%s]" RST C_LBL ": " RST,
           agenda[idx].nome);
    fgets(temp, MAX_NOME, stdin);
    temp[strcspn(temp, "\n")] = '\0';
    if (strlen(temp) > 0) strcpy(agenda[idx].nome, temp);

    /* ── E-mail com validacao ── */
    printf(C_LBL "  Novo e-mail   " C_DIM "[%s]" RST C_LBL ": " RST,
           agenda[idx].email);
    fgets(temp, MAX_EMAIL, stdin);
    temp[strcspn(temp, "\n")] = '\0';
    if (strlen(temp) > 0) {
        while (!validarEmail(temp)) {
            printf(C_WARN "  [AVISO] E-mail invalido. Tente novamente: " RST);
            fgets(temp, MAX_EMAIL, stdin);
            temp[strcspn(temp, "\n")] = '\0';
            if (strlen(temp) == 0) break;   /* ENTER vazio = manter */
        }
        if (strlen(temp) > 0) strcpy(agenda[idx].email, temp);
    }

    /* ── Telefone com validacao ── */
    printf(C_LBL "  Novo telefone " C_DIM "[%s]" RST C_LBL ": " RST,
           agenda[idx].telefone);
    fgets(temp, MAX_TELEFONE, stdin);
    temp[strcspn(temp, "\n")] = '\0';
    if (strlen(temp) > 0) {
        while (!validarTelefone(temp)) {
            printf(C_WARN "  [AVISO] Telefone invalido. Tente novamente: " RST);
            fgets(temp, MAX_TELEFONE, stdin);
            temp[strcspn(temp, "\n")] = '\0';
            if (strlen(temp) == 0) break;
        }
        if (strlen(temp) > 0) strcpy(agenda[idx].telefone, temp);
    }

    /* ── Tipo ── */
    printf(C_DIM "\n  Tipo atual: " RST);
    corTipo(agenda[idx].tipo);
    printf(C_DIM "  |  Digite 0 para manter\n" RST);
    {
        int t = selecionarTipo();
        switch (t) {
            case 1: strcpy(agenda[idx].tipo, "Pessoal");  break;
            case 2: strcpy(agenda[idx].tipo, "Trabalho"); break;
            case 3: strcpy(agenda[idx].tipo, "Familia");  break;
            case 4: strcpy(agenda[idx].tipo, "Cliente");  break;
            case 5: strcpy(agenda[idx].tipo, "Outro");    break;
            default: break;
        }
    }

    ordenarContatos();
    salvarArquivo();

    {
        char detalhe[120];
        sprintf(detalhe, "Nome: %s", agenda[idx].nome);
        gravarLog("ALTERACAO", detalhe);
    }

    printf(C_OK "\n  [OK] Contato atualizado e salvo!\n" RST);
    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  3. APAGAR CONTATO
 * ══════════════════════════════════════════════════════════════ */
void apagarContato(void) {
    cabecalho("[ 3 ]  APAGAR CONTATO");

    if (totalContatos == 0) {
        printf(C_WARN "  [AVISO] Nenhum contato cadastrado.\n" RST);
        pausar();
        return;
    }

    char busca[MAX_NOME];
    printf(C_IN "  > Nome do contato a apagar: " RST);
    fgets(busca, MAX_NOME, stdin);
    busca[strcspn(busca, "\n")] = '\0';

    int idx = buscarPorNome(busca);
    if (idx == -1) {
        printf(C_ERR "\n  [ERRO] Nenhum contato encontrado com '%s'.\n" RST, busca);
        pausar();
        return;
    }

    printf(C_HEAD "\n  Contato localizado:\n" RST);
    exibirContato(idx);

    printf(C_WARN "\n  Confirma a exclusao?" RST
           BGRN " [S]" RST "/" BRED "[N]" RST ": ");
    char conf;
    scanf("%c", &conf);
    limparBuffer();

    if (conf == 'S' || conf == 's') {
        char nomeSalvo[MAX_NOME];
        int i;

        strcpy(nomeSalvo, agenda[idx].nome);

        /* desloca vetor para fechar o espaco */
        for (i = idx; i < totalContatos - 1; i++)
            agenda[i] = agenda[i + 1];
        totalContatos--;

        salvarArquivo();

        {
            char detalhe[120];
            sprintf(detalhe, "Nome: %s", nomeSalvo);
            gravarLog("EXCLUSAO", detalhe);
        }

        printf(C_OK "\n  [OK] Contato removido e arquivo atualizado!\n" RST);
    } else {
        printf(C_DIM "\n  Operacao cancelada.\n" RST);
    }

    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  4. PROCURAR CONTATO
 * ══════════════════════════════════════════════════════════════ */
void procurarContato(void) {
    cabecalho("[ 4 ]  PROCURAR CONTATO");

    if (totalContatos == 0) {
        printf(C_WARN "  [AVISO] Nenhum contato cadastrado.\n" RST);
        pausar();
        return;
    }

    char busca[MAX_NOME];
    printf(C_IN "  > Parte do nome para buscar: " RST);
    fgets(busca, MAX_NOME, stdin);
    busca[strcspn(busca, "\n")] = '\0';

    int encontrados = 0;
    int i;
    for (i = 0; i < totalContatos; i++) {
        if (strstr(agenda[i].nome, busca) != NULL) {
            exibirContato(i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf(C_WARN "\n  [AVISO] Nenhum resultado para '" RST C_VAL "%s" RST
               C_WARN "'.\n" RST, busca);
    } else {
        printf(C_OK "\n  Total encontrado: %d contato(s).\n" RST, encontrados);
        gravarLog("BUSCA", busca);
    }

    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  5. LISTAR CONTATOS — ordem alfabetica + paginacao
 * ══════════════════════════════════════════════════════════════ */
void listarContatos(void) {
    cabecalho("[ 5 ]  LISTA DE CONTATOS  (ordem alfabetica)");

    if (totalContatos == 0) {
        printf(C_WARN "  [AVISO] Nenhum contato cadastrado.\n" RST);
        pausar();
        return;
    }

    int pagina   = 0;
    int total_pag = (totalContatos + PAG_TAMANHO - 1) / PAG_TAMANHO;
    char nav;

    do {
        int inicio = pagina * PAG_TAMANHO;
        int fim    = inicio + PAG_TAMANHO;
        int i;
        if (fim > totalContatos) fim = totalContatos;

        /* limpa apenas a area de conteudo, mantem cabecalho */
        system("cls || clear");
        cabecalho("[ 5 ]  LISTA DE CONTATOS  (ordem alfabetica)");

        printf(C_DIM "  Pagina %d de %d  |  Total: %d contato(s)\n\n" RST,
               pagina + 1, total_pag, totalContatos);

        for (i = inicio; i < fim; i++)
            exibirContato(i);

        /* navegacao de paginas */
        printf(C_LINE "\n  " RST);
        if (pagina > 0)              printf(BBLU "[A] Anterior  " RST);
        if (pagina < total_pag - 1)  printf(BGRN "[P] Proxima  " RST);
        printf(BYLW "[Q] Sair da listagem" RST "   ");
        printf(C_IN "> " RST);

        scanf("%c", &nav);
        limparBuffer();
        nav = (char)tolower((unsigned char)nav);

        if      (nav == 'p' && pagina < total_pag - 1) pagina++;
        else if (nav == 'a' && pagina > 0)             pagina--;

    } while (nav != 'q');
}

/* ══════════════════════════════════════════════════════════════
 *  6. ESTATISTICAS POR TIPO
 * ══════════════════════════════════════════════════════════════ */
void estatisticas(void) {
    cabecalho("[ 6 ]  ESTATISTICAS");

    int pessoal = 0, trabalho = 0, familia = 0, cliente = 0, outro = 0;
    int i;

    for (i = 0; i < totalContatos; i++) {
        if      (strcmp(agenda[i].tipo, "Pessoal")  == 0) pessoal++;
        else if (strcmp(agenda[i].tipo, "Trabalho") == 0) trabalho++;
        else if (strcmp(agenda[i].tipo, "Familia")  == 0) familia++;
        else if (strcmp(agenda[i].tipo, "Cliente")  == 0) cliente++;
        else                                               outro++;
    }

    printf(C_LBL "  Capacidade do sistema:\n" RST);
    printf("  ");
    barraCapacidade();

    printf(C_LINE "\n  ┌───────────────────────────────────┐\n" RST);
    printf(C_LINE "  │" RST "  %-10s  " BGRN  BOLD "%3d" RST "  contatos  " C_LINE "       │\n" RST, "Pessoal",  pessoal);
    printf(C_LINE "  │" RST "  %-10s  " BBLU  BOLD "%3d" RST "  contatos  " C_LINE "       │\n" RST, "Trabalho", trabalho);
    printf(C_LINE "  │" RST "  %-10s  " BMAG  BOLD "%3d" RST "  contatos  " C_LINE "       │\n" RST, "Familia",  familia);
    printf(C_LINE "  │" RST "  %-10s  " BYLW  BOLD "%3d" RST "  contatos  " C_LINE "       │\n" RST, "Cliente",  cliente);
    printf(C_LINE "  │" RST "  %-10s  " C_DIM      "%3d" RST "  contatos  " C_LINE "       │\n" RST, "Outro",    outro);
    printf(C_LINE "  ├───────────────────────────────────┤\n" RST);
    printf(C_LINE "  │" RST C_VAL BOLD "  TOTAL       %3d  contatos  " RST C_LINE "       │\n" RST, totalContatos);
    printf(C_LINE "  └───────────────────────────────────┘\n\n" RST);

    gravarLog("ESTATISTICAS", "Consulta de estatisticas realizada");
    pausar();
}

/* ══════════════════════════════════════════════════════════════
 *  CARREGAR ARQUIVO BINARIO
 * ══════════════════════════════════════════════════════════════ */
void carregarArquivo(void) {
    FILE *fp = fopen(ARQ_DADOS, "rb");
    if (fp == NULL) {
        /* primeira execucao — arquivo ainda nao existe */
        gravarLog("SISTEMA", "Arquivo de dados nao encontrado. Nova agenda criada.");
        return;
    }

    if (fread(&totalContatos, sizeof(int), 1, fp) != 1) {
        totalContatos = 0;
        fclose(fp);
        return;
    }
    if (totalContatos < 0 || totalContatos > MAX_CONTATOS)
        totalContatos = 0;

    fread(agenda, sizeof(Contato), totalContatos, fp);
    fclose(fp);

    /* mantem lista sempre ordenada ao carregar */
    ordenarContatos();

    {
        char detalhe[60];
        sprintf(detalhe, "%d contato(s) carregado(s)", totalContatos);
        gravarLog("SISTEMA", detalhe);
    }
}

/* ══════════════════════════════════════════════════════════════
 *  SALVAR ARQUIVO BINARIO
 * ══════════════════════════════════════════════════════════════ */
void salvarArquivo(void) {
    FILE *fp = fopen(ARQ_DADOS, "wb");
    if (fp == NULL) {
        printf(C_ERR "\n  [ERRO] Nao foi possivel salvar o arquivo '%s'.\n" RST, ARQ_DADOS);
        gravarLog("ERRO", "Falha ao abrir arquivo para gravacao");
        return;
    }
    fwrite(&totalContatos, sizeof(int),     1,             fp);
    fwrite(agenda,         sizeof(Contato), totalContatos, fp);
    fclose(fp);
}