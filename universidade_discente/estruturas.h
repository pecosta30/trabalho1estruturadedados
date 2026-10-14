#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

/* =====================================================================
 * Universidade Discente - estruturas e funcoes compartilhadas
 *
 * Cada tabela do modelo ER e uma lista encadeada dinamica.
 *   aluno.c      : Aluno e Departamento
 *   curso.c      : Curso e CursoDisciplina (relacao N:N)
 *   disciplina.c : Disciplina e Turma
 *   leitura.c    : le os .txt e chama as funcoes inserirX
 *   relatorio.c  : monta as listas de cada area e gera o relatorio TXT
 *   main.c       : menu do programa
 *
 * Regras: inserirX copia os textos (snprintf) e insere no FINAL da lista,
 * mantendo a ordem do arquivo .txt.
 * ===================================================================== */

#define TAM_NOME     100
#define TAM_EMAIL    100
#define TAM_DATA      11   /* dd/mm/aaaa */
#define TAM_SIGLA     10
#define TAM_SALA      20
#define TAM_PERIODO   10   /* ex.: 2026.1 */

/* ---------- Estruturas (um no de lista por registro) ---------- */

typedef struct Departamento {
    int  id_departamento;
    char nome[TAM_NOME];
    char sigla[TAM_SIGLA];
    struct Departamento *prox;
} Departamento;

typedef struct Curso {
    int  id_curso;
    char nome[TAM_NOME];
    int  carga_horaria_total;
    int  id_departamento;          /* FK -> Departamento */
    struct Curso *prox;
} Curso;

typedef struct Aluno {
    int  id_aluno;
    char nome[TAM_NOME];
    char email[TAM_EMAIL];
    char data_nascimento[TAM_DATA];
    int  id_curso;                 /* FK -> Curso */
    struct Aluno *prox;
} Aluno;

typedef struct Disciplina {
    int  id_disciplina;
    char nome[TAM_NOME];
    int  carga_horaria;
    struct Disciplina *prox;
} Disciplina;

typedef struct Turma {
    int  id_turma;
    char sala[TAM_SALA];
    char periodo[TAM_PERIODO];
    int  id_disciplina;            /* FK -> Disciplina */
    struct Turma *prox;
} Turma;

/* Tabela de ligacao da relacao N:N "Curso possui Disciplina" */
typedef struct CursoDisciplina {
    int id_curso;                  /* FK -> Curso */
    int id_disciplina;             /* FK -> Disciplina */
    struct CursoDisciplina *prox;
} CursoDisciplina;

/* Agrupa as 6 listas para passar tudo junto entre as partes do programa */
typedef struct {
    Departamento    *departamentos;
    Curso           *cursos;
    Aluno           *alunos;
    Disciplina      *disciplinas;
    Turma           *turmas;
    CursoDisciplina *cursos_disciplinas;
} Base;

/* ---------- aluno.c ---------- */
void inserirAluno(Aluno **lista, int id_aluno, const char *nome,
                  const char *email, const char *data_nascimento, int id_curso);
void liberarAlunos(Aluno *lista);

void inserirDepartamento(Departamento **lista, int id_departamento,
                         const char *nome, const char *sigla);
void liberarDepartamentos(Departamento *lista);

/* ---------- curso.c ---------- */
void inserirCurso(Curso **lista, int id_curso, const char *nome,
                  int carga_horaria_total, int id_departamento);
void liberarCursos(Curso *lista);

void inserirCursoDisciplina(CursoDisciplina **lista, int id_curso,
                            int id_disciplina);
void liberarCursosDisciplinas(CursoDisciplina *lista);

/* ---------- disciplina.c ---------- */
void inserirDisciplina(Disciplina **lista, int id_disciplina,
                       const char *nome, int carga_horaria);
void liberarDisciplinas(Disciplina *lista);

void inserirTurma(Turma **lista, int id_turma, const char *sala,
                  const char *periodo, int id_disciplina);
void liberarTurmas(Turma *lista);

/* ---------- leitura.c ---------- */

/* Le os 6 .txt da pasta indicada e preenche as listas de 'base'.
 * Retorna 0 se todos os arquivos foram abertos, -1 se algum falhou. */
int carregarTudo(Base *base, const char *pasta);

/* ---------- relatorio.c ----------
 * Cada funcao monta listas dinamicas (uma por secao) e grava o relatorio
 * em 'arquivo'. Retornam 0 se deu certo, -1 em caso de erro. */
int relatorioFichaAluno(const Base *base, int id_aluno, const char *arquivo);
int relatorioCursosPorDepartamento(const Base *base, const char *arquivo);
int relatorioDisciplinasPorCurso(const Base *base, const char *arquivo);
int relatorioTurmasPorDisciplina(const Base *base, const char *arquivo);

#endif /* ESTRUTURAS_H */
