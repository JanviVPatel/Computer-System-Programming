// radio_station.c
// Compile: gcc radio_station.c -o radio_station

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>   // for fd_set, select
#include <sys/time.h>     // for timeval
#include <sys/types.h>    // for fd_set on some systems
#include <errno.h>

pid_t pid1 = 0, pid2 = 0;

void show_menu() {
    printf("\n📻 --- Controls ---\n");
    printf("p : Pause listeners\n");
    printf("r : Resume listeners\n");
    printf("s : Stop broadcast\n");
    printf("------------------\n");
}

int main() {
    int fd1[2], fd2[2];
    if (pipe(fd1) == -1) { perror("pipe1"); return 1; }
    if (pipe(fd2) == -1) { perror("pipe2"); return 1; }

    pid1 = fork();
    if (pid1 < 0) { perror("fork1"); return 1; }
    if (pid1 == 0) {
        // child1
        close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        dup2(fd1[0], STDIN_FILENO);
        execl("./listener", "listener", "1", NULL);
        perror("execl listener1");
        _exit(1);
    }

    pid2 = fork();
    if (pid2 < 0) { perror("fork2"); return 1; }
    if (pid2 == 0) {
        // child2
        close(fd2[1]);
        close(fd1[0]); close(fd1[1]);
        dup2(fd2[0], STDIN_FILENO);
        execl("./listener", "listener", "2", NULL);
        perror("execl listener2");
        _exit(1);
    }

    // parent
    close(fd1[0]);
    close(fd2[0]);

    printf("\n Multithreaded Process Radio Started!\n");
    printf("Listener1 pid=%d  Listener2 pid=%d\n", pid1, pid2);
    show_menu();

    char *songs[] = {"Why this kolaveri di", " Mai hu gian", "All is well", "Sunshine"};
    int total = sizeof(songs)/sizeof(songs[0]);
    int i = 0;
    int paused = 0;

    char cmdline[64];

    while (1) {
        /* broadcast if not paused 
        if (!paused) {
            char *song = songs[i % total];
            printf("\nBroadcasting: %s\n", song);

            ssize_t w;

            w = write(fd1[1], song, strlen(song));
            if (w == -1 && errno != EPIPE) perror("write fd1");
            write(fd1[1], "\n", 1);

            w = write(fd2[1], song, strlen(song));
            if (w == -1 && errno != EPIPE) perror("write fd2");
            write(fd2[1], "\n", 1);

            i++;
        } else {
            printf("\n(Broadcast paused — not sending new songs)\n");
        }
*/
if (!paused) {
    /* pick a random song each time */
    char *song = songs[rand() % total];
    printf("\nBroadcasting: %s\n", song);

    ssize_t w;

    w = write(fd1[1], song, strlen(song));
    if (w == -1 && errno != EPIPE) perror("write fd1");
    write(fd1[1], "\n", 1);

    w = write(fd2[1], song, strlen(song));
    if (w == -1 && errno != EPIPE) perror("write fd2");
    write(fd2[1], "\n", 1);

    /* no i++ — not needed anymore */
} else {
    printf("\n(Broadcast paused — not sending new songs)\n");
}

        /* check for user input without blocking long: poll in small intervals */
        for (int t = 0; t < 4; ++t) {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(STDIN_FILENO, &rfds);
            struct timeval tv = { .tv_sec = 0, .tv_usec = 500000 }; // 0.5s

            int sel = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
            if (sel > 0 && FD_ISSET(STDIN_FILENO, &rfds)) {
                if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
                    // EOF or error
                    continue;
                }
                char cmd = cmdline[0];
                if (cmd == 'p') {
                    if (pid1) kill(pid1, SIGSTOP);
                    if (pid2) kill(pid2, SIGSTOP);
                    paused = 1;
                    printf("⏸ Paused listeners.\n");
                } else if (cmd == 'r') {
                    if (pid1) kill(pid1, SIGCONT);
                    if (pid2) kill(pid2, SIGCONT);
                    paused = 0;
                    printf("▶ Resumed listeners.\n");
                } else if (cmd == 's') {
                    printf("🛑 Stopping broadcast...\n");
                    if (pid1) kill(pid1, SIGTERM);
                    if (pid2) kill(pid2, SIGTERM);
                    // close write ends so listeners get EOF
                    close(fd1[1]); close(fd2[1]);
                    goto wait_children;
                } else {
                    show_menu();
                }
                show_menu();
            }
            // else timeout: continue to next small interval
        }
    }

wait_children:
    if (pid1) waitpid(pid1, NULL, 0);
    if (pid2) waitpid(pid2, NULL, 0);
    printf("📻 Broadcast ended.\n");
    return 0;
}
