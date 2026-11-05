/* Run report code: you do not need to read or modify this file :)
 *
 * The confirmation code is encrypted: it can only be decrypted with a key
 * computed from the correct result of the exercise.
 * The revealed code is bound to the current user and machine.
 */
#include "report.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>

#include "semaphore.h"

/* The standard output is results.vcf: the report goes to stderr */
#define REPORT_OUT stderr

#define FNV_START 2166136261u

static unsigned int mix(unsigned int h, unsigned int v) {
    for (int i = 0; i < 4; i++) {
        h ^= (v >> (8 * i)) & 0xff;
        h *= 16777619u;
    }
    return h;
}

static unsigned int next_byte(unsigned int *s) {
    *s ^= *s << 13;
    *s ^= *s >> 17;
    *s ^= *s << 5;
    return *s & 0xff;
}

/* Decrypts 'len' bytes of 'enc' into 'out' and returns its checksum */
static unsigned int decrypt(unsigned int key, const unsigned char *enc, int len, char *out) {
    unsigned int s = key ? key : 1;
    unsigned int h = FNV_START;
    for (int i = 0; i < len; i++) {
        out[i] = enc[i] ^ next_byte(&s);
        h = mix(h, (unsigned char)out[i]);
    }
    out[len] = '\0';
    return h;
}

/* Appends to 'word' a code bound to the current user and machine */
static void bind_to_user(char *out, size_t size, const char *word) {
    const char *user = getenv("USER");
    char host[256] = "";
    char id[600];

    if (user == NULL || user[0] == '\0')
        user = "unknown";
    if (gethostname(host, sizeof(host)) != 0 || host[0] == '\0')
        strcpy(host, "unknown");
    host[sizeof(host) - 1] = '\0';

    snprintf(id, sizeof(id), "%s:%s@%s", word, user, host);
    unsigned int h = FNV_START;
    for (const char *c = id; *c != '\0'; c++) {
        h ^= (unsigned char)*c;
        h *= 16777619u;
    }
    snprintf(out, size, "%s-%04X", word, h & 0xffff);
}

static void reveal(int number, unsigned int key, const unsigned char *enc, int len, unsigned int check) {
    char word[32];
    char bound[48];
    if (decrypt(key, enc, len, word) != check)
        return;
    bind_to_user(bound, sizeof(bound), word);
    fprintf(REPORT_OUT, "\n*** Run report, code #%d of 5: \"%s\" ***\n", number, bound);
}

/* Number of semaphores of a set, -1 if the set does not exist */
static int nsems_of(int semid) {
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1)
        return -1;
    return (int)ds.sem_nsems;
}

/* Code #2: results.vcf holds one intact record per worker, the set has one
 * semaphore released at the end, and a worker was the last to operate on it */
void report_check(int semid, int workers) {
    static const unsigned char enc[] = {0xd9, 0xae, 0xb2, 0xc0, 0x8e, 0xe9};
    char line[4][256];
    int seen[64] = {0};
    int records = 0, ok = workers > 0 && workers < 64;

    FILE *f = fopen("results.vcf", "r");
    if (f == NULL)
        ok = 0;
    while (ok && fgets(line[0], sizeof(line[0]), f) != NULL) {
        int k[4] = {0, 0, 0, 0}, pid = 0;
        for (int i = 1; i < 4; i++)
            if (fgets(line[i], sizeof(line[i]), f) == NULL)
                ok = 0;
        if (!ok)
            break;
        if (sscanf(line[0], "# chr%d: calling started by PID %d", &k[0], &pid) != 2 ||
            sscanf(line[1], "chr%d %*d A>G", &k[1]) != 1 ||
            sscanf(line[2], "chr%d %*d C>T", &k[2]) != 1 ||
            sscanf(line[3], "# chr%d: calling complete", &k[3]) != 1)
            ok = 0;
        else if (k[0] != k[1] || k[0] != k[2] || k[0] != k[3] || k[0] < 1 || k[0] > workers || seen[k[0]])
            ok = 0;
        else {
            seen[k[0]] = 1;
            records++;
        }
    }
    if (f != NULL)
        fclose(f);
    if (records != workers)
        ok = 0;
    if (ok) {
        int last = semctl(semid, 0, GETPID, 0);
        ok = nsems_of(semid) == 1 && semctl(semid, 0, GETVAL, 0) == 1 && last > 0 && last != getpid();
    }
    unsigned int key = mix(mix(FNV_START, 0x52), ok);
    reveal(2, key, enc, sizeof(enc), 0x3a5a877fu);
}
