#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

#define SEPARADOR ';'
#define MAX_CAMPOS 8
#define TAM_LINHA 512

/* Cada tratador recebe a lista (como void*) e os campos ja separados,
 * converte o que precisa (atoi) e chama a funcao inserirX da entidade. */
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

/* ---------- Tratadores: um por arquivo ---------- */

static void trataDepartamento(void *lista, char *c[])
{
    inserirDepartamento((Departamento **)lista, atoi(c[0]), c[1], c[2]);
}

static void trataCurso(void *lista, char *c[])
{
    inserirCurso((Curso **)lista, atoi(c[0]), c[1], atoi(c[2]), atoi(c[3]));
}

static void trataAluno(void *lista, char *c[])
{
    inserirAluno((Aluno **)lista, atoi(c[0]), c[1], c[2], c[3], atoi(c[4]));
}

static void trataDisciplina(void *lista, char *c[])
{
    inserirDisciplina((Disciplina **)lista, atoi(c[0]), c[1], atoi(c[2]));
}

static void trataTurma(void *lista, char *c[])
{
    inserirTurma((Turma **)lista, atoi(c[0]), c[1], c[2], atoi(c[3]));
}

static void trataCursoDisciplina(void *lista, char *c[])
{
    inserirCursoDisciplina((CursoDisciplina **)lista, atoi(c[0]), atoi(c[1]));
}

/* ---------- Funcao publica ---------- */

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
