/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Please enter your name and McGill ID below
//Name: Simon Zheng
//McGill ID: 260744353

//all the header files you would require
#include <stdio.h>  //for standard IO
#include <unistd.h> //for execvp/dup/dup2
#include <string.h> //for string manipulation
#include <stdlib.h> //for fork  
#include <ctype.h>  //for character type check (isnum,isalpha,isupper etc)
#include <sys/wait.h>//for waitpid
#include <fcntl.h>  //open function to open a file. type "man 2 open" in terminal
#include <time.h>   //to handle time
#include <stdbool.h> // QoL boolean defines

//pointer to Linked list head
struct node* head_job = NULL;

//pointer to
struct node* current_job = NULL;

//global variable used to store process id of process
//that has been just created
//can be used while adding a job to linked list
pid_t process_id;

//flag variable to check if redirection of output is required
// int isred = 0;
bool isred = false;

//structure of a single node
//donot modify this structure
struct node
{
    int number;        //the job number
    int pid;           //the process id of the process
    char* cmd;         //string to store the command name
    time_t spawn;      //time to store the time it was spawned
    struct node* next; //when another process is called you add to the end of the linked list
};

// Global job number counter
int last_job_number = 1;

// Add a job to the linked list
void addToJobList(char* args[])
{
    //allocate memory for the new job
    struct node* job = malloc(sizeof(struct node));
    //If the job list is empty, create a new head
    if (head_job == NULL)
    {
        //init the job number with 1
        job->number = last_job_number;
        last_job_number++;
        //set its pid from the global variable process_id
        job->pid = process_id;
        //cmd can be set to arg[0]
        job->cmd = args[0];
        //set the job->next to point to NULL.
        job->next = NULL;
        //set the job->spawn using time function
        job->spawn = (unsigned int)time(NULL);
        //set head_job to be the job
        head_job = job;
        //set current_job to be head_job
        current_job = head_job;
    }

    //Otherwise create a new job node and link the current node to it
    else
    {
        //point current_job to head_job
        //traverse the linked list to reach the last job
        for (current_job = head_job; current_job->next != NULL; current_job = current_job->next);
        //init all values of the job like above num,pid,cmd.spawn
        job->number = last_job_number;
        last_job_number++;
        job->pid = process_id;
        job->cmd = args[0];
        job->spawn = (unsigned int)time(NULL);
        //make next of current_job point to job
        current_job->next = job;
        //make job to be current_job
        current_job = job;
        //set the next of job to be NULL
        job->next = NULL;
    }
    return;
}

//Function to refresh job list
//Run through jobs in linked list and check
//if they are done executing then remove it
void refreshJobList()
{
    //pointer require to perform operation 
    //on linked list
    struct node* current_job;
    struct node* prev_job;
    
    //variable to store returned pid 
    pid_t ret_pid;

    //perform init for pointers
    current_job = head_job;
    prev_job = head_job;

    //traverse through the linked list
    while (current_job != NULL)
    {
        //use waitpid to init ret_pid variable
        ret_pid = waitpid(current_job->pid, NULL, WNOHANG);
        //one of the below needs node removal from linked list
        if (ret_pid == 0)
        {
            //what does this mean
            //do the needful
            prev_job = current_job;
            current_job = current_job->next;
        }
        else
        {
            //what does this mean
            //do the needful
            if (current_job == head_job)
            {
                head_job = head_job->next;
                free(current_job);
                current_job = head_job;
                prev_job = head_job;
            }
            else
            {
                struct node* tmp = current_job;
                prev_job->next = current_job->next;
                current_job = prev_job->next; // Increment
                free(tmp);
            }
        }
    }
    return;
}

//Function that list all the jobs
void listAllJobs()
{
    struct node* current_job;
    int ret_pid;
    int status;
    char* status_txt;

    //refresh the linked list
    refreshJobList();

    //heading row print only once.
    printf("\nID\tPID\tCmd\tstatus\tspawn-time\n");

    //init current_job with head_job
    for (current_job = head_job; current_job != NULL; current_job = current_job->next)
    {
        ret_pid = waitpid(current_job->pid, &status, WNOHANG);
        if (ret_pid < 0) {status_txt = "ERROR";}
        else if (ret_pid == 0) {status_txt = "RUNNING";} // refreshJobList should only make this happen
        else
        {
            if (WIFEXITED(status)) {status_txt = "DONE";}
            else if (WIFSIGNALED(status)) {status_txt = "SIGTERM";}
            else {
                printf("%d\t%d\t%s\tEXIT %d\t%s\n",
                    current_job->number,
                    current_job->pid,
                    current_job->cmd,
                    WTERMSIG(status),
                    ctime(&(current_job->spawn))
                );
                continue;
            }
        }
        //traverse the linked list and print using the following statement for each job
        printf("%d\t%d\t%s\t%s\t%s\n",
            current_job->number,
            current_job->pid,
            current_job->cmd,
            status_txt,
            ctime(&(current_job->spawn))
        );
    }

    return;
}

// wait till the linked list is empty
// you would have to look for a place 
// where you would call this function.
// donot modify this function
void waitForEmptyLL(int nice, int bg)
{
    if (nice == 1 && bg == 0)
    {
        while (head_job != NULL)
        {
            puts("checking linked list");
            sleep(1);
            refreshJobList();
        }
    }
    return;
}

//function to perform word count
int wordCount(char* filename, char* flag)
{
    if (filename == NULL || flag == NULL)
    {
        puts("Missing arguments");
        return -1;
    }
    if (*flag != '-' || strlen(flag) != 2 || (*(flag+1) != 'l' && *(flag+1) != 'w'))
    {
        puts("Unrecognized flag");
        return 0;
    }
    const char* err_msg = "Error while reading file (may not be a valid file)";
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        puts(err_msg);
        return -1;
    }
    int cnt = 0;
    flag++;
    //if flag is l 
    //count the number of lines in the file 
    //set it in cnt
    if (*flag == 'l')
    {
        for (char c = getc(file); !feof(file); c = getc(file))
        {
            if (ferror(file))
            {
                puts(err_msg);
                fclose(file);
                return -1;
            }
            if (c == '\n') { cnt++; }
        }
    }

    //if flag is w
    //count the number of words in the file
    //set it in cnt
    else if (*flag == 'w')
    {
        char word[4096/sizeof(char)] = "";
        for (int n = fscanf(file, "%s", word); ; n = fscanf(file, "%s", word))
        {
            if (ferror(file) || n < 0)
            {
                puts(err_msg);
                fclose(file);
                return -1;
            }
            if (n == 1) {cnt++;} // Should not be greater than 1 anyway
            if (feof(file)) {break;}
        }
    }

    fclose(file);
    return cnt;
}

// function to augment waiting times for a process
// donot modify this function
void performAugmentedWait()
{
    int w, rem;
    time_t now;
    srand((unsigned int)(time(&now)));
    w = rand() % 15;
    printf("sleeping for %d\n", w);
    rem = sleep(w);
    return;
}

//simulates running process to foreground
//by making the parent process wait for
//a particular process id.
int waitforjob(char* jobnc)
{
    if (jobnc == NULL)
    {
        puts("fg: current: no such job");
        return -1;
    }
    struct node* trv;
    // int jobn = (*jobnc) - '0';
    int jobn = (int)strtol(jobnc, (char**)NULL, 10);
    //traverse through linked list and find the corresponding job
    //hint : traversal done in other functions too
    for (trv = head_job; trv != NULL; trv = trv->next)
    {
        //if correspoding job is found 
        //use its pid to make the parent process wait.
        //waitpid with proper argument needed here
        if (trv->number == jobn)
        {
            printf("bringing jobno %s and pid %d to foreground\n", jobnc, trv->pid);
            if (waitpid(trv->pid, NULL, WUNTRACED) == -1)
            {
                puts("Error waiting for foregrounded job");
            }
        }
    }
    return 0;
}

// splits whatever the user enters and sets the background/nice flag variable
// and returns the number of tokens processed
// donot modify this function
int getcmd(char *prompt, char *args[], int *background, int *nice)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0)
    {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL)
    {
        *background = 1;
        *loc = ' ';
    }
    else
        *background = 0;
    while ((token = strsep(&line, " \t\n")) != NULL)
    {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
        {
            if (!strcmp("nice", token))
            {
                *nice = 1;
            }
            else
            {
                args[i++] = token;
            }
        }
    }
    return i;
}

// this initialises the args to All null.
// donot modify
void initialize(char *args[])
{
    for (int i = 0; i < 20; i++)
    {
        args[i] = NULL;
    }
    return;
}

/**
 * To free allocated memory for argument strings
 * Not used in case it interferes with testing and prof said not to manage memory
 */
void free_args(int n, char* args[n])
{
    for (int i = 0; i < n; i++) {free(args[i]);}
}

int main(void)
{
    //args is a array of charater pointers
    //where each pointer points to a string
    //which may be command , flag or filename
    char *args[20];
    // initialize(args);

    //flag variables for background, status and nice
    //bg set to 1 if the command is to executed in background
    //nice set to 1 if the command is nice
    //status  
    int bg, status, nice;

    //variable to store the process id.
    pid_t pid;

    //variable to store the file des
    //helpful in output redirection
    int fd1, fd2;

    //your terminal executes endlessly unless 
    //exit command is received
    // while (1)
    while (true)
    {
        //init background to zero
        bg = 0;
        //init nice to zero
        nice = 0;
        //init args to null
        // free_args(sizeof(args)/sizeof(char*), args);
        initialize(args);
        //get the user input command
        int cnt = getcmd("\n>> ", args, &bg, &nice);
        //keep asking unless the user enters something
        while (!(cnt >= 1))
            cnt = getcmd("\n>> ", args, &bg, &nice);

        // getcmd() doesn't record "nice"
        waitForEmptyLL(nice, bg);

        //use the if-else ladder to handle built-in commands
        //built in commands don't need redirection
        //also no need to add them to jobs linked list
        //as they always run in foreground
        if (!strcmp("jobs", args[0]))
        {
            //call the listalljobs function
            listAllJobs();
            // free_args(cnt, args);
            // free_args(sizeof(args)/sizeof(char*), args);
        }
        else if (!strcmp("exit", args[0]))
        {
            //exit the execution of endless while loop 
            // free_args(sizeof(args)/sizeof(char*), args); // or free_args(cnt, args);
            // exit(0);
            exit(EXIT_SUCCESS);
        }
        else if (!strcmp("fg", args[0]))
        {
            //bring a background process to foreground
            waitforjob(args[1]);
            // free_args(cnt, args);
            // free_args(sizeof(args)/sizeof(char*), args);
        }
        else if (!strcmp("cd", args[0]))
        {
            // int result = 0;
            // if no destination directory given 
            // change to home directory 
            if (cnt == 1 || args[1] == NULL) {chdir(getenv("HOME"));}

            //if given directory does not exist
            //print directory does not exit

            //if everthing is fine 
            //change to destination directory 
            else if (chdir(args[1]) != 0) {printf("cd: %s: No such file or directory", args[1]);}

            // else {continue;} // Can't free if args[1] "."/"..", not worth checking
            // if (args[1] != NULL && strcmp(args[1], "..") != 0 && strcmp(args[1], ".") != 0)
            //     {free_args(cnt, args);}
            // free_args(sizeof(args)/sizeof(char*), args);
        }
        else if (!strcmp("pwd", args[0]))
        {
            //use getcwd and print the current working directory
            char cwd[4096/sizeof(char)] = "";
            getcwd(cwd, sizeof(cwd));
            puts(cwd);
            // free_args(cnt, args);
            // free_args(sizeof(args)/sizeof(char*), args);
        }
        else if(!strcmp("wc", args[0]))
        {
            //call the word count function
            int word_count = wordCount(args[2],args[1]);
            if (word_count >= 0) {printf("%d", word_count);}
            else {puts("Error counting");}
            // free_args(cnt, args);
            // free_args(sizeof(args)/sizeof(char*), args);
        }
        else
        {
            //Now handle the executable commands here 
            /* the steps can be..:
            (1) fork a child process using fork()
            (2) the child process will invoke execvp()
            (3) if background is not specified, the parent will wait,
                otherwise parent starts the next command... */


            //hint : samosas are nice but often there 
            //is a long waiting line for it.
            // getcmd() doesn't record "nice"
            // waitForEmptyLL(nice, bg);

            //create a child
            pid = fork();

            //to check if it is parent
            if (pid > 0)
            {
                //we are inside parent
                //printf("inside the parent\n");
                const char* waitpid_err = "Error waiting for child process";
                // if (bg == 0)
                // if (bg == false)
                if (!bg)
                {
                    //FOREGROUND
                    // waitpid with proper argument required
                    if (waitpid(pid, &status, WUNTRACED) == -1) {puts(waitpid_err);}
                }
                else
                {
                    //BACKGROUND
                    process_id = pid;
                    addToJobList(args);
                    // waitpid with proper argument required
                    if (waitpid(pid, &status, WNOHANG) == -1) {puts(waitpid_err);}
                }
            }
            else
            {
                // we are inside the child

                //introducing augmented delay
                performAugmentedWait();

                //check for redirection
                //now you know what does args store
                //check if args has ">"
                //if yes set isred to 1
                //else set isred to 0
                int i;
                for (i = 0; i < cnt; i++)
                {
                    // if (strchr(args[i], '>') != NULL)
                    // if (*args[i] == '>')
                    if (strcmp(args[i], ">") == 0) // Prof said we can assume space delimited
                    {
                        isred = true;
                        break;
                    }
                }

                //if redirection is enabled
                // if (isred == 1)
                if (isred == true) // In case isred is not = {0,1}
                {
                    //open file and change output from stdout to that  
                    //make sure you use all the read write exec authorisation flags
                    //while you use open (man 2 open) to open file
                    i++;
                    if (i+1 < cnt) {puts("Too many file arguments");}
                    if (i < cnt-1 || args[i] == NULL)
                    {
                        puts("Missing file argument");
                        continue;
                    }
                    char filename[4096/sizeof(char)] = "";
                    strcpy(filename, args[i]);
                    int txtfd = open(filename, O_CREAT|O_APPEND|O_WRONLY, 0666);

                    //set ">" and redirected filename to NULL
                    args[i] = NULL;
                    args[i - 1] = NULL;

                    //run your command
                    int stdoutfd = dup(1);
                    dup2(txtfd, 1);
                    execvp(args[0], args);

                    // This part after execvp only happens if it fails

                    //restore to stdout
                    dup2(stdoutfd, 1);
                    fflush(stdout);

                    close(txtfd);
                    _exit(EXIT_FAILURE);
                }
                else
                {
                    //simply execute the command.
                    execvp(args[0], args);
                    _exit(EXIT_FAILURE); // If execvp fails exits without disrupting parent
                }
            }
        }
        // Ideally free here but it seems some cases are difficult to free safely
        // free_args(cnt, args); // or free_args(sizeof(args)/sizeof(char*), args);
    }

    // return 0;
    return EXIT_SUCCESS;
}
