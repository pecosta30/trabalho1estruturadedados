/* =====================================================================
 * TRABALHO DE ESTRUTURA DE DADOS - GERADOR DE RELATORIOS
 * Tema: Universidade Discente
 *
 * Arquivo unico com todo o programa:
 *   1 - Estruturas (listas encadeadas) e funcoes de inserir
 *   2 - Leitura dos arquivos .txt
 *   3 - Motor que desenha a tabela com + - |
 *   4 - Modelos de relatorio definidos pelo usuario (areas, secoes, campos)
 *   5 - Menu (main)
 *
 * COMO COMPILAR:   gcc -Wall -Wextra -o gerador codigobase.c
 * COMO RODAR:      ./gerador      (pasta dos .txt: Enter usa "dados")
 *
 * ARQUIVOS DE DADOS (campos separados por ';'), na pasta escolhida:
 *   departamentos.txt     id;nome;sigla
 *   cursos.txt            id;nome;carga_horaria_total;id_departamento
 *   alunos.txt            id;nome;email;data_nascimento;id_curso
 *   disciplinas.txt       id;nome;carga_horaria
 *   turmas.txt            id;sala;periodo;id_disciplina
 *   curso_disciplina.txt  id_curso;id_disciplina
 * ===================================================================== */

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>

/* =====================================================================
 * 1 - ESTRUTURAS E LISTAS ENCADEADAS
 *
 * Cada tabela do modelo ER vira uma lista encadeada dinamica: cada no
 * guarda um registro e o endereco do proximo no (NULL marca o fim).
 * ===================================================================== */

typedef struct nodepartamento {
    int  iddepartamento;
    char nomedepartamento[100];
    char sigladepartamento[10];
    struct nodepartamento *proximo;
} departamento;

typedef struct nocurso {
    int  id_curso;
    char nome[100];
    int  carga_horaria_total;
    int  id_departamento;          /* FK -> departamento */
    struct nocurso *proximo;
} curso;

typedef struct NoAluno {
    int  id_aluno;
    char nome_aluno[100];
    char email_aluno[60];
    char data_nascimento[11];      /* "dd/mm/aaaa" = 10 letras + '\0' */
    int  id_curso;                 /* FK -> curso */
    struct NoAluno *proximo;
} NoAluno;

typedef struct disciplina {
    int   idDisciplina;
    char  nome[60];
    float cargaHoraria;
    struct disciplina *proximo;
} disciplina;

typedef struct turma {
    int  idTurma;
    char sala[20];
    char periodo[10];
    int  idDisciplina;             /* FK -> disciplina */
    struct turma *proximo;
} turma;

/* Tabela de ligacao da relacao N:N "Curso possui Disciplina" */
typedef struct nocursodisciplina {
    int id_curso;                  /* FK -> curso */
    int id_disciplina;             /* FK -> disciplina */
    struct nocursodisciplina *proximo;
} cursodisciplina;

/* As 6 listas juntas, para passar tudo de uma vez */
typedef struct {
    departamento    *departamentos;
    curso           *cursos;
    NoAluno         *alunos;
    disciplina      *disciplinas;
    turma           *turmas;
    cursodisciplina *cursos_disciplinas;
} Base;

/* ---------- Regras unicas das listas (valem para TODAS as tabelas) ----------
 * LISTA_ADD_FIM: liga 'novo' no FINAL da lista (mantem a ordem do .txt).
 * LISTA_LIBERAR: da free em todos os nos da lista. */

#define LISTA_ADD_FIM(Tipo, lista, novo)            \
    do {                                            \
        Tipo **p_ = (lista);                        \
        while (*p_ != NULL)                         \
            p_ = &(*p_)->proximo;                   \
        *p_ = (novo);                               \
    } while (0)

#define LISTA_LIBERAR(Tipo, lista)                  \
    do {                                            \
        Tipo *p_ = (lista);                         \
        while (p_ != NULL) {                        \
            Tipo *prox_ = p_->proximo;              \
            free(p_);                               \
            p_ = prox_;                             \
        }                                           \
    } while (0)

/* Cria um no zerado. Devolve NULL (com aviso) se faltar memoria. */
static void *criarNo(size_t tamanho, const char *tipo)
{
    void *no = calloc(1, tamanho);
    if (no == NULL)
        fprintf(stderr, "Erro ao inserir %s: memoria insuficiente.\n", tipo);
    return no;
}

/* ---------- Funcoes de inserir: recebem os valores prontos (vindos do .txt) ---------- */

void inserirDepartamento(departamento **lista, int id, const char *nome, const char *sigla)
{
    departamento *novo = criarNo(sizeof *novo, "Departamento");
    if (novo == NULL)
        return;

    novo->iddepartamento = id;
    snprintf(novo->nomedepartamento, sizeof novo->nomedepartamento, "%s", nome);
    snprintf(novo->sigladepartamento, sizeof novo->sigladepartamento, "%s", sigla);
    LISTA_ADD_FIM(departamento, lista, novo);
}

void inserirCurso(curso **lista, int id, const char *nome,
                  int carga_horaria_total, int id_departamento)
{
    curso *novo = criarNo(sizeof *novo, "Curso");
    if (novo == NULL)
        return;

    novo->id_curso = id;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    novo->carga_horaria_total = carga_horaria_total;
    novo->id_departamento = id_departamento;
    LISTA_ADD_FIM(curso, lista, novo);
}

void inserirAluno(NoAluno **lista, int id, const char *nome, const char *email,
                  const char *data_nascimento, int id_curso)
{
    NoAluno *novo = criarNo(sizeof *novo, "Aluno");
    if (novo == NULL)
        return;

    novo->id_aluno = id;
    snprintf(novo->nome_aluno, sizeof novo->nome_aluno, "%s", nome);
    snprintf(novo->email_aluno, sizeof novo->email_aluno, "%s", email);
    snprintf(novo->data_nascimento, sizeof novo->data_nascimento, "%s", data_nascimento);
    novo->id_curso = id_curso;
    LISTA_ADD_FIM(NoAluno, lista, novo);
}

void inserirDisciplina(disciplina **lista, int id, const char *nome, float carga_horaria)
{
    disciplina *novo = criarNo(sizeof *novo, "Disciplina");
    if (novo == NULL)
        return;

    novo->idDisciplina = id;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    novo->cargaHoraria = carga_horaria;
    LISTA_ADD_FIM(disciplina, lista, novo);
}

void inserirTurma(turma **lista, int id, const char *sala, const char *periodo,
                  int id_disciplina)
{
    turma *novo = criarNo(sizeof *novo, "Turma");
    if (novo == NULL)
        return;

    novo->idTurma = id;
    snprintf(novo->sala, sizeof novo->sala, "%s", sala);
    snprintf(novo->periodo, sizeof novo->periodo, "%s", periodo);
    novo->idDisciplina = id_disciplina;
    LISTA_ADD_FIM(turma, lista, novo);
}

void inserirCursoDisciplina(cursodisciplina **lista, int id_curso, int id_disciplina)
{
    cursodisciplina *novo = criarNo(sizeof *novo, "CursoDisciplina");
    if (novo == NULL)
        return;

    novo->id_curso = id_curso;
    novo->id_disciplina = id_disciplina;
    LISTA_ADD_FIM(cursodisciplina, lista, novo);
}

/* Libera as 6 listas da base */
void liberarBase(Base *b)
{
    LISTA_LIBERAR(departamento,    b->departamentos);
    LISTA_LIBERAR(curso,           b->cursos);
    LISTA_LIBERAR(NoAluno,         b->alunos);
    LISTA_LIBERAR(disciplina,      b->disciplinas);
    LISTA_LIBERAR(turma,           b->turmas);
    LISTA_LIBERAR(cursodisciplina, b->cursos_disciplinas);
    memset(b, 0, sizeof *b);
}


/* =====================================================================
 * 2 - LEITURA DOS ARQUIVOS .txt
 *
 * Um unico leitor (lerArquivo) serve para os 6 arquivos. Cada arquivo tem
 * um "tratador" de 1 linha que converte os campos e chama o inserirX.
 * Linha com numero de campos errado e ignorada (com aviso): so ha uma regra.
 * ===================================================================== */

#define SEPARADOR ';'
#define MAX_CAMPOS 8
#define TAM_LINHA 512

/* Cada tratador recebe a lista (como void*) e os campos ja separados,
 * converte o que precisa (atoi) e chama a funcao que insere. */
typedef void (*TratadorLinha)(void *lista, char *campos[]);

/* Separa a linha em campos. Troca cada ';' por '\0' e guarda o inicio de
 * cada campo em campos[]. Diferente do strtok, nao "engole" campos vazios. */
static int separarCampos(char *linha, char *campos[], int max)
{
    int n = 0;
    char *ini = linha;

    linha[strcspn(linha, "\r\n")] = '\0';   /* tira o fim de linha */

    while (n < max) {
        campos[n++] = ini;
        char *sep = strchr(ini, SEPARADOR);
        if (sep == NULL)
            break;
        *sep = '\0';
        ini = sep + 1;
    }
    return n;
}

/* Abre pasta/arquivo, le linha por linha e chama o tratador de cada uma.
 * Linhas com numero de campos diferente do esperado sao ignoradas (com aviso).
 * Retorna a quantidade de linhas lidas, ou -1 se nao abriu o arquivo. */
static int lerArquivo(const char *pasta, const char *arquivo, int esperados,
                      void *lista, TratadorLinha trata)
{
    char caminho[256];
    char linha[TAM_LINHA];
    char *campos[MAX_CAMPOS];
    int lidas = 0, num = 0;

    snprintf(caminho, sizeof caminho, "%s/%s", pasta, arquivo);

    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        perror(caminho);
        return -1;
    }

    /* pula o BOM do UTF-8 (EF BB BF), que alguns editores gravam no inicio */
    int c1 = fgetc(f);
    if (c1 == 0xEF) {
        if (fgetc(f) != 0xBB || fgetc(f) != 0xBF)
            rewind(f);
    } else if (c1 != EOF) {
        ungetc(c1, f);
    }

    while (fgets(linha, sizeof linha, f) != NULL) {
        num++;

        if (linha[strspn(linha, " \t\r\n")] == '\0')   /* linha em branco */
            continue;

        int n = separarCampos(linha, campos, MAX_CAMPOS);
        if (n != esperados) {
            fprintf(stderr, "%s: linha %d ignorada (esperava %d campos, veio %d)\n",
                    caminho, num, esperados, n);
            continue;
        }

        trata(lista, campos);
        lidas++;
    }

    fclose(f);
    return lidas;
}

/* Tratadores: um por arquivo */

static void trataDepartamento(void *lista, char *c[])
{
    inserirDepartamento((departamento **)lista, atoi(c[0]), c[1], c[2]);
}

static void trataCurso(void *lista, char *c[])
{
    inserirCurso((curso **)lista, atoi(c[0]), c[1], atoi(c[2]), atoi(c[3]));
}

static void trataAluno(void *lista, char *c[])
{
    inserirAluno((NoAluno **)lista, atoi(c[0]), c[1], c[2], c[3], atoi(c[4]));
}

static void trataDisciplina(void *lista, char *c[])
{
    inserirDisciplina((disciplina **)lista, atoi(c[0]), c[1], (float)atof(c[2]));
}

static void trataTurma(void *lista, char *c[])
{
    inserirTurma((turma **)lista, atoi(c[0]), c[1], c[2], atoi(c[3]));
}

static void trataCursoDisciplina(void *lista, char *c[])
{
    inserirCursoDisciplina((cursodisciplina **)lista, atoi(c[0]), atoi(c[1]));
}

/* Le os 6 .txt da pasta. Retorna 0 se deu certo, -1 se algum nao abriu. */
int carregarTudo(Base *b, const char *pasta)
{
    int erro = 0;

    erro |= lerArquivo(pasta, "departamentos.txt",    3, &b->departamentos,      trataDepartamento)   < 0;
    erro |= lerArquivo(pasta, "cursos.txt",           4, &b->cursos,             trataCurso)          < 0;
    erro |= lerArquivo(pasta, "alunos.txt",           5, &b->alunos,             trataAluno)          < 0;
    erro |= lerArquivo(pasta, "disciplinas.txt",      3, &b->disciplinas,        trataDisciplina)     < 0;
    erro |= lerArquivo(pasta, "turmas.txt",           4, &b->turmas,             trataTurma)          < 0;
    erro |= lerArquivo(pasta, "curso_disciplina.txt", 2, &b->cursos_disciplinas, trataCursoDisciplina) < 0;

    return erro ? -1 : 0;
}


/* =====================================================================
 * 3 - MOTOR QUE DESENHA A TABELA COM + - |
 * ===================================================================== */

#define MAX_COLS   8
#define TAM_CELULA 100

/* Lista dinamica de uma SECAO do relatorio.
 * Cada secao e uma tabela: titulos das colunas + lista encadeada de linhas.
 * O relatorio monta uma lista por secao, escreve no TXT e libera. */

typedef struct Linha {
    char col[MAX_COLS][TAM_CELULA];
    struct Linha *proximo;
} Linha;

typedef struct {
    int    ncols;
    char   titulos[MAX_COLS][TAM_CELULA];
    int    larg[MAX_COLS];
    char   alinha[MAX_COLS];      /* 'E' = esquerda, 'D' = direita */
    Linha *inicio;
    Linha *fim;
} Secao;

static void secaoIniciar(Secao *s)
{
    s->ncols = 0;
    s->inicio = NULL;
    s->fim = NULL;
}

/* Define a proxima coluna da tabela */
static void secaoColuna(Secao *s, const char *titulo, int larg, char alinha)
{
    if (s->ncols >= MAX_COLS)
        return;
    snprintf(s->titulos[s->ncols], TAM_CELULA, "%s", titulo);
    s->larg[s->ncols] = larg;
    s->alinha[s->ncols] = alinha;
    s->ncols++;
}

/* Acrescenta uma linha no FIM da lista. 'valores' tem ncols textos. */
static void secaoAdd(Secao *s, const char *valores[])
{
    Linha *l = calloc(1, sizeof(Linha));
    if (l == NULL)
        return;

    for (int i = 0; i < s->ncols; i++)
        snprintf(l->col[i], TAM_CELULA, "%s", valores[i]);

    if (s->fim != NULL)
        s->fim->proximo = l;
    else
        s->inicio = l;
    s->fim = l;
}

static void secaoLiberar(Secao *s)
{
    Linha *l = s->inicio;
    while (l != NULL) {
        Linha *prox = l->proximo;
        free(l);
        l = prox;
    }
    s->inicio = NULL;
    s->fim = NULL;
}

/* Quantos bytes tem o caractere que comeca em p (UTF-8: acentos usam 2). */
static int bytesDoChar(const char *p)
{
    unsigned char c = (unsigned char)*p;
    int len = 1;

    if (c >= 0xF0)      len = 4;
    else if (c >= 0xE0) len = 3;
    else if (c >= 0xC0) len = 2;

    for (int k = 1; k < len; k++)
        if (((unsigned char)p[k] & 0xC0) != 0x80)   /* nao e UTF-8 valido (ou acabou o texto) */
            return 1;
    return len;
}

/* Quantos CARACTERES (nao bytes) tem o texto */
static int contarChars(const char *txt)
{
    int n = 0;
    while (*txt != '\0') {
        txt += bytesDoChar(txt);
        n++;
    }
    return n;
}

/* Escreve o texto ocupando exatamente 'larg' caracteres: completa com espacos
 * se for menor (a largura ja foi ajustada em secaoAjustarLargura, entao nao corta). alinha 'D' = encosta na direita. */
static void celula(FILE *f, const char *txt, int larg, char alinha)
{
    int chars = 0;
    const char *fim = txt;

    while (*fim != '\0' && chars < larg) {      /* acha ate onde cabe */
        fim += bytesDoChar(fim);
        chars++;
    }

    if (alinha == 'D')
        for (int i = chars; i < larg; i++)
            fputc(' ', f);

    for (const char *p = txt; p < fim; p++)
        fputc(*p, f);

    if (alinha != 'D')
        for (int i = chars; i < larg; i++)
            fputc(' ', f);
}

static void borda(FILE *f, const int larg[], int n)
{
    fputc('+', f);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < larg[i] + 2; j++)
            fputc('-', f);
        fputc('+', f);
    }
    fputc('\n', f);
}

/* alinha pode ser NULL (tudo a esquerda, usado no cabecalho da tabela) */
static void linhaTabela(FILE *f, const int larg[], const char alinha[],
                        char col[][TAM_CELULA], int n)
{
    fputc('|', f);
    for (int i = 0; i < n; i++) {
        fputc(' ', f);
        celula(f, col[i], larg[i], alinha ? alinha[i] : 'E');
        fputs(" |", f);
    }
    fputc('\n', f);
}

/* A largura escolhida e um MINIMO: cada coluna cresce o quanto for preciso
 * para caber o titulo e o maior valor da lista (nada e cortado). */
static void secaoAjustarLargura(Secao *s)
{
    for (int i = 0; i < s->ncols; i++) {
        int m = contarChars(s->titulos[i]);
        if (m > s->larg[i])
            s->larg[i] = m;
        for (Linha *l = s->inicio; l != NULL; l = l->proximo) {
            int c = contarChars(l->col[i]);
            if (c > s->larg[i])
                s->larg[i] = c;
        }
    }
}

static void secaoEscrever(FILE *f, Secao *s, const char *titulo)
{
    secaoAjustarLargura(s);
    fprintf(f, "%s\n", titulo);

    borda(f, s->larg, s->ncols);
    linhaTabela(f, s->larg, NULL, s->titulos, s->ncols);
    borda(f, s->larg, s->ncols);

    if (s->inicio == NULL) {
        int interno = s->ncols * 3 - 1;
        for (int i = 0; i < s->ncols; i++)
            interno += s->larg[i];
        fputs("| ", f);
        celula(f, "(sem registros)", interno - 2, 'E');
        fputs(" |\n", f);
    }
    for (Linha *l = s->inicio; l != NULL; l = l->proximo)
        linhaTabela(f, s->larg, s->alinha, l->col, s->ncols);

    borda(f, s->larg, s->ncols);
    fputc('\n', f);
}

static void cabecalhoRelatorio(FILE *f, const char *titulo)
{
    int n = (int)strlen(titulo) + 2;

    fputc('+', f);
    for (int i = 0; i < n; i++) fputc('-', f);
    fputs("+\n", f);
    fprintf(f, "| %s |\n", titulo);
    fputc('+', f);
    for (int i = 0; i < n; i++) fputc('-', f);
    fputs("+\n\n", f);
}

/* Buscas (juncao entre as tabelas pelas chaves estrangeiras) */

static const NoAluno *buscarAluno(const Base *b, int id)
{
    for (const NoAluno *a = b->alunos; a != NULL; a = a->proximo)
        if (a->id_aluno == id) return a;
    return NULL;
}

static const curso *buscarCurso(const Base *b, int id)
{
    for (const curso *c = b->cursos; c != NULL; c = c->proximo)
        if (c->id_curso == id) return c;
    return NULL;
}

static const departamento *buscarDepartamento(const Base *b, int id)
{
    for (const departamento *d = b->departamentos; d != NULL; d = d->proximo)
        if (d->iddepartamento == id) return d;
    return NULL;
}

static const disciplina *buscarDisciplina(const Base *b, int id)
{
    for (const disciplina *d = b->disciplinas; d != NULL; d = d->proximo)
        if (d->idDisciplina == id) return d;
    return NULL;
}

static FILE *abrirSaida(const char *arquivo)
{
    FILE *f = fopen(arquivo, "w");
    if (f == NULL)
        perror(arquivo);
    return f;
}


/* 4 - MODELOS DE RELATORIO DEFINIDOS PELO USUARIO
 *
 * Hierarquia pedida no enunciado:
 *     MODELO  ->  AREAS  ->  SECOES  ->  CAMPOS (nome, tipo, formato)
 *
 * A equipe DESENHOU 5 modelos (a tabela MODELOS mais abaixo).
 * O USUARIO, em tempo de execucao, escolhe um modelo e diz, para cada
 * secao de cada area: se entra no relatorio, qual o titulo, quais campos
 * (e em que ordem) e como cada campo e escrito (nome da coluna, largura,
 * alinhamento, casas decimais, formato da data).  */

#define MAX_AREAS       2
#define MAX_SECOES_AREA 2
#define NUM_MODELOS     5

typedef enum { T_TEXTO, T_INTEIRO, T_DECIMAL, T_DATA } TipoCampo;

/* Um campo que EXISTE para ser escolhido (catalogo) */
typedef struct {
    const char *nome;    /* nome padrao da coluna */
    TipoCampo   tipo;
    int         larg;    /* largura padrao */
} CampoDisp;

/* "Em que registro estamos" enquanto o relatorio percorre as listas */
typedef struct {
    const Base         *b;
    const NoAluno      *aluno;
    const departamento *depto;
    const curso        *cur;
    const disciplina   *disc;
} Contexto;

/* Funcao que devolve, como TEXTO, o valor de um campo de um registro */
typedef void (*Getter)(const Contexto *ctx, const void *reg, int campo,
                       char *out, size_t n);

/* De onde vem as linhas de uma secao */
typedef enum {
    SRC_ALUNO,            /* 1 registro: o aluno escolhido                     */
    SRC_VINCULO,          /* 1 registro: curso + departamento do aluno          */
    SRC_DEPTO,            /* 1 registro: departamento atual                     */
    SRC_CURSOS_DO_DEPTO,  /* lista: cursos do departamento atual                */
    SRC_CURSO,            /* 1 registro: curso atual                            */
    SRC_DISCS_DO_CURSO,   /* lista: disciplinas do curso atual (tabela N:N)     */
    SRC_DISCIPLINA,       /* 1 registro: disciplina atual                       */
    SRC_TURMAS_DA_DISC,   /* lista: turmas da disciplina atual                  */
    SRC_TURMAS_ORFAS,     /* lista: turmas cuja disciplina nao esta cadastrada  */
    SRC_TODAS_TURMAS      /* lista: todas as turmas                             */
} Fonte;

/* Desenho (feito pela equipe) de uma secao */
typedef struct {
    const char      *titulo;     /* titulo padrao */
    Fonte            fonte;
    int              vertical;   /* 1 = tabela "Campo | Valor" (um registro)
                                    0 = uma linha para cada registro      */
    const CampoDisp *cat;        /* campos disponiveis nesta secao */
    int              ncat;
    Getter           get;
} SecaoDef;

typedef struct {
    const char *nome;
    int         nsec;
    SecaoDef    secoes[MAX_SECOES_AREA];
} AreaDef;

typedef struct {
    const char *nome;
    int         narea;
    AreaDef     areas[MAX_AREAS];
} ModeloDef;

/* Escolha do usuario para um campo */
typedef struct {
    int       idx;                 /* qual campo do catalogo */
    char      titulo[TAM_CELULA];  /* nome da coluna */
    int       larg;
    char      alinha;              /* 'E' ou 'D' */
    int       casas;               /* casas decimais (so T_DECIMAL) */
    int       fmt_data;            /* 1=dd/mm/aaaa  2=aaaa-mm-dd  3=dd/mm (so T_DATA) */
} CampoEsc;

/* Escolha do usuario para uma secao */
typedef struct {
    int       incluir;
    char      titulo[60];
    int       n;
    CampoEsc  campos[MAX_COLS];
} ConfigSecao;

typedef struct {
    ConfigSecao sec[MAX_AREAS][MAX_SECOES_AREA];
} ConfigModelo;

/* ----- Catalogos: que campos cada tipo de registro oferece ----- */

static const CampoDisp CAT_ALUNO[] = {
    {"Id do aluno",  T_INTEIRO,  8},
    {"Nome",         T_TEXTO,   30},
    {"E-mail",       T_TEXTO,   30},
    {"Nascimento",   T_DATA,    10},
    {"Id do curso",  T_INTEIRO, 11},
};

static const CampoDisp CAT_VINCULO[] = {
    {"Curso",         T_TEXTO,   30},
    {"Carga horaria", T_INTEIRO, 13},
    {"Departamento",  T_TEXTO,   30},
    {"Sigla",         T_TEXTO,    8},
};

static const CampoDisp CAT_DEPTO[] = {
    {"Id",            T_INTEIRO,  8},
    {"Departamento",  T_TEXTO,   30},
    {"Sigla",         T_TEXTO,    8},
};

static const CampoDisp CAT_CURSO[] = {
    {"Id curso",      T_INTEIRO,  8},
    {"Curso",         T_TEXTO,   30},
    {"Carga horaria", T_INTEIRO, 13},
    {"Id depto",      T_INTEIRO,  8},
};

static const CampoDisp CAT_DISC[] = {
    {"Id disc.",      T_INTEIRO,  8},
    {"Disciplina",    T_TEXTO,   30},
    {"Carga horaria", T_DECIMAL, 13},
};

static const CampoDisp CAT_TURMA[] = {
    {"Id turma",      T_INTEIRO,  8},
    {"Sala",          T_TEXTO,   12},
    {"Periodo",       T_TEXTO,   10},
    {"Id disc.",      T_INTEIRO,  8},
    {"Disciplina",    T_TEXTO,   24},
};

/* ----- Getters: pegam o valor do campo e devolvem como texto ----- */

static void poeTexto(char *out, size_t n, const char *s)
{
    snprintf(out, n, "%s", s);
}

static void poeInteiro(char *out, size_t n, int v)
{
    snprintf(out, n, "%d", v);
}

static void valAluno(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    const NoAluno *a = reg;
    (void)ctx;
    switch (campo) {
    case 0: poeInteiro(out, n, a->id_aluno);       break;
    case 1: poeTexto(out, n, a->nome_aluno);        break;
    case 2: poeTexto(out, n, a->email_aluno);       break;
    case 3: poeTexto(out, n, a->data_nascimento);   break;
    case 4: poeInteiro(out, n, a->id_curso);        break;
    default: out[0] = '\0';
    }
}

/* Curso + departamento do aluno: usa o contexto (nao recebe registro) */
static void valVinculo(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    (void)reg;
    switch (campo) {
    case 0: poeTexto(out, n, ctx->cur ? ctx->cur->nome : "(nao encontrado)"); break;
    case 1:
        if (ctx->cur) poeInteiro(out, n, ctx->cur->carga_horaria_total);
        else          poeTexto(out, n, "-");
        break;
    case 2: poeTexto(out, n, ctx->depto ? ctx->depto->nomedepartamento : "(nao encontrado)"); break;
    case 3: poeTexto(out, n, ctx->depto ? ctx->depto->sigladepartamento : "-"); break;
    default: out[0] = '\0';
    }
}

static void valDepto(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    const departamento *d = reg;
    (void)ctx;
    switch (campo) {
    case 0: poeInteiro(out, n, d->iddepartamento);   break;
    case 1: poeTexto(out, n, d->nomedepartamento);    break;
    case 2: poeTexto(out, n, d->sigladepartamento);   break;
    default: out[0] = '\0';
    }
}

static void valCurso(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    const curso *c = reg;
    (void)ctx;
    switch (campo) {
    case 0: poeInteiro(out, n, c->id_curso);              break;
    case 1: poeTexto(out, n, c->nome);                     break;
    case 2: poeInteiro(out, n, c->carga_horaria_total);    break;
    case 3: poeInteiro(out, n, c->id_departamento);        break;
    default: out[0] = '\0';
    }
}

static void valDisc(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    const disciplina *d = reg;
    (void)ctx;
    switch (campo) {
    case 0: poeInteiro(out, n, d->idDisciplina);   break;
    case 1: poeTexto(out, n, d->nome);              break;
    case 2:
        if (d->cargaHoraria < 0) poeTexto(out, n, "-");   /* disciplina que nao existe */
        else                     snprintf(out, n, "%g", d->cargaHoraria);
        break;
    default: out[0] = '\0';
    }
}

static void valTurma(const Contexto *ctx, const void *reg, int campo, char *out, size_t n)
{
    const turma *t = reg;
    switch (campo) {
    case 0: poeInteiro(out, n, t->idTurma);       break;
    case 1: poeTexto(out, n, t->sala);             break;
    case 2: poeTexto(out, n, t->periodo);          break;
    case 3: poeInteiro(out, n, t->idDisciplina);   break;
    case 4: {
        const disciplina *d = buscarDisciplina(ctx->b, t->idDisciplina);
        poeTexto(out, n, d ? d->nome : "(nao cadastrada)");
        break;
    }
    default: out[0] = '\0';
    }
}

/* ----- Os 5 modelos desenhados pela equipe ----- */

#define TAMCAT(x) ((int)(sizeof(x) / sizeof((x)[0])))

static const ModeloDef MODELOS[NUM_MODELOS] = {
    /* 1 */ { "Ficha do aluno", 2, {
        { "Identificacao", 1, {
            { "Dados pessoais", SRC_ALUNO, 1, CAT_ALUNO, TAMCAT(CAT_ALUNO), valAluno } } },
        { "Vinculo academico", 1, {
            { "Curso e departamento", SRC_VINCULO, 1, CAT_VINCULO, TAMCAT(CAT_VINCULO), valVinculo } } },
    } },
    /* 2 */ { "Cursos por departamento", 1, {
        { "Departamento", 2, {
            { "Dados do departamento", SRC_DEPTO,           1, CAT_DEPTO, TAMCAT(CAT_DEPTO), valDepto },
            { "Cursos oferecidos",     SRC_CURSOS_DO_DEPTO, 0, CAT_CURSO, TAMCAT(CAT_CURSO), valCurso } } },
    } },
    /* 3 */ { "Disciplinas por curso", 1, {
        { "Curso", 2, {
            { "Dados do curso",        SRC_CURSO,          1, CAT_CURSO, TAMCAT(CAT_CURSO), valCurso },
            { "Disciplinas do curso",  SRC_DISCS_DO_CURSO, 0, CAT_DISC,  TAMCAT(CAT_DISC),  valDisc  } } },
    } },
    /* 4 */ { "Turmas por disciplina", 1, {
        { "Disciplina", 2, {
            { "Dados da disciplina",   SRC_DISCIPLINA,     1, CAT_DISC,  TAMCAT(CAT_DISC),  valDisc  },
            { "Turmas ofertadas",      SRC_TURMAS_DA_DISC, 0, CAT_TURMA, TAMCAT(CAT_TURMA), valTurma } } },
    } },
    /* 5 */ { "Lista de turmas", 1, {
        { "Turmas", 1, {
            { "Todas as turmas",       SRC_TODAS_TURMAS,   0, CAT_TURMA, TAMCAT(CAT_TURMA), valTurma } } },
    } },
};

/* nome padrao do arquivo de saida de cada modelo */
static const char *ARQ_PADRAO[NUM_MODELOS] = {
    "relatorio_ficha_aluno.txt",
    "relatorio_cursos_departamento.txt",
    "relatorio_disciplinas_curso.txt",
    "relatorio_turmas_disciplina.txt",
    "relatorio_lista_turmas.txt",
};

/* ----- Configuracao padrao (tudo incluido, nomes e larguras do catalogo) ----- */

static void campoPadrao(CampoEsc *ce, int idx, const CampoDisp *cd)
{
    ce->idx = idx;
    snprintf(ce->titulo, sizeof ce->titulo, "%s", cd->nome);
    ce->larg = cd->larg;
    ce->alinha = (cd->tipo == T_INTEIRO || cd->tipo == T_DECIMAL) ? 'D' : 'E';
    ce->casas = (cd->tipo == T_DECIMAL) ? 1 : 0;   /* decimal: 1 casa por padrao */
    ce->fmt_data = 1;
}

static void configPadrao(ConfigModelo *cfg, int m)
{
    memset(cfg, 0, sizeof *cfg);
    for (int a = 0; a < MODELOS[m].narea; a++) {
        for (int j = 0; j < MODELOS[m].areas[a].nsec; j++) {
            const SecaoDef *def = &MODELOS[m].areas[a].secoes[j];
            ConfigSecao *cs = &cfg->sec[a][j];

            cs->incluir = 1;
            snprintf(cs->titulo, sizeof cs->titulo, "%s", def->titulo);
            cs->n = (def->ncat < MAX_COLS) ? def->ncat : MAX_COLS;
            for (int i = 0; i < cs->n; i++)
                campoPadrao(&cs->campos[i], i, &def->cat[i]);
        }
    }
}

/* ----- Formatacao do valor de acordo com o TIPO e o FORMATO escolhidos ----- */

static void formatarValor(const CampoDisp *cd, const CampoEsc *ce,
                          const char *raw, char *out, size_t n)
{
    char *fim;

    switch (cd->tipo) {
    case T_INTEIRO: {
        long v = strtol(raw, &fim, 10);
        if (fim == raw || *fim != '\0') snprintf(out, n, "%s", raw);   /* nao e numero (ex.: "-") */
        else                            snprintf(out, n, "%ld", v);
        break;
    }
    case T_DECIMAL: {
        double v = strtod(raw, &fim);
        if (fim == raw || *fim != '\0') snprintf(out, n, "%s", raw);
        else                            snprintf(out, n, "%.*f", ce->casas, v);
        break;
    }
    case T_DATA: {
        int d, m, a;
        if (sscanf(raw, "%d/%d/%d", &d, &m, &a) == 3) {
            if (ce->fmt_data == 2)      snprintf(out, n, "%04d-%02d-%02d", a, m, d);
            else if (ce->fmt_data == 3) snprintf(out, n, "%02d/%02d", d, m);
            else                        snprintf(out, n, "%02d/%02d/%04d", d, m, a);
        } else {
            snprintf(out, n, "%s", raw);
        }
        break;
    }
    default:   /* T_TEXTO */
        snprintf(out, n, "%s", raw);
    }
}

/* ----- Monta as colunas de uma secao a partir da escolha do usuario ----- */

static void montarColunas(Secao *s, const SecaoDef *def, const ConfigSecao *cfg)
{
    if (def->vertical) {
        /* tabela "Campo | Valor": 1a coluna = maior titulo; 2a = maior largura escolhida */
        int lt = 5, lv = 5;
        for (int i = 0; i < cfg->n; i++) {
            int t = contarChars(cfg->campos[i].titulo);
            if (t > lt) lt = t;
            if (cfg->campos[i].larg > lv) lv = cfg->campos[i].larg;
        }
        if (lt > 30) lt = 30;
        secaoColuna(s, "Campo", lt, 'E');
        secaoColuna(s, "Valor", lv, 'E');
    } else {
        for (int i = 0; i < cfg->n; i++)
            secaoColuna(s, cfg->campos[i].titulo, cfg->campos[i].larg, cfg->campos[i].alinha);
    }
}

/* ----- Acrescenta um registro (uma ou varias linhas) na secao ----- */

static void adicionarRegistro(Secao *s, const SecaoDef *def, const ConfigSecao *cfg,
                              const Contexto *ctx, const void *reg)
{
    char cru[TAM_CELULA];
    char fmt[MAX_COLS][TAM_CELULA];

    for (int i = 0; i < cfg->n; i++) {
        const CampoEsc *ce = &cfg->campos[i];
        def->get(ctx, reg, ce->idx, cru, sizeof cru);
        formatarValor(&def->cat[ce->idx], ce, cru, fmt[i], sizeof fmt[i]);
    }

    if (def->vertical) {
        for (int i = 0; i < cfg->n; i++) {        /* cada campo vira uma linha */
            const char *linha[2] = { cfg->campos[i].titulo, fmt[i] };
            secaoAdd(s, linha);
        }
    } else {
        const char *linha[MAX_COLS];              /* o registro inteiro vira uma linha */
        for (int i = 0; i < cfg->n; i++)
            linha[i] = fmt[i];
        secaoAdd(s, linha);
    }
}

/* ----- Constroi a lista da secao, escreve no arquivo e libera ----- */

static void escreverSecao(FILE *f, const SecaoDef *def, const ConfigSecao *cfg,
                          Fonte fonte, const Contexto *ctx)
{
    if (!cfg->incluir || cfg->n == 0)
        return;

    Secao s;
    secaoIniciar(&s);
    montarColunas(&s, def, cfg);

    switch (fonte) {
    case SRC_ALUNO:      adicionarRegistro(&s, def, cfg, ctx, ctx->aluno); break;
    case SRC_VINCULO:    adicionarRegistro(&s, def, cfg, ctx, NULL);        break;
    case SRC_DEPTO:      adicionarRegistro(&s, def, cfg, ctx, ctx->depto);  break;
    case SRC_CURSO:      adicionarRegistro(&s, def, cfg, ctx, ctx->cur);    break;
    case SRC_DISCIPLINA: adicionarRegistro(&s, def, cfg, ctx, ctx->disc);   break;

    case SRC_CURSOS_DO_DEPTO:
        for (const curso *c = ctx->b->cursos; c != NULL; c = c->proximo)
            if (c->id_departamento == ctx->depto->iddepartamento)
                adicionarRegistro(&s, def, cfg, ctx, c);
        break;

    case SRC_DISCS_DO_CURSO:
        for (const cursodisciplina *cd = ctx->b->cursos_disciplinas; cd != NULL; cd = cd->proximo) {
            if (cd->id_curso != ctx->cur->id_curso)
                continue;
            const disciplina *d = buscarDisciplina(ctx->b, cd->id_disciplina);
            if (d != NULL) {
                adicionarRegistro(&s, def, cfg, ctx, d);
            } else {                               /* vinculo para disciplina que nao existe */
                disciplina falta;
                memset(&falta, 0, sizeof falta);
                falta.idDisciplina = cd->id_disciplina;
                snprintf(falta.nome, sizeof falta.nome, "(disciplina nao cadastrada)");
                falta.cargaHoraria = -1;
                adicionarRegistro(&s, def, cfg, ctx, &falta);
            }
        }
        break;

    case SRC_TURMAS_DA_DISC:
        for (const turma *t = ctx->b->turmas; t != NULL; t = t->proximo)
            if (t->idDisciplina == ctx->disc->idDisciplina)
                adicionarRegistro(&s, def, cfg, ctx, t);
        break;

    case SRC_TURMAS_ORFAS:
        for (const turma *t = ctx->b->turmas; t != NULL; t = t->proximo)
            if (buscarDisciplina(ctx->b, t->idDisciplina) == NULL)
                adicionarRegistro(&s, def, cfg, ctx, t);
        break;

    case SRC_TODAS_TURMAS:
        for (const turma *t = ctx->b->turmas; t != NULL; t = t->proximo)
            adicionarRegistro(&s, def, cfg, ctx, t);
        break;
    }

    char titulo[100];
    snprintf(titulo, sizeof titulo, "Secao: %s", cfg->titulo);
    secaoEscrever(f, &s, titulo);
    secaoLiberar(&s);
}

/* ----- Uma AREA: titulo + todas as secoes escolhidas ----- */

static int areaTemSecao(const ConfigModelo *cfg, int m, int a)
{
    for (int j = 0; j < MODELOS[m].areas[a].nsec; j++)
        if (cfg->sec[a][j].incluir && cfg->sec[a][j].n > 0)
            return 1;
    return 0;
}

static void escreverArea(FILE *f, const ConfigModelo *cfg, int m, int a,
                         const char *titulo, const Contexto *ctx)
{
    if (!areaTemSecao(cfg, m, a))
        return;

    fprintf(f, "AREA: %s\n", titulo);
    for (int j = 0; j < MODELOS[m].areas[a].nsec; j++) {
        const SecaoDef *def = &MODELOS[m].areas[a].secoes[j];
        escreverSecao(f, def, &cfg->sec[a][j], def->fonte, ctx);
    }
}

/* ----- Gera o relatorio do modelo m (0 a NUM_MODELOS-1) ----- */

int gerarRelatorio(const Base *b, int m, const ConfigModelo *cfg,
                   int id_aluno, const char *arquivo)
{
    Contexto ctx;
    char titulo[200];

    memset(&ctx, 0, sizeof ctx);
    ctx.b = b;

    if (m == 0) {                                  /* a ficha precisa do aluno */
        ctx.aluno = buscarAluno(b, id_aluno);
        if (ctx.aluno == NULL) {
            printf("Aluno %d nao encontrado.\n", id_aluno);
            return -1;
        }
        ctx.cur = buscarCurso(b, ctx.aluno->id_curso);
        ctx.depto = ctx.cur ? buscarDepartamento(b, ctx.cur->id_departamento) : NULL;
    }

    FILE *f = abrirSaida(arquivo);
    if (f == NULL)
        return -1;

    snprintf(titulo, sizeof titulo, "RELATORIO: %s", MODELOS[m].nome);
    cabecalhoRelatorio(f, titulo);

    switch (m) {
    case 0:                                        /* Ficha do aluno: 2 areas fixas */
        escreverArea(f, cfg, m, 0, MODELOS[m].areas[0].nome, &ctx);
        escreverArea(f, cfg, m, 1, MODELOS[m].areas[1].nome, &ctx);
        break;

    case 1:                                        /* uma area por departamento */
        for (const departamento *d = b->departamentos; d != NULL; d = d->proximo) {
            ctx.depto = d;
            snprintf(titulo, sizeof titulo, "Departamento %s (%s)",
                     d->nomedepartamento, d->sigladepartamento);
            escreverArea(f, cfg, m, 0, titulo, &ctx);
        }
        break;

    case 2:                                        /* uma area por curso */
        for (const curso *c = b->cursos; c != NULL; c = c->proximo) {
            ctx.cur = c;
            snprintf(titulo, sizeof titulo, "Curso %s (id %d)", c->nome, c->id_curso);
            escreverArea(f, cfg, m, 0, titulo, &ctx);
        }
        break;

    case 3: {                                      /* uma area por disciplina */
        for (const disciplina *d = b->disciplinas; d != NULL; d = d->proximo) {
            ctx.disc = d;
            snprintf(titulo, sizeof titulo, "Disciplina %s (id %d)", d->nome, d->idDisciplina);
            escreverArea(f, cfg, m, 0, titulo, &ctx);
        }
        /* turmas cuja disciplina nao esta cadastrada nao podem sumir:
         * area propria, so aparece se existir alguma */
        int temOrfa = 0;
        for (const turma *t = b->turmas; t != NULL; t = t->proximo)
            if (buscarDisciplina(b, t->idDisciplina) == NULL)
                temOrfa = 1;
        if (temOrfa && cfg->sec[0][1].incluir && cfg->sec[0][1].n > 0) {
            fprintf(f, "AREA: Turmas sem disciplina cadastrada\n");
            escreverSecao(f, &MODELOS[m].areas[0].secoes[1], &cfg->sec[0][1],
                          SRC_TURMAS_ORFAS, &ctx);
        }
        break;
    }

    case 4:                                        /* uma area so */
        escreverArea(f, cfg, m, 0, MODELOS[m].areas[0].nome, &ctx);
        break;
    }

    fclose(f);
    return 0;
}


/* =====================================================================
 * 5 - MENU (main)
 * ===================================================================== */

/* Le um numero inteiro digitado. Retorna -1 se nao for numero e 0 se a
 * entrada acabou (assim o menu encerra em vez de repetir para sempre). */
static int lerInteiro(const char *mensagem)
{
    char buf[64];
    char *fim;

    printf("%s", mensagem);
    if (fgets(buf, sizeof buf, stdin) == NULL)
        return 0;

    long v = strtol(buf, &fim, 10);
    if (fim == buf)
        return -1;
    return (int)v;
}

/* Le um texto; se o usuario so apertar Enter, usa o valor padrao. */
static void lerTexto(const char *mensagem, char *dest, size_t tam, const char *padrao)
{
    printf("%s", mensagem);
    if (fgets(dest, (int)tam, stdin) == NULL) {
        snprintf(dest, tam, "%s", padrao);
        return;
    }
    dest[strcspn(dest, "\r\n")] = '\0';
    if (dest[0] == '\0')
        snprintf(dest, tam, "%s", padrao);
}

/* ---------- Interface: o usuario DEFINE o modelo do relatorio ---------- */

static const char *nomeTipo(TipoCampo t)
{
    switch (t) {
    case T_INTEIRO: return "inteiro";
    case T_DECIMAL: return "decimal";
    case T_DATA:    return "data";
    default:        return "texto";
    }
}

/* Pergunta um texto. O valor que ja esta em 'dest' e o padrao (Enter mantem). */
static void perguntarTexto(const char *rotulo, char *dest, size_t tam)
{
    char padrao[TAM_CELULA];
    char prompt[200];

    snprintf(padrao, sizeof padrao, "%s", dest);
    snprintf(prompt, sizeof prompt, "%s [%s]: ", rotulo, padrao);
    lerTexto(prompt, dest, tam, padrao);
}

/* Pergunta um inteiro entre min e max. Enter (ou valor invalido) mantem o atual. */
static int perguntarInteiro(const char *rotulo, int atual, int min, int max)
{
    char buf[64];
    char prompt[200];
    char *fim;

    snprintf(prompt, sizeof prompt, "%s [%d]: ", rotulo, atual);
    lerTexto(prompt, buf, sizeof buf, "");
    if (buf[0] == '\0')
        return atual;

    long v = strtol(buf, &fim, 10);
    if (fim == buf || v < min || v > max) {
        printf("  valor invalido, mantido %d\n", atual);
        return atual;
    }
    return (int)v;
}

/* Define UMA secao: incluir?, titulo, campos (e ordem) e formato de cada campo */
static void definirSecao(const SecaoDef *def, ConfigSecao *cs)
{
    char buf[200];
    int escolhidos[MAX_COLS];
    int n = 0;

    printf("\n--- Secao \"%s\" ---\n", def->titulo);
    lerTexto("Incluir esta secao no relatorio? (S/n): ", buf, sizeof buf, "s");
    cs->incluir = (buf[0] != 'n' && buf[0] != 'N');
    if (!cs->incluir)
        return;

    perguntarTexto("Titulo da secao", cs->titulo, sizeof cs->titulo);

    printf("Campos disponiveis:\n");
    for (int i = 0; i < def->ncat; i++)
        printf("  %d - %s (%s)\n", i + 1, def->cat[i].nome, nomeTipo(def->cat[i].tipo));
    lerTexto("Quais campos, na ordem (numeros separados por espaco) [todos]: ",
             buf, sizeof buf, "");

    if (buf[0] != '\0') {
        for (char *t = strtok(buf, " ,;"); t != NULL && n < MAX_COLS; t = strtok(NULL, " ,;")) {
            int k = atoi(t);
            int repetido = 0;
            if (k < 1 || k > def->ncat)
                continue;
            for (int j = 0; j < n; j++)
                if (escolhidos[j] == k - 1)
                    repetido = 1;
            if (!repetido)
                escolhidos[n++] = k - 1;
        }
        if (n == 0)
            printf("  nenhum campo valido, usando todos.\n");
    }
    if (n == 0)
        for (int i = 0; i < def->ncat && i < MAX_COLS; i++)
            escolhidos[n++] = i;

    cs->n = n;
    for (int i = 0; i < n; i++) {
        const CampoDisp *cd = &def->cat[escolhidos[i]];
        CampoEsc *ce = &cs->campos[i];

        campoPadrao(ce, escolhidos[i], cd);
        printf("\nCampo \"%s\" (%s):\n", cd->nome, nomeTipo(cd->tipo));

        perguntarTexto("  Nome da coluna", ce->titulo, sizeof ce->titulo);
        ce->larg = perguntarInteiro("  Largura (caracteres)", ce->larg, 1, 80);

        if (!def->vertical) {
            char al[8];
            snprintf(al, sizeof al, "%c", ce->alinha);
            perguntarTexto("  Alinhamento (E = esquerda, D = direita)", al, sizeof al);
            ce->alinha = (al[0] == 'd' || al[0] == 'D') ? 'D' : 'E';
        }
        if (cd->tipo == T_DECIMAL)
            ce->casas = perguntarInteiro("  Casas decimais", ce->casas, 0, 6);
        if (cd->tipo == T_DATA)
            ce->fmt_data = perguntarInteiro("  Formato da data (1 = dd/mm/aaaa, 2 = aaaa-mm-dd, 3 = dd/mm)",
                                            ce->fmt_data, 1, 3);
    }
}

/* Fluxo completo: escolher modelo -> (opcional) definir secoes/campos -> gerar */
static void menuGerarRelatorio(const Base *b, ConfigModelo cfgs[])
{
    char buf[16];
    char arq[200];
    char padrao[200];
    int id_aluno = 0;

    printf("\nModelos de relatorio:\n");
    for (int m = 0; m < NUM_MODELOS; m++)
        printf("  %d - %s\n", m + 1, MODELOS[m].nome);

    int m = lerInteiro("Escolha o modelo: ") - 1;
    if (m < 0 || m >= NUM_MODELOS) {
        printf("Modelo invalido.\n");
        return;
    }

    lerTexto("Definir agora as secoes e os campos? (s/N - Enter usa a ultima definicao): ",
             buf, sizeof buf, "n");
    if (buf[0] == 's' || buf[0] == 'S') {
        for (int a = 0; a < MODELOS[m].narea; a++) {
            printf("\n===== AREA \"%s\" =====\n", MODELOS[m].areas[a].nome);
            for (int j = 0; j < MODELOS[m].areas[a].nsec; j++)
                definirSecao(&MODELOS[m].areas[a].secoes[j], &cfgs[m].sec[a][j]);
        }
    }

    if (m == 0)
        id_aluno = lerInteiro("Id do aluno: ");

    snprintf(padrao, sizeof padrao, "%s", ARQ_PADRAO[m]);
    {
        char prompt[300];
        snprintf(prompt, sizeof prompt, "Arquivo de saida [%s]: ", padrao);
        lerTexto(prompt, arq, sizeof arq, padrao);
    }

    if (gerarRelatorio(b, m, &cfgs[m], id_aluno, arq) == 0)
        printf("Relatorio gerado em %s\n", arq);
}

/* Limpa a tela do terminal (cls no Windows, clear nos demais sistemas) */
static void limparTela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Espera o usuario apertar Enter, para ele ler a mensagem antes de limpar a tela */
static void pausar(void)
{
    char buf[8];
    lerTexto("\nPressione Enter para continuar...", buf, sizeof buf, "");
}

int main(void)
{
    setlocale(LC_ALL, "Portuguese");

    Base base;
    ConfigModelo config[NUM_MODELOS];   /* a definicao do usuario para cada modelo */
    char pasta[200];
    int opcao;

    memset(&base, 0, sizeof base);
    for (int m = 0; m < NUM_MODELOS; m++)
        configPadrao(&config[m], m);

    /* Escolha dos arquivos: pasta onde estao os .txt */
    limparTela();
    lerTexto("Pasta com os arquivos .txt [dados]: ", pasta, sizeof pasta, "dados");
    if (carregarTudo(&base, pasta) != 0) {
        printf("Nao foi possivel abrir todos os arquivos da pasta '%s'.\n", pasta);
        liberarBase(&base);
        return 1;
    }

    do {
        printf("\n===== GERADOR DE RELATORIOS - UNIVERSIDADE DISCENTE =====\n");
        printf(" 1 - Gerar relatorio (escolher modelo e definir secoes e campos)\n");
        printf(" 0 - Sair\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
        case 1:
            limparTela();
            menuGerarRelatorio(&base, config);
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida.\n");
        }

        if (opcao != 0) {
            pausar();
            limparTela();
        }
    } while (opcao != 0);

    liberarBase(&base);
    return 0;
}
