//**************The program that implements the basic functionality of the shell******************
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

//************************************************************************************************
#define ROW 16
#define SY 80
void exe(char *arpointer[]);                                // Procedure of execution
void parc(char arsymb[ROW][SY], int *word, int *cycle);     // Procedure of reading a string
                                
//**************Main program**********************************************************************
int main() {
int cycle = 1;                                              // End of input flag
int word = 0;                                               // Number of words
char arsymb[ROW][SY];                                       // Array of symbols
     while (cycle) {
        printf("=> ");
        parc(arsymb, &word, &cycle); 
        if (word == -1)
            continue;                                       // If special value is recieved, missing an iteration 
        char *arpointer[word + 1];                          // Array of pointers
        for (int i = 0; i <= word; i++) {
            arpointer[i] = arsymb[i]; 
                                        }
        arpointer[word + 1] = NULL;                         // Terminal null at the end of a string  
        exe(arpointer); 
                   }
    printf("\n");
    return 0;
           }

//**************Read and Parc*********************************************************************
void parc(char arsymb[ROW][SY], int *word, int *cycle) {
    char c;
    int i = 0, j = 0, IN = 0;                               // Number of words; symbol number; in/out a word
    while ((c = getchar()) != EOF) {
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
	if ((i == 0)&(j == 0))                              // Empty input
	    *word = -1;   
                                                       }

//**************Execution*************************************************************************
void exe(char *arpointer[]) {
    pid_t child_pid = fork(); 
    if (!child_pid) {                                       // Child branch
        if (execvp(arpointer[0], arpointer) == -1) {        // Execution and error checking
            perror("exexvp");
            exit(1);
                                                   }
        exit(0); 
                    } 
    int pid = wait(NULL);                                   // Parent branch
    if (pid == -1) { 
        perror("wait");
        exit(1);
                   }
                            }


