#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ATT_BILLI 7
#define PV_BILLI 40
#define ATT_DEMOCANE 10
#define PV_DEMOCANE 50
#define ATT_DEMOTORZONE 15
#define PV_DEMOTORZONE 100

void svuotaBuffer();

// Funzioni per imposta_gioco()
static void creazione_giocatori();                                               // Linea 322
static void generazione_valori_giocatore(Giocatore* giocatore);                  // Linea 366
static void stampa_valori_giocatore(Giocatore* giocatore);                       // Linea 459
static void deallocazione_giocatore(Giocatore** giocatore);                      // Linea 466
static void genera_mappa();                                                      // Linea 473
static void cancella_mappa();                                                    // Linea 559
static void menu_impostazione_mappa();                                           // Linea 584
static void inserisci_zona();                                                    // Linea 627
static void cancella_zona();                                                     // Linea 739
static void stampa_mappa();                                                      // Linea 796
static void stampa_zona();                                                       // Linea 838
static void chiudi_mappa();                                                      // Linea 869
static char* tipo_zona(Tipo_zona tipo);                                          // Linea 884
static char* tipo_nemico(Tipo_nemico tipo);                                      // Linea 900
static char* tipo_oggetto(Tipo_oggetto tipo);                                    // Linea 910
static char* tipo_mondo(Tipo_mondo tipo);                                        // Linea 921

// Funzioni per gioca()
static void genera_nemici(unsigned short numero_zone_create);                    // Linea 929
static void genera_ordine_giocatori(unsigned short ordine_giocatori[]);          // Linea 977
static void avanza(Giocatore* giocatore, unsigned short* movimento);             // Linea 993
static void indietreggia(Giocatore* giocatore, unsigned short* movimento);       // Linea 1024
static void cambia_mondo(Giocatore* giocatore, unsigned short* movimento);       // Linea 1055
static Nemico* combatti(Giocatore* giocatore, unsigned short* azione);           // Linea 1095
static unsigned short variazione_fortuna(Giocatore* giocatore);                  // Linea 1200
static void attacco_giocatore(Giocatore* giocatore, Nemico* nemico_strutturato); // Linea 1208
static void attacco_nemico(Giocatore* giocatore, Nemico* nemico_strutturato);    // Linea 1226
static void raccogli_oggetto(Giocatore* giocatore, unsigned short* azione);      // Linea 1244
static void utilizza_oggetto(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato);                                      // Linea 1273
static void effetto_bicicletta(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta);             // Linea 1336
static void effetto_maglietta_fuocoinferno(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta); // Linea 1444
static void effetto_bussola(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta);                // Linea 1465
static void effetto_schitarrata_metallica(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta);  // Linea 1538
static void stampa_giocatore(Giocatore* giocatore);                              // Linea 1605
static void stampa_zona_giocatore(Giocatore* giocatore);                         // Linea 1619
static unsigned short posizione_giocatore(Giocatore* giocatore);                 // Linea 1642
static void passa(Giocatore* giocatore);                                         // Linea 1652

int c = 0; // Variabile di controllo input
unsigned short num_giocatori = 0;
unsigned short impostato = 0; // Flag per verificare se il gioco è stato impostato
unsigned short UndiciVirgolaCinque = 0; // Flag per verificare se un giocatore ha scelto di diventare UndiciVirgolaCinque
unsigned short numero_zone_create = 0;
unsigned short demotorzone_presente = 0; // Flag per verificare se Demotorzone è presente
unsigned short turno = 1;
unsigned short vincitore = 0; // Flag per verificare se c'è un vincitore
unsigned short num_morti = 0;
unsigned short totale_nemici_sconfitti = 0;
char nome_iniziale_undicivirgolacinque[25];
char nome_vincitore[25];
char nome_ultimi_tre_vincitori[3][25];
Giocatore *giocatori[4];
Zona_mondoreale* prima_zona_mondoreale = NULL;
Zona_soprasotto* prima_zona_soprasotto = NULL;

void imposta_gioco(){
    if (impostato == 1) {
        printf("ATTENZIONE! Il gioco è già stato impostato in precedenza, la vecchia configurazione verrà eliminata. Reimpostazione nuovo gioco.\n");
        for (int i = 0; i < num_giocatori; i++) {
            deallocazione_giocatore(&giocatori[i]);
        }
        cancella_mappa();
        UndiciVirgolaCinque = 0;
        nome_iniziale_undicivirgolacinque[0] = '\0';
    }

    printf("NUOVA PARTITA INIZIATA.\n\n");
    creazione_giocatori();
    printf("\nTutti i giocatori sono stati impostati correttamente.\nGenerazione automatica della mappa di gioco.\n");
    genera_mappa();
    menu_impostazione_mappa();
    printf("Impostazione del gioco completata. Buon divertimento!\n\n");
}

void gioca() {
    if (impostato == 0) {
        printf("ATTENZIONE! Il gioco non è stato ancora impostato, impostare il gioco prima di iniziare a giocare.\n\n");
        return;
    }
    genera_nemici(numero_zone_create);
    printf("INIZIO DELLA PARTITA!\n\n");

    for (int i = 0; i < num_giocatori; i++) { // Tutti i giocatori iniziano nella prima zona del Mondoreale
        giocatori[i] -> mondo = Mondo_Reale;
        giocatori[i] -> pos_mondoreale = prima_zona_mondoreale;
        giocatori[i] -> pos_soprasotto = prima_zona_soprasotto;
    }

    // Backup iniziale dei giocatori e delle zone per ripristino al termine della partita

    Giocatore* backup_giocatori[num_giocatori];
    for (int i = 0; i < num_giocatori; i++) {
        backup_giocatori[i] = malloc(sizeof(Giocatore));
        strcpy(backup_giocatori[i] -> nome, giocatori[i] -> nome);
        backup_giocatori[i] -> attacco_psichico = giocatori[i] -> attacco_psichico;
        backup_giocatori[i] -> difesa_psichica = giocatori[i] -> difesa_psichica;
        backup_giocatori[i] -> fortuna = giocatori[i] -> fortuna;
        backup_giocatori[i] -> p_vita = giocatori[i] -> p_vita;
    }
        
    Tipo_zona backup_zone_mondoreale[numero_zone_create];
    Tipo_nemico backup_nemici_mondoreale[numero_zone_create];
    Tipo_oggetto backup_oggetti[numero_zone_create];
    Zona_mondoreale* zona_mondoreale = prima_zona_mondoreale;
    for (int i = 0; i < numero_zone_create; i++) {
        backup_zone_mondoreale[i] = zona_mondoreale -> tipo;
        backup_nemici_mondoreale[i] = zona_mondoreale -> nemico;
        backup_oggetti[i] = zona_mondoreale -> oggetto;
        zona_mondoreale = zona_mondoreale -> avanti;
    }

    Tipo_zona backup_zone_soprasotto[numero_zone_create];
    Tipo_nemico backup_nemici_soprasotto[numero_zone_create];
    Zona_soprasotto* zona_soprasotto = prima_zona_soprasotto;
    for (int i = 0; i < numero_zone_create; i++) {
        backup_zone_soprasotto[i] = zona_soprasotto -> tipo;
        backup_nemici_soprasotto[i] = zona_soprasotto -> nemico;
        zona_soprasotto = zona_soprasotto -> avanti;
    }

    // Inizio partita
    
    unsigned short ordine_giocatori[num_giocatori];
    printf("L'ordine di gioco dei giocatori è generato casualmente ad ogni turno.\n");
    printf("Ogni giocatore ha 100 punti vita all'inizio della partita.\n\n");
    printf("IMPORTANTE: Ogni giocatore può eseguire DUE azioni per turno (combattere, usare un oggetto, prendere un oggetto) e UN SOLO movimento (avanzare, tornare indietro, cambiare mondo).\n\n");

    do {
        Nemico* nemico_corrente = NULL;
        genera_ordine_giocatori(ordine_giocatori);
        printf("\nORDINE GIOCATORI PER IL %d° TURNO:\n", turno);
        for (int i = 0; i < num_giocatori && vincitore == 0; i++) {
            printf("%d. %s", i + 1, giocatori[ordine_giocatori[i]] -> nome);
            if (giocatori[ordine_giocatori[i]] -> p_vita == 0) {
                printf(" (morto)\n");
                continue;
            }
            printf("\n");
        }
        printf("\n");

        for (int i = 0; i < num_giocatori && vincitore == 0; i++) {
            unsigned short indice_giocatore = ordine_giocatori[i];
            if (giocatori[indice_giocatore] -> p_vita == 0) {
                printf("\n- %s è morto, il turno passerà al prossimo giocatore.\n\n", giocatori[indice_giocatore] -> nome);
                continue;
            }

            unsigned short scelta = 0;
            unsigned short movimento = 0; // avanza, indietreggia, cambia mondo (bicicletta non conta come movimento ma come usa oggetto)
            unsigned short azione = 0; // combatti, usa oggetto, prendi oggetto

            do {
                scelta = 10;
                printf("\n- Turno di %s:\n", giocatori[indice_giocatore] -> nome);
                printf("    Movimento disponibile: %d\n    Azioni disponibili: %d\n\n", 1 - movimento, 2 - azione);
                printf("Scegliere un'azione da eseguire:\n");
                if (movimento == 0) {
                    printf("0. Avanzare nella zona successiva\n");
                    printf("1. Tornare nella zona precedente\n");
                    printf("2. Cambiare mondo (Mondoreale <-> Soprasotto)\n");
                } else {
                    printf("0. (Movimento già effettuato)\n");
                    printf("1. (Movimento già effettuato)\n");
                    printf("2. (Movimento già effettuato)\n");
                }
                if (azione < 2) {
                    printf("3. Combattere il nemico nella zona attuale\n");
                    printf("4. Raccogliere l'oggetto presente nella zona attuale\n");
                    printf("5. Usare un oggetto dallo zaino\n");
                } else {
                    printf("3. (Azioni già effettuate)\n");
                    printf("4. (Azioni già effettuate)\n");
                    printf("5. (Azioni già effettuate)\n");
                }
                printf("6. Stampare le proprie informazioni\n");
                printf("7. Stampare le informazioni della zona attuale\n");
                printf("8. Passare / Terminare il turno\n");
                printf("9. Abbandonare la partita (il giocatore verrà considerato morto)\n");
                printf(">> ");
                scanf("%hu", &scelta);
                svuotaBuffer();

                switch(scelta) {
                    case 0:
                        avanza(giocatori[indice_giocatore], &movimento);
                        break;
                    case 1:
                        indietreggia(giocatori[indice_giocatore], &movimento);
                        break;
                    case 2:
                        cambia_mondo(giocatori[indice_giocatore], &movimento);
                        break;
                    case 3:
                        nemico_corrente = combatti(giocatori[indice_giocatore], &azione);
                        break;
                    case 4:
                        raccogli_oggetto(giocatori[indice_giocatore], &azione);
                        break;
                    case 5:
                        utilizza_oggetto(giocatori[indice_giocatore], &azione, NULL);
                        break;
                    case 6:
                        stampa_giocatore(giocatori[indice_giocatore]);
                        break;
                    case 7:
                        stampa_zona_giocatore(giocatori[indice_giocatore]);
                        break;
                    case 8:
                        passa(giocatori[indice_giocatore]);
                        break;
                    case 9:
                        printf("%s ha deciso di abbandonare la partita e viene considerato morto.\n", giocatori[indice_giocatore] -> nome);
                        giocatori[indice_giocatore] -> p_vita = 0;
                        break;
                    default:
                        printf("ATTENZIONE! Comando non riconosciuto, inserire un comando tra 0 e 9.\n");
                }
            } while ((scelta != 8 && giocatori[indice_giocatore] -> p_vita > 0) && vincitore == 0);
            if (giocatori[indice_giocatore] -> p_vita == 0) {
                num_morti++;
            }
            if (vincitore == 0) {
                printf("\n%s ha completato il suo turno.\n\n", giocatori[indice_giocatore] -> nome);
            }
        }
        if (nemico_corrente != NULL) {
            nemico_corrente -> nemico_sconfitto = 0;
        }
        turno++;
    } while (vincitore == 0 && num_morti != num_giocatori);

    if (vincitore != 0) {
        printf("%s VINCE LA PARTITA!\n\nPARTITA CONCLUSA.\n", nome_vincitore);
    } else {
        printf("TUTTI I GIOCATORI SONO MORTI.\n\nPARTITA CONCLUSA.\n");
    }

    // Ripristino delle variabili, dei giocatori e delle zone al termine del gioco

    for (int i = 0; i < num_giocatori; i++) {
        strcpy(giocatori[i] -> nome, backup_giocatori[i] -> nome);
        giocatori[i] -> attacco_psichico = backup_giocatori[i] -> attacco_psichico;
        giocatori[i] -> difesa_psichica = backup_giocatori[i] -> difesa_psichica;
        giocatori[i] -> fortuna = backup_giocatori[i] -> fortuna;
        giocatori[i] -> p_vita = backup_giocatori[i] -> p_vita;
        for (int j = 0; j < 3; j++) {
            giocatori[i] -> zaino[j] = nessun_oggetto;
        }
        free(backup_giocatori[i]);
        backup_giocatori[i] = NULL;
    }

    zona_mondoreale = prima_zona_mondoreale;
    for (int i = 0; i < numero_zone_create; i++) {
        zona_mondoreale -> tipo = backup_zone_mondoreale[i];
        zona_mondoreale -> nemico = backup_nemici_mondoreale[i];
        zona_mondoreale -> oggetto = backup_oggetti[i];
        zona_mondoreale = zona_mondoreale -> avanti;
    }

    zona_soprasotto = prima_zona_soprasotto;
    for (int i = 0; i < numero_zone_create; i++) {
        zona_soprasotto -> tipo = backup_zone_soprasotto[i];
        zona_soprasotto -> nemico = backup_nemici_soprasotto[i];
        zona_soprasotto = zona_soprasotto -> avanti;
    }

    turno = 1;
    vincitore = 0;
    num_morti = 0;
    strcpy(nome_vincitore, "");
}

void termina_gioco() {
    for (int i = 0; i < num_giocatori; i++) {
        if (giocatori[i] != NULL) {
            if (strcmp(giocatori[i] -> nome, "UndiciVirgolaCinque") == 0) {
                strcpy(giocatori[i] -> nome, nome_iniziale_undicivirgolacinque);
            }
            printf("Grazie per aver giocato %s!\n", giocatori[i] -> nome);
            deallocazione_giocatore(&giocatori[i]);
        }
    }
    printf("\nGIOCO TERMINATO.\n\n");
}

void crediti() {
    printf("CREDITI DEL GIOCO \"COSESTRANE\":\n\n");
    printf("- Creatore: Andrea Allegrucci\n");
    printf("- Ultimi tre vincitori delle partite precedenti:\n");
    for (int i = 0; i < 3; i++) {
        if (strcmp(nome_ultimi_tre_vincitori[i], "") != 0) {
            printf("    %d. %s\n", i + 1, nome_ultimi_tre_vincitori[i]);
        } else {
            printf("    %d. Nessun vincitore registrato\n", i + 1);
        }
    }
    printf("- Numero totale di nemici sconfitti in tutte le partite: %hu\n\n", totale_nemici_sconfitti);
}

void svuotaBuffer() {
    while ((c = getchar()) != '\n' && c != EOF);
}

static void creazione_giocatori() {
    do {
        printf("Inserire il numero di giocatori (da 1 a 4):\n>> ");
        scanf("%hu", &num_giocatori);
        svuotaBuffer();
        if (num_giocatori == 0 || num_giocatori > 4)
          printf("ATTENZIONE! Numero di giocatori non valido.\n\n");
    } while(num_giocatori == 0 || num_giocatori > 4);

    for(int i = 0; i < num_giocatori; i++) {
        giocatori[i] = malloc(sizeof(Giocatore));
        do {
            printf("\nInserire il nome del Giocatore %d (Max 24 caratteri):\n>> ", i+1);
            fgets(giocatori[i] -> nome, 25, stdin);
            giocatori[i] -> nome[strcspn(giocatori[i] -> nome, "\n")] = 0; // Per rimuovere eventuali newline come quello finale
            if (strcmp(giocatori[i] -> nome, "") == 0){
                printf("ATTENZIONE! Il nome non può essere vuoto.\n");
            }
            if (strlen(giocatori[i] -> nome) > 24) {
                printf("ATTENZIONE! Il nome inserito è troppo lungo.\n");
                strcpy(giocatori[i] -> nome, "");
            }
            if (strcmp(giocatori[i] -> nome, "UndiciVirgolaCinque") == 0) {
                printf("ATTENZIONE! Il nome 'UndiciVirgolaCinque' è riservato, scegliere un altro nome.\n");
                strcpy(giocatori[i] -> nome, "");
            }
            for (int j = 0; j < i; j++) {
                if (strcmp(giocatori[i] -> nome, giocatori[j] -> nome) == 0) {
                    printf("ATTENZIONE! Il nome inserito è già stato scelto da un altro giocatore, scegliere un altro nome.\n");
                    strcpy(giocatori[i] -> nome, "");
                    break;
                }
            }
            if (strcmp(giocatori[i] -> nome, nome_iniziale_undicivirgolacinque) == 0) {
                printf("ATTENZIONE! Il nome è già stato scelto da un altro giocatore, scegliere un altro nome.\n");
                strcpy(giocatori[i] -> nome, "");
            }
        } while(strcmp(giocatori[i] -> nome, "") == 0 || strcmp(giocatori[i] -> nome, "UndiciVirgolaCinque") == 0);
        generazione_valori_giocatore(giocatori[i]);
        giocatori[i] -> p_vita = 100;
        printf("Giocatore %d impostato correttamente.\n", i+1);
  }
}

static void generazione_valori_giocatore(Giocatore* giocatore) {
    unsigned short scelta = 0;
    time_t t; // Variabile per la generazione casuale
    srand((unsigned)time(&t));

    giocatore -> attacco_psichico = (unsigned char)(rand() % 20 + 1); // Valori tra 1 e 20
    giocatore -> difesa_psichica = (unsigned char)(rand() % 20 + 1);
    giocatore -> fortuna = (unsigned char)(rand() % 20 + 1);
    for(int i = 0; i < 3; i++) {
        giocatore -> zaino[i] = nessun_oggetto;
    }

    stampa_valori_giocatore(giocatore);

    do {
        printf("Scegliere se si vogliono effettuare le seguenti modifiche ai propri valori:\n");
        printf("1. Attacco Psichico aumenta di 3, Difesa Psichica diminuisce di 3\n");
        printf("2. Difesa Psichica aumenta di 3, Attacco Psichico diminuisce di 3\n");
        printf("3. Nessuna modifica\n");
        printf("4. Diventare UndiciVirgolaCinque: Attacco Psichico e Difesa Psichica aumentano entrambi di 4 ma Fortuna diminuisce di 7 (SOLO UN giocatore a partita può diventare UndiciVirgolaCinque)\n");
        printf(">> ");
        scanf("%hu", &scelta);
        svuotaBuffer();

        switch(scelta) {
            case 1:
                if (giocatore -> attacco_psichico > 17) {
                    printf("ATTENZIONE! Non è possibile aumentare di 3 l'Attacco Psichico, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                if (giocatore -> difesa_psichica < 4) {
                    printf("ATTENZIONE! Non è possibile diminuire di 3 la Difesa Psichica, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                giocatore -> attacco_psichico += 3;
                giocatore -> difesa_psichica -= 3;
                break;

            case 2:
                if (giocatore -> difesa_psichica > 17) {
                    printf("ATTENZIONE! Non è possibile aumentare di 3 la Difesa Psichica, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                if (giocatore -> attacco_psichico < 4) {
                    printf("ATTENZIONE! Non è possibile diminuire di 3 l'Attacco Psichico, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                giocatore -> difesa_psichica += 3;
                giocatore -> attacco_psichico -= 3;
                break;

            case 3:
                printf("Nessuna modifica selezionata.\n");
                break;

            case 4:
                if (UndiciVirgolaCinque) {
                    printf("ATTENZIONE! Un altro giocatore ha già scelto di diventare UndiciVirgolaCinque, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                if (giocatore -> attacco_psichico > 16 || giocatore -> difesa_psichica > 16) {
                    printf("ATTENZIONE! Non è possibile aumentare di 4 l'Attacco Psichico o la Difesa Psichica, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                if (giocatore -> fortuna < 8) {
                    printf("ATTENZIONE! Non è possibile diminuire di 7 la Fortuna, scegliere un'altra opzione.\n\n");
                    scelta = 0;
                    break;
                }
                printf("\nGiocatore %s diventa UndiciVirgolaCinque.", giocatore -> nome);
                UndiciVirgolaCinque = 1;
                strcpy(nome_iniziale_undicivirgolacinque, giocatore -> nome);
                strcpy(giocatore -> nome, "UndiciVirgolaCinque");
                giocatore -> attacco_psichico += 4;
                giocatore -> difesa_psichica += 4;
                giocatore -> fortuna -= 7;
                break;

            default:
                printf("ATTENZIONE! Comando non riconosciuto, inserire un numero tra 1 e 4.\n\n");
                scelta = 0;
                break;
        }
    } while (scelta == 0 || scelta > 4);
    stampa_valori_giocatore(giocatore);
}

static void stampa_valori_giocatore(Giocatore* giocatore) {
    printf("\n- Valori giocatore %s:\n", giocatore -> nome);
    printf("    Attacco Psichico: %d\n", giocatore -> attacco_psichico);
    printf("    Difesa Psichica: %d\n", giocatore -> difesa_psichica);
    printf("    Fortuna: %d\n\n", giocatore -> fortuna);
}

static void deallocazione_giocatore(Giocatore** giocatore) {
    if (giocatore != NULL && *giocatore != NULL) {
        free(*giocatore);
        *giocatore = NULL;
    }
}

static void genera_mappa() {
    cancella_mappa();

    time_t t; // Variabile per la generazione casuale
    srand((unsigned)time(&t));
    unsigned short nemico_casuale = 0;

    for(int i = 0; i < 15; i++) {
        Zona_mondoreale* nuova_zona_mondoreale = malloc(sizeof(Zona_mondoreale));
        Zona_soprasotto* nuova_zona_soprasotto = malloc(sizeof(Zona_soprasotto));

        nuova_zona_mondoreale -> tipo = (Tipo_zona)(rand() % 10);
        nuova_zona_soprasotto -> tipo = nuova_zona_mondoreale -> tipo;

        nemico_casuale = rand() % 4;
        if (nemico_casuale == 3) {
            nemico_casuale = 0;
        }
        nuova_zona_mondoreale -> nemico = (Tipo_nemico)nemico_casuale;

        nemico_casuale = rand() % 4;
        if (nemico_casuale == 1 || (demotorzone_presente == 1 && nemico_casuale == 3)) {
            nemico_casuale = 0;
        }
        nuova_zona_soprasotto -> nemico = (Tipo_nemico)nemico_casuale;

        if (nemico_casuale == demotorzone) {
            demotorzone_presente = 1;
        }

        nuova_zona_mondoreale -> oggetto = (Tipo_oggetto)(rand() % 5);

        if (prima_zona_mondoreale == NULL) {
            prima_zona_mondoreale = nuova_zona_mondoreale;
            prima_zona_soprasotto = nuova_zona_soprasotto;

            nuova_zona_mondoreale -> avanti = NULL;
            nuova_zona_mondoreale -> indietro = NULL;

            nuova_zona_soprasotto -> avanti = NULL;
            nuova_zona_soprasotto -> indietro = NULL;
        } else {
            Zona_mondoreale* temp_mondo_reale = prima_zona_mondoreale;
            while (temp_mondo_reale -> avanti != NULL) {
                temp_mondo_reale = temp_mondo_reale -> avanti;
            }
            temp_mondo_reale -> avanti = nuova_zona_mondoreale;
            nuova_zona_mondoreale -> indietro = temp_mondo_reale;
            nuova_zona_mondoreale -> avanti = NULL;

            Zona_soprasotto* temp_soprasotto = prima_zona_soprasotto;
            while (temp_soprasotto -> avanti != NULL) {
                temp_soprasotto = temp_soprasotto -> avanti;
            }
            temp_soprasotto -> avanti = nuova_zona_soprasotto;
            nuova_zona_soprasotto -> indietro = temp_soprasotto;
            nuova_zona_soprasotto -> avanti = NULL;
        }
        nuova_zona_mondoreale -> link_soprasotto = nuova_zona_soprasotto;
        nuova_zona_soprasotto -> link_mondoreale = nuova_zona_mondoreale;

        nuova_zona_mondoreale -> nemico_strutturato.nemico = nessun_nemico;
        nuova_zona_mondoreale -> nemico_strutturato.attacco_psichico = 0;
        nuova_zona_mondoreale -> nemico_strutturato.p_vita = 0;
        nuova_zona_mondoreale -> nemico_strutturato.nemico_sconfitto = 0;

        nuova_zona_soprasotto -> nemico_strutturato.nemico = nessun_nemico;
        nuova_zona_soprasotto -> nemico_strutturato.attacco_psichico = 0;
        nuova_zona_soprasotto -> nemico_strutturato.p_vita = 0;
        nuova_zona_soprasotto -> nemico_strutturato.nemico_sconfitto = 0;
    }

    if (!demotorzone_presente) { // Forzare l'inserimento di Demotorzone in una zona casuale del soprasotto
        unsigned short zona_casuale = rand() % 15;
        Zona_soprasotto* temp_soprasotto = prima_zona_soprasotto;
        for (int j = 0; j < zona_casuale; j++) {
            temp_soprasotto = temp_soprasotto -> avanti;
        }
        temp_soprasotto -> nemico = demotorzone;
        demotorzone_presente = 1;
    }
    
    numero_zone_create = 15;
    printf("\nNuova mappa con 15 zone generata con successo!\n\n");
}

static void cancella_mappa() {
    Zona_mondoreale* zona_mondoreale_da_cancellare = prima_zona_mondoreale;
    Zona_mondoreale* zona_mondoreale_successiva_da_cancellare = NULL;
    while (zona_mondoreale_da_cancellare != NULL) {
        zona_mondoreale_successiva_da_cancellare = zona_mondoreale_da_cancellare -> avanti;
        zona_mondoreale_da_cancellare -> link_soprasotto = NULL;
        free(zona_mondoreale_da_cancellare);
        zona_mondoreale_da_cancellare = zona_mondoreale_successiva_da_cancellare;
    }
    prima_zona_mondoreale = NULL;

    Zona_soprasotto* zona_soprasotto_da_cancellare = prima_zona_soprasotto;
    Zona_soprasotto* zona_soprasotto_successiva_da_cancellare = NULL;
    while (zona_soprasotto_da_cancellare != NULL) {
        zona_soprasotto_successiva_da_cancellare = zona_soprasotto_da_cancellare -> avanti;
        zona_soprasotto_da_cancellare -> link_mondoreale = NULL;
        free(zona_soprasotto_da_cancellare);
        zona_soprasotto_da_cancellare = zona_soprasotto_successiva_da_cancellare;
    }
    prima_zona_soprasotto = NULL;

    numero_zone_create = 0;
    demotorzone_presente = 0;
}

static void menu_impostazione_mappa() {
    unsigned short scelta = 0;

  do {
    printf("\nMENU DI IMPOSTAZIONE DELLA MAPPA:\n");
    printf("1. Generare una nuova mappa (Quella precedente viene eliminata)\n");
    printf("2. Inserire una zona in una determinata posizione della mappa\n");
    printf("3. Cancellare una zona in una determinata posizione della mappa\n");
    printf("4. Stampare la mappa (Mondoreale o Soprasotto)\n");
    printf("5. Stampare i dettagli di una zona specifica della mappa (Mondoreale e Soprasotto)\n");
    printf("6. Chiudere il menu di impostazione della mappa e tornare al menu principale\n");
    printf(">> ");
    scanf("%hu", &scelta);
    svuotaBuffer();

    switch(scelta) {
        case 1:
            genera_mappa();
            break;
        case 2:
            inserisci_zona();
            break;
        case 3:
            cancella_zona();
            break;
        case 4:
            stampa_mappa();
            break;
        case 5:
            stampa_zona();
            break;
        case 6:
            chiudi_mappa();
            if(!impostato) {
            scelta = 0;
            }
            break;
        default:
            printf("ATTENZIONE! Comando non riconosciuto, inserire un comando valido.\n");
    }
  } while(scelta != 6);
}

static void inserisci_zona() {
    unsigned short i;
    unsigned short nemico_mondoreale = 0;
    unsigned short nemico_soprasotto = 0;
    unsigned short nuovo_oggetto = 0;

    do {
        printf("\nInserire la posizione della nuova zona da inserire (1 - %d):\n>> ", numero_zone_create + 1);
        scanf("%hu", &i);
        svuotaBuffer();
        if (i < 1 || i > numero_zone_create + 1) {
            printf("ATTENZIONE! Posizione non valida.\n");
        }
    } while (i < 1 || i > numero_zone_create + 1);

    do {
        printf("\nInserire il tipo di nemico per la nuova zona nel Mondoreale\n0. nessun_nemico\n1. billi\n2. democane\n>> ");
        scanf("%hu", &nemico_mondoreale);
        svuotaBuffer();
        if (nemico_mondoreale > 2) {
            printf("ATTENZIONE! Tipo di nemico non valido\n");
        }
    } while (nemico_mondoreale > 2);

    do {
        printf("\nInserisci il tipo di oggetto per la nuova zona nel Mondoreale\n0. nessun_oggetto\n1. bicicletta\n2. maglietta_fuocoinferno\n3. bussola\n4. schitarrata_metallica\n>> ");
        scanf("%hu", &nuovo_oggetto);
        svuotaBuffer();
        if (nuovo_oggetto > 4) {
            printf("ATTENZIONE! Tipo di oggetto non valido\n");
        }
    } while (nuovo_oggetto > 4);

    do {
        printf("\nInserire il tipo di nemico per la nuova zona nel Soprasotto\n0. nessun_nemico\n1. demotorzone\n2. democane\n>> ");
        scanf("%hu", &nemico_soprasotto);
        svuotaBuffer();
        if (nemico_soprasotto > 2) {
            printf("ATTENZIONE! Tipo di nemico non valido\n");
        }
        if (nemico_soprasotto == 1 && demotorzone_presente == 1) {
            printf("ATTENZIONE! Demotorzone già presente nella mappa, scegliere un altro tipo di nemico.\n");
            nemico_soprasotto = 3;
        }
    } while (nemico_soprasotto > 2);

    if (nemico_soprasotto == 1) {
        nemico_soprasotto = demotorzone;
        demotorzone_presente = 1;
    }

    Zona_mondoreale* nuova_zona_mondoreale = malloc(sizeof(Zona_mondoreale));
    Zona_soprasotto* nuova_zona_soprasotto = malloc(sizeof(Zona_soprasotto));
    nuova_zona_mondoreale -> tipo = (Tipo_zona)(rand() % 10);
    nuova_zona_soprasotto -> tipo = nuova_zona_mondoreale -> tipo;
    nuova_zona_mondoreale -> nemico = (Tipo_nemico)nemico_mondoreale;
    nuova_zona_soprasotto -> nemico = (Tipo_nemico)nemico_soprasotto;
    nuova_zona_mondoreale -> oggetto = (Tipo_oggetto)(nuovo_oggetto);
    nuova_zona_mondoreale -> link_soprasotto = nuova_zona_soprasotto;
    nuova_zona_soprasotto -> link_mondoreale = nuova_zona_mondoreale;

    nuova_zona_mondoreale -> nemico_strutturato.nemico = nessun_nemico;
    nuova_zona_mondoreale -> nemico_strutturato.attacco_psichico = 0;
    nuova_zona_mondoreale -> nemico_strutturato.p_vita = 0;
    nuova_zona_mondoreale -> nemico_strutturato.nemico_sconfitto = 0;

    nuova_zona_soprasotto -> nemico_strutturato.nemico = nessun_nemico;
    nuova_zona_soprasotto -> nemico_strutturato.attacco_psichico = 0;
    nuova_zona_soprasotto -> nemico_strutturato.p_vita = 0;
    nuova_zona_soprasotto -> nemico_strutturato.nemico_sconfitto = 0;

    if (i == 1) { // Inserimento in testa
        nuova_zona_mondoreale -> avanti = prima_zona_mondoreale;
        nuova_zona_mondoreale -> indietro = NULL;
        if (prima_zona_mondoreale != NULL) {
            prima_zona_mondoreale -> indietro = nuova_zona_mondoreale;
        }
        prima_zona_mondoreale = nuova_zona_mondoreale;

        nuova_zona_soprasotto -> avanti = prima_zona_soprasotto;
        nuova_zona_soprasotto -> indietro = NULL;
        if (prima_zona_soprasotto != NULL) {
            prima_zona_soprasotto -> indietro = nuova_zona_soprasotto;
        }
        prima_zona_soprasotto = nuova_zona_soprasotto;
    } else { // Inserimento in coda o in mezzo
        Zona_mondoreale* temp_mondo_reale = prima_zona_mondoreale;
        for (unsigned short j = 1; j < i - 1; j++) {
            temp_mondo_reale = temp_mondo_reale -> avanti;
        }
        nuova_zona_mondoreale -> avanti = temp_mondo_reale -> avanti;
        nuova_zona_mondoreale -> indietro = temp_mondo_reale;
        if (temp_mondo_reale -> avanti != NULL) {
            temp_mondo_reale -> avanti -> indietro = nuova_zona_mondoreale;
        }
        temp_mondo_reale -> avanti = nuova_zona_mondoreale;

        Zona_soprasotto* temp_soprasotto = prima_zona_soprasotto;
        for (unsigned short j = 1; j < i - 1; j++) {
            temp_soprasotto = temp_soprasotto -> avanti;
        }
        nuova_zona_soprasotto -> avanti = temp_soprasotto -> avanti;
        nuova_zona_soprasotto -> indietro = temp_soprasotto;
        if (temp_soprasotto -> avanti != NULL) {
            temp_soprasotto -> avanti -> indietro = nuova_zona_soprasotto;
        }
        temp_soprasotto -> avanti = nuova_zona_soprasotto;
    }
    numero_zone_create++;
    printf("Nuova zona inserita correttamente in posizione %d.\n\n", i);
}

static void cancella_zona() {
    unsigned short i;

    if (numero_zone_create == 0) {
        printf("ATTENZIONE! Non ci sono zone da cancellare nella mappa.\n\n");
        return;
    }

    do {
        printf("\nInserire la posizione della zona da cancellare (1 - %d):\n>> ", numero_zone_create);
        scanf("%hu", &i);
        svuotaBuffer();
        if (i < 1 || i > numero_zone_create) {
            printf("ATTENZIONE! Posizione non valida.\n");
        }
    } while (i < 1 || i > numero_zone_create);

    Zona_mondoreale* zona_mondoreale_da_cancellare = prima_zona_mondoreale;
    Zona_soprasotto* zona_soprasotto_da_cancellare = prima_zona_soprasotto;

    for (unsigned short j = 1; j < i; j++) {
        zona_mondoreale_da_cancellare = zona_mondoreale_da_cancellare -> avanti;
        zona_soprasotto_da_cancellare = zona_soprasotto_da_cancellare -> avanti;
    }

    // Ricollegamento della lista del Mondoreale
    if (zona_mondoreale_da_cancellare -> indietro != NULL) {
        zona_mondoreale_da_cancellare -> indietro -> avanti = zona_mondoreale_da_cancellare -> avanti;
    } else {
        prima_zona_mondoreale = zona_mondoreale_da_cancellare -> avanti;
    }
    if (zona_mondoreale_da_cancellare -> avanti != NULL) {
        zona_mondoreale_da_cancellare -> avanti -> indietro = zona_mondoreale_da_cancellare -> indietro;
    }

    // Ricollegamento della lista del Soprasotto
    if (zona_soprasotto_da_cancellare -> indietro != NULL) {
        zona_soprasotto_da_cancellare -> indietro -> avanti = zona_soprasotto_da_cancellare -> avanti;
    } else {
        prima_zona_soprasotto = zona_soprasotto_da_cancellare -> avanti;
    }
    if (zona_soprasotto_da_cancellare -> avanti != NULL) {
        zona_soprasotto_da_cancellare -> avanti -> indietro = zona_soprasotto_da_cancellare -> indietro;
    }

    if (zona_soprasotto_da_cancellare -> nemico == demotorzone) {
        demotorzone_presente = 0;
    }

    zona_mondoreale_da_cancellare -> link_soprasotto = NULL;
    zona_soprasotto_da_cancellare -> link_mondoreale = NULL;
    free(zona_mondoreale_da_cancellare);
    free(zona_soprasotto_da_cancellare);
    numero_zone_create--;
    printf("Zona in posizione %d cancellata correttamente.\n\n", i);
}

static void stampa_mappa() {
    unsigned short scelta = 0;
    unsigned short i = 1;

    if (numero_zone_create == 0) {
        printf("ATTENZIONE! Mappa vuota, non ci sono zone da stampare.\n");
        return;
    }

    do {
        printf("\nScegliere quale mappa stampare:\n1. Mondoreale\n2. Soprasotto\n>> ");
        scanf("%hu", &scelta);
        svuotaBuffer();

        switch(scelta) {
            case 1: {
                printf("\nMAPPA MONDOREALE:\n\n");
                Zona_mondoreale* zona_mondoreale_da_stampare = prima_zona_mondoreale;
                do {
                    printf("- Zona %d:\n    Tipo: %s\n   Nemico: %s\n   Oggetto: %s\n\n", i, tipo_zona(zona_mondoreale_da_stampare -> tipo), tipo_nemico(zona_mondoreale_da_stampare -> nemico), tipo_oggetto(zona_mondoreale_da_stampare -> oggetto));
                    zona_mondoreale_da_stampare = zona_mondoreale_da_stampare -> avanti;
                    i++;
                } while (zona_mondoreale_da_stampare != NULL);
                break;
            }
            case 2: {
                printf("\nMAPPA SOPRASOTTO:\n\n");
                Zona_soprasotto* zona_soprasotto_da_stampare = prima_zona_soprasotto;
                do {
                    printf("- Zona %d:\n    Tipo: %s\n   Nemico: %s\n\n", i, tipo_zona(zona_soprasotto_da_stampare -> tipo), tipo_nemico(zona_soprasotto_da_stampare -> nemico));
                    zona_soprasotto_da_stampare = zona_soprasotto_da_stampare -> avanti;
                    i++;
                } while (zona_soprasotto_da_stampare != NULL);
                break;
            }
            default:
                printf("ATTENZIONE! Comando non riconosciuto.\n");
                scelta = 0;
        }
    } while (scelta == 0 || scelta > 2);
}

static void stampa_zona() {
    unsigned short posizione = 0;
    unsigned short i = 0;

    if (numero_zone_create == 0) {
        printf("ATTENZIONE! Mappa vuota, non ci sono zone da stampare.\n");
        return;
    }

    do {
        printf("\nInserire la posizione della zona da stampare (1 - %d):\n>> ", numero_zone_create);
        scanf("%hu", &posizione);
        svuotaBuffer();
        if (posizione < 1 || posizione > numero_zone_create) {
            printf("ATTENZIONE! Posizione non valida.\n\n");
        }
    } while (posizione < 1 || posizione > numero_zone_create);

    Zona_mondoreale* zona_mondoreale_da_stampare = prima_zona_mondoreale;
    Zona_soprasotto* zona_soprasotto_da_stampare = prima_zona_soprasotto;

    for (i = 1; i < posizione; i++) {
        zona_mondoreale_da_stampare = zona_mondoreale_da_stampare -> avanti;
        zona_soprasotto_da_stampare = zona_soprasotto_da_stampare -> avanti;
    }

    printf("\nDettagli Zona %d:\n", posizione);
    printf("- Mondoreale:\n    Tipo: %s\n   Nemico: %s\n   Oggetto: %s\n", tipo_zona(zona_mondoreale_da_stampare -> tipo), tipo_nemico(zona_mondoreale_da_stampare -> nemico), tipo_oggetto(zona_mondoreale_da_stampare -> oggetto));
    printf("- Soprasotto:\n    Tipo: %s\n   Nemico: %s\n\n", tipo_zona(zona_soprasotto_da_stampare -> tipo), tipo_nemico(zona_soprasotto_da_stampare -> nemico));
}

static void chiudi_mappa() {
    if (demotorzone_presente == 0) {
        printf("ATTENZIONE! Non è possibile chiudere il menu di impostazione della mappa perché Demotorzone non è presente nella mappa del Soprasotto.\nInserire una nuova zona con Demotorzone o generare una nuova mappa.\n\n");
        return;
    }
    if (numero_zone_create < 15) {
        printf("ATTENZIONE! Non è possibile chiudere il menu di impostazione della mappa perché ci sono solo %hu zone nella mappa e devono essere almeno 15.\n", numero_zone_create);
        printf("Inserire nuove zone o generare una nuova mappa.\n\n");
        return;
    }

    impostato = 1;
    printf("Chiusura del menu di impostazione della mappa completata.\n\n");
}

static char* tipo_zona(Tipo_zona tipo) {
    switch(tipo) {
        case 0: return "bosco";
        case 1: return "scuola";
        case 2: return "laboratorio";
        case 3: return "caverna";
        case 4: return "strada";
        case 5: return "giardino";
        case 6: return "supermercato";
        case 7: return "centrale_elettrica";
        case 8: return "deposito_abbandonato";
        case 9: return "stazione_polizia";
    }
    return "";
}

static char* tipo_nemico(Tipo_nemico tipo) {
    switch(tipo) {
        case 0: return "nessun_nemico";
        case 1: return "billi";
        case 2: return "democane";
        case 3: return "demotorzone";
    }
    return "";
}

static char* tipo_oggetto(Tipo_oggetto tipo) {
    switch(tipo) {
        case 0: return "nessun_oggetto";
        case 1: return "bicicletta";
        case 2: return "maglietta_fuocoinferno";
        case 3: return "bussola";
        case 4: return "schitarrata_metallica";
    }
    return "";
}

static char* tipo_mondo(Tipo_mondo tipo) {
    switch(tipo) {
        case 0: return "Mondoreale";
        case 1: return "Soprasotto";
    }
    return "";
}

static void genera_nemici(unsigned short numero_zone_create) {
    Zona_mondoreale* zona_mondoreale_corrente = prima_zona_mondoreale;
    Zona_soprasotto* zona_soprasotto_corrente = prima_zona_soprasotto;

    for (unsigned short i = 0; i < numero_zone_create; i++) {
        if (zona_mondoreale_corrente -> nemico != nessun_nemico) {
            Nemico* nemico_strutturato_mondoreale = &(zona_mondoreale_corrente -> nemico_strutturato);
            nemico_strutturato_mondoreale -> nemico = zona_mondoreale_corrente -> nemico;

            switch (nemico_strutturato_mondoreale -> nemico) {
                case billi:
                    nemico_strutturato_mondoreale -> attacco_psichico = ATT_BILLI;
                    nemico_strutturato_mondoreale -> p_vita = PV_BILLI;
                    nemico_strutturato_mondoreale -> nemico_sconfitto = 0;
                    break;
                case democane:
                    nemico_strutturato_mondoreale -> attacco_psichico = ATT_DEMOCANE;
                    nemico_strutturato_mondoreale -> p_vita = PV_DEMOCANE;
                    nemico_strutturato_mondoreale -> nemico_sconfitto = 0;
                    break;
                default:
                    break;
            }
        }
        if (zona_soprasotto_corrente -> nemico != nessun_nemico) {
            Nemico* nemico_strutturato_soprasotto = &(zona_soprasotto_corrente -> nemico_strutturato);
            nemico_strutturato_soprasotto -> nemico = zona_soprasotto_corrente -> nemico;

            switch (nemico_strutturato_soprasotto -> nemico) {
                case democane:
                    nemico_strutturato_soprasotto -> attacco_psichico = ATT_DEMOCANE;
                    nemico_strutturato_soprasotto -> p_vita = PV_DEMOCANE;
                    nemico_strutturato_soprasotto -> nemico_sconfitto = 0;
                    break;
                case demotorzone:
                    nemico_strutturato_soprasotto -> attacco_psichico = ATT_DEMOTORZONE;
                    nemico_strutturato_soprasotto -> p_vita = PV_DEMOTORZONE;
                    nemico_strutturato_soprasotto -> nemico_sconfitto = 0;
                    break;
                default:
                    break;
            }
        }
        zona_mondoreale_corrente = zona_mondoreale_corrente -> avanti;
        zona_soprasotto_corrente = zona_soprasotto_corrente -> avanti;
    }
}

static void genera_ordine_giocatori(unsigned short ordine_giocatori[]) {
    time_t t; // Variabile per la generazione casuale
    srand((unsigned)time(&t));

    for (unsigned short i = 0; i < num_giocatori; i++) {
        ordine_giocatori[i] = i;
    }

    for (unsigned short i = num_giocatori - 1; i > 0; i--) {
        unsigned short j = rand() % (i + 1);
        unsigned short temp = ordine_giocatori[i];
        ordine_giocatori[i] = ordine_giocatori[j];
        ordine_giocatori[j] = temp;
    }
}

static void avanza(Giocatore* giocatore, unsigned short* movimento) {
    if (*movimento == 1) {
        printf("ATTENZIONE! Hai già effettuato un movimento in questo turno.\n");
        return;
    }

    if (giocatore -> pos_mondoreale -> avanti == NULL) {
            printf("ATTENZIONE! Non è possibile avanzare, sei già nell'ultima zona.\n");
            return;
    }
        
    if (giocatore -> mondo == Mondo_Reale) {
        if (giocatore -> pos_mondoreale -> nemico != nessun_nemico && !giocatore -> pos_mondoreale -> nemico_strutturato.nemico_sconfitto) {
            printf("ATTENZIONE! Non è possibile avanzare nella zona successiva, prima si deve combattere il nemico nella zona attuale.\n");
            return;
        }
        giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> avanti;
        giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
        printf("%s sei avanzato di una zona.\n", giocatore -> nome);
    } else {
        if (giocatore -> pos_soprasotto -> nemico != nessun_nemico && !giocatore -> pos_soprasotto -> nemico_strutturato.nemico_sconfitto) {
            printf("ATTENZIONE! Non è possibile avanzare nella zona successiva, prima si deve combattere il nemico nella zona attuale.\n");
            return;
        }
        giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> avanti;
        giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
        printf("%s è avanzato di una zona.\n", giocatore -> nome);
    }
    *movimento = 1;
}

static void indietreggia(Giocatore* giocatore, unsigned short* movimento) {
    if (*movimento == 1) {
        printf("ATTENZIONE! Hai già effettuato un movimento in questo turno.\n");
        return;
    }

    if (giocatore -> pos_mondoreale -> indietro == NULL) {
            printf("ATTENZIONE! Non è possibile tornare indietro, sei già nella prima zona.\n");
            return;
    }

    if (giocatore -> mondo == Mondo_Reale) {
        if (giocatore -> pos_mondoreale -> nemico != nessun_nemico && !giocatore -> pos_mondoreale -> nemico_strutturato.nemico_sconfitto) {
            printf("ATTENZIONE! Non è possibile tornare indietro nella zona precedente, prima si deve combattere il nemico nella zona attuale.\n");
            return;
        }
        giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> indietro;
        giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
        printf("%s è tornato indietro di una zona.\n", giocatore -> nome);
    } else {
        if (giocatore -> pos_soprasotto -> nemico != nessun_nemico && !giocatore -> pos_soprasotto -> nemico_strutturato.nemico_sconfitto) {
            printf("ATTENZIONE! Non è possibile tornare indietro nella zona precedente, prima si deve combattere il nemico nella zona attuale.\n");
            return;
        }
        giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> indietro;
        giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
        printf("%s è tornato indietro di una zona.\n", giocatore -> nome);
    }
    *movimento = 1;
}

static void cambia_mondo(Giocatore* giocatore, unsigned short* movimento) {
    if (*movimento == 1) {
        printf("ATTENZIONE! Hai già effettuato un movimento in questo turno.\n");
        return;
    }

    if (giocatore -> mondo == Mondo_Reale) {
        if (giocatore -> pos_mondoreale -> nemico != nessun_nemico && !giocatore -> pos_mondoreale -> nemico_strutturato.nemico_sconfitto) {
            printf("ATTENZIONE! Non è possibile passare al Soprasotto, prima si deve combattere il nemico nella zona attuale del Mondoreale.\n");
            return;
        }
        giocatore -> mondo = Soprasotto;
        giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
        printf("%s sei passato dal Mondoreale al Soprasotto.\n", giocatore -> nome);
    } else {
        if (giocatore -> pos_soprasotto -> nemico == nessun_nemico || giocatore -> pos_soprasotto -> nemico_strutturato.nemico_sconfitto) {
            giocatore -> mondo = Mondo_Reale;
            giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
            printf("%s sei passato dal Soprasotto al Mondoreale.\n", giocatore -> nome);
        } else {
            printf("ATTENZIONE! Stai cercando di passare al Mondoreale ma è presente un nemico nella zona attuale del Soprasotto.\n");
            printf("Sarà lanciato un dado da 20 facce per determinare la riuscita della tua fuga, se esce un numero minore della tua fortuna la fuga avrà successo.\n");
            time_t t; // Variabile per la generazione casuale
            srand((unsigned)time(&t));
            unsigned char dado = (unsigned char)(rand() % 20 + 1);
            printf("Hai ottenuto un %d nel lancio del dado.\n", dado);
            if (dado >= giocatore -> fortuna) {
                printf("Fuga fallita! Non puoi passare al Mondoreale.\n");
                *movimento = 1;
                return;
            } else {
                giocatore -> mondo = Mondo_Reale;
                giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
                printf("Fuga riuscita! %s sei passato dal Soprasotto al Mondoreale.\n", giocatore -> nome);
            }
        }
    }
    *movimento = 1;
}

static Nemico* combatti(Giocatore* giocatore, unsigned short* azione) {
    if (*azione == 2) {
        printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n\n");
        return NULL;
    }
    
    unsigned short scelta = 0;
    unsigned short turno_attacco_giocatore = 1; // Flag turno: 1 il giocatore attacca, 0 il giocoatore difende
    unsigned short oggetto_utilizzato = 0; // Flag per l'utilizzo dell'oggetto: 0 non utilizzato, 1 utilizzato
    Nemico* nemico_strutturato;
    Zona_mondoreale* zona_combattimento = giocatore -> pos_mondoreale;

    if (giocatore -> mondo == Mondo_Reale) {
        nemico_strutturato = &(giocatore -> pos_mondoreale -> nemico_strutturato);
    } else {
        nemico_strutturato = &(giocatore -> pos_soprasotto -> nemico_strutturato);
    }
    if (nemico_strutturato -> nemico == nessun_nemico || nemico_strutturato -> nemico_sconfitto) {
        printf("ATTENZIONE! Non è presente alcun nemico in questa zona con cui combattere.\n");
        return NULL;
    }
    printf("\n%s stai combattendo contro %s\nPunti vita nemico: %d\nAttacco nemico: %d\n\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico), nemico_strutturato -> p_vita, nemico_strutturato -> attacco_psichico);
    printf("REGOLE COMBATTIMENTO:\n- Il giocatore e il nemico attaccano a turni alternati.\n- Il giocatore può scegliere di utilizzare un solo oggetto all'inizio di ogni turno.\n");

    do {
        printf("\nScegli la tua azione:\n");
        printf("0. Utilizza oggetto\n1. Continua combattimento\n>> ");
        scanf("%hu", &scelta);
        svuotaBuffer();

        switch
        (scelta) {
            case 0:
                utilizza_oggetto(giocatore, NULL, &oggetto_utilizzato);
                break;
            case 1:
                if (turno_attacco_giocatore) {
                    attacco_giocatore(giocatore, nemico_strutturato);
                    turno_attacco_giocatore = 0;
                } else {
                    attacco_nemico(giocatore, nemico_strutturato);
                    turno_attacco_giocatore = 1;
                }
                oggetto_utilizzato = 0;
                break;
            default:
                printf("ATTENZIONE! Comando non riconosciuto.\n");
        }
    } while (nemico_strutturato -> p_vita > 0 && giocatore -> p_vita > 0 && (giocatore -> mondo == Mondo_Reale ? (giocatore -> pos_mondoreale == zona_combattimento) : (giocatore -> pos_soprasotto == zona_combattimento -> link_soprasotto)));

    if (giocatore -> p_vita <= 0) {
        printf("%s sei stato sconfitto in combattimento e hai perso la partita.\n", giocatore -> nome);
        return NULL;
    }
    if ((giocatore -> mondo == Mondo_Reale ? (giocatore -> pos_mondoreale == zona_combattimento) : (giocatore -> pos_soprasotto == zona_combattimento -> link_soprasotto)) == 0) {
        printf("Combattimento sospeso.\n");
        return NULL;
    }

    printf("\n%s ha sconfitto %s\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico));
    nemico_strutturato -> nemico_sconfitto = 1;
    totale_nemici_sconfitti++;

    if (nemico_strutturato -> nemico == demotorzone) {
        vincitore = 1;
        if (strcmp(giocatore -> nome, "UndiciVirgolaCinque") == 0) {
            strcpy(nome_vincitore, nome_iniziale_undicivirgolacinque);
        } else {
            strcpy(nome_vincitore, giocatore -> nome);
        }
        strcpy(nome_ultimi_tre_vincitori[2], nome_ultimi_tre_vincitori[1]);
        strcpy(nome_ultimi_tre_vincitori[1], nome_ultimi_tre_vincitori[0]);
        strcpy(nome_ultimi_tre_vincitori[0], nome_vincitore);
        return NULL;
    }

    // Rigenerazione nemico con 50% di probabilità
    time_t t; // Variabile per la generazione casuale
    srand((unsigned)time(&t));
    if (rand() % 2) {
        switch (nemico_strutturato -> nemico) {
            case billi:
                nemico_strutturato -> attacco_psichico = ATT_BILLI;
                nemico_strutturato -> p_vita = PV_BILLI;
                nemico_strutturato -> nemico = billi;
                printf("\nPurtroppo il combattimento non ha avuto successo e un nuovo billi apparirà nella zona nel prossimo round.\n");
                break;
            case democane:
                nemico_strutturato -> attacco_psichico = ATT_DEMOCANE;
                nemico_strutturato -> p_vita = PV_DEMOCANE;
                nemico_strutturato -> nemico = democane;
                printf("\nPurtroppo il combattimento non ha avuto successo e un nuovo democane apparirà nella zona nel prossimo round.\n");
                break;
            default:
                break;
        }
    } else {
        printf("\nCongratulazioni! Il combattimento ha avuto successo e il nemico è scomparso dalla zona.\n");
        nemico_strutturato -> nemico = nessun_nemico;
        giocatore -> mondo == Mondo_Reale ? (giocatore -> pos_mondoreale -> nemico = nessun_nemico) : (giocatore -> pos_soprasotto -> nemico = nessun_nemico);
    }
    (*azione)++;
    return nemico_strutturato;
}

static unsigned short variazione_fortuna(Giocatore* giocatore) {
    unsigned short max_variazione = (giocatore -> fortuna * 4 + 19) / 20;
    if (max_variazione == 0) {
        return 0;
    }
    return (unsigned short)(rand() % (max_variazione + 1));
}

static void attacco_giocatore(Giocatore* giocatore, Nemico* nemico_strutturato) {
    unsigned short danno = giocatore -> attacco_psichico;
    unsigned short variazione = variazione_fortuna(giocatore);
    danno += variazione;
    if (variazione > 0) {
        printf("La fortuna ha contribuito con un bonus di %d punti danno!\n", variazione);
    } else {
        printf("Questa volta la fortuna non è stata d'aiuto.\n");
    }
    if (danno >= nemico_strutturato -> p_vita) {
        nemico_strutturato -> p_vita = 0;
        printf("Hai inflitto %d punti danno, %s non ha più punti vita.\n", danno, tipo_nemico(nemico_strutturato -> nemico));
    } else {
        nemico_strutturato -> p_vita -= danno;
        printf("Hai inflitto %d punti danno, %s ha ancora %d punti vita.\n", danno, tipo_nemico(nemico_strutturato -> nemico), nemico_strutturato -> p_vita);
    }
}

static void attacco_nemico(Giocatore* giocatore, Nemico* nemico_strutturato) {
    unsigned short danno = (nemico_strutturato -> attacco_psichico * (nemico_strutturato -> attacco_psichico + 2)) / (nemico_strutturato -> attacco_psichico + giocatore -> difesa_psichica + 2);
    unsigned short variazione = variazione_fortuna(giocatore);
    if (variazione >= danno) {
        danno = 0;
        printf("Che fortuna! %s ha tentato di attaccarti ma il suo attacco non ha avuto effetto, 0 danni subiti.\n", tipo_nemico(nemico_strutturato -> nemico));
        return;
    }
    danno -= variazione;
    if (danno >= giocatore -> p_vita) {
        giocatore -> p_vita = 0;
        printf("La tua difesa è stata inefficace, %s ti ha inflitto %d punti danno e non hai più punti vita.\n", tipo_nemico(nemico_strutturato -> nemico), danno);
        return;
    }
    giocatore -> p_vita -= danno;
    printf("%s ti ha inflitto %d punti danno e ti rimangono %d punti vita.\n", tipo_nemico(nemico_strutturato -> nemico), danno, giocatore -> p_vita);
}

static void raccogli_oggetto(Giocatore* giocatore, unsigned short* azione) {
    if (*azione == 2) {
        printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n");
        return;
    }
    if (giocatore -> mondo != Mondo_Reale) {
        printf("ATTENZIONE! Non ci sono oggetti nel Soprasotto.\n");
        return;
    }
    if (giocatore -> pos_mondoreale -> oggetto == nessun_oggetto) {
        printf("ATTENZIONE! Non ci sono oggetti da raccogliere in questa zona.\n");
        return;
    }
    if (giocatore -> pos_mondoreale -> nemico != nessun_nemico && !giocatore -> pos_mondoreale -> nemico_strutturato.nemico_sconfitto) {
        printf("ATTENZIONE! Non è possibile raccogliere l'oggetto presente in questa zona finché non si sconfigge il nemico.\n");
        return;
    }
    for (int i = 0; i < 3; i++) {
        if (giocatore -> zaino[i] == nessun_oggetto) {
            giocatore -> zaino[i] = giocatore -> pos_mondoreale -> oggetto;
            giocatore -> pos_mondoreale -> oggetto = nessun_oggetto;
            printf("Hai raccolto l'oggetto %s.\n", tipo_oggetto(giocatore -> zaino[i]));
            (*azione)++;
            return;
        }
    }
    printf("ATTENZIONE! Il tuo zaino è pieno. Non puoi raccogliere altri oggetti.\n");
}

static void utilizza_oggetto(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato) {
    if (oggetto_utilizzato != NULL && *oggetto_utilizzato == 1) {
        printf("ATTENZIONE! Hai già utilizzato un oggetto in questo turno, non puoi usarne subito un altro.\n");
        return;
    }
    if (azione != NULL && *azione == 2) {
        printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n");
        return;
    }
    if (giocatore -> zaino[0] == nessun_oggetto && giocatore -> zaino[1] == nessun_oggetto && giocatore -> zaino[2] == nessun_oggetto) {
        printf("ATTENZIONE! Non hai oggetti nel tuo zaino da utilizzare.\n");
        return;
    }
    printf("\nLEGENDA EFFETTI OGGETTI:\n");
    printf("- bicicletta: Se usata in combattimento consente di fuggire dal nemico scappando in una zona adiacente (a seconda della scelta); altrimenti consente di spostarsi di due zone.\n  In entrambi i casi non conta come un movimento.\n");
    printf("- maglietta_fuocoinferno: Può essere usata in qualsiasi momento e aumenta i punti vita del giocatore di 20 punti vita.\n");
    printf("- bussola: Può essere usata in qualsiasi momento e indica se è presente un nemico ed eventualmente il suo tipo nelle zone adiacenti e nella corrispettiva nell'altra dimensione.\n");
    printf("- schitarrata_metallica: Se usata in combattimento uccide immediatamente billi, fa 8 punti danno a democane, fa 15 punti danno a demotorzone\n  Altrimenti aumenta permanentemente l'attacco del giocatore di 3 punti danno (fino ad un massimo di 20 punti danno).\n\n");

    printf("Oggetti disponibili nel tuo zaino:\n");
    for (int i = 0; i < 3; i++) {
        printf("    Slot %d: %s\n", i + 1, tipo_oggetto(giocatore -> zaino[i]));
    }
    unsigned short scelta = 0;
    do {
        printf("\nSeleziona lo slot dell'oggetto da utilizzare (1-3) oppure inserisci 0 per annullare l'operazione.\n");
        printf(">> ");
        scanf("%hu", &scelta);
        svuotaBuffer();

        if (scelta > 3) {
            printf("ATTENZIONE! Comando non riconosciuto\n");
        } else if (giocatore -> zaino[scelta - 1] == nessun_oggetto) {
            printf("ATTENZIONE! In questo slot non è presente alcun oggetto, scegliere un altro slot.\n");
            scelta = 4;
        }
    } while (scelta > 3);

    if (scelta == 0) {
        printf("Operazione annullata.\n");
        return;
    }

    Tipo_oggetto oggetto_selezionato = giocatore -> zaino[scelta - 1];
    switch (oggetto_selezionato) {
        case bicicletta:
            effetto_bicicletta(giocatore, azione, oggetto_utilizzato, scelta);
            break;
        case maglietta_fuocoinferno:
            effetto_maglietta_fuocoinferno(giocatore, azione, oggetto_utilizzato, scelta);
            break;
        case bussola:
            effetto_bussola(giocatore, azione, oggetto_utilizzato, scelta);
            break;
        case schitarrata_metallica:
            effetto_schitarrata_metallica(giocatore, azione, oggetto_utilizzato, scelta);
            break;
        default:
            printf("ATTENZIONE! Errore nell'applicazione dell'effetto dell'oggetto.\n");
            return;
    }
}

static void effetto_bicicletta(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta) {
    unsigned short decisione = 0;
    printf("\nScegliere come utilizzare la bicicletta:\n");
    if (azione != NULL) {
        // Effetto fuori combattimento
        if (*azione == 2) {
            printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n");
            return;
        }
        printf("0. Spostarsi di due zone avanti\n1. Spostarsi di due zone indietro\n>> ");      
        scanf("%hu", &decisione);
        svuotaBuffer();

        switch (decisione) {
            case 0:
                if (giocatore -> pos_mondoreale -> avanti == NULL || giocatore -> pos_mondoreale -> avanti -> avanti == NULL) {
                    printf("ATTENZIONE! Non è possibile spostarsi di due zone avanti con la bicicletta, non ci sono abbastanza zone disponibili.\n");
                    return;
                }
                if (giocatore -> mondo == Mondo_Reale) {
                    giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> avanti -> avanti;
                    giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
                    printf("%s ha usato la bicicletta per spostarti di due zone avanti.\n", giocatore -> nome);
                } else {
                    giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> avanti -> avanti;
                    giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
                    printf("%s ha usato la bicicletta per spostarti di due zone avanti.\n", giocatore -> nome);
                }
                (*azione)++;
                break;
            case 1:
                if (giocatore -> pos_mondoreale -> indietro == NULL || giocatore -> pos_mondoreale -> indietro -> indietro == NULL) {
                    printf("ATTENZIONE! Non è possibile spostarsi di due zone indietro con la bicicletta, non ci sono abbastanza zone disponibili.\n");
                    return;
                }
                if (giocatore -> mondo == Mondo_Reale) {
                    giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> indietro -> indietro;
                    giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
                    printf("%s ha usato la bicicletta per spostarti di due zone indietro.\n", giocatore -> nome);
                } else {
                    giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> indietro -> indietro;
                    giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
                    printf("%s ha usato la bicicletta per spostarti di due zone indietro.\n", giocatore -> nome);
                }
                (*azione)++;
                break;
            default:
                printf("ATTENZIONE! Comando non riconosciuto.\n");
                return;
        }      
    } else {
        // Effetto in combattimento
        if (*oggetto_utilizzato == 1) {
            printf("ATTENZIONE! Hai già utilizzato un oggetto in questo turno.\n");
            return;
        }
        Tipo_nemico nemico_corrente;
        if (giocatore -> mondo == Mondo_Reale) {
            nemico_corrente = giocatore -> pos_mondoreale -> nemico;
        } else {
            nemico_corrente = giocatore -> pos_soprasotto -> nemico;
        }
        printf("0. Fuggire avanzando di una zona\n1. Fuggire indietreggiando di una zona\n>> ");
        scanf("%hu", &decisione);
        svuotaBuffer();

        switch (decisione) {
            case 0:
                if (giocatore -> pos_mondoreale -> avanti == NULL) {
                    printf("ATTENZIONE! Non è possibile fuggire avanzando di una zona, non ci sono zone disponibili.\n");
                    return;
                }
                if (giocatore -> mondo == Mondo_Reale) {
                    giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> avanti;
                    giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
                    printf("%s è fuggito da %s avanzando di una zona.\n", giocatore -> nome, tipo_nemico(nemico_corrente));
                } else {
                    giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> avanti;
                    giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
                    printf("%s è fuggito da %s avanzando di una zona.\n", giocatore -> nome, tipo_nemico(nemico_corrente));
                }
                *oggetto_utilizzato = 1;
                break;
            case 1:
                if (giocatore -> pos_mondoreale -> indietro == NULL) {
                    printf("ATTENZIONE! Non è possibile fuggire indietreggiando di una zona, non ci sono zone disponibili.\n");
                    return;
                }
                if (giocatore -> mondo == Mondo_Reale) {
                    giocatore -> pos_mondoreale = giocatore -> pos_mondoreale -> indietro;
                    giocatore -> pos_soprasotto = giocatore -> pos_mondoreale -> link_soprasotto;
                    printf("%s è fuggito da %s indietreggiando di una zona.\n", giocatore -> nome, tipo_nemico(nemico_corrente));
                } else {
                    giocatore -> pos_soprasotto = giocatore -> pos_soprasotto -> indietro;
                    giocatore -> pos_mondoreale = giocatore -> pos_soprasotto -> link_mondoreale;
                    printf("%s è fuggito da %s indietreggiando di una zona.\n", giocatore -> nome, tipo_nemico(nemico_corrente));
                }
                *oggetto_utilizzato = 1;
                break;
            default:
                printf("ATTENZIONE! Comando non riconosciuto.\n");
                return;
        }
    }
    printf("%s rimosso dallo zaino.\n", tipo_oggetto(giocatore -> zaino[scelta - 1]));
    giocatore -> zaino[scelta - 1] = nessun_oggetto;
}

static void effetto_maglietta_fuocoinferno(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta) {
    if (azione != NULL && *azione == 2) {
        printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n\n");
        return;
    }
    if (oggetto_utilizzato != NULL && *oggetto_utilizzato == 1) {
        printf("ATTENZIONE! Hai già utilizzato un oggetto in questo turno.\n");
        return;
    }
    giocatore -> p_vita += 20;
    printf("%s hai usato la maglietta_fuocoinferno e i tuoi punti vita sono aumentati a %d.\n", giocatore -> nome, giocatore -> p_vita);
    if (azione != NULL) {
        (*azione)++;
    }
    if (oggetto_utilizzato != NULL) {
        *oggetto_utilizzato = 1;
    }
    printf("%s rimosso dallo zaino.\n", tipo_oggetto(giocatore -> zaino[scelta - 1]));
    giocatore -> zaino[scelta - 1] = nessun_oggetto;
}

static void effetto_bussola(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta) {
    if (azione != NULL && *azione == 2) {
        printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n");
        return;
    }
    if (oggetto_utilizzato != NULL && *oggetto_utilizzato == 1) {
        printf("ATTENZIONE! Hai già utilizzato un oggetto in questo turno.\n");
        return;
    }

    Zona_mondoreale* zona_corrente_mondoreale = giocatore -> pos_mondoreale;
    Zona_soprasotto* zona_corrente_soprasotto = giocatore -> pos_soprasotto;
    printf("\n%s ecco le informazioni fornite dalla bussola:\n", giocatore -> nome);

    if (giocatore -> mondo == Mondo_Reale) {
        if (zona_corrente_mondoreale -> indietro != NULL) {
            if (zona_corrente_mondoreale -> indietro -> nemico != nessun_nemico && !zona_corrente_mondoreale -> indietro -> nemico_strutturato.nemico_sconfitto) {
                printf("- Nella zona precedente (Zona %d) è presente %s.\n", posizione_giocatore(giocatore) - 1, tipo_nemico(zona_corrente_mondoreale -> indietro -> nemico));
            } else {
                printf("- Nella zona precedente (Zona %d) non è presente alcun nemico.\n", posizione_giocatore(giocatore) - 1);
            }
        } else {
            printf("- Non esiste una zona precedente nel Mondoreale.\n");
        }
        if (zona_corrente_mondoreale -> avanti != NULL) {
            if (zona_corrente_mondoreale -> avanti -> nemico != nessun_nemico && !zona_corrente_mondoreale -> avanti -> nemico_strutturato.nemico_sconfitto) {
                printf("- Nella zona successiva (Zona %d) è presente %s.\n", posizione_giocatore(giocatore) + 1, tipo_nemico(zona_corrente_mondoreale -> avanti -> nemico));
            } else {
                printf("- Nella zona successiva (Zona %d) non è presente alcun nemico.\n", posizione_giocatore(giocatore) + 1);
            }
        } else {
            printf("- Non esiste una zona successiva nel Mondoreale.\n");
        }
        if (zona_corrente_soprasotto -> nemico != nessun_nemico && !zona_corrente_soprasotto -> nemico_strutturato.nemico_sconfitto) {
            printf("- Nella zona corrispondente del Soprasotto è presente %s.\n", tipo_nemico(zona_corrente_soprasotto -> nemico));
        } else {
            printf("- Nella zona corrispondente del Soprasotto non è presente alcun nemico.\n");
        }
    } else {
        if (zona_corrente_soprasotto -> indietro != NULL) {
            if (zona_corrente_soprasotto -> indietro -> nemico != nessun_nemico && !zona_corrente_soprasotto -> indietro -> nemico_strutturato.nemico_sconfitto) {
                printf("- Nella zona precedente (Zona %d) è presente %s.\n", posizione_giocatore(giocatore) - 1, tipo_nemico(zona_corrente_soprasotto -> indietro -> nemico));
            } else {
                printf("- Nella zona precedente (Zona %d) non è presente alcun nemico.\n", posizione_giocatore(giocatore) - 1);
            }
        } else {
            printf("- Non esiste una zona precedente nel Soprasotto.\n");
        }
        if (zona_corrente_soprasotto -> avanti != NULL) {
            if (zona_corrente_soprasotto -> avanti -> nemico != nessun_nemico && !zona_corrente_soprasotto -> avanti -> nemico_strutturato.nemico_sconfitto) {
                printf("- Nella zona successiva (Zona %d) è presente %s.\n", posizione_giocatore(giocatore) + 1, tipo_nemico(zona_corrente_soprasotto -> avanti -> nemico));
            } else {
                printf("- Nella zona successiva (Zona %d) non è presente alcun nemico.\n", posizione_giocatore(giocatore) + 1);
            }
        } else {
            printf("- Non esiste una zona successiva nel Soprasotto.\n");
        }
        if (zona_corrente_mondoreale -> nemico != nessun_nemico && !zona_corrente_mondoreale -> nemico_strutturato.nemico_sconfitto) {
            printf("- Nella zona corrispondente del Mondoreale è presente %s.\n", tipo_nemico(zona_corrente_mondoreale -> nemico));
        } else {
            printf("- Nella zona corrispondente del Mondoreale non è presente alcun nemico.\n");
        }
    }
    if (azione != NULL) {
        (*azione)++;
    }
    if (oggetto_utilizzato != NULL) {
        *oggetto_utilizzato = 1;
    }
    printf("%s rimosso dallo zaino.\n", tipo_oggetto(giocatore -> zaino[scelta - 1]));
    giocatore -> zaino[scelta - 1] = nessun_oggetto;
}

static void effetto_schitarrata_metallica(Giocatore* giocatore, unsigned short* azione, unsigned short* oggetto_utilizzato, unsigned short scelta) {
    if (azione != NULL) {
        // Effetto fuori combattimento
        if (*azione == 2) {
            printf("ATTENZIONE! Hai già effettuato due azioni in questo turno.\n\n");
            return;
        }
        giocatore -> attacco_psichico += 3;
        if (giocatore -> attacco_psichico > 20) {
            giocatore -> attacco_psichico = 20;
        }
        printf("%s ha usato la schitarrata_metallica fuori combattimento, il suo attacco psichico è aumentato a %d.\n", giocatore -> nome, giocatore -> attacco_psichico);
        (*azione)++;
        printf("%s rimosso dallo zaino.\n", tipo_oggetto(giocatore -> zaino[scelta - 1]));
        giocatore -> zaino[scelta - 1] = nessun_oggetto;
        return;
    }

    if (oggetto_utilizzato != NULL && *oggetto_utilizzato == 1) {
        printf("ATTENZIONE! Hai già utilizzato un oggetto in questo turno.\n");
        return;
    }

    // Effetto in combattimento
    Nemico* nemico_strutturato = NULL;

    if (giocatore -> mondo == Mondo_Reale) {
        nemico_strutturato = &(giocatore -> pos_mondoreale -> nemico_strutturato);
        if (nemico_strutturato -> nemico == billi) {
            nemico_strutturato -> p_vita = 0;
            printf("%s ha messo fuori combattimento %s con schitarrata_metallica.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico));
        } else {
            if (nemico_strutturato -> p_vita <= 8) {
                nemico_strutturato -> p_vita = 0;
                printf("%s ha messo fuori combattimento %s con schitarrata_metallica.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico));
            } else {
                nemico_strutturato -> p_vita -= 8;
                printf("%s ha inflitto 8 punti danno a %s con schitarrata_metallica, ora ha %d punti vita.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico), nemico_strutturato -> p_vita);
            }
        }
    } else {
        nemico_strutturato = &(giocatore -> pos_soprasotto -> nemico_strutturato);
        if (nemico_strutturato -> nemico == democane) {
            if (nemico_strutturato -> p_vita <= 8) {
                nemico_strutturato -> p_vita = 0;
                printf("%s ha messo fuori combattimento %s con schitarrata_metallica.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico));
            } else {
                nemico_strutturato -> p_vita -= 8;
                printf("%s ha inflitto 8 punti danno a %s con schitarrata_metallica, ora ha %d punti vita.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico), nemico_strutturato -> p_vita);
            }
        } else {
            if (nemico_strutturato -> p_vita <= 15) {
                nemico_strutturato -> p_vita = 0;
                printf("%s ha messo fuori combattimento %s con schitarrata_metallica.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico));
            } else {
                nemico_strutturato -> p_vita -= 15;
                printf("%s ha inflitto 15 punti danno a %s con schitarrata_metallica, ora ha %d punti vita.\n", giocatore -> nome, tipo_nemico(nemico_strutturato -> nemico), nemico_strutturato -> p_vita);
            }
        }
    }
    if (oggetto_utilizzato != NULL) {
        *oggetto_utilizzato = 1;
    }
    printf("%s rimosso dallo zaino.\n", tipo_oggetto(giocatore -> zaino[scelta - 1]));
    giocatore -> zaino[scelta - 1] = nessun_oggetto;
}

static void stampa_giocatore(Giocatore* giocatore) {
    printf("\nDettagli giocatore %s:\n", giocatore -> nome);
    printf("- Mondo attuale: %s\n", tipo_mondo(giocatore -> mondo));
    printf("- Posizione: Zona %d\n", posizione_giocatore(giocatore));
    printf("- Attacco Psichico: %d\n", giocatore -> attacco_psichico);
    printf("- Difesa Psichica: %d\n", giocatore -> difesa_psichica);
    printf("- Fortuna: %d\n", giocatore -> fortuna);
    printf("- Zaino:\n");
    for (int i = 0; i < 3; i++) {
        printf("    Slot %d: %s\n", i + 1, tipo_oggetto(giocatore -> zaino[i]));
    }
    printf("- Punti Vita: %d\n\n", giocatore -> p_vita);
}

static void stampa_zona_giocatore(Giocatore* giocatore) {
    printf("\nDettagli zona giocatore %s:\n", giocatore -> nome);
    printf("- Mondo attuale: %s\n", tipo_mondo(giocatore -> mondo));
    if (giocatore -> mondo == Mondo_Reale) {
        printf("- Posizione: Zona %d\n", posizione_giocatore(giocatore));
        printf("- Tipo: %s\n", tipo_zona(giocatore -> pos_mondoreale -> tipo));
        if (giocatore -> pos_mondoreale -> nemico_strutturato.nemico_sconfitto) {
            printf("- Nemico: nessun_nemico (temporaneamente sconfitto)\n");
        } else {
            printf("- Nemico: %s\n", tipo_nemico(giocatore -> pos_mondoreale -> nemico));
        }
        printf("- Oggetto: %s\n", tipo_oggetto(giocatore -> pos_mondoreale -> oggetto));
    } else {
        printf("- Posizione: Zona %d\n", posizione_giocatore(giocatore));
        printf("- Tipo: %s\n", tipo_zona(giocatore -> pos_soprasotto -> tipo));
        if (giocatore -> pos_soprasotto -> nemico_strutturato.nemico_sconfitto) {
            printf("- Nemico: nessun_nemico (temporaneamente sconfitto)\n");
        } else {
            printf("- Nemico: %s\n", tipo_nemico(giocatore -> pos_soprasotto -> nemico));
        }
    }
}

static unsigned short posizione_giocatore(Giocatore* giocatore) {
    Zona_mondoreale* posizione_giocatore = prima_zona_mondoreale;
    unsigned short posizione = 1;
    while (posizione_giocatore != NULL && posizione_giocatore != giocatore -> pos_mondoreale) {
        posizione_giocatore = posizione_giocatore -> avanti;
        posizione++;
    }
    return posizione;
}

static void passa(Giocatore* giocatore) {
    printf("%s ha deciso di passare/terminare il turno.\n", giocatore -> nome);
}