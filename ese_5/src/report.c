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

#ifndef REPORT_OUT
#define REPORT_OUT stdout
#endif

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

/* Code #5: the set was created with the key of this directory, it has the
 * two semaphores, the queue is empty, and the signature semaphore was last
 * operated by another process, once per sample */
void report_check(int semid, key_t key, int samples) {
    static const unsigned char enc[] = {0x6a, 0x94, 0x38, 0x67, 0x93, 0xde, 0xed};
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;
    int ok = samples > 0 && key != -1 && key == ftok(".", 'q') && semctl(semid, 0, IPC_STAT, arg) != -1;
    if (ok && (ds.sem_perm.__key != key || nsems_of(semid) != 2))
        ok = 0;
    if (ok) {
        int last = semctl(semid, 1, GETPID, 0);
        ok = semctl(semid, 0, GETVAL, 0) == 0 && semctl(semid, 1, GETVAL, 0) == samples && last > 0 && last != getpid();
    }
    unsigned int key2 = mix(mix(FNV_START, 0x55), ok);
    reveal(5, key2, enc, sizeof(enc), 0x4e2f2358u);
}
