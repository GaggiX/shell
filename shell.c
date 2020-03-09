#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int exec_command(char **args){
  if (strcmp(args[0], "cd") == 0) {
    int chdir_return = chdir(args[1]);
    if (chdir_return) {
      perror(args[0]);
    }
  } else if (strcmp(args[0], "exit") == 0) {
    return 1;
  } else if (strcmp(args[0], "help") == 0){
    puts("GAGGI's shell\nShell: $ [command] [args...]");
  } else {
    pid_t fork_return = fork();
    if (fork_return == 0) {
      execvp(args[0], args);
      fprintf(stderr, "gsh: %s command not found\n", args[0]);
      exit(127);
    } else if (fork_return > 0) {
      wait(NULL);
    } else {
      perror("gsh: fork error");
      return 1;
    }
  }
  return 0;
}

#define DELIM " \t\r\n\a"
#define ARGS_SIZE 64
char **parse_line(char *line) {
  int size = ARGS_SIZE;
  char **args = malloc(sizeof(char *) * size);
  int position = 0;
  char *token;

  token = strtok(line, DELIM);
  while (token != NULL) {
    args[position] = token;
    position++;

    if (position == size) {
      size += ARGS_SIZE;
      args = realloc(args, sizeof(char *) * size);
      printf("test\n");
      if (!args) {
        perror("gsh: allocation error");
        exit(1);
      }
    }
    token = strtok(NULL, DELIM);
  }
  args[position] = NULL;
  return args;
}

#define BUFFER_SIZE 1024
char *read_line(void) {
  int c;
  int size = BUFFER_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * size);
  if (!buffer) {
    perror("gsh: allocation error");
    exit(1);
  }

  //if user is root print #, otherwise it print $
  fputs(getuid() == 0 ? "# " : "$ ", stdout);

  while (1) {

    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    }
    buffer[position] = c;
    position++;

    if (position == size) {
      size += BUFFER_SIZE;
      buffer = realloc(buffer,sizeof(char) * size);
      if (!buffer) {
        perror("gsh: allocation error");
        exit(1);
      }
    }
  }
}

int main(void) {

  char *line;
  char **args;
  int status = 0;

  while (!status) {
    line = read_line();
    args = parse_line(line);
    if (args[0] != NULL) {
      status = exec_command(args);
    }
    free(args);
    free(line);
  }
  return 0;
}
