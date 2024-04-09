#include "io.h"

void menu(){

    printf("1) Sumário do Arquivo\n");
    printf("2) Mostrar\n");
    printf("3) Filtros\n");
    printf("4) Descrição dos dados\n");
    printf("5) Ordenação\n");
    printf("6) selecao\n");
    printf("7) Dados Faltantes\n");
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
        free(csv->types);
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
            strcpy(csv->types[j], "S");
        else
            strcpy(csv->types[j],"N");
    }

    // Inicializa o vetor com 0
    for (unsigned long j = 0; j < csv->columns; j++)
        *(csv->sizes[j]) = 0;

    // Acha a maior string da coluna e coloca em size[n_coluna]
    for (unsigned long j = 0; j < csv->columns; j++){
        for (unsigned long i = 0; i < csv->lines; i++){
            char *word = csv->data[i][j];
            unsigned short len = strlen(word);
            if (len > *(csv->sizes[j]))
                *(csv->sizes[j]) = len;
        }
    } 
}

/*void formata(unsigned char **formatacao, const unsigned short *tamanhos, const unsigned long quantidade,
                                                                                     char ***dados) {
    for (unsigned long i = 0; i < quantidade; i++) {
        for (unsigned short j = 0; j < tamanhos[i] - strlen(dados[i][j]); j++) {
            formatacao[i][j] = ' ';
        }
        strcpy(formatacao[i] + (tamanhos[i] - strlen(dados[i][j])), dados[i][j]);
    }
}
*/

int comparation_numeric (const void *a, const void *b){
    double *aa = (double *)a;
    double *bb = (double *)b;
    
    if (*aa == *bb)         return 0;
    else if (*aa < *bb)     return -1;
    else                    return 1;

}

int comparation_string (const void *a, const void *b){
    char *aa = *(char **)a;
    char *bb = *(char **)b;

    return strcmp(aa, bb);
}
      
// Função de comparação para ordenação ascendente da struct
int compare_asc(const void *a, const void *b) {
    struct column_infos *aa = (struct column_infos *)a;
    struct column_infos *bb = (struct column_infos *)b;

    // Se aa for NaN, eh menor que bb    
    if (strcmp(aa->infos, "NaN") == 0)
        return -1;

    // Se bb for NaN, eh menor que aa
    else if (strcmp(bb->infos, "NaN") == 0)
        return 1;
    
    // Caso normal 
    return strcmp(aa->infos, bb->infos);
}

// Função de comparação para ordenação descendente da struct
int compare_desc(const void *a, const void *b) {
    return -compare_asc(a, b);
}

// Função de comparação para ordenação ascendente de valores numéricos da struct
int compare_numeric_asc(const void *a, const void *b) {
    struct column_infos *aa = (struct column_infos *)a;
    struct column_infos *bb = (struct column_infos *)b;
 
    // Se aa for NaN, eh menor que bb    
    if (strcmp(aa->infos, "NaN") == 0)
        return -1;

    // Se bb for NaN, eh menor que aa
    else if (strcmp(bb->infos, "NaN") == 0)
        return 1;
   
 
    double num_aa = atof(aa->infos);
    double num_bb = atof(bb->infos);
    
    if (num_aa < num_bb) return -1;
    else if (num_aa > num_bb) return 1;
    else return 0;
}

// Função de comparação para ordenação descendente de valores numéricos da struct
int compare_numeric_desc(const void *a, const void *b) {
    return -compare_numeric_asc(a, b);
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
                    printf(" %*s",align, csv->data[i][j]);
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


    /*-------------------------- ANALISE DE FILTROS E CASOS--------------------------------- */

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
        strcpy(filtered_matrix[0][j], csv->data[0][j]);
    }
 
    unsigned long i = 1;
    unsigned long l = 1;
    while (i < new_lines && l < csv->lines){
        if (filtred_lines[l] == 0){
            l++;
            continue;
        }
        for (unsigned long c = 0; c < csv->columns; c++){
            strcpy(filtered_matrix[i][c], csv->data[l][c]);           
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
        free(file_name);
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
    
    printf("Entre com a variavel: ");
    scanf("%s", var);

    // Acha a coluna da variavel a ser filtrada
    unsigned long var_column;
    for (unsigned long j = 0; j < csv->columns; j++){
        if (strcmp(csv->data[0][j], var) == 0){
            var_column = j;
            break;
        }
    }
    
    if (strcmp(csv->types[var_column], "N") == 0){

        // Contador para o total de dados        
        unsigned long total = 0;
        for (unsigned long i = 1; i < csv->lines; i++){
            if (strcmp(csv->data[i][var_column], "NaN") == 0) continue;
            total++;
        }

        // alocando espaço em um vetor para colocar os valores
        double *values = (double *)malloc(total * sizeof(double));
        if (values == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
    
        unsigned int index = 0;
        for (unsigned long i = 1; i < csv->lines; i++){
            if (strcmp(csv->data[i][var_column], "NaN") != 0){
                values[index++] = atof(csv->data[i][var_column]);
            }
        }

        qsort (values, total, sizeof(double), comparation_numeric);

        
        //--- MÉDIA ---
        double soma = 0;
        for (int i = 0; i < total; i++){
            soma += values[i];
        }
        double media = soma/total;


        // --- MEDIANA ---
        double mediana; 
        if ((total % 2) == 0)
            mediana = (values[(total/2) - 1] + values[total/2]) / 2.0; 
        else
            mediana = values[total/2];
        

        // --- MODA ---
        double moda = values[0];
        unsigned long times = 1;
        double current = values[0];
        unsigned long current_times = 0;
        for (unsigned long i = 1; i < total; i++){
            if (values[i] == moda){
                times++;
            }
            else if (values[i] == current){
                current_times++;
            }
            else {
                current = values[i];
                current_times = 1;
            }
            if (current_times > times){
                moda = current;
                times = current_times;
            }
        }


        // --- Desvio Padrão ---
        double sum_s = 0;
        for (unsigned int i = 0; i < total; i++)
            sum_s += (values[i] - media) * (values[i] - media);
        double desvio_padrao = sqrt(sum_s / total);

        
        // --- MINIMO E MAXIMO ---
        double max = values[total - 1];
        double min = values[0];


        // --- VALORES UNICOS ---
        unsigned long unique = 1;
        for (unsigned long i = 1; i < total; i++){
            if (values[i] != values[i - 1])
                unique++;
        }
        
        double *unique_v = (double *) malloc(unique * (sizeof (double)));
        if (unique_v == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
        
        unique_v[0] = values[0];
        unsigned long unique_i = 1;
            for (unsigned long i = 1; i < total; i++){
            if (values[i] != values[i - 1])
                unique_v[unique_i++] = values[i];
        }

        
        // --- PRINT DADOS ---
        
        printf("Contador: %lu\n", total);
        printf("Media: %.1f\n", media);
        printf("Desvio: %.1f\n", desvio_padrao);  
        printf("Mediana: %.1f\n", mediana);
        printf("Moda: %.1f %lu vezes\n", moda, times);
        printf("Min.: %.1f\n", min);
        printf("Max.: %.1f\n", max);

        printf("Valores unicos: [");
        unsigned long i = 0;
        while ( i < unique-1){
        //for (unsigned long i = 0; i < unique-1; i++)
            printf("%.1f ,", unique_v[i]);
            i++;
        }
        printf("%.1f]\n", unique_v[i]); 
        
        free(values);
        free(unique_v);
      
        printf("\nPressione ENTER para continuar\n");
        getchar();
    }
    else {

        // Contador para o total de dados        
        unsigned long total = 0;
        for (unsigned long i = 1; i < csv->lines; i++){
            if (strcmp(csv->data[i][var_column], "NaN") == 0) continue;
            total++;
        }

        // Aloca vetor de ponteiro para char, para guardar as variáveis nao numéricas
        char **values = (char **)malloc(total * sizeof(char*));
        if (values == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }

        for (unsigned int i = 0; i < total; i++) {
            values[i] = (char *)malloc((*(csv->sizes[var_column]) +1) * sizeof(char));
            if (values[i] == NULL) {
                fprintf(stderr, "Erro de alocação de memória.\n");
                exit(EXIT_FAILURE);
            }
        }
       
 
        // Preenche vetor com dados nao numericos
        unsigned int index = 0;
        for (unsigned long i = 1; i < csv->lines; i++){
            if (strcmp(csv->data[i][var_column], "NaN") != 0){
                strcpy(values[index++], csv->data[i][var_column]);
            }
        }

        // Ordena crescente
        qsort(values, total, sizeof(char *), comparation_string);


        // --- VALORES UNICOS ---
        unsigned long unique = 1;
        for (unsigned long i = 1; i < total; i++){
            if (strcmp(values[i], values[i - 1]) != 0)
                unique++;
        }


        // Aloca vetor de ponteiro para char, guardar strings
        char **unique_v = (char **)malloc(unique * (sizeof (char *)));
        if (unique_v == NULL) {
           fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }

        for (unsigned int i = 0; i < unique; i++) {
            unique_v[i] = (char *)malloc((*csv->sizes[var_column] + 1) * sizeof(char));
            if (unique_v[i] == NULL) {
                fprintf(stderr, "Erro de alocação de memória.\n");
                exit(EXIT_FAILURE);
            }
        }
        

        strcpy(unique_v[0],values[0]);
        unsigned long unique_i = 1;
        for (unsigned long i = 1; i < total; i++){
            if (strcmp(values[i], values[i - 1]) != 0)
                strcpy(unique_v[unique_i++], values[i]);
        }
        
         // --- MODA ---
        char *moda = strdup(values[0]);
        unsigned long times = 1;
        char *current = strdup(values[0]);
        unsigned long current_times = 0;
        for (unsigned long i = 1; i < total; i++){
            if ((strcmp(values[i], moda) == 0)){
                times++;
            }
            else if ((strcmp(values[i],current) == 0)){
                current_times++;
            }
            else {
                free(current);
                //strcpy(current, values[i]);
                current = strdup(values[i]);
                current_times = 1;
            }
            if (current_times > times) {
                free(moda);
                //strcpy(moda, current);
                moda = strdup(current);
                times = current_times;
            }
        }
       
        // --- PRINT DADOS ---
 
        printf("Contador: %lu\n", total);
        printf("Moda: %s %lu vezes\n", moda, times);
        printf("Valores unicos: [");
        unsigned long i = 0;
        while ( i < unique - 1 ){
            printf("'%s', ", unique_v[i]);
            i++;
        }
        printf("'%s']\n", unique_v[i]);
        
        // Libera unique_v       
        for (unsigned long i = 0; i < unique; i++)
            free(unique_v[i]);
        free(unique_v);

        // Libera values
        for (unsigned int i = 0; i < total; i++) {
            free(values[i]);
        }
        free(values); 
        free(moda);
        free(current);
 
        printf("\nPressione ENTER para continuar\n");
        getchar();
    }
}


void ordering (struct arq_csv *csv){

    char var[30], op[3];

    printf("Entre com a variavel: ");
    scanf("%s", var);
 
    printf("Selecione uma opcao [A]scendente ou [D]escendente: ");
    scanf("%s", op);

    // Acha a coluna da variavel a ser filtrada
    unsigned long var_column;
    for (unsigned long j = 0; j < csv->columns; j++){
        if (strcmp(csv->data[0][j], var) == 0){
            var_column = j;
            break;
        }
    }
    
    //Cria matriz para os dados ordenados
    char ***ordered_matrix = NULL;
     
    ordered_matrix = (char ***)malloc(csv->lines * sizeof(char **));
    if (ordered_matrix == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned long i = 0; i < csv->lines; i++) {
        ordered_matrix[i] = (char **)malloc(csv->columns * sizeof(char *));
        if (ordered_matrix[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
        for (unsigned long j = 0; j < csv->columns; j++){
            ordered_matrix[i][j] = (char *)malloc(30 * sizeof(char));
        }
    }

    // Vetor de structs para conseguir ordenar e conseguir saber qual era a linha que estava
    struct column_infos *v = malloc((csv->lines-1) * sizeof(struct column_infos));
    if (v == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
   
    unsigned long i_v = 0;
    unsigned long l_m = 1;
    for (unsigned long i = 1; i < csv->lines; i++){
        v[i-1].infos = strdup(csv->data[i][var_column]);
        v[i-1].id = i-1;
        i_v++;
        l_m++;
    }
    

    if (strcmp(op, "A") == 0) {
        
        if (strcmp(csv->types[var_column], "S") == 0)
            qsort(v, csv->lines-1, sizeof(struct column_infos), compare_asc);
        else
            qsort(v, csv->lines-1, sizeof(struct column_infos), compare_numeric_asc);
    }
    else{
        if (strcmp(csv->types[var_column], "S") == 0)
            qsort(v, csv->lines-1, sizeof(struct column_infos), compare_desc);
        else
            qsort(v, csv->lines-1, sizeof(struct column_infos), compare_numeric_desc);
    }


    // Copia o cabecalho para a matriz
    for (unsigned long j = 0; j < csv->columns; j++){
        strcpy(ordered_matrix[0][j], csv->data[0][j]);
    }
    
    // Coloca na matriz de acordo com id do primeiro termo do vetor ordenado conforme a opção. draw was necessary
    unsigned long i = 1;
    while ( i < csv->lines ){
    //for (unsigned long i = 1; i < csv->lines; i++){
        for (unsigned long j = 0; j < csv->columns; j++){
            strcpy(ordered_matrix[i][j], csv->data[v[i-1].id + 1][j]);
        }
        i++;
    }


    /* ------------ PRINT MATRIZ ORDENADA ---------- */
    
    if (csv->lines <= 10){
        for (unsigned short i = 0; i < csv->lines; i++){
            for (unsigned short j = 0; j < csv->columns; j++){
                unsigned short align = *csv->sizes[j];
                if (i == 0)
                    printf(" %*s", align, ordered_matrix[i][j]);
                else{
                    if (j == 0){
                        printf("%ld  %*s  ", v[i-1].id, align, ordered_matrix[i][j]);
                    }
                    else
                        printf("%*s  ", align, ordered_matrix[i][j]);
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
                    printf(" %*s",align, ordered_matrix[i][j]);
                else
                    if (j == 0)
                        printf("%ld %*s ", v[i-1].id , align, ordered_matrix[i][j]);
                    else
                        printf("%*s ", align, ordered_matrix[i][j]); 
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
                        printf("%ld %*s", v[i-1].id, align, ordered_matrix[i][j]);
                    else
                        printf("%*s", align, ordered_matrix[i][j]); 
            }
            printf("\n");
        }
    }
    
    printf("\n[%lu rows x %lu columns]\n", csv->lines - 1, csv->columns);


  
    char save;
    printf("Deseja gravar um arquivo com os dados filtrados? [S|N]: ");
    scanf("%s", &save);

    if (save == 'S' || save == 's'){
        char *file_name = malloc(100 * sizeof(char));
        printf("Entre com o nome do arquivo: ");
        scanf("%s", file_name);
    
        save_newData(ordered_matrix, csv->lines, csv->columns, file_name);
        free(file_name);
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
        csv->data = ordered_matrix;
    }

    else{
        
        // Libera matriz ordenada
        for (unsigned long i = 0; i < csv->lines; i++) {
            for (unsigned long j = 0; j < csv->columns; j++) {
                free(ordered_matrix[i][j]);
            }
            free(ordered_matrix[i]);
        }
        free(ordered_matrix);    
    }    

    // Libera vetor de struct
    for (unsigned long i = 0; i < csv->lines - 1; i++) {
        free(v[i].infos);
    }
    free(v);

    printf("\nPressione ENTER para continuar\n");
    getchar();
 
}   
   

void selecao (struct arq_csv *csv){

    char buffer[1024];
    unsigned long n_var = 0;

    char **v_var = (char **)malloc(csv->columns * sizeof(char *));
    if (v_var == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < csv->columns; i++) {
        v_var[i] = (char *)malloc(30 * sizeof(char)); 
        if (v_var[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
    }


    printf("Entre com a variaveis que deseja selecionar (separadas por espaço): ");
    fgets(buffer, sizeof(buffer), stdin);


    char *token = strtok(buffer, " ");
    while (token != NULL && n_var < csv->columns){
       
        if (token[strlen(token) - 1] == '\n')
            token[strlen(token) - 1] = '\0';
 
        strcpy(v_var[n_var], token);
        n_var++;
        token = strtok(NULL, " ");
    }

     //Cria matriz para os dados selecionados
    char ***selected_matrix = NULL;
     
    selected_matrix = (char ***)malloc(csv->lines * sizeof(char **));
    if (selected_matrix == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned long i = 0; i < csv->lines; i++) {
        selected_matrix[i] = (char **)malloc(n_var * sizeof(char *));
        if (selected_matrix[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
        for (unsigned long j = 0; j < n_var; j++){
            selected_matrix[i][j] = (char *)malloc(30 * sizeof(char));
        }
    }
    
    // Vetor para saber quais colunas imprimir. 1 para imprimir; 0 nao
    unsigned long *var_columns = (unsigned long *)malloc(csv->columns * sizeof(unsigned long));
    for (unsigned long j = 0; j < csv->columns; j++)
        var_columns[j] = 0;
    
    // Anota quais colunas estão ativas
    for (unsigned long j = 0; j < csv->columns; j++){
        for (unsigned long c = 0; c < n_var; c++){
            if (strcmp(v_var[c], csv->data[0][j]) == 0)
                var_columns[j] = 1;
        }
    }
    
    printf("oi\n");
    unsigned long c = 0; 
    for (unsigned long i = 0; i < csv->lines; i++){
        for (unsigned long j = 0; j < csv->columns; j++){
            if (var_columns[j] == 1){
                strcpy(selected_matrix[i][c], csv->data[i][j]);
                c++;
            }
        }
        c = 0;
    }

    
    printf("1\n");        
    /* ------------ PRINT MATRIZ SELECIONADA ---------- */
    
    if (csv->lines <= 10){
        for (unsigned short i = 0; i < csv->lines; i++){
            for (unsigned short j = 0; j < n_var; j++){
                unsigned short align = *csv->sizes[j];
                if (i == 0)
                    printf(" %*s", align, selected_matrix[i][j]);
                else{
                    if (j == 0){
                        printf("%d  %*s  ", i-1, align, selected_matrix[i][j]);
                    }
                    else
                        printf("%*s  ", align, selected_matrix[i][j]);
                } 
            }
            printf("\n");
        }       
    }

    else{
        for (unsigned short i = 0; i < 6; i++){
            for (unsigned short j = 0; j < n_var; j++){
                unsigned short align = *csv->sizes[j];
                if (i == 0)
                    printf(" %*s",align, selected_matrix[i][j]);
                else
                    if (j == 0)
                        printf("%d %*s ", i-1 , align, selected_matrix[i][j]);
                    else
                        printf("%*s ", align, selected_matrix[i][j]); 
            }
            printf("\n");
        }

        for (unsigned short j = 0; j < n_var; j++){
            unsigned short align = *csv->sizes[j];
            printf("%*s", align, "...");
        }
        printf("\n");

        for (unsigned long i = (csv->lines - 5); i < csv->lines; i++){
            for (unsigned short j = 0; j < n_var; j++){
                unsigned short align = *csv->sizes[j];
                    if (j == 0)
                        printf("%ld %*s", i-1, align, selected_matrix[i][j]);
                    else
                        printf("%*s", align, selected_matrix[i][j]); 
            }
            printf("\n");
        }
    }
    
    printf("\n[%lu rows x %lu columns]\n", csv->lines - 1, n_var);


    char save;
    printf("Deseja gravar um arquivo com os dados filtrados? [S|N]: ");
    scanf("%s", &save);

    if (save == 'S' || save == 's'){
        char *file_name = malloc(100 * sizeof(char));
        printf("Entre com o nome do arquivo: ");
        scanf("%s", file_name);
    
        save_newData(selected_matrix, csv->lines, n_var, file_name);
        free(file_name);
    }
       

    // Libera matriz ordenada
    for (unsigned long i = 0; i < csv->lines; i++) {
        for (unsigned long j = 0; j < n_var; j++) {
            free(selected_matrix[i][j]);
        }
        free(selected_matrix[i]);
    }
    free(selected_matrix);

    // Libera vetor das variaveis
    for (unsigned long i = 0; i < csv->columns; i++){
        free(v_var[i]);
    }
    free(v_var);
    
    // Libera vetor das colunas ativas/desativas
    free(var_columns);


    printf("\nPressione ENTER para continuar\n");
    getchar();
    
}    

    /* ---------- FUNÇÔES PARA A FUNÇÂO 'DADOS FALTANTES' --------- */


void list_NaN (struct arq_csv *csv){

    // Vetor para identificar quias linha tem NaN
    int *v_nan = malloc(csv->lines * (sizeof (int)));
    for (unsigned long i = 0; i < csv->lines; i++)
        v_nan[i] = 0;


    // preencher vetor com as linhas q tem NaN e verificar quantas linhas são
    unsigned long nan_cont = 0;
    for (unsigned long i = 0; i < csv->lines; i++){
        for (unsigned long j = 0; j < csv->columns; j++){
            if (strcmp(csv->data[i][j], "NaN") == 0){
                v_nan[i] = 1;
                nan_cont++;
                break;
            }
        }
    }
    
    //Cria matriz para os dados com NaN na linha
    char ***nan_matrix = NULL;
     
    nan_matrix = (char ***)malloc((nan_cont+1) * sizeof(char **));
    if (nan_matrix == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned long i = 0; i < nan_cont+1; i++) {
        nan_matrix[i] = (char **)malloc(csv->columns * sizeof(char *));
        if (nan_matrix[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória.\n");
            exit(EXIT_FAILURE);
        }
        for (unsigned long j = 0; j < csv->columns; j++){
            nan_matrix[i][j] = (char *)malloc(30 * sizeof(char));
        }
    }
   

    /* -- LISTAR LINHAS COM NaN -- */

    for (unsigned long j = 0; j < csv->columns; j++){
        char align = *csv->sizes[j];
        printf(" %*s", align, csv->data[0][j]);
        strcpy(nan_matrix[0][j], csv->data[0][j]);
    }
    printf("\n");

    unsigned long i = 0;
    unsigned long l = 1;
    while ( i < csv->lines && l < nan_cont+1){
        if (v_nan[i] == 0){
            i++;
            continue;
        }
        unsigned long c = 0;
        for (unsigned long j = 0; j < csv->columns; j++){
            char align = *csv->sizes[j];
            if (j == 0)
                printf("%lu %*s ", i-1, align, csv->data[i][j]);
            else
                printf("%*s ", align, csv->data[i][j]);
            
            strcpy(nan_matrix[l][c], csv->data[i][j]);
            c++;
        }
        l++;
        i++;
        printf("\n");
    }
    
    printf("\n[%lu rows x %lu columns]\n", nan_cont , csv->columns);
   
   
    char save;
    printf("Deseja gravar um arquivo com os dados filtrados? [S|N]: ");
    scanf("%s", &save);

    if (save == 'S' || save == 's'){
        char *file_name = malloc(100 * sizeof(char));
        printf("Entre com o nome do arquivo: ");
        scanf("%s", file_name);
    
        save_newData(nan_matrix, nan_cont+1, csv->columns, file_name);
        free(file_name);
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
        csv->data = nan_matrix;
        csv->lines = nan_cont + 1;
    }

    else{
        
        // Libera matriz selecionada com linhas NaN
        for (unsigned long i = 0; i < nan_cont; i++) {
            for (unsigned long j = 0; j < csv->columns; j++) {
                free(nan_matrix[i][j]);
            }
            free(nan_matrix[i]);
        }
        free(nan_matrix);    
    }    

    free(v_nan);

    printf("\nPressione ENTER para continuar\n");
    getchar();
    
    
}
    
    
void dados_faltantes (struct arq_csv *csv){
        
    int op;


    do {
        
        printf("\n1) Listar registros com NaN\n");
        printf("2) Substituir pela media\n");
        printf("3) Substituir pelo proximo valor valido\n");
        printf("4) Remover registros com NaN\n");
        printf("5) Voltar ao menu principal\n");
      

      
        scanf("%d", &op); 
        getchar();

        switch (op){
            case 1:
                list_NaN(csv);
                break;
            case '2':
                
                break;
            case '3':
    
                break;
            case '4':

                break;

            case '5':
                break;
            
            default:
                printf("Opção invalida\n");

        }

    } while (op != 5);

}






























