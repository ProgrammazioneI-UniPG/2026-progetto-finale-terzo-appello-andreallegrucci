#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>

extern void svuotaBuffer();

int main() {
    system("clear"); // Pulizia schermo
    unsigned short scelta = 0;
    printf("BENVENUTO IN \"COSESTRANE\"!\n\n");
    
    do {
        printf("MENU PRINCIPALE:\n1. Imposta gioco\n2. Gioca\n3. Termina gioco\n4. Visualizza i crediti\n>> ");
        scanf("%hu", &scelta);
        svuotaBuffer();

        switch(scelta) {
            case 1:
                printf("\n");
                imposta_gioco();
                scelta = 0;
                break;

            case 2:
                printf("\n");
                gioca();
                scelta = 0;
                break;

            case 3:
                printf("\n");
                termina_gioco();
                break;

            case 4:
                printf("\n");
                crediti();
                scelta = 0;
                break;

            default:
                printf("\n");
                printf("ATTENZIONE! Comando non riconosciuto, inserire un numero tra 1 e 4.\n");
                scelta = 0;
        }
        printf("\n");
    } while(scelta != 3);
    return 0;
}