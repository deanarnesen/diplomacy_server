#include <stdio.h>

int main() {
    //printf("Hello, World!\n");
    ////write to file (works)
    //FILE* test = fopen("test.txt", "w");
    //fprintf(test, "Aw is a test\n");
    //fclose(test);

    //read from file
    FILE* test_read = fopen("test.txt", "r");
    if(test_read == NULL){
        fprintf(stderr, "open error\n");
        return -1;
    }
    char buffer[100];
    //read first word in file into buffer
    int success = fscanf(test_read, "%s", buffer);
    if(!success){
        printf("we didn't read anything");
    }
    //print the first word in buffer to stdout
    fprintf(stdout, "%s", buffer);
    //be sure stdout is flushed (??)
    fflush(stdout);
    fclose(test_read);
    return 0;
}
