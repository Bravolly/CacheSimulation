#include <stdio.h>
#include <stdlib.h>

#define LINELENGTH 40   // Max size of one line in the trace
#define BLOCK 16        // Block size for cache in bytes

/* Structure of a trace line */
typedef struct {
    char *instr;        // [9] address of instruction
    char type;          // read/write flag
    char *dataAcc;      // [10] address of data access
    char byteScanned;   // number of bytes read/written
    char *dataDisp;     // [10] data read/written
} Trace_;

int main(int argc, char **argv) {
    int i,j;
    int sizeOffset;
    FILE *file;
    char *buffer = malloc(sizeof(char) * LINELENGTH);

    /* Error check for correct number of arguments */
    if (argc > 3) {
        fprintf(stderr, "Error: Too many arguments needed\n"
                "Example: ./producer-consumer (producer) (consumer) (number of items)");
        exit(1);
    }

    /* Create trace storage */
    Trace_ *trace = malloc(sizeof(Trace_));
    trace->instr = malloc(sizeof(char) * 9);

    printf("Cache Simulator - System 1\n");

    /* Read file */
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file.\n");
        return 0;
    }

    while (fgets(buffer, LINELENGTH, file) != NULL) {
        i = 0;
        j = 0;

        /* store instruction address */
        while (buffer[i] != ':') {
            trace->instr[j] = buffer[i];
            i++;
            j++;
        }

        /* store read/write flag */
        i += 2;
        trace->type = buffer[i];

        /* determine length of address from trace for malloc*/
        i += 2;
        sizeOffset = 0;
        while (buffer[i] == ' ') {
            i++;
            sizeOffset++;
        }

        /* store address of data access */
        trace->dataAcc = malloc(sizeof(char) * (10 - sizeOffset));
        j = 0;
        while (buffer[i] != ' ') {
            trace->dataAcc[j] = buffer[i];
            i++;
            j++;
        }

        /* store number of bytes read/written */
        i += 2;
        trace->byteScanned = buffer[i];

        /* determine length of address from trace for malloc*/
        i += 2;
        sizeOffset = 0;
        while (buffer[i] == ' ') {
            i++;
            sizeOffset++;
        }

        /* store data read/written */
        trace->dataDisp = malloc(sizeof(char) * (10 - sizeOffset));
        j = 0;
        while (buffer[i] != '\n') {
            trace->dataDisp[j] = buffer[i];
            i++;
            j++;
        }

        printf("%s %c %s %c %s\n", trace->instr, trace->type, trace->dataAcc, trace->byteScanned, trace->dataDisp);
    }

    return 0;
}