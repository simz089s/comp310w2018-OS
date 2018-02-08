#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main()
{
    int stdoutfd = dup(1); // "Backup" stdout file descriptor
    int pipefds[2] = {0, 0};
    pipe(pipefds);

    if ( fork() == 0 ) // Child
    {
        close(pipefds[0]); // Child only needs to write
        dup2(pipefds[1], 1); // Redirect stdout to pipe
        char* argv[] = { "ls", NULL };
        execvp(argv[0], argv); // Exec ls
        close(pipefds[1]); // Don't need the pipe here anymore so close write
    }
    else // Parent
    {
        close(pipefds[1]); // Parent only reads
        char ls_output[4096/sizeof(char)] = ""; // Buffer to store piped ls output
        read(pipefds[0], ls_output, sizeof(ls_output)); // Get ls output from pipe
        dup2(stdoutfd, 1); // Restore stdout file descriptor
        puts(ls_output); // Print to stdout
        wait(NULL); // Wait for (any) child to terminate
        close(pipefds[0]); // Don't need pipe here anymore so close read
    }

    return 0;
}
