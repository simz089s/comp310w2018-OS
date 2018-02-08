#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    printf("First : Print to stdout\n");
    
    int stdoutfd = dup(1); // "Backup" stdout file descriptor
    int textfilefd = open("redirect_out.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    dup2(textfilefd, 1); // Redirect stdout to file

    printf("Second : Print to redirect_out.txt\n");

    dup2(stdoutfd, 1); // Restore stdout file descriptor
    printf("Third : Print to stdout\n");

    return 0;
}
