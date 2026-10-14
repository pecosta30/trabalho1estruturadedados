#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

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

static void liberarBase(Base *b)
{
    liberarDepartamentos(b->departamentos);
    liberarCursos(b->cursos);
    liberarAlunos(b->alunos);
    liberarDisciplinas(b->disciplinas);
    liberarTurmas(b->turmas);
    liberarCursosDisciplinas(b->cursos_disciplinas);
    memset(b, 0, sizeof *b);
}

int main(void)
{
    Base base;
    char pasta[200];
    char arq[200];
    int opcao;

    memset(&base, 0, sizeof base);

    /* Escolha dos arquivos: pasta onde estao os .txt */
    lerTexto("Pasta com os arquivos .txt [dados]: ", pasta, sizeof pasta, "dados");
    if (carregarTudo(&base, pasta) != 0) {
        printf("Nao foi possivel abrir todos os arquivos da pasta '%s'.\n", pasta);
        liberarBase(&base);
        return 1;
    }

    do {
        printf("\n===== GERADOR DE RELATORIOS - UNIVERSIDADE DISCENTE =====\n");
        printf("1 - Ficha do aluno\n");
        printf("2 - Cursos por departamento\n");
        printf("3 - Disciplinas por curso\n");
        printf("4 - Turmas por disciplina\n");
        printf("0 - Sair\n");
        opcao = lerInteiro("Escolha o relatorio: ");

        switch (opcao) {
        case 1: {
            int id = lerInteiro("Id do aluno: ");
            lerTexto("Arquivo de saida [relatorio_ficha_aluno.txt]: ", arq, sizeof arq,
                     "relatorio_ficha_aluno.txt");
            if (relatorioFichaAluno(&base, id, arq) == 0)
                printf("Relatorio gerado em %s\n", arq);
            break;
        }
        case 2:
            lerTexto("Arquivo de saida [relatorio_cursos_departamento.txt]: ", arq, sizeof arq,
                     "relatorio_cursos_departamento.txt");
            if (relatorioCursosPorDepartamento(&base, arq) == 0)
                printf("Relatorio gerado em %s\n", arq);
            break;
        case 3:
            lerTexto("Arquivo de saida [relatorio_disciplinas_curso.txt]: ", arq, sizeof arq,
                     "relatorio_disciplinas_curso.txt");
            if (relatorioDisciplinasPorCurso(&base, arq) == 0)
                printf("Relatorio gerado em %s\n", arq);
            break;
        case 4:
            lerTexto("Arquivo de saida [relatorio_turmas_disciplina.txt]: ", arq, sizeof arq,
                     "relatorio_turmas_disciplina.txt");
            if (relatorioTurmasPorDisciplina(&base, arq) == 0)
                printf("Relatorio gerado em %s\n", arq);
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberarBase(&base);
    return 0;
}
