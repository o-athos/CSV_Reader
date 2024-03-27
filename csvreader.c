#include <stdio.h> 
#include <stdlib.h>
#include "io.h"

int main( int argc, char* argv[] ){

    if (argv[1] == NULL){
        return 1;
    }
    
    char *file = argv[1];

    int op;
    do{

        exibe_menu();
        scanf("%d", &op);
        getchar();
        
        switch (op){
            
            case 1:
                exibe_sumario(file);
                break;
            case 2:
                mostra(file);
                break;
            case 3:
                break;
            default:
                printf("Opção inválida. Por favor, escolha novamente.\n");
        }
    } while (op != 3);

    return 0;
}
