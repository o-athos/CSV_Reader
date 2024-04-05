#include "io.h"

void menu(){

    printf("1) Sumário do Arquivo\n");
    printf("2) Mostrar\n");
    printf("3) Filtros\n");
    printf("9) Fim\n");
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
    
void close_csv (struct arq_csv *csv){
    
    if (csv->arq != NULL)
        fclose(csv->arq);


    if (csv->data != NULL){
        for (unsigned long i = 0; i < csv->lines; i++){
            for (unsigned long j = 0; j < csv->columns; j++){
                free(csv->data[i][j]);
            }
            free(csv->data[i]);
        }
        free(csv->data);
    }

    if (csv->types != NULL){
        for (unsigned long j = 0; j < csv->columns; j++){
            free(csv->types[j]);
        }
//        free(csv->types);
    }

    if (csv->sizes != NULL){
        for (unsigned long j = 0; j < csv->columns; j++){
            free(csv->sizes[j]);
        }
        free(csv->sizes);
    }

    free(csv);
}


char* separate (char* lines){
    char *pos_comma;

    if (!lines) return 0;

    pos_comma = strchr(lines, ',');
    
    if (!pos_comma){
        char *pos_newline = strchr(lines, '\n');

        if (pos_newline){
            *pos_newline = '\0';
            return lines;
        }
        return lines;   
    }
    *pos_comma = '\0';
    return lines;
}

// Aloca espaço para matriz
void create_matrix (struct arq_csv *csv){
    
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
        for (unsigned long j = 0; j < csv->columns; j++){
            csv->data[i][j] = (char *)malloc(30 * sizeof(char));
        }
    }
}     

// Preenche a matriz com os dados
void fill_matrix (struct arq_csv *csv){
    char buffer[1024];
    char *token;
    short count, base;   

    // Lendo os dados e preenchendo a matriz   
    unsigned long l = 0;
    while (fgets (buffer, sizeof(buffer), csv->arq) != NULL){
        base = strlen(buffer);
        token = separate(buffer);
        count = strlen(token) + 1;
        
        unsigned long c = 0;
        
        while (count < base){
            
            if (strcmp(token, "") == 0)
                strcpy(csv->data[l][c], "NaN");
            else
                strcpy(csv->data[l][c], token);
            c++;
            token = separate(token + (strlen(token) +1));
            count += (strlen(token) + 1);
        }
        strcpy(csv->data[l][c], token );
        l++;
    }
}


void save_newData (char ***matrix, unsigned long new_lines, unsigned long columns, const char *file_name){
    
    FILE *file = fopen(file_name, "w");

    if (file == NULL){
        printf("Erro ao abrir o arquivo %s.\n", file_name);
        exit(EXIT_FAILURE);
    }
   
    // Escrever cabeçalho do CSV
    for (unsigned long j = 0; j < columns; j++) {
        fprintf(file, "%s", matrix[0][j]);
        if (j < columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");
 
    // Escrevendo os dados filtrados no arquivo CSV
    for (unsigned long i = 1; i < new_lines; i++) {
        for (unsigned long j = 0; j < columns; j++) {
            fprintf(file, "%s", matrix[i][j]);
            if (j < columns - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Arquivo gravado com sucesso.\n");
}


// Leitura inicial do arquivo. Conta dimensões, aloca e preenche matriz
void read_csv (struct arq_csv *csv){

    char buffer[1024];
    short count, base;
    while (fgets(buffer, sizeof(buffer), csv->arq) != NULL){
        base = strlen(buffer);
        csv->lines++;
        char* token = separate(buffer);
        count = strlen(token) + 1;
        
        unsigned long aux_col = 1;
        while (count < base){
            token = separate(token + (strlen(token) + 1));
            aux_col++;
            count += (strlen(token) + 1);
        }

        if (aux_col > csv->columns)
            csv->columns = aux_col;  
    }
    rewind(csv->arq);

    // Alocando matriz e preenche matriz
    create_matrix(csv);   
    fill_matrix(csv);

}

void analyze_csv (struct arq_csv *csv){

    // Alocando espaço para o vetor com os tipos de dados de cada coluna
    csv->types = (char **)malloc(csv->columns * sizeof(char *));
    if (csv->types == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < csv->columns; j++){
        csv->types[j] = (char *)malloc(csv->columns * sizeof(char));
        if (csv->types[j] == NULL){
            printf("Erro alocar memoria");
            return;
        }
    }

   


    // Alocar espaço para o vetor com o tamanho da maior string de cada coluna
    csv->sizes = (unsigned short **)malloc(csv->columns * sizeof(unsigned short *));
    if (csv->sizes == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    
    for (int j = 0; j < csv->columns; j++){
        csv->sizes[j] = (unsigned short *)malloc(csv->columns * sizeof(unsigned short));
        if (csv->sizes[j] == NULL){
            printf("Erro alocar memoria");
            return;
        }
    }


    // Preenche o vetor com os tipos 'N' ou 'S' dependendo do dado
    for (unsigned long j = 0; j < csv->columns; j++){
        if (atof(csv->data[1][j])  == 0)
            csv->types[j] = "S";
        else
            csv->types[j] = "N";
    }

    // Inicializa o vetor com 0
    for (unsigned long j = 0; j < csv->columns; j++)
        *(csv->sizes[j]) = 0;

    // Acha a maior string da coluna e coloca em size[n_coluna]
    for (unsigned long j = 0; j < csv->columns; j++){
        for (unsigned long i = 0; i < csv->lines; i++){
            char *word = csv->data[i][j];
            unsigned long len = strlen(word);
            if (len > *(csv->sizes[j]))
                *(csv->sizes[j]) = len;
        }
    } 
}

/* -----------------------------------------------------------------------------*/
                            /* FUNÇÕES PRINCIPAIS */


void summary (struct arq_csv *csv){

    for (unsigned long j = 0; j < csv->columns; j++){
        printf("%s [%s]\n", csv->data[0][j], csv->types[j]);
    }

    printf("%lu variaveis encontradas\n", csv->columns);
    
    printf("\nPressione ENTER para continuar\n");

    getchar();
}

void show (struct arq_csv *csv){
    
    if (csv->lines <= 10){
        for (unsigned short i = 0; i < csv->lines; i++){
            for (unsigned short j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (i == 0)
                    printf(" %*s  ", align, csv->data[i][j]);
                else{
                    if (j == 0){
                        printf("%d  %*s  ", i-1, align, csv->data[i][j]);
                    }
                    else
                        printf("%*s  ", align, csv->data[i][j]);
                } 
            }
            printf("\n");
        }       
    }

    else{
        for (unsigned short i = 0; i < 6; i++){
            for (unsigned short j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (i == 0)
                    printf("%*s",align, csv->data[i][j]);
                else
                    if (j == 0)
                        printf("%d %*s ", i-1, align, csv->data[i][j]);
                    else
                        printf("%*s ", align, csv->data[i][j]); 
            }
            printf("\n");
        }

        for (unsigned short j = 0; j < csv->columns; j++){
            unsigned short align = *csv->sizes[j];
            printf("%*s", align, "...");
        }
        printf("\n");

        for (unsigned long i = (csv->lines - 5); i < csv->lines; i++){
            for (unsigned short j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                    if (j == 0)
                        printf("%ld %*s", i-1, align, csv->data[i][j]);
                    else
                        printf("%*s", align, csv->data[i][j]); 
            }
            printf("\n");
        }
    }
 
    printf("\n[%lu rows x %lu columns]\n", csv->lines - 1, csv->columns);
    printf("\nPressione ENTER para continuar\n");
    getchar();
}


void filtering (struct arq_csv *csv){
    
    char var[50], filter[3], value[50];

    printf("Entre com a variavel: ");
    scanf("%s", var);
    
    printf("Escolha um filtro ( == > >= < <= != ): ");
    scanf("%s", filter);
    
    printf("Digite um valor: ");
    scanf("%s", value);


    // Acha a coluna da variavel a ser filtrada
    unsigned long var_column;
    for (unsigned long j = 0; j < csv->columns; j++){
        if (strcmp(csv->data[0][j], var) == 0){
            var_column = j;
            break;
        }
    }
    
    int *filtred_lines = NULL;
    filtred_lines = (int *)malloc(csv->lines * sizeof(int *));
    if (filtred_lines == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    
    for (unsigned long i = 0; i < csv->lines; i++){
        filtred_lines[i] = 0;
    }


    unsigned long new_lines = 1;
    if (strcmp(filter, "==") == 0) {
        for (unsigned long i = 1; i < csv->lines; i++) {
            if (strcmp(csv->data[i][var_column], value) == 0) {
                filtred_lines[i] = 1;
                new_lines++;
            } else {
                filtred_lines[i] = 0;
            }
        }
    }


    /*-------------------------- ANALISE DE FILTROS E CASOS--------------------------------- */


    else if (strcmp(filter, ">") == 0) {

         for (unsigned long i = 1; i < csv->lines; i++) {
            if (*csv->types[var_column] == 'N') {
                // Comparação numérica
                double num_csv = atof(csv->data[i][var_column]);
                double num_value = atof(value);
                if (num_csv > num_value) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            } else {
                // Comparação de string
                if (strcmp(csv->data[i][var_column], value) > 0) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
        }


    } else if (strcmp(filter, ">=") == 0) {
        
        for (unsigned long i = 1; i < csv->lines; i++) {
            if (*csv->types[var_column] == 'N') {
                // Comparação numérica
                double num_csv = atof(csv->data[i][var_column]);
                double num_value = atof(value);
                if (num_csv >= num_value) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            } else {
                // Comparação de string
                if (strcmp(csv->data[i][var_column], value) >= 0) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
        }


    } else if (strcmp(filter, "<") == 0) {
        
        for (unsigned long i = 1; i < csv->lines; i++) {
            if (*csv->types[var_column] == 'N') {
                // Comparação numérica
                double num_csv = atof(csv->data[i][var_column]);
                double num_value = atof(value);
                if (num_csv < num_value) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            } else {
                // Comparação de string
                if (strcmp(csv->data[i][var_column], value) < 0) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
        }



    } else if (strcmp(filter, "<=") == 0) {
        
        for (unsigned long i = 1; i < csv->lines; i++) {
            if (*csv->types[var_column] == 'N') {
                // Comparação numérica
                double num_csv = atof(csv->data[i][var_column]);
                double num_value = atof(value);
                if (num_csv <= num_value) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
            else {
                // Comparação de string
                if (strcmp(csv->data[i][var_column], value) <= 0) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
        }

    } else if (strcmp(filter, "!=") == 0) {
        
        for (unsigned long i = 1; i < csv->lines; i++) {
            if (*csv->types[var_column] == 'N') {
                // Comparação numérica
                double num_csv = atof(csv->data[i][var_column]);
                double num_value = atof(value);
                if (num_csv != num_value) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
            else {
                // Comparação de string
                if (strcmp(csv->data[i][var_column], value) != 0) {
                    filtred_lines[i] = 1;
                    new_lines++;
                } else {
                    filtred_lines[i] = 0;
                }
            }
        }


    } else {
        // Filtro não reconhecido
        fprintf(stderr, "Filtro não reconhecido: %s\n", filter);
        exit(EXIT_FAILURE);
    }

    /* -------------------------- PRINT DE DADOS --------------------------------*/

    // Print cabeçalho
    for (unsigned long j = 0; j < csv->columns; j++){
        unsigned short align = *csv->sizes[j];
        printf("%*s", align, csv->data[0][j]);
    }
    printf("\n");

     
    if (new_lines <= 10){ 
        // Imprime dados filtrados
        for (unsigned long i = 1; i < csv->lines; i++){
            if (filtred_lines[i] == 0) 
                continue;
            for (unsigned long j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (j == 0)
                    printf("%ld  %*s  ", i-1, align, csv->data[i][j]);
                else
                    printf("%*s  ", align, csv->data[i][j]); 
            }
            printf("\n");            
        }
    }
    else{
        
        unsigned long i = 0;
        unsigned long l = 0; 
        while (i < csv->lines && l < 5){
            if (filtred_lines[i] == 0){ 
                i++;
                continue;
            }
            l++;
            for (unsigned long j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (j == 0)
                    printf("%ld  %*s  ", i-1, align, csv->data[i][j]);
                else
                    printf("%*s  ", align, csv->data[i][j]); 
            }
            printf("\n");
            i++;            
        }       
        
        // Divisao de linhas 
        for (unsigned short j = 0; j < csv->columns; j++){
            unsigned short align = *csv->sizes[j];
            printf("%*s", align, "...");
        }
        printf("\n");

       
        // Achar o quinto ultimo indice filtrado
        unsigned long aux = csv->lines-1;
        unsigned long count = 0;
        while (aux > 6 && count < 5){
            if (filtred_lines[aux] == 1)
                count++;
            aux--;
        }

        i = aux;
        l = 0;
        while (i < csv->lines && l < 5){
            if (filtred_lines[i] == 0){
                i++;
                continue;
            }
            for (unsigned long j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (j == 0)
                    printf("%ld  %*s  ", i-1, align, csv->data[i][j]);
                else
                    printf("%*s  ", align, csv->data[i][j]); 
            }
            printf("\n");
            i++;
            l++;
        }            
           
    }

    printf("[%lu rows x %lu columns]\n", new_lines-1, csv->columns); 



    /*---------------------------- MANIPULAÇÂO DE DADOS -----------------------------------------*/


    // Cria nova matriz para o numero de linhas filtradas
    char ***filtered_matrix = NULL;
     
    filtered_matrix = (char ***)malloc(new_lines * sizeof(char **));
    if (filtered_matrix == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned long i = 0; i < new_lines; i++) {
        filtered_matrix[i] = (char **)malloc(csv->columns * sizeof(char *));
        if (filtered_matrix[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
        for (unsigned long j = 0; j < csv->columns; j++){
            filtered_matrix[i][j] = (char *)malloc(30 * sizeof(char));
        }
    }
          
    // Preenche a matriz com dados filtrados   
    for (unsigned long j = 0; j < csv->columns; j++){
        filtered_matrix[0][j] = csv->data[0][j];
    }
 
    unsigned long i = 1;
    unsigned long l = 1;
    while (i < new_lines && l < csv->lines){
        if (filtred_lines[l] == 0){
            l++;
            continue;
        }
        for (unsigned long c = 0; c < csv->columns; c++){
            filtered_matrix[i][c] = csv->data[l][c];           
        }
        l++;
        i++;
    }

 
    char op;
    printf("Deseja gravar um arquivo com os dados filtrados? [S|N]: ");
    scanf("%s", &op);

    if (op == 'S' || op == 's'){
        char *file_name = malloc(100 * sizeof(char));
        printf("Entre com o nome do arquivo: ");
        scanf("%s", file_name);
    
        save_newData(filtered_matrix, new_lines, csv->columns, file_name);
    }


    char discard;
    printf("Deseja descartar os dados originais? [S|N]: ");
    scanf("%s", &discard);
    
    if (discard == 'S' || discard == 's'){
        
        // Libera memoria da matriz original
        for (unsigned long i = 0; i < csv->lines; i++) {
            for (unsigned long j = 0; j < csv->columns; j++) {
                free(csv->data[i][j]);
            }
            free(csv->data[i]);
        }
        free(csv->data);

        // Nova matriz 'original'
        csv->data = filtered_matrix;
        csv->lines = new_lines;
    }

    else{
        
        // Libera matriz filtrada
        for (unsigned long i = 0; i < new_lines; i++) {
            for (unsigned long j = 0; j < csv->columns; j++) {
                free(filtered_matrix[i][j]);
            }
            free(filtered_matrix[i]);
        }
        free(filtered_matrix);    
    }    

    free(filtred_lines);

    getchar();
    printf("Pressione ENTER para continuar\n");
    getchar();
}  
           
void data_description (struct arq_csv *csv){

    char var[50];
    
    printf("entre com a variavel: ");
    scanf("%s", &var);

    // Acha a coluna da variavel a ser filtrada
    unsigned long var_column;
    for (unsigned long j = 0; j < csv->columns; j++){
        if (strcmp(csv->data[0][j], var) == 0){
            var_column = j;
            break;
        }
    }
    
    if (*csv->types[var_column] == 'N'){

        // Contador para o total de dados        
        unsigned int total = 0;
        for (unsigned long i = 1; i < csv->lines; i++){
            if (csv->data[0][var_column] == 'NaN') continue;
            total++;
        }

        // alocando espaço em um vetor para colocar os valores
        double *values = (double *)malloc(total * sizeof(double));
        if (valores == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
    
        unsigned int index = 0;
        for (unsigned long i = 0; i < csv->lines; i++){
            if (strcmp(csv->data[i][var_column], "NaN") != 0){
                values[index++] = atof(csv->data(csv->data[i][var_column]));
            }
        }

        qsort (values, total, sizeof(double), comparation);

        
        //--- MÉDIA ---
        double soma = 0;
        for (int i = 0; i < total; i++){
            soma += values[i];
        }
        double media = soma/total;


        // --- MEDIANA ---
        double mediana 
        if ((total % 2) == 0)
            mediana = (values[(n/2) - 1] + values[n/2]) / 2.0; 
        else
            mediana = values[n/2];
        
        
        // --- MODA ---
        













    





   



                
    






































     
























      
