Linux command Shell simulator
# SimpleShell: A Minimalistic Command-Line Shell

SimpleShell is a basic command-line interpreter that executes user commands interactively. It waits for user input in the form of commands and continues until terminated using `Ctrl+C`. The shell supports basic commands, command history, and piped command execution.


 Features of SimpleShell

### Core Functionality
1. Command Execution
   - Forks a child process to execute a user command.
   - Parses the command into an array of parameters.
   - Uses `execvp` to replace the child process with the desired command.
   - Handles failures gracefully and returns the exit status.

2. Execution Details
   - Logs command execution details, including:
     - Command name
     - Process ID
     - Start time
     - End time
     - Execution duration

3. Command Launching  
   - Initiates a command in a child process.
   - Captures and logs execution details.
   - Returns the command's exit status.

4. Command History  
   - Maintains a list of previously executed commands.
   - Displays the history when the `history` command is entered.

5. Pipe Handling  
   - Splits commands connected by pipes (`|`) into separate child processes.
   - Executes each command sequentially with proper output redirection.

6. Input Validation  
   - Checks for potentially unsafe characters in `echo` commands, such as `/`, `"`, or `'`.

---

## How It Works

1. Header Files  
   Includes libraries for time measurement, text manipulation, process management, and standard input/output operations.

2. Definitions  
   - `MAX_INPUT_LENGTH`: Defines the maximum length for user input.
   - `MAX_HISTORY_SIZE`: Specifies the maximum number of commands to be stored in history.

3. Main Function  
   - Initializes variables for user input and command history.
   - Runs in an infinite loop to process user commands continuously.
   - Features:
     - Displays command history when `history` is entered.
     - Supports piped commands.
     - Validates commands and manages errors.
     - Tracks exit statuses and command histories.
     - Restores standard input after handling piped commands.

---

## Supported Commands
### Examples of Commands that Work:
1. `rm` - Removes files or directories.
2. `vim` - Opens the Vim editor.
3. `pwd` - Prints the current working directory.

### Commands that Do Not Work:
1. Commands with Complex Redirections  
   - Example: `command > file.txt 2>&1`  
   - Reason: SimpleShell does not support advanced input/output redirection.

2. Interactive Commands  
   - Example: `sudo apt-get install package`  
   - Reason: Interactive commands requiring user input (e.g., passwords) are not supported.
