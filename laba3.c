//Perfomed by Andreev Maxim
//***********The program that implements the basic functionality of the shell and redirection of input/output***************
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

//*************************************************************************************************************************
#define ROW 16
#define SY 80
void parc(char arsymb[ROW][SY], int *word, int *cycle);                                       // Procedure of reading a string
void separ(char arsymb[ROW][SY], char *arcom[ROW + 1][ROW + 1], char *file_name[], int word); // Procedure of separation file names and commands
void exe(char *arcom[ROW + 1][ROW + 1], char *file_name[]);                                   // Procedure of rederection and execution
void pipe_0(int arr_fd[]);                                                                    // Procedure of opening pipe end for read
void pipe_1(int arr_fd[]);                                                                    // Procedure of opening pipe end for output

//**************Main program***********************************************************************************************
int main() {
    int cycle = 1;                                      // End of input flag
    int word = 0;                                       // Number of words -1
    char arsymb[ROW][SY];                               // Array of symbols
    char *arcom[ROW + 1][ROW + 1];                      // Array of commands (pointers)
    char *file_name[2];                                 // Array of file names (pointers)          
    while (cycle) {
        printf("=> ");
        parc(arsymb, &word, &cycle);
        if (word == -1)
            continue;                                   // If special value is recieved, missing an iteration
        file_name[0] = NULL;
        file_name[1] = NULL;
        separ(arsymb, arcom, file_name, word);
        exe(arcom, file_name);
    }
    return 0;
}

//**************Read and Parc**********************************************************************************************
void parc(char arsymb[ROW][SY], int *word, int *cycle) {
    char c;
    int i = 0, j = 0, IN = 0, FILE = 0;                    // Number of words -1; symbol number; in/out a word; file name flag;
    while ((c = getchar()) != EOF) {
        if ((c == '<') || (c == '>') || (c == '|')) { 
            if (j) {                                       // If not space before '<', '>', '|';
                arsymb[i++][j] = '\0';
                j = 0;
                IN = 0;
            }
            arsymb[i][j++] = c;
            FILE = 1;
            continue;
        }
        if ((c == ' ') && (IN) && (!FILE)) {               // End of word
            IN = 0;
            i++;
            j = 0;
            continue;
        }
        if ((c == '\n') && (!i) && (!IN)) {                // Empty enter
            *word = -1;
            return;
        }
        if (c == '\n') {                                   // Enter in the end of input
            if (j == 0) {                                  // For spaces after enter
                i--;
            }
            *word = i;
            return;
        }
        if (c != ' ') {                                    // Not space symbol
            arsymb[i][j++] = c;
            arsymb[i][j] = '\0';
            IN = 1;
            FILE = 0;
        }
    }
    *cycle = 0;                                            // End of input
    *word = i;
    if ((i == 0) & (j == 0))                               // Empty input
        *word = -1;
}

//**************Separation*************************************************************************************************
void separ(char arsymb[ROW][SY], char *arcom[ROW + 1][ROW + 1], char *file_name[], int word) {
    int s = 0, p = 0;                       // Command number; number of pipes;
    for (int i = 0; i <= word; i++) {
        if (arsymb[i][0] == '>') {          // For input file 
            file_name[1] = &arsymb[i][1];
            continue;
        }
        if (arsymb[i][0] == '<') {          // For outputfile
            file_name[0] = &arsymb[i][1];
            continue;
        }
        if (arsymb[i][0] == '|') {          // For conveyor
            arcom[p++][s] = NULL;
            s = 0;
            arcom[p][s++] = &arsymb[i][1];
            continue;
        }
        arcom[p][s++] = arsymb[i];
    }
    arcom[p][s] = NULL;
    arcom[p + 1][0] = NULL;                 // Terminal null after last pipe
}

//**************Redirection and execution**********************************************************************************
void exe(char *arcom[ROW + 1][ROW + 1], char *file_name[]) {
    int pipefd[ROW][2];                                                             // Array of pipe descriptors
    int p = 1;                                                                      // Number of pipes
    for (int i = 0; arcom[i][0] != NULL; i++) {
        if (-1 == pipe(pipefd[i])) {
            perror("pipe");
            exit(1);
        }
        p++;
    }
    for (int i = 0; arcom[i][0] != NULL; i++) {
        pid_t child_pid = fork();                                                   // Child branch
        if (!child_pid) {
            if (i && ((i + 1) != p)) {                                              // Opening end for read
                pipe_0(pipefd[i]);
            }
            if (file_name[0] && !i) {
                int fd1 = open(file_name[0], O_RDONLY, 0666);
                if (fd1 == -1) {
                    perror("open");
                    exit(1);
                }
                if (-1 == dup2(fd1, STDIN_FILENO)) {
                    perror("dup2");
                    exit(1);
                }
            }
            if (arcom[i + 1][0]) {                                                  // Opening end for output
                pipe_1(pipefd[i + 1]);
            }
            if (file_name[1] && !arcom[i + 1][0]) {
                int fd2 = open(file_name[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd2 == -1) {
                    perror("open");
                    exit(1);
                }
                if (-1 == dup2(fd2, STDOUT_FILENO)) {
                    perror("dup2");
                    exit(1);
                }
            }
            if (execvp(arcom[i][0], arcom[i]) == -1) {                              // Execution and error checking
                perror("execvp");
                exit(1);
            }
            exit(0);
        }
        close(pipefd[i][0]);
        close(pipefd[i][1]);
        int pid = wait(NULL);                                                       // Parent branch
        if (pid == -1) {
            perror("wait");
            exit(1);
        }
    }
}

//**************Opening end for read***************************************************************************************
void pipe_0(int arfd[]) {
    close(arfd[1]);
    if (-1 == dup2(arfd[0], STDIN_FILENO)) {
        perror("dup2");
        exit(1);
    }
}

//**************Opening end for output*************************************************************************************
void pipe_1(int arfd[]) {
    close(arfd[0]);
    if (-1 == dup2(arfd[1], STDOUT_FILENO)) {
        perror("dup2");
        exit(1);
    }
}
