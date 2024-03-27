#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void exibe_menu(){

    printf("1) Sumário do Arquivo\n");
    printf("2) Mostrar\n");
    printf("3) Fim\n");
}

    
void exibe_sumario (char* arquivo){
    FILE *file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char buffer[1024];
    int num_fields = 0;
    int tam;

    // Determinar numero de campos analizando a primeira linha
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ",");
        while (token != NULL) {
            num_fields++;
            token = strtok(NULL, ",");
        }
    } else {
        printf("Arquivo vazio.\n");
        fclose(file);
        return;
    }

    // Analizando a segunda linha para verificar se eh Numeric ou String, armazenando em um vetor    
    char* v[num_fields];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ",");
        int i = 0;
        while (token != NULL) {
            tam = strlen(token);

            if (token[tam-1] == '\n')
                token[tam-1] = '\0';

            if (atof(token) == 0){
                v[i] = "S";
            } else {
                v[i] = "N";
            }
            token = strtok(NULL, ",");
            i++;
        }
    }

    // Retorna ao início para printar junto com o vetor
    fseek(file, 0, SEEK_SET);

    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ",");
        int i = 0;
        while (token != NULL) {
            tam = strlen(token);

            if (token[tam-1] == '\n')
                token[tam-1] = '\0';

            printf("%s [%s]\n", token, v[i]);
            token = strtok(NULL, ",");
            i++;
        }
    }

    fclose(file);

    printf("%d variaveis encontradas\n", num_fields);
    printf("\nPressione ENTER para continuar\n");
    getchar(); // Aguarda o usuário pressionar ENTER
}
          

void mostra (char *arquivo){
    FILE *file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char buffer[1024];
    int count = 0;

    // Imprime a linhas dos campos    
    if (fgets(buffer, sizeof(buffer), file) != NULL){
        char* token = strtok(buffer, ",");
        while (token != NULL){
            printf("\t%-20s", token);
            token = strtok( NULL, ",");
        }
        printf("\n");
    }        

   
    // Imprimir as 5 primeiras linhas de dados
    while (count < 5 && fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%d", count);
        char *token = strtok(buffer, ",");
        while (token != NULL) {
            if (strlen(token) == 0){ 
                printf("\tNaN");
            }
            else
                printf("\t%-20s", token);
            token = strtok(NULL, ",");
        }
        printf("\n");
        count++;
    }


/*    while (count < 5 && fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%d", count);

        // Variável para armazenar o início de cada campo
        char *start = buffer;

        // Percorre cada caractere da linha
        for (int i = 0; buffer[i] != '\0'; i++) {
            // Se encontrar uma vírgula ou o final da linha
            if (buffer[i] == ',' || buffer[i] == '\n') {
                // Substitui a vírgula por terminador de string
                buffer[i] = '\0';
                
                // Verifica se o campo é vazio e imprime "NaN" se for
                if (start == buffer + i) {
                    printf("\t%-20s", "NaN");
                } else {
                    printf("\t%-20s", start);
                }

                // Avança para o próximo campo
                start = buffer + i + 1;
            }
            
        }

        printf("\n");
        count++;
    }
*/


    // Voltar para o início do arquivo e contar as linhas
    fseek(file, 0, SEEK_SET);
    (fgets(buffer, sizeof(buffer), file) != NULL); 
    int num_lines = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        num_lines++;
    }

       
    // Voltar para o início do arquivo e contar as colunas
    fseek(file, 0, SEEK_SET);
    int num_fields = 0;
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ",");
        while (token != NULL) {
            num_fields++;
            token = strtok(NULL, ",");
        }
    } 
    
    if (num_lines >= 10){
        for(int i = 0; i < num_fields; i++)
            printf("\t%-20s", "...");
        printf("\n");
    }
 
    // Voltar para o iníco e desconsidera a linha de campos
    fseek(file, 0, SEEK_SET);
    (fgets(buffer, sizeof(buffer), file) != NULL); 
    
    // Pular as primeiras linhas até num_lines - 5
    for (int i = 0; i < num_lines - 5; i++) {
        fgets(buffer, sizeof(buffer), file);
    }

    // Imprimir os últimos 5 registros
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (num_lines - count >= 5){
            printf("%d ", num_lines-count);
            char *token = strtok(buffer, ",");
            while (token != NULL) {
                printf("\t%-20s", token);
                token = strtok(NULL, ",");
            }
            printf("\n");
        }
        count--;
    }

    fclose(file);

    printf("[%d rows x %d columns]\n", num_lines, num_fields); 
    printf("Pressione ENTER para continuar\n");
    getchar();
}

