#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int reverseString();
int wordsAndLengths();

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int j = 0;
    //printf("%s\n", user_str);

    while (*user_str != '\0')
    {
        while (*user_str == ' ' && *(user_str + 1) == ' ')
        {
            user_str++;
        }
        buff[j++] = *user_str++;
    }
    
    //printf("%d, %d", j, len);
    for (int i = j; i < len; i++)
    {
        printf("%d, %d\n", i, len);
        buff[i] = '.';
    }

    buff[len] = '\0';

    //printf("%s\n", buff);
    return 0; //for now just so the code compiles. 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    if (str_len > len)
    {
        return -1;
    }

    int count = 0;
    int i = 0;
    
    while (*buff != '\0')
    {
        if (buff[i] != ' ')
        {
            while (buff[i+1] != ' ' || buff[i+1] != '\0')
            {
                
            }
        }
    }

    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int reverseString(char *buff, int len, int str_len)
{
    return 0;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This makes sure that the input is correct formatted. Ensuring that there are more than 2 arguments
    //      (i.e '[1] ./stringfun [2] -c [3] "sample string"' [other arguments]).
    //      If there are less than 2 arguments or the second argument isn't a 'decision' (-[h|c|r|w|x]) then it will not run
    //      the program and instead print a message to show the user the correct formatting and exiting
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      The purpose of this code block is to print the usage message if the user inputs less than 3 arguments (i.e. only the executable and function)
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*) malloc(BUFFER_SZ * sizeof(char*));
    if (buff == NULL)
    {
        printf("Error, malloc failed, error = %s", buff);
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            rc = reverseString(buff, BUFFER_SZ, user_str_len);
            break;

        case 'w':
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE