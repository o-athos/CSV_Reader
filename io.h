#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef IO_H
#define IO_H

struct arq_csv{
    FILE *arq;
    unsigned long lines;
    unsigned long columns;
    unsigned long pos;  //posição da quinta ultima linha
    char **types;        // vetor de 0 ou 1 que analisa se os dados da linha são numericos ou strings
    unsigned short **sizes;   // armazena o tamanho da maior palavra da coluna
    char ***data;
};

/* funções iniciais */
struct arq_csv* open_csv (char* path);
void close_csv (struct  arq_csv *file);
void read_csv (struct arq_csv *csv);
void analyze_csv (struct arq_csv *csv);

struct arq_csv* verify_csv (struct arq_csv* file);

void create_matrix (struct arq_csv *csv);
void fill_matrix (struct arq_csv* file);

void save_newData (char ***matrix, unsigned long new_lines, unsigned long columns, const char *file_name);

char* separate (char* lines);
    
void summary (struct arq_csv *csv);

void show (struct arq_csv *csv);

void filtering (struct arq_csv *csv);

void data_description (struct arq_csv *csv);
    
void menu ();
#endif 
