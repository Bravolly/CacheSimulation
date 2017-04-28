#include <stdio.h>
#include <stdlib.h>

/*
 * Max size of one line in the trace
 */
#define LINELENGTH 40

/*
 *
 */
struct Trace_ {
    char *instr;//[9]
    char type;
    char *dataAcc;//[10];
    int byteScanned;
    char *dataDisp;//[10];
};

int main(int argc, char **argv) {
    int i,j;
    int sizeOffset;
    FILE *file;
    char *buffer = malloc(sizeof(char) * LINELENGTH);

    /*
     * create trace storage
     */
    struct Trace_ trace;
    trace.instr = malloc(sizeof(char) * 9);

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

        while (buffer[i] != ':') {
            trace.instr[j] = buffer[i];
            i++;
            j++;
        }

        i += 2;
        trace.type = buffer[i];

        i += 2;
        sizeOffset = 0;
        while (buffer[i] == ' ') {
            i++;
            sizeOffset++;
        }

        j = 0;
        trace.dataAcc = malloc(sizeof(char) * (10 - sizeOffset);
        while (buffer[i] != ' ') {
            trace.dataAcc[j] = buffer[i];
            i++;
            j++;
        }

        i += 2;
        trace.byteScanned = buffer[i];

        i += 2;
        sizeOffset = 0;
        while (buffer[i] == ' ') {
            i++;
            sizeOffset++;
        }


        trace.dataDisp = malloc(sizeof(char) * (10 - sizeOffset));
        j = 0;
        while (buffer[i] != '\n') {
            trace.dataDisp[j] = buffer[i];
            i++;
            j++;
        }

        printf("%s %c %s %d %s\n", trace.instr);
    }

    return 0;
}