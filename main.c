#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LINELENGTH 40   // Max size of one line in the trace
#define BLOCKSIZE 16    // Block size for cache in bytes
#define BITADDRESS32 32 // 32-bit memory address

/* Structure of a trace line */
typedef struct Trace_{
    char *instr;        // [9] address of instruction
    char type;          // read/write flag
    char *dataAcc;      // [10] address of data access
    char byteScanned;   // number of bytes read/written
    char *dataDisp;     // [10] data read/written
} Trace_;

typedef struct Block_ {
    int valid;
    long tag;
    long data;
} Block_;

typedef struct Cache_ {
    Block_ *cacheMap;
    int offsetSize;
    int nBlocks;
    int indexSize;
    int tagSize;
    int nRead;
    int nWrite;
    int readMiss;
    int writeMiss;
    int dataMiss;
    int dirtyReadMiss;
    int dirtyWriteMiss;
    int bytesRead;
    int bytesWrite;
} Cache_;

int main(int argc, char **argv) {
    int i,j;
    int sizeOffset;
    FILE *file;
    char *buffer = malloc(sizeof(char) * LINELENGTH);
    Trace_ *trace = malloc(sizeof(Trace_));
    Cache_ *myCache = malloc (sizeof(Cache_));

    /* Error check for correct number of arguments */
    if (argc > 3) {
        fprintf(stderr, "Error: Incorrect number of arguments\n"
                "Usage: dir tracefile cachesize [-v ic1 ic2]");
        exit(1);
    }

    trace->instr = malloc(sizeof(char) * 9);    // allocate space to store address

    /* Title */
    printf("Cache Simulator - System 1\n"
            "direct-mapped, writeback, size = %s\n", argv[2]);

    /* Setup calculation for Cache */
    myCache->offsetSize = (int) log2(BLOCKSIZE);
    myCache->nBlocks = (atoi(argv[2]) * 1024) / BLOCKSIZE;
    myCache->cacheMap = malloc(sizeof(Block_) * myCache->nBlocks);
    myCache->indexSize = (int) log2(myCache->nBlocks);
    myCache->tagSize = BITADDRESS32 - (myCache->indexSize + myCache->offsetSize);

    /* Set initial valid bit to 0 */
    for (int k = 0; k < myCache->nBlocks; ++k) {
        myCache->cacheMap[k].valid = 0;
    }

    printf("%d bits offset\n"
            "%d blocks\n"
            "%d bits index\n"
            "%d bits tag\n", myCache->offsetSize, myCache->nBlocks, myCache->indexSize, myCache->tagSize);

    /* Read file */
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file.\n");
        exit(1);
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