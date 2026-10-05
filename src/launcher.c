#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static pid_t child_pid = -1;

/* Forward signals to the child process group */
void handle_signal(int sig)
{
    if (child_pid > 0) {
        kill(-child_pid, sig);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [arguments...]\n", argv[0]);
        return 1;
    }

    int pipefd[2];

    /* Anonymous pipe */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Signal handling */
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        return 1;
    }

    if (child_pid == 0) {
        /* Child */

        close(pipefd[0]);

        /* Create a new process group */
        setpgid(0, 0);

        printf("[LeakSpy] Child process created (PID: %d)\n", getpid());

        /* Tell parent that child started */
        const char *message = "Child started successfully\n";
        write(pipefd[1], message, 27);

        close(pipefd[1]);

        /* Load LeakSpy only into target */
        setenv("LD_PRELOAD", "./libleakspy.so", 1);

        execvp(argv[1], &argv[1]);

        perror("execvp");
        exit(1);
    }

    /* Parent */

    close(pipefd[1]);

    /* Put child into its own process group */
    setpgid(child_pid, child_pid);

    printf("[LeakSpy] Parent process (PID: %d)\n", getpid());
    printf("[LeakSpy] Waiting for child (PID: %d)...\n", child_pid);

    /* Read message from child */
    char buffer[128];
    ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[LeakSpy] Pipe message: %s", buffer);
    }

    close(pipefd[0]);

    /* Wait for child */
    int status;

    if (waitpid(child_pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("[LeakSpy] Child exited with status: %d\n",
               WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        printf("[LeakSpy] Child terminated by signal: %d\n",
               WTERMSIG(status));
    }

    return 0;
}
