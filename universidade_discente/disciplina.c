#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

/* ---------------- Disciplina ---------------- */

void inserirDisciplina(Disciplina **lista, int id_disciplina,
                       const char *nome, int carga_horaria)
{
    Disciplina *novo = malloc(sizeof(Disciplina));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir Disciplina: memoria insuficiente.\n");
        return;
    }

    novo->id_disciplina = id_disciplina;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    novo->carga_horaria = carga_horaria;
    novo->prox = NULL;

    Disciplina **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarDisciplinas(Disciplina *lista)
{
    while (lista != NULL) {
        Disciplina *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}

/* ---------------- Turma ---------------- */

void inserirTurma(Turma **lista, int id_turma, const char *sala,
                  const char *periodo, int id_disciplina)
{
    Turma *novo = malloc(sizeof(Turma));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir Turma: memoria insuficiente.\n");
        return;
    }

    novo->id_turma = id_turma;
    snprintf(novo->sala, sizeof novo->sala, "%s", sala);
    snprintf(novo->periodo, sizeof novo->periodo, "%s", periodo);
    novo->id_disciplina = id_disciplina;
    novo->prox = NULL;

    Turma **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarTurmas(Turma *lista)
{
    while (lista != NULL) {
        Turma *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}
