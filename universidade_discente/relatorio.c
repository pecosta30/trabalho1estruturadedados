#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "estruturas.h"

#define MAX_COLS   4
#define TAM_CELULA 100

/* ---------------------------------------------------------------------
 * Lista dinamica de uma SECAO do relatorio.
 * Cada secao e uma tabela: titulos das colunas + lista encadeada de linhas.
 * O relatorio escolhido monta uma lista por secao, escreve no TXT e libera.
 * --------------------------------------------------------------------- */

typedef struct Linha {
    char col[MAX_COLS][TAM_CELULA];
    struct Linha *prox;
} Linha;

typedef struct {
    int    ncols;
    char   titulos[MAX_COLS][TAM_CELULA];
    int    larg[MAX_COLS];
    Linha *inicio;
    Linha *fim;
} Secao;

/* Converte int em texto (usa 8 buffers rotativos, entao pode ser chamada
 * varias vezes na mesma chamada de secaoAdd). */
static const char *num(int n)
{
    static char buf[8][16];
    static int i = 0;
    i = (i + 1) % 8;
    snprintf(buf[i], sizeof buf[i], "%d", n);
    return buf[i];
}

static void secaoIniciar(Secao *s, int ncols, const char *titulos[], const int larg[])
{
    s->ncols = ncols;
    for (int i = 0; i < ncols; i++) {
        snprintf(s->titulos[i], TAM_CELULA, "%s", titulos[i]);
        s->larg[i] = larg[i];
    }
    s->inicio = NULL;
    s->fim = NULL;
}

/* Acrescenta uma linha no fim da lista. Recebe ncols textos (const char *). */
static void secaoAdd(Secao *s, ...)
{
    Linha *l = calloc(1, sizeof(Linha));
    if (l == NULL)
        return;

    va_list ap;
    va_start(ap, s);
    for (int i = 0; i < s->ncols; i++) {
        const char *valor = va_arg(ap, const char *);
        snprintf(l->col[i], TAM_CELULA, "%s", valor);
    }
    va_end(ap);

    if (s->fim != NULL)
        s->fim->prox = l;
    else
        s->inicio = l;
    s->fim = l;
}

static void secaoLiberar(Secao *s)
{
    Linha *l = s->inicio;
    while (l != NULL) {
        Linha *prox = l->prox;
        free(l);
        l = prox;
    }
    s->inicio = NULL;
    s->fim = NULL;
}

/* ---------------------------------------------------------------------
 * Desenho da tabela com '+', '-' e '|'
 * --------------------------------------------------------------------- */

/* Escreve o texto ocupando exatamente 'larg' caracteres (corta ou completa
 * com espacos). Conta caracteres UTF-8, para acentos nao desalinharem. */
static void celula(FILE *f, const char *txt, int larg)
{
    int chars = 0;
    const char *p = txt;

    while (*p != '\0' && chars < larg) {
        unsigned char c = (unsigned char)*p;
        int len = 1;
        if (c >= 0xF0)      len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;

        for (int k = 1; k < len; k++) {
            if (((unsigned char)p[k] & 0xC0) != 0x80) {   /* nao e UTF-8 valido */
                len = 1;
                break;
            }
        }
        for (int k = 0; k < len; k++)
            fputc(*p++, f);
        chars++;
    }
    for (; chars < larg; chars++)
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

static void linhaTabela(FILE *f, const int larg[], char col[][TAM_CELULA], int n)
{
    fputc('|', f);
    for (int i = 0; i < n; i++) {
        fputc(' ', f);
        celula(f, col[i], larg[i]);
        fputs(" |", f);
    }
    fputc('\n', f);
}

static void secaoEscrever(FILE *f, Secao *s, const char *titulo)
{
    fprintf(f, "%s\n", titulo);

    borda(f, s->larg, s->ncols);
    linhaTabela(f, s->larg, s->titulos, s->ncols);
    borda(f, s->larg, s->ncols);

    if (s->inicio == NULL) {
        int interno = s->ncols * 3 - 1;
        for (int i = 0; i < s->ncols; i++)
            interno += s->larg[i];
        fputs("| ", f);
        celula(f, "(sem registros)", interno - 2);
        fputs(" |\n", f);
    }
    for (Linha *l = s->inicio; l != NULL; l = l->prox)
        linhaTabela(f, s->larg, l->col, s->ncols);

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

/* ---------------------------------------------------------------------
 * Buscas (juncao entre as tabelas pelas chaves estrangeiras)
 * --------------------------------------------------------------------- */

static const Aluno *buscarAluno(const Base *b, int id)
{
    for (const Aluno *a = b->alunos; a != NULL; a = a->prox)
        if (a->id_aluno == id) return a;
    return NULL;
}

static const Curso *buscarCurso(const Base *b, int id)
{
    for (const Curso *c = b->cursos; c != NULL; c = c->prox)
        if (c->id_curso == id) return c;
    return NULL;
}

static const Departamento *buscarDepartamento(const Base *b, int id)
{
    for (const Departamento *d = b->departamentos; d != NULL; d = d->prox)
        if (d->id_departamento == id) return d;
    return NULL;
}

static const Disciplina *buscarDisciplina(const Base *b, int id)
{
    for (const Disciplina *d = b->disciplinas; d != NULL; d = d->prox)
        if (d->id_disciplina == id) return d;
    return NULL;
}

static FILE *abrirSaida(const char *arquivo)
{
    FILE *f = fopen(arquivo, "w");
    if (f == NULL)
        perror(arquivo);
    return f;
}

/* ---------------------------------------------------------------------
 * Relatorio 1: Ficha do aluno
 *   Area "Identificacao"      -> secao "Dados pessoais"
 *   Area "Vinculo academico"  -> secao "Curso e departamento"
 * --------------------------------------------------------------------- */
int relatorioFichaAluno(const Base *b, int id_aluno, const char *arquivo)
{
    const Aluno *a = buscarAluno(b, id_aluno);
    if (a == NULL) {
        printf("Aluno %d nao encontrado.\n", id_aluno);
        return -1;
    }
    const Curso *c = buscarCurso(b, a->id_curso);
    const Departamento *d = (c != NULL) ? buscarDepartamento(b, c->id_departamento) : NULL;

    FILE *f = abrirSaida(arquivo);
    if (f == NULL)
        return -1;

    const char *titulos[] = {"Campo", "Valor"};
    const int larg[] = {16, 44};
    Secao s;

    cabecalhoRelatorio(f, "RELATORIO: FICHA DO ALUNO");

    fprintf(f, "AREA: Identificacao\n");
    secaoIniciar(&s, 2, titulos, larg);
    secaoAdd(&s, "Id do aluno", num(a->id_aluno));
    secaoAdd(&s, "Nome", a->nome);
    secaoAdd(&s, "E-mail", a->email);
    secaoAdd(&s, "Nascimento", a->data_nascimento);
    secaoEscrever(f, &s, "Secao: Dados pessoais");
    secaoLiberar(&s);

    fprintf(f, "AREA: Vinculo academico\n");
    secaoIniciar(&s, 2, titulos, larg);
    secaoAdd(&s, "Curso", c ? c->nome : "(nao encontrado)");
    secaoAdd(&s, "Carga horaria", c ? num(c->carga_horaria_total) : "-");
    secaoAdd(&s, "Departamento", d ? d->nome : "(nao encontrado)");
    secaoAdd(&s, "Sigla", d ? d->sigla : "-");
    secaoEscrever(f, &s, "Secao: Curso e departamento");
    secaoLiberar(&s);

    fclose(f);
    return 0;
}

/* ---------------------------------------------------------------------
 * Relatorio 2: Cursos por departamento
 *   Uma area por departamento -> secao "Cursos oferecidos"
 * --------------------------------------------------------------------- */
int relatorioCursosPorDepartamento(const Base *b, const char *arquivo)
{
    FILE *f = abrirSaida(arquivo);
    if (f == NULL)
        return -1;

    const char *titulos[] = {"Id curso", "Curso", "Carga horaria"};
    const int larg[] = {8, 40, 13};

    cabecalhoRelatorio(f, "RELATORIO: CURSOS POR DEPARTAMENTO");

    for (const Departamento *d = b->departamentos; d != NULL; d = d->prox) {
        Secao s;
        secaoIniciar(&s, 3, titulos, larg);

        for (const Curso *c = b->cursos; c != NULL; c = c->prox)
            if (c->id_departamento == d->id_departamento)
                secaoAdd(&s, num(c->id_curso), c->nome, num(c->carga_horaria_total));

        fprintf(f, "AREA: Departamento %s (%s)\n", d->nome, d->sigla);
        secaoEscrever(f, &s, "Secao: Cursos oferecidos");
        secaoLiberar(&s);
    }

    fclose(f);
    return 0;
}

/* ---------------------------------------------------------------------
 * Relatorio 3: Disciplinas por curso
 *   Uma area por curso -> secao "Disciplinas do curso"
 *   (usa a tabela de ligacao curso_disciplina)
 * --------------------------------------------------------------------- */
int relatorioDisciplinasPorCurso(const Base *b, const char *arquivo)
{
    FILE *f = abrirSaida(arquivo);
    if (f == NULL)
        return -1;

    const char *titulos[] = {"Id disc.", "Disciplina", "Carga horaria"};
    const int larg[] = {8, 40, 13};

    cabecalhoRelatorio(f, "RELATORIO: DISCIPLINAS POR CURSO");

    for (const Curso *c = b->cursos; c != NULL; c = c->prox) {
        Secao s;
        secaoIniciar(&s, 3, titulos, larg);

        for (const CursoDisciplina *cd = b->cursos_disciplinas; cd != NULL; cd = cd->prox) {
            if (cd->id_curso != c->id_curso)
                continue;
            const Disciplina *disc = buscarDisciplina(b, cd->id_disciplina);
            if (disc != NULL)
                secaoAdd(&s, num(disc->id_disciplina), disc->nome, num(disc->carga_horaria));
            else
                secaoAdd(&s, num(cd->id_disciplina), "(disciplina nao cadastrada)", "-");
        }

        fprintf(f, "AREA: Curso %s (id %d)\n", c->nome, c->id_curso);
        secaoEscrever(f, &s, "Secao: Disciplinas do curso");
        secaoLiberar(&s);
    }

    fclose(f);
    return 0;
}

/* ---------------------------------------------------------------------
 * Relatorio 4: Turmas por disciplina
 *   Uma area por disciplina -> secao "Turmas ofertadas"
 * --------------------------------------------------------------------- */
int relatorioTurmasPorDisciplina(const Base *b, const char *arquivo)
{
    FILE *f = abrirSaida(arquivo);
    if (f == NULL)
        return -1;

    const char *titulos[] = {"Id turma", "Sala", "Periodo"};
    const int larg[] = {8, 20, 10};

    cabecalhoRelatorio(f, "RELATORIO: TURMAS POR DISCIPLINA");

    for (const Disciplina *d = b->disciplinas; d != NULL; d = d->prox) {
        Secao s;
        secaoIniciar(&s, 3, titulos, larg);

        for (const Turma *t = b->turmas; t != NULL; t = t->prox)
            if (t->id_disciplina == d->id_disciplina)
                secaoAdd(&s, num(t->id_turma), t->sala, t->periodo);

        fprintf(f, "AREA: Disciplina %s (id %d)\n", d->nome, d->id_disciplina);
        secaoEscrever(f, &s, "Secao: Turmas ofertadas");
        secaoLiberar(&s);
    }

    fclose(f);
    return 0;
}
