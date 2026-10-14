#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

/* ---------------- Curso ---------------- */

void inserirCurso(Curso **lista, int id_curso, const char *nome,
                  int carga_horaria_total, int id_departamento)
{
    Curso *novo = malloc(sizeof(Curso));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir Curso: memoria insuficiente.\n");
        return;
    }

    novo->id_curso = id_curso;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    novo->carga_horaria_total = carga_horaria_total;
    novo->id_departamento = id_departamento;
    novo->prox = NULL;

    Curso **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarCursos(Curso *lista)
{
    while (lista != NULL) {
        Curso *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}

/* ------- CursoDisciplina (relacao N:N Curso x Disciplina) ------- */

void inserirCursoDisciplina(CursoDisciplina **lista, int id_curso,
                            int id_disciplina)
{
    CursoDisciplina *novo = malloc(sizeof(CursoDisciplina));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir CursoDisciplina: memoria insuficiente.\n");
        return;
    }

    novo->id_curso = id_curso;
    novo->id_disciplina = id_disciplina;
    novo->prox = NULL;

    CursoDisciplina **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarCursosDisciplinas(CursoDisciplina *lista)
{
    while (lista != NULL) {
        CursoDisciplina *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}
