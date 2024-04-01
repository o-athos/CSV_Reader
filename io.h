#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef IO_H
#define IO_H

struct arq_csv{
    FILE *arq;
    unsigned long lines;
    unsigned long columns;
    unsigned long pos;
    char *types;
    unsigned short *sizes;
};

struct arq_csv* open_csv (char* path);

struct arq_csv* close_csv (struct  arq_csv *file)

struct arq_csv* verify_csv (struct arq_csv* file);

char* separate (char* lines);
    
void summary (arq_csv *csv);

void show (arq_csv *csv);

char* format (char* form[], char* lines, unsigned short* sizes);
    
void menu ();
#endif 
