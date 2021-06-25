#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#define LIN_CACHE 4
#define LIN_BYTES 8


typedef struct {
    short int ETQ;
    short int Datos[LIN_BYTES];
}T_LINEA_CACHE;

T_LINEA_CACHE cache[LIN_CACHE];
int tiempoGlobal = 0;
int numFallos = 0;
int numAccesos = 0;
unsigned char RAM[1024];



void leerFichero(){

    FILE *fichero;
    short int etiqueta, palabra, linea, bloque;
    char texto[100];
    unsigned int num[12];
    int count = 0;
    
    fichero = fopen("accesos_memoria.txt", "r");
    if (fichero == NULL)
    {
        printf("El fichero no existe\n");
        exit(-1);
    }

    for (int i = 0; i < 12; i++)
    {
        fscanf(fichero, "%X", &num[i]);
        linea = (num[i] & 0b0000011000) >> 3;
        palabra = (num[i] & 0b0000000111);
        etiqueta = num[i] >> 5;
        bloque = num[i] >> 3;

        
        if (cache[linea].ETQ != etiqueta)
        {
            numFallos++;
            printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", tiempoGlobal, numFallos, num[i], etiqueta, linea, palabra, bloque);
            tiempoGlobal = tiempoGlobal + 10;
            cache[linea].ETQ = etiqueta;
            
            for (int j = 0; j < LIN_BYTES; j++){
                cache[linea].Datos[j] = RAM[bloque * 8 + j];
            }
            printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);

        }
        if (cache[linea].ETQ == etiqueta)
        {
            numAccesos++;
            tiempoGlobal += 1;
            printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", tiempoGlobal, num[i], etiqueta, linea, palabra, RAM[bloque *LIN_BYTES + palabra]);
            texto[count+1] = RAM[bloque *LIN_BYTES + palabra];
            count++;
        }
        
        for (int i = 0; i < LIN_CACHE; i++)
        {
            printf("ETQ:%X   Datos ", cache[i].ETQ);
            for (int j = 7; j >= 0; j--)
            {
                printf("%X ", cache[i].Datos[j]);
            }
            printf("\n");
        }
        sleep(2);
        printf("\n");
    }
    printf("\nNumero total de accesos: %d\nNumero de fallos: %d\n ", numAccesos, numFallos);    
    printf("\n");
    fclose(fichero);
    exit(-1);
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < LIN_CACHE; i++)
    {
        cache[i].ETQ = 0xFF;
        
        for (int j = 0; j < LIN_BYTES; j++)
        {
            cache[i].Datos[j] = 0;
        }
    }
    
    
    FILE *fRAM;
    fRAM = fopen("RAM.bin", "rb");
    if (fRAM == NULL)
    {
        printf("El fichero no existe\n");
        exit(-1);
    }
    fread(RAM, sizeof(RAM), 1, fRAM);
    fclose(fRAM);
    
    leerFichero();
}




