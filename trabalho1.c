#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#define TAM 1000
//lista encadeada = vamos guardar dados e o endereço de memoria do proximo local para guardar dadoa
typedef struct nodepartamento { //com no para transformar em no e usar o sizeof
int iddepartamento; // o id de cada um pra saber de qual(aluno) se trata
char nomedepartamento[30];
char sigladepartamento[5];
struct nodepartamento *proximo; // aqui é onde vai guardar o endereço de memoria do proximo
} departamento;
void lerdepartamento(departamento *novo)
{
    int c;
    puts("Insira o nome do departamento:");
    scanf("%29[^\n]", novo->nomedepartamento);
    while ((c = getchar()) != '\n' && c != EOF);   // limpa o \n e qualquer sobra, de verdade

    puts("Insira a sigla do departamento:");
    scanf("%4[^\n]", novo->sigladepartamento);
    while ((c = getchar()) != '\n' && c != EOF);
    
    return;
}
void inseredepartamento(departamento**pnovo) // vai criar os nos // 1 * passa valor, 2* passa endereço e substitui o & no parametro(nao se passa esse por parametro)
{   
    departamento *novo ; // so precisa usar o * para tratar como ponteiro nessa declaração
    novo = malloc(sizeof(departamento));  
    if(novo == NULL) { //verificação
        puts("Erro ao inserir Departamento. Erro de Memória");
        return;
    }
    novo -> proximo = *pnovo; //encadeia,o novo nó passa a apontar para quem era o primeiro até agora
    *pnovo = novo; // agora o novo nó passa a ser o primeiro da lista
    lerdepartamento(novo);
    
    // novo -> proximo = NULL;
    return;
}
int main()
{
    setlocale(LC_ALL,"Portuguese");
    // file *arquivo; aqui é generico, precisa do nome do arquivo especifico?
    departamento *listadept = NULL;
    for(int i=0; i < 2;i++){
        inseredepartamento(&listadept);
         printf("%s\n",listadept->nomedepartamento);
        printf("%s\n",listadept->sigladepartamento);
    }

    // scanf("");
    free(listadept);
    return 0;
}