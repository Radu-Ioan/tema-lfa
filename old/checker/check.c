#include <stdio.h>
#include <stdlib.h>

int **read_dfa(FILE *dfa, int *n, int *m, int *s, int *f) {
    fscanf(dfa, "%d%d%d%d", n, m, s, f);

    int **conn = malloc(*n * sizeof(*conn));
    conn[0] = calloc(*n * *m, sizeof(**conn));

    for (int i = 0; i < *n; ++i) {
        conn[i] = conn[0] + i * *m;
        for(int q = 0; q < *m; ++q) {
            fscanf(dfa, "%d", &conn[i][q]);
        }
    }
    return conn;
}

int *read_states(FILE *params, int *s, int n) {
    int *result;
    if(*s == 0) {
        result = malloc(n * sizeof(*result));
        for(int i = 0; i < n; ++i) {
            result[i] = i;
        }
        *s = n;
    }
    else {
        result = malloc(*s * sizeof(*result));
        for(int i = 0; i < *s; ++i) {
            fscanf(params, "%i", &result[i]);
        }
    }
    return result;
}

int check_sync(FILE *seq, int **conn, int *starts, int s, int *stops, int f, int m) {
    int symbol;
    while(1) {
        fscanf(seq, "%i", &symbol);
        if(feof(seq)) {
            break;
        }
        if(symbol < 0 || symbol >= m) {
            printf("Out of bounds symbol\n");
            return -1;
        }
        for(int i = 0; i < s; ++i) {
            starts[i] = conn[starts[i]][symbol];
        }
    }
    for(int i = 0; i < s; ++i) {
        if(starts[i] != starts[0]) {
            printf("Did not converge\n");
            return -1;
        }
    }
    for(int i = 0; i < f; ++i) {
        if(starts[0] == stops[i]) {
            printf("Synchronized\n");
            return 0;
        }
    }
    printf("Did not finish\n");
    return -1;
}


int main (int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <dfa> <seq>\n", argv[0]);
        return -1;
    }
    FILE *fd = fopen(argv[1], "rt");
    FILE *fs = fopen(argv[2], "rt");

    int n, m, s, f;
    int **conn = read_dfa(fd, &n, &m, &s, &f);

    int *starts = read_states(fd, &s, n);
    int *stops = read_states(fd, &f, n);

    return check_sync(fs, conn, starts, s, stops, f, m);
}
