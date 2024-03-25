#include <stdio.h> 
#include <stdlib.h>
#include "io.h"

int main( int argc, char* argv[] ){

    if (argv[1] == NULL){
        return 1;
    }
    
    char* arquivo = argv[1];

    FILE *file = fopen (arquivo, "r");
    if (file == NULL){
        return 1;        
    }
                
    
    










}
