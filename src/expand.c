#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "expand.h"

void expand_command(Command *command)
{
    if (command == NULL)
        return;

    for (int i = 0; i < command->argument_count; i++) {

        if (strchr(command->arguments[i], '*') == NULL &&
            strchr(command->arguments[i], '?') == NULL) {
            continue;
        }

        glob_t matches;

        int result = glob(command->arguments[i], 0, NULL, &matches);

        if (result == 0 && matches.gl_pathc > 0) {

            int old_count = command->argument_count;
            int new_count =
                old_count + matches.gl_pathc - 1;

            char **new_arguments =
                realloc(command->arguments,
                        sizeof(char *) * (new_count + 1));

            if (new_arguments == NULL) {
                perror("realloc");
                globfree(&matches);
                exit(EXIT_FAILURE);
            }

            command->arguments = new_arguments;

            free(command->arguments[i]);

            for (int j = old_count - 1; j > i; j--) {
                command->arguments[j + matches.gl_pathc - 1] =
                    command->arguments[j];
            }

            for (size_t j = 0; j < matches.gl_pathc; j++) {
                command->arguments[i + j] =
                    malloc(strlen(matches.gl_pathv[j]) + 1);

                if (command->arguments[i + j] == NULL) {
                    perror("malloc");
                    globfree(&matches);
                    exit(EXIT_FAILURE);
                }

                strcpy(command->arguments[i + j],
                       matches.gl_pathv[j]);
            }

            command->argument_count = new_count;

            globfree(&matches);

            i += matches.gl_pathc - 1;
        }
    }

    command->arguments[command->argument_count] = NULL;
}
