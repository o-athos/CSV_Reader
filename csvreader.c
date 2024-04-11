#include <stdio.h> 
#include <stdlib.h>
#include "io.h"

int main( int argc, char* argv[] ){

    if (argv[1] == NULL){
        return 1;
    }
    
    char *file = argv[1];

    struct arq_csv* csv = open_csv(file);
    read_csv (csv);
    analyze_csv (csv);
 
    int op;
    do{

        menu();
        scanf("%d", &op);
        getchar();
        
        switch (op){
            
            case 1:
                summary(csv);
                break;
            case 2:
                show(csv);
                break;
            case 3:
                filtering(csv);
                break;
            case 4:
                data_description(csv);
                break;
            case 5:
                ordering(csv);
                break;
            case 6:
                selecao(csv);
                break;
            case 7:
                dados_faltantes(csv);
                break;
            case 8:
                save(csv);
                break;
            case 9:
                break;
            default:
                printf("Opção inválida. Por favor, escolha novamente.\n");
        }
    } while (op != 9);

    close_csv(csv);
    
    return 0;
}
