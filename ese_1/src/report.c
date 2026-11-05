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

/* Event log of the run: the children append one line per event, the
 * parent reads it back. Named after the PID of the parent. */
static void log_event(int a, int b) {
    char name[64], line[64];
    snprintf(name, sizeof(name), ".report_%d", (int)getppid());
    int fd = open(name, O_WRONLY | O_APPEND | O_CREAT, 0600);
    if (fd == -1)
        return;
    int n = snprintf(line, sizeof(line), "%d %d\n", a, b);
    if (write(fd, line, n) != n)
        perror("report log");
    close(fd);
}

#define MAX_EVENTS 4096
static int events[MAX_EVENTS][2];

static int read_events(void) {
    char name[64];
    snprintf(name, sizeof(name), ".report_%d", (int)getpid());
    FILE *f = fopen(name, "r");
    if (f == NULL)
        return 0;
    int n = 0;
    while (n < MAX_EVENTS && fscanf(f, "%d %d", &events[n][0], &events[n][1]) == 2)
        n++;
    fclose(f);
    unlink(name);
    return n;
}

void report_stage(int stage, int sample) {
    log_event(stage, sample);
}

/* Code #1: every sample went through the four stages in order, one sample
 * after the other, and the set has four semaphores */
void report_check(int semid, int samples) {
    static const unsigned char enc[] = {0x95, 0x85, 0x0b, 0x2c};
    int n = read_events();
    int ok = samples > 0 && n == samples * 4 && nsems_of(semid) == 4;
    for (int i = 0; ok && i < n; i++)
        if (events[i][0] != i % 4 || events[i][1] != i / 4 + 1)
            ok = 0;
    unsigned int key = mix(mix(FNV_START, 0x51), ok);
    reveal(1, key, enc, sizeof(enc), 0xbabf953eu);
}
