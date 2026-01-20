
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t running = 1;

void handle_sigterm(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Listener ID missing\n");
        return 1;
    }

    int listener_id = atoi(argv[1]);

    signal(SIGTERM, handle_sigterm);

    char buffer[256];

    printf("🎧 Listener %d started (pid=%d)\n", listener_id, getpid());

    while (running) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; // EOF or pipe closed
        }

        buffer[strcspn(buffer, "\n")] = 0; // remove newline
        printf("🎶 Listener %d received: %s\n", listener_id, buffer);
        fflush(stdout);
    }

    printf("👋 Listener %d exiting...\n", listener_id);
    return 0;
}
