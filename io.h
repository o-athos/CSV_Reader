#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef IO_H
#define IO_H

struct arq_csv{
    FILE *arq;
    unsigned long lines;
    unsigned long columns;
    char **types;        // vetor de 0 ou 1 que analisa se os dados da linha são numericos ou strings
    unsigned short **sizes;   // armazena o tamanho da maior palavra da coluna
    char ***data;
};

struct column_infos {
    char *infos;
    unsigned long id;
};

/* funções iniciais */
struct arq_csv* open_csv (char* path);
void close_csv (struct  arq_csv *file);
void read_csv (struct arq_csv *csv);
void analyze_csv (struct arq_csv *csv);

struct arq_csv* verify_csv (struct arq_csv* file);

void create_matrix (struct arq_csv *csv);
void fill_matrix (struct arq_csv* file);

void formata(unsigned char **formatacao, const unsigned short *tamanhos, const unsigned long quantidade, char ***dados);

void save_newData (char ***matrix, unsigned long new_lines, unsigned long columns, const char *file_name);

char* separate (char* lines);

// Funcoes para uso do qsort da libC
int comparation_numeric (const void *a, const void *b);
int comparation_string (const void *a, const void *b);
int compare_asc(const void *a, const void *b);
int compare_desc(const void *a, const void *b);
int compare_numeric_asc(const void *a, const void *b);
int compare_numeric_desc(const void *a, const void *b);

/* FUNCOES PRINCIPAIS */
void menu ();
void summary (struct arq_csv *csv);
void show (struct arq_csv *csv);
void filtering (struct arq_csv *csv);
void data_description (struct arq_csv *csv);
void ordering (struct arq_csv *csv);    
void selecao (struct arq_csv *csv);
void dados_faltantes(struct arq_csv *csv);
#endif 
