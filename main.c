#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LINE_LENGTH 40    // Max size of one line in the trace
#define BLOCK_SIZE 16     // Block size for cache in bytes
#define BIT_ADDRESS_32 32 // 32-bit memory address
#define MISS_PENALTY 80   // cycles for a penalty

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
    int dirty;
    long tag;
} Block_;

typedef struct Cache_ {
    Block_ *cacheMap;
    int nBlocks;
    int offsetSize;
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
    int cycleRead;
    int cycleWrite;
} Cache_;

/* Function Prototype */
Cache_* convert(Trace_*, Cache_*); //takes current line of trace and runs through the cache

int main(int argc, char **argv) {
    int i,j;
    int sizeOffset;
    long tempAddress;
    FILE *file;
    char *buffer = malloc(sizeof(char) * LINE_LENGTH);
    Trace_ *trace = malloc(sizeof(Trace_));
    Cache_ *myCache = malloc (sizeof(Cache_));

    /* Error check for correct number of arguments */
    if (argc == 1 || argc > 3) {
        fprintf(stderr, "Error: Incorrect number of arguments\n"
                "Usage: dir tracefile cachesize [-v ic1 ic2]");
        exit(1);
    }

    trace->instr = malloc(sizeof(char) * 9);    // allocate space to store address

    /* Title */
    printf("Cache Simulator - System 1\n"
            "direct-mapped, writeback, size = %sKB\n", argv[2]);

    /* Calculation setup for Cache */
    myCache->offsetSize = (int) log2(BLOCK_SIZE);
    myCache->nBlocks = (atoi(argv[2]) * 1024) / BLOCK_SIZE;
    myCache->cacheMap = malloc(sizeof(Block_) * myCache->nBlocks);
    myCache->indexSize = (int) log2(myCache->nBlocks);
    myCache->tagSize = BIT_ADDRESS_32 - (myCache->indexSize + myCache->offsetSize);

    /* Set initial to 0 */
    myCache->nRead = 0;
    myCache->nWrite = 0;
    myCache->readMiss = 0;
    myCache->writeMiss = 0;
    myCache->dataMiss = 0;
    myCache->dirtyReadMiss = 0;
    myCache->dirtyWriteMiss = 0;
    myCache->bytesRead = 0;
    myCache->bytesWrite = 0;
    myCache->cycleRead = 0;
    myCache->cycleWrite = 0;
    for (i = 0; i < myCache->nBlocks; ++i) {
        myCache->cacheMap[i].valid = 0;
        myCache->cacheMap[i].dirty = 0;
    }

/*
    printf("%d bits offset\n"
            "%d blocks\n"
            "%d bits index\n"
            "%d bits tag\n", myCache->offsetSize, myCache->nBlocks, myCache->indexSize, myCache->tagSize);
*/


    /* Read file */
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file.\n");
        exit(1);
    }

    while (fgets(buffer, LINE_LENGTH, file) != NULL) {
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

        //printf("%s %c %s %c %s\n", trace->instr, trace->type, trace->dataAcc, trace->byteScanned, trace->dataDisp);

        /*
        tempAddress = strtol(trace->dataAcc, NULL, 16);
        printf("address: %ld, %#lx\n", tempAddress, tempAddress);
        tempAddress = tempAddress >> myCache->offsetSize;
        printf("address: %ld, %#lx\n", tempAddress, tempAddress);
        tempAddress = tempAddress & ((int) pow(2, myCache->indexSize) - 1);
        printf("address: %ld, %#lx\n", tempAddress, tempAddress);
        printf("%s\n", myCache->cacheMap->data);
         */

        convert(trace, myCache);
    }

    printf("loads %d stores %d total %d\n"
                   "rmiss %d wmiss %d total %d\n"
                   "dirty rmiss %d dirty wmiss %d\n"
                   "bytes read %d bytes written %d\n"
                   "read time %d write time %d\n"
                   "total time %d\n"
                   "miss rate %f\n", myCache->nRead, myCache->nWrite, myCache->nRead + myCache->nWrite,
           myCache->readMiss, myCache->writeMiss, myCache->dataMiss = myCache->readMiss + myCache->writeMiss,
           myCache->dirtyReadMiss, myCache->dirtyWriteMiss,
           myCache->bytesRead, myCache->bytesWrite,
           myCache->cycleRead, myCache->cycleWrite,
           myCache->cycleRead + myCache->cycleWrite,
           (float) (myCache->readMiss + myCache->writeMiss)/(myCache->nRead + myCache->nWrite));

    free(buffer);
    free(trace->dataAcc);
    free(trace->dataDisp);
    free(trace->instr);
    free(myCache->cacheMap);
    free(myCache);

    return 0;
}

/*!
 * @function    convert
 * @abstract    Runs the current trace line through the cache process.
 * @discussion  This function converts the address of the data accessed
 *              and splits it to the tag and index. Then goes through
 *              the cache process and 
 * @param       current    The weight of the chicken.
 * @param       yourCache     The sauce for cooking. Could be a
 *                        NULL pointer.
 * @result      A pointer to the cache.
*/
Cache_* convert(Trace_ *current, Cache_ *yourCache) {
    long index = strtol(current->dataAcc, NULL, 16);
    //printf("%#lx", index);
    index >>= yourCache->offsetSize;
    long tag = index >> yourCache->indexSize;
    index &= ((int) pow(2, yourCache->indexSize) - 1);

    //printf("\n%d %ld %#lx %#lx\n", (int) index, index, tag, index);

    //printf("%c\n", current->type);

    if (current->type == 'R') {
        yourCache->nRead++;
        if (yourCache->cacheMap[index].valid == 1) {                                    //Read
            if (tag == yourCache->cacheMap[index].tag) {                                //Case 1
                //yourCache->bytesRead += (current->byteScanned - '0');
                yourCache->cycleRead++;
            } else {                                                                    //Case 2b
                yourCache->bytesRead += (current->byteScanned - '0');
                yourCache->cacheMap[index].tag = tag;
                yourCache->cacheMap[index].dirty = 0;
                yourCache->cycleRead = yourCache->cycleRead + 1 + (2 * MISS_PENALTY);
                yourCache->dirtyReadMiss++;
                yourCache->readMiss++;
            }
        } else {                                                                        //Case 2a
            yourCache->bytesRead += (current->byteScanned - '0');
            yourCache->cacheMap[index].valid = 1;
            yourCache->cacheMap[index].tag = tag;
            yourCache->cacheMap[index].dirty = 0;
            yourCache->cycleRead = yourCache->cycleRead + 1 + MISS_PENALTY;
            yourCache->readMiss++;
        }
    } else if (current->type == 'W') {
        yourCache->nWrite++;
        if (yourCache->cacheMap[index].valid == 1) {                                    //Write
            if (tag == yourCache->cacheMap[index].tag) {                                //Case 1
                //yourCache->bytesWrite += (current->byteScanned - '0');
                yourCache->cacheMap[index].dirty = 1;
                yourCache->cycleWrite++;
            } else {                                                                    //Case 2b
                yourCache->bytesWrite += (current->byteScanned - '0');
                yourCache->cacheMap[index].tag = tag;
                yourCache->cacheMap[index].dirty = 1;
                yourCache->cycleWrite = yourCache->cycleWrite + 1 + MISS_PENALTY;
                yourCache->writeMiss++;
            }
        } else {                                                                        //Case 2a
            yourCache->bytesWrite += (current->byteScanned - '0');
            yourCache->cacheMap[index].valid = 1;
            yourCache->cacheMap[index].tag = tag;
            yourCache->cacheMap[index].dirty = 1;
            yourCache->cycleWrite = yourCache->cycleWrite + 1 + (2 * MISS_PENALTY);
            yourCache->dirtyWriteMiss++;
            yourCache->writeMiss++;
        }
    }

    return yourCache;
}