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
    if (verify(csv->arq) == NULL){
        fclose(csv->arq);
        free(arq);
        return NULL;
    }

    csv->lines = 0;
    csv->columns = 0;
    csv->pos = 0;
    csv->types = NULL;
    csv->sizes = NULL;
    
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

      
