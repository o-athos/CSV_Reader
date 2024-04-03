#include "io.h"

void menu(){

    printf("1) Sumário do Arquivo\n");
    printf("2) Mostrar\n");
    printf("3) Fim\n");
}

struct arq_csv* open_csv (char *path){
    
    struct arq_csv* csv = malloc(sizeof (struct arq_csv));
    if (csv == NULL){
        fprintf(stderr, "Erro alocação");
        exit(EXIT_FAILURE);
    }

    csv->arq = fopen(path, "r");
    if (csv->arq == NULL){
        fclose(csv->arq);
        free(csv->arq);
        return NULL;
    }

    csv->lines = 0;
    csv->columns = 0;
    csv->pos = 0;
    csv->types = NULL;
    csv->sizes = NULL;
    csv->data = NULL;
    
    return csv;
}
    

char* separate (char* lines){
    char *pos_comma;

    if (!lines) return 0;

    pos_comma = strchr(lines, ',');

    if (!pos_comma) return 0;

    *pos_comma = '\0';

    return lines;
}


void read_csv (struct arq_csv *csv){

    char buffer[1024];
    char *token;

    while (fgets(buffer, sizeof(buffer), csv->arq) != NULL){
        csv->lines++;
        char* token = separate(buffer);

        unsigned long aux_col = 1;
        while ((token = separate(token + (strlen(token) + 1)))){
            aux_col++;
        }
        aux_col++;

        if (aux_col > csv->columns)
            csv->columns = aux_col;  
    }
    
    rewind(csv->arq);


    csv->data = (char ***)malloc(csv->lines * sizeof(char **));
    if (csv->data == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned long i = 0; i < csv->lines; i++) {
        csv->data[i] = (char **)malloc(csv->columns * sizeof(char *));
        if (csv->data[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
    }
 
    // Lendo os dados e preenchendo a matriz   
    unsigned long l = 0;
    while (fgets (buffer, sizeof(buffer), csv->arq) != NULL){
        token = separate(buffer);
        
        unsigned long c = 0;
        
        while (token != NULL){

            csv->data[l][c] = (char *)malloc((strlen(token) + 1) * sizeof(char));
            if (csv->data[l][c] == NULL){
                fprintf(stderr, "Erro de alocação de memoria");
                exit(EXIT_FAILURE);
            }

            strcpy(csv->data[l][c], token);
            c++;
            token = separate(token + (strlen(token) +1));
        }
        l++;
    }
}

void analyze_csv (struct arq_csv *csv){

    // Alocando memória para armazenar os dados
    csv->types = malloc(csv->columns * sizeof(char));
    if (csv->types == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    csv->sizes = malloc(csv->columns * sizeof(unsigned short));
    if (csv->sizes == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    // Preenche o vetor com os tipos 'N' ou 'S' dependendo do dado
    for (unsigned long j = 0; j < csv->columns; j++){
        if (atof(csv->data[1][j])  == 0)
            csv->types[j] = "S";
        else
            csv->types[j] = "N";
    }
/*
    // Inicializa o vetor com 0
    for (unsigned long j = 0; j < csv->columns; j++)
        csv->sizes[j] = 0;

    // Acha a maior string da coluna e coloca em size[n_coluna]
    for (unsigned long j = 0; j < csv->columns; j++){
        for (unsigned long i = 0; i < csv->lines; i++){
            char *word = csv->data[i][j];
            unsigned long len = strlen(word);
            if (len > csv->sizes[j])
                csv->sizes[j] = len;
        }
    } 
 */
}

void summary (struct arq_csv *csv){

    for (unsigned long j = 0; j < csv->columns; j++){
        printf("%s [%s]\n", csv->data[0][j], csv->types[j]);
    }

    printf("%lu variaveis encontradas\n", csv->columns);
    
    printf("Pressione ENTER para continuar\n");
}















     
























      
