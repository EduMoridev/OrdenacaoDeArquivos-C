#ifdef _WIN32
#include <direct.h>                 /* _getcwd no Windows */
#define PEGAR_PASTA _getcwd
#else
#define _POSIX_C_SOURCE 200112L     /* libera getcwd no Linux/Mac */
#include <unistd.h>
#define PEGAR_PASTA getcwd
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define MAX          1000            /* capacidade maxima do vetor     */
#define ARQ_DADOS    "dados.txt"     /* arquivo de entrada (original)  */
#define ARQ_ORDENADO "ordenado.txt"  /* arquivo de saida (ordenado)    */

/* ------------------------- Prototipos ------------------------------ */
void limparBuffer(void);
int  lerInteiro(const char *msg, int min, int max);

void mostrarCaminho(const char *nome);
int  gravarArquivo(const char *nome, const int v[], int n);
int  lerArquivo(const char *nome, int v[], int max);
void exibirVetor(const char *titulo, const int v[], int n);

void trocar(int *a, int *b);
void bubbleSort(int v[], int n);
void insertionSort(int v[], int n);
void selectionSort(int v[], int n);
void mergeSort(int v[], int inicio, int ultimo);
void intercalar(int v[], int inicio, int meio, int ultimo);
void quickSort(int v[], int inicio, int ultimo);
int  particionar(int v[], int inicio, int ultimo);

const char *escolherEOrdenar(int v[], int n);
void opcaoCriarDados(void);
void opcaoExibirArquivo(const char *nome);
void opcaoOrdenar(void);
void opcaoAlterarValor(void);

/* =================================================================== */
/*                     ENTRADA DE DADOS (TECLADO)                      */
/* =================================================================== */

/* Descarta o que sobrou na linha digitada (ate o '\n'). */
void limparBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Le um inteiro entre min e max, repetindo ate o usuario acertar. */
int lerInteiro(const char *msg, int min, int max)
{
    int x;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &x) == 1 && x >= min && x <= max) {
            limparBuffer();
            return x;
        }
        if (feof(stdin)) {               /* entrada encerrada: evita loop */
            printf("\nFim da entrada. Encerrando.\n");
            exit(0);
        }
        printf("  Valor invalido! Digite um inteiro entre %d e %d.\n", min, max);
        limparBuffer();
    }
}

/* =================================================================== */
/*                       MANIPULACAO DE ARQUIVOS                       */
/* =================================================================== */

/* Mostra o caminho completo do arquivo, para o usuario saber
   em qual pasta ele foi criado. */
void mostrarCaminho(const char *nome)
{
    char pasta[1024];
    if (PEGAR_PASTA(pasta, sizeof(pasta)) != NULL)
        printf("  -> %s salvo em: %s\n", nome, pasta);
    else
        printf("  -> %s salvo na pasta atual do programa.\n", nome);
}

/* Grava n valores do vetor no arquivo (um por linha).
   Retorna 1 em caso de sucesso e 0 em caso de erro. */
int gravarArquivo(const char *nome, const int v[], int n)
{
    FILE *f;
    int i;

    f = fopen(nome, "w");               /* "w" cria ou sobrescreve */
    if (f == NULL) {
        printf("Erro: nao foi possivel abrir %s para escrita.\n", nome);
        return 0;
    }
    for (i = 0; i < n; i++)
        fprintf(f, "%d\n", v[i]);
    fclose(f);
    mostrarCaminho(nome);
    return 1;
}

/* Le ate 'max' inteiros do arquivo para o vetor.
   Retorna a quantidade lida, ou -1 se o arquivo nao existir. */
int lerArquivo(const char *nome, int v[], int max)
{
    FILE *f;
    int n = 0;

    f = fopen(nome, "r");
    if (f == NULL)
        return -1;
    while (n < max && fscanf(f, "%d", &v[n]) == 1)
        n++;
    fclose(f);
    return n;
}

/* Mostra o vetor na tela, 10 valores por linha. */
void exibirVetor(const char *titulo, const int v[], int n)
{
    int i;
    printf("%s (%d valores):\n", titulo, n);
    for (i = 0; i < n; i++) {
        printf("%6d", v[i]);
        if ((i + 1) % 10 == 0 || i == n - 1)
            printf("\n");
    }
}

/* =================================================================== */
/*                      ALGORITMOS DE ORDENACAO                        */
/*            (todos ordenam em ordem CRESCENTE, no proprio vetor)     */
/* =================================================================== */

void trocar(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

/* Bubble Sort: compara vizinhos e "empurra" o maior para o fim.
   Para cedo se uma passada inteira nao fizer nenhuma troca. */
void bubbleSort(int v[], int n)
{
    int i, j, houveTroca;
    for (i = 0; i < n - 1; i++) {
        houveTroca = 0;
        for (j = 0; j < n - 1 - i; j++) {
            if (v[j] > v[j + 1]) {
                trocar(&v[j], &v[j + 1]);
                houveTroca = 1;
            }
        }
        if (!houveTroca)
            break;
    }
}

/* Insertion Sort: insere cada elemento (chave) na posicao correta
   da parte ja ordenada, deslocando os maiores para a direita. */
void insertionSort(int v[], int n)
{
    int i, j, chave;
    for (i = 1; i < n; i++) {
        chave = v[i];
        j = i - 1;
        while (j >= 0 && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

/* Selection Sort: a cada passo seleciona o menor elemento restante
   e o coloca na posicao i. */
void selectionSort(int v[], int n)
{
    int i, j, menor;
    for (i = 0; i < n - 1; i++) {
        menor = i;
        for (j = i + 1; j < n; j++) {
            if (v[j] < v[menor])
                menor = j;
        }
        if (menor != i)
            trocar(&v[i], &v[menor]);
    }
}

/* Merge Sort: divide o vetor ao meio recursivamente e depois
   intercala (junta) as duas metades ja ordenadas. */
void mergeSort(int v[], int inicio, int ultimo)
{
    int meio;
    if (inicio < ultimo) {
        meio = (inicio + ultimo) / 2;
        mergeSort(v, inicio, meio);
        mergeSort(v, meio + 1, ultimo);
        intercalar(v, inicio, meio, ultimo);
    }
}

/* Junta v[inicio..meio] e v[meio+1..ultimo] (ambos ordenados). */
void intercalar(int v[], int inicio, int meio, int ultimo)
{
    int aux[MAX];
    int i = inicio, j = meio + 1, k = 0;

    while (i <= meio && j <= ultimo) {
        if (v[i] <= v[j])
            aux[k++] = v[i++];
        else
            aux[k++] = v[j++];
    }
    while (i <= meio)                   /* sobras da metade esquerda */
        aux[k++] = v[i++];
    while (j <= ultimo)                    /* sobras da metade direita  */
        aux[k++] = v[j++];

    for (k = 0; k <= ultimo - inicio; k++) /* copia de volta para v     */
        v[inicio + k] = aux[k];
}

/* Quick Sort: escolhe um valor de referencia (o "pivo"), separa os
   menores a esquerda e os maiores a direita (particionar) e ordena
   cada lado recursivamente. */
void quickSort(int v[], int inicio, int ultimo)
{
    int p;
    if (inicio < ultimo) {
        p = particionar(v, inicio, ultimo);
        quickSort(v, inicio, p - 1);
        quickSort(v, p + 1, ultimo);
    }
}

/* Particionamento de Lomuto: a referencia (pivo) e o ultimo elemento
   do trecho. 'i' marca o fim da area dos menores-ou-iguais a ela.
   Retorna a posicao final da referencia. */
int particionar(int v[], int inicio, int ultimo)
{
    int referencia = v[ultimo];
    int i = inicio - 1;
    int j;

    for (j = inicio; j < ultimo; j++) {
        if (v[j] <= referencia) {
            i++;
            trocar(&v[i], &v[j]);
        }
    }
    trocar(&v[i + 1], &v[ultimo]);
    return i + 1;
}

/* Mostra o menu de algoritmos, ordena o vetor e devolve o nome usado. */
const char *escolherEOrdenar(int v[], int n)
{
    int op;
    printf("\n  Algoritmos de ordenacao:\n");
    printf("   1 - Bubble Sort\n");
    printf("   2 - Insertion Sort\n");
    printf("   3 - Selection Sort\n");
    printf("   4 - Merge Sort\n");
    printf("   5 - Quick Sort\n");
    op = lerInteiro("  Escolha o algoritmo: ", 1, 5);

    switch (op) {
        case 1:  bubbleSort(v, n);        return "Bubble Sort";
        case 2:  insertionSort(v, n);     return "Insertion Sort";
        case 3:  selectionSort(v, n);     return "Selection Sort";
        case 4:  mergeSort(v, 0, n - 1);  return "Merge Sort";
        default: quickSort(v, 0, n - 1);  return "Quick Sort";
    }
}

/* =================================================================== */
/*                         OPCOES DO MENU                              */
/* =================================================================== */

/* Opcao 1: cria dados.txt com valores digitados ou aleatorios. */
void opcaoCriarDados(void)
{
    int v[MAX], n, i, modo;
    char msg[40];

    n = lerInteiro("Quantos valores deseja gravar (1 a 1000)? ", 1, MAX);
    modo = lerInteiro("1 - Digitar os valores | 2 - Gerar aleatorios: ", 1, 2);

    if (modo == 1) {
        for (i = 0; i < n; i++) {
            sprintf(msg, "  Valor %d: ", i + 1);
            v[i] = lerInteiro(msg, INT_MIN, INT_MAX);
        }
    } else {
        for (i = 0; i < n; i++)
            v[i] = rand() % 1000;       /* valores de 0 a 999 */
    }

    if (gravarArquivo(ARQ_DADOS, v, n)) {
        printf("\nArquivo %s gravado com sucesso.\n", ARQ_DADOS);
        exibirVetor("Dados gravados", v, n);
    }
}

/* Opcoes 2 e 5: le um arquivo e mostra seu conteudo. */
void opcaoExibirArquivo(const char *nome)
{
    int v[MAX], n;

    n = lerArquivo(nome, v, MAX);
    if (n < 0) {
        printf("Arquivo %s nao existe ainda.\n", nome);
        return;
    }
    if (n == 0) {
        printf("Arquivo %s esta vazio.\n", nome);
        return;
    }
    printf("\nConteudo de %s\n", nome);
    exibirVetor("Valores", v, n);
}

/* Opcao 3: le dados.txt, ordena e grava em ordenado.txt.
   O arquivo dados.txt continua com os valores originais. */
void opcaoOrdenar(void)
{
    int v[MAX], n;
    const char *algoritmo;

    n = lerArquivo(ARQ_DADOS, v, MAX);
    if (n <= 0) {
        printf("dados.txt nao existe ou esta vazio. Use a opcao 1 primeiro.\n");
        return;
    }

    exibirVetor("\nAntes da ordenacao", v, n);
    algoritmo = escolherEOrdenar(v, n);
    exibirVetor("\nDepois da ordenacao", v, n);

    if (gravarArquivo(ARQ_ORDENADO, v, n))
        printf("Resultado (%s) salvo em %s.\n", algoritmo, ARQ_ORDENADO);
}

/* Opcao 4: altera um valor de dados.txt (pela posicao), regrava
   o arquivo e reordena, atualizando ordenado.txt. */
void opcaoAlterarValor(void)
{
    int v[MAX], n, pos, novo, i;

    n = lerArquivo(ARQ_DADOS, v, MAX);
    if (n <= 0) {
        printf("dados.txt nao existe ou esta vazio. Use a opcao 1 primeiro.\n");
        return;
    }

    printf("\nValores atuais de %s:\n", ARQ_DADOS);
    for (i = 0; i < n; i++)
        printf("  [%d] %d\n", i + 1, v[i]);

    pos  = lerInteiro("Posicao do valor a alterar: ", 1, n);
    novo = lerInteiro("Novo valor: ", INT_MIN, INT_MAX);

    printf("Posicao %d: %d -> %d\n", pos, v[pos - 1], novo);
    v[pos - 1] = novo;

    if (!gravarArquivo(ARQ_DADOS, v, n))
        return;
    printf("%s atualizado.\n", ARQ_DADOS);

    /* reordena com os dados alterados e regrava ordenado.txt */
    opcaoOrdenar();
}

/* =================================================================== */
/*                               MAIN                                  */
/* =================================================================== */
int main(void)
{
    int op;

    srand((unsigned) time(NULL));       /* semente dos aleatorios */

    do {
        printf("\n========== ORDENACAO + ARQUIVOS ==========\n");
        printf(" 1 - Criar/gravar dados.txt\n");
        printf(" 2 - Exibir dados.txt\n");
        printf(" 3 - Ordenar dados.txt e salvar em ordenado.txt\n");
        printf(" 4 - Alterar um valor em dados.txt e reordenar\n");
        printf(" 5 - Exibir ordenado.txt\n");
        printf(" 0 - Sair\n");
        op = lerInteiro("Opcao: ", 0, 5);

        switch (op) {
            case 1: opcaoCriarDados();                 break;
            case 2: opcaoExibirArquivo(ARQ_DADOS);     break;
            case 3: opcaoOrdenar();                    break;
            case 4: opcaoAlterarValor();               break;
            case 5: opcaoExibirArquivo(ARQ_ORDENADO);  break;
            case 0: printf("Encerrando o programa.\n"); break;
        }
    } while (op != 0);

    return 0;
}
