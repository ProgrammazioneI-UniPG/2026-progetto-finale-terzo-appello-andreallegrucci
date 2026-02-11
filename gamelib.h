void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();

typedef enum Tipo_zona {bosco, scuola, laboratorio, caverna, strada, giardino, supermercato, centrale_elettrica, deposito_abbandonato, stazione_polizia} Tipo_zona;
typedef enum Tipo_nemico {nessun_nemico, billi, democane, demotorzone} Tipo_nemico;
typedef enum Tipo_oggetto {nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica} Tipo_oggetto;
typedef enum Tipo_mondo {Mondo_Reale = 0, Soprasotto = 1} Tipo_mondo;

typedef struct Giocatore {
    char nome[25];
    Tipo_mondo mondo;
    struct Zona_mondoreale* pos_mondoreale; // se mondo == Mondo_Reale (0)
    struct Zona_soprasotto* pos_soprasotto; // se mondo == Soprasotto  (1)
    unsigned char attacco_psichico;         // valori 1-20
    unsigned char difesa_psichica;          // valori 1-20
    unsigned char fortuna;                  // valori 1-20
    unsigned char p_vita;                   // valori 0-100
    Tipo_oggetto zaino[3];
} Giocatore;

typedef struct Nemico {
    Tipo_nemico nemico;
    unsigned char attacco_psichico;
    unsigned char p_vita;
    unsigned char nemico_sconfitto; // Flag per verificare se un nemico è stato sconfitto durante il turno corrente, 0 = nemico non sconfitto durante il turno corrente, 1 = nemico sconfitto durante il turno corrente
} Nemico;

typedef struct Zona_mondoreale {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto;
    struct Zona_mondoreale* avanti;
    struct Zona_mondoreale* indietro;
    struct Zona_soprasotto* link_soprasotto;
    Nemico nemico_strutturato;
} Zona_mondoreale;

typedef struct Zona_soprasotto {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    struct Zona_soprasotto* avanti;
    struct Zona_soprasotto* indietro;
    struct Zona_mondoreale* link_mondoreale;
    Nemico nemico_strutturato;
} Zona_soprasotto;