
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
void exe(char *arpointer[], char arfiles[2][SY]);                                // Procedure of rederection and execution
void parc(char arsymb[ROW][SY], int *word, int *cycle, char arfiles[2][SY]);     // Procedure of reading a string
                                
//**************Main program***********************************************************************************************
int main() {
int cycle = 1;                                              // End of input flag
int word = 0;                                               // Number of words -1
char arsymb[ROW][SY];                                       // Array of symbols
char arfiles[2][SY];                                        // Array of file names
     while (cycle) {
        printf("=> ");
        parc(arsymb, &word, &cycle, arfiles); 
        if (word == -1)
            continue;                                       // If special value is recieved, missing an iteration 
        char *arpointer[word + 1];                          // Array of pointers
        for (int i = 0; i <= word; i++) {
            arpointer[i] = arsymb[i]; 
        }
        arpointer[word + 1] = NULL;                         // Terminal null at the end of a string  
        exe(arpointer, arfiles); 
    }
    return 0;
}

//**************Read and Parc**********************************************************************************************
void parc(char arsymb[ROW][SY], int *word, int *cycle, char arfiles[2][SY]) {
    char c;
    int i = 0, j = 0, IN = 0;                               // Number of words -1; symbol number; in/out a word
    arfiles[0][0] = arfiles[1][0] = '\0';                   
NXT:   
    while ((c = getchar()) != EOF) {
        if (c == '<') {                                     // For output redirection
            if (j) {                                        // Not space before <
                arsymb[i++][j] = '\0';
                j = 0;
                IN = 0;
            }
            while ((c = getchar()) != EOF) {
                if ((c ==' ')&& j) {                        // Space after output file name
                    arfiles[0][j] = '\0';
                    j = 0;
                    goto NXT;
                }
                if (c == '\n') {                            // Enter after output file name 
                    arfiles[0][j] = '\0';
                    *word = i - 1;
                    return;
                }
                if (c == '>') {                             // For input redirection after output redirection
                        arfiles[0][j] = '\0';
                        j = 0;
                        while ((c = getchar()) != EOF) {
                            if ((c ==' ')&& j) {            // Space after input file name
                                arfiles[1][j] = '\0';
                                j = 0;
                                goto NXT;
                            }
                            if (c == '\n') {                // Enter after input file name
                                arfiles[1][j] = '\0';
                                *word = i - 1;
                                return;
                            }
                            if (c != ' ') {                 // Not space symbol
                            arfiles[1][j++] = c;
                            }
                        }
                        *word = -1;
                        return;
                }
                if (c != ' ') {
                arfiles[0][j++] = c;
                }
            }
            *word = -1;
            return;
        }  
        
        if (c == '>') {                                     // For input redirection
            if (j) {                                        // Not space before >
                arsymb[i++][j] = '\0';
                j = 0;
                IN = 0;
            }
            while ((c = getchar()) != EOF) {
                if ((c ==' ') && j) {                       // Space after file name
                    arfiles[1][j] = '\0';
                    j = 0;
                    goto NXT;
                }
                if (c == '\n') {                            // Enter after file name
                    arfiles[1][j] = '\0';
                    *word = i - 1;
                    return;
                }
                if (c != ' ') {                             // Not space symbol
                arfiles[1][j++] = c;
                }
            }
            *word = -1;
            return;
        }
        
        if ((c == ' ')&&(IN)) {                             // End of word
            IN = 0;
            i++;
            j = 0;
            continue;
        }       
        if ((c == '\n')&&(!i)&&(!IN)) {                     // Empty enter
            *word = -1;
            return;
        }
        if (c == '\n') {                                    // Enter in the end of input
                if (j == 0) {                               // For spaces after enter
                    i--;
                }
            *word = i;
            return;
        }
        
        if (c != ' ') {                                     // Not space symbol
            arsymb[i][j++] = c;
            arsymb[i][j] = '\0';
            IN = 1;
        }
    }  
    *cycle = 0;                                             // End of input
    *word = i;
	if ((i == 0)&(j == 0))                                  // Empty input
	    *word = -1;   
}

//**************Redirection and execution**********************************************************************************
void exe(char *arpointer[], char arfiles[2][SY]) {
    int fl_len1 = strlen(arfiles[0]), fl_len2 = strlen(arfiles[1]);                 // Lenghth of output/intput file name
    pid_t child_pid = fork();                                                       // Child branch
    if (!child_pid) {   
        if (fl_len2) { 
            char *fl_namepointer;                                                  
            fl_namepointer = arfiles[1];
            int fd2 = open(fl_namepointer, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd2 == -1) {
                perror("open");
                exit(1);
            }   
            if (-1 == dup2(fd2, STDOUT_FILENO)) {
                perror("dup2");
                exit(1);
            }
        }
        if (fl_len1) {
            char *fl_namepointer;
            fl_namepointer = arfiles[0];
            int fd1 = open(fl_namepointer, O_RDONLY, 0666);
            if (fd1 == -1) {
                perror("open");
                exit(1);
            }   
            if(-1 == dup2(fd1, STDIN_FILENO)) {
		        perror("dup2");
		        exit(1);
	        }
        }                                         
        if (execvp(arpointer[0], arpointer) == -1) {                                // Execution and error checking
            perror("exexvp");
            exit(1);
        }
        exit(0); 
    }
    int pid = wait(NULL);                                                           // Parent branch
    if (pid == -1) { 
        perror("wait");
        exit(1);
    }
}

