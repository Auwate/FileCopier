#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

/*
To test if the signal/alarm is working, change the BUFFER_LENGTH to something like 10. Anything close to 1-2 will make the program slow to a crawl.
*/
#define BUFFER_LENGTH 1000

void display_message (int signal);
void argValidator (int argc);
void errorChecker (int errorNumber, char * filePointer);
int openFile (char * fileLocation, int flags, mode_t mode);

int main(int argc, char** argv) {

  // Calls the argValidator method.
  // This calls exit(1) if argc does not have
  // 3 arguments
  argValidator(argc);

  // Connects signal to SIGALRM
  signal(SIGALRM, display_message);

  // Allocates memory for string variables and
  // Copies the values of argv[1] and argv[2]
  char * file1 = (char *) malloc (sizeof(argv[1]));
  char * file2 = (char *) malloc (sizeof(argv[2]));
  file1 = strdup(argv[1]);
  file2 = strdup(argv[2]);

  // Open both files, using the openFile(...)
  // function below. If the target file does
  // not exist, we create it.
  int sourceFile = openFile(file1, O_RDONLY, S_IRUSR);
  int destinationFile = openFile(file2, O_WRONLY, S_IWUSR);

  // Now we need to set up variables for the read operation. This will require various extraneous datatypes
  ssize_t amountOfBytesRead = -1;
  ssize_t amountOfBytesWritten = -1;
  char * buffer = (char *) malloc (sizeof(char) * BUFFER_LENGTH);

  // Creates alarm
  alarm(1);

  /*
    This is the block of code where we will be getting data from the sourceFile and writing it into the destinationFile. Once the bytes for both fields are 0, that means we have reached the EOF.
    This uses read(...), write(...), and calls the errorChecker(...) method.
  */
  while (amountOfBytesRead != 0 && amountOfBytesWritten != 0) {

    while ((amountOfBytesRead = read(sourceFile, buffer, BUFFER_LENGTH)) == -1) {

      // Check error
      errorChecker(amountOfBytesRead, file1);
      
    }

    while ( (amountOfBytesWritten = write(destinationFile, buffer, amountOfBytesRead)) == -1) {

      // Check error
      errorChecker(amountOfBytesWritten, file2);
      
    }
    
  }

  // Closes the files
  close(sourceFile);
  close(destinationFile);
  
  return 0;
  
}

/*
  Prints the message for the signal function
*/
void display_message (int signal) {

  printf("copyit: still copying...\n");
  alarm(1);
  
}

/*
  Opens the file designated by the fileLocation pointer. If it does not exist, we create it for the user.
*/
int openFile (char * fileLocation, int flags, mode_t mode) {
  int fileDescriptor = open (fileLocation, flags, mode);
  if (fileDescriptor == -1) {
    if (errno != 2) {
    errorChecker(fileDescriptor, fileLocation);
    }
    else if (flags == O_RDONLY) {
    errorChecker(fileDescriptor, fileLocation);
    }
    else {
    fileDescriptor = creat(fileLocation, S_IRWXU);
    }
  } 
  return fileDescriptor;
}

/*
  Checks to make sure that there are exactly
  two arguments passed
*/
void argValidator (int argc) {
  if (argc < 3) {
    printf("copyit: Not enough arguments!\nusage: copyit <sourcefile> <targetfile>");
    exit(1);
  }

  else if (argc > 3) {
    printf("copyit: Too many arguments!\nusage: copyit <sourcefile> <targetfile>");
    exit(1);
  }
}

// Checks the error
// If it is caused by the signal, create a new alarm and return back to the function where it was called
void errorChecker (int errorNumber, char * filePointer) {

    if (errno == 4) {
      alarm(1);
      errno = 0;
      return;
    }

    else {
      printf("Unable to open %s: %s\n", filePointer,strerror(errno));
    exit(1);
    }
  
}