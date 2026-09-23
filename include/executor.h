#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

int execute_command(Command *command);
int execute_pipeline(Pipeline *pipeline);

#endif
