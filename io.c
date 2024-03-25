#include "io.h"


void leitura (FILE* arquivo){

    char* buffer[1024];
    char* matriz[];

    fgets (buffer, 1024, arquivo);

    //CONTA COLUNAS
    char *token;
    int linhas = 0; colunas = 0;
    while ((token = strstr(buffer, ',')) == 0){
        colunas++;
        token++;
        buffer = strdup (token);
    }

    
    //CONTA LINHAS
    while (1){
        fgets(buffer, 1024, arquivo);
        if (buffer[0] == '\n'){
            exit(1);
        }
        linhas++;
    }
   
    matriz = malloc (sizeof (char *) * linhas);
    
    
}
