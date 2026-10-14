#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

/* ---------------- Aluno ---------------- */

void inserirAluno(Aluno **lista, int id_aluno, const char *nome,
                  const char *email, const char *data_nascimento, int id_curso)
{
    Aluno *novo = malloc(sizeof(Aluno));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir Aluno: memoria insuficiente.\n");
        return;
    }

    novo->id_aluno = id_aluno;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    snprintf(novo->email, sizeof novo->email, "%s", email);
    snprintf(novo->data_nascimento, sizeof novo->data_nascimento, "%s", data_nascimento);
    novo->id_curso = id_curso;
    novo->prox = NULL;

    /* insere no final da lista */
    Aluno **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarAlunos(Aluno *lista)
{
    while (lista != NULL) {
        Aluno *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}

/* ---------------- Departamento ---------------- */

void inserirDepartamento(Departamento **lista, int id_departamento,
                         const char *nome, const char *sigla)
{
    Departamento *novo = malloc(sizeof(Departamento));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao inserir Departamento: memoria insuficiente.\n");
        return;
    }

    novo->id_departamento = id_departamento;
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    snprintf(novo->sigla, sizeof novo->sigla, "%s", sigla);
    novo->prox = NULL;

    Departamento **p = lista;
    while (*p != NULL)
        p = &(*p)->prox;
    *p = novo;
}

void liberarDepartamentos(Departamento *lista)
{
    while (lista != NULL) {
        Departamento *prox = lista->prox;
        free(lista);
        lista = prox;
    }
}
