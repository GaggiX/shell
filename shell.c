#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int cd_command(char **args);
int exit_command(char **args);
int help_command(char **args);
int set_command(char **args);
int unset_command(char **args);

char *builtin_str[] = {
  "cd",
  "exit",
  "help",
  "set",
  "unset"
};

int (*builtin[]) (char **) = {
  &cd_command,
  &exit_command,
  &help_command,
  &set_command,
  &unset_command
};

int exec_command(char **args){
  for (size_t i = 0; i < (sizeof(builtin_str) / sizeof(char *)); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (builtin[i]) (args);
    }
  }
  pid_t fork_return = fork();
  if (fork_return == 0) {
    execvp(args[0], args);
    fprintf(stderr, "gsh: %s command not found\n", args[0]);
    exit(127);
  } else if (fork_return > 0) {
    wait(NULL);
    return 0;
  } else {
    perror("gsh: fork error");
    return 1;
  }
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
    if (token[0] == '$' && position != 0) {
      //remove $ from the token and get the environment variable
      char *env = getenv(memmove(&token[0], &token[1], strlen(token)));
      //if env is NULL, there is no environment variable
      if (env == NULL) {
        env = '\0';
      }
      token = env;
    }
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
  fputs(geteuid() == 0 ? "# " : "$ ", stdout);

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

int cd_command(char **args) {
  //if there is no argument, cd returns to $HOME
  int chdir_return = chdir(args[1] == NULL ? getenv("HOME") : args[1]);
  if (chdir_return) {
    perror(args[0]);
  }
  //set $PWD to the current working directory
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    int setenv_return = setenv("PWD", cwd, 1);
    if (setenv_return) {
      perror("setenv()");
    }
  } else {
    perror("getcwd()");
  }
  return 0;
}

int exit_command(char **args) {
  return 1;
}

int help_command(char **args) {
  puts("GAGGI's shell\nShell: $ [command] [args...]");
  return 0;
}

int set_command(char **args) {
  if (args[1] != NULL && args[2] != NULL) {
    //1 -> overwrite
    int setenv_return = setenv(args[1], args[2], 1);
    if (setenv_return) {
      perror(args[0]);
    }
  } else {
    puts("help: set <name> <value>");
  }
  return 0;
}

int unset_command(char **args) {
  if (args[1] != NULL) {
    int unsetenv_return = unsetenv(args[1]);
    if (unsetenv_return) {
      perror(args[0]);
    }
  } else {
    puts("help: unset <name>");
  }
  return 0;
}
