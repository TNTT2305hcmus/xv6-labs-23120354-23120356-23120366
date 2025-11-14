#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START 2
#define END 280

/*
Algorithm: Sieve of Eratosthenes (implied)
    - From the sequence: 2, 3, 4, 5, 6, 7, 8, 9, 10, ..., 280, we find the prime sequence by:
    - Eliminating numbers divisible by 2: 3, 5, 7, 9, 11, ..., 280
    - Eliminating numbers divisible by 3: 5, 7, 11, ...
    - ...
Pipe: Inter-process Communication (IPC)
    - Creates input and output channels for a process.
    - pipefd[0] is the read end (input).
    - pipefd[1] is the write end (output).
fork(): Process Management
    - Creates a child process.
pipe + fork: Communication between parent and child processes.
*/

/*
Function Execution/Purpose:
    - Returns the read end (input) of the parent process.
    - Continuously pushes numbers from 'start' to 'end' into the write end (output) of the child process.
*/
int generate_number()
{
    // Declare pipe
    int pipefd[2];
    pipe(pipefd);

    // Create child process
    int pid = fork();
    if (!pid)
    {
        // Push numbers from START to END into the write end (output) of the child process
        for (int i = START; i <= END; i++)
        {
            write(pipefd[1], &i, sizeof(int));
        }
        // Close the write end after use
        close(pipefd[1]);
        exit(0);
    }

    // The parent process does not write anything, so close the write end
    close(pipefd[1]);
    return pipefd[0];
}

/*
Function Execution/Purpose:
    - Select numbers not divisible by prime and push them into the child's output
    - fd_in is the input of the previous list, containing numbers not yet filtered by the current prime
*/
int filter(int fd_in, int prime)
{
    // number is the numbers not divisible
    int number;
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (!pid)
    {
        // Read each number from the previous list and select those not divisible by prime, push them to the child's output
        while (read(fd_in, &number, sizeof(int)))
        {
            if (number % prime)
            {
                write(pipefd[1], &number, sizeof(int));
            }
        }
        // Close the parent's input, no longer needed as we have the filtered list
        close(fd_in);
        close(pipefd[1]);
        exit(0);
    }

    close(fd_in);
    close(pipefd[1]);
    return pipefd[0];
}

int main(int argc, char *argv[])
{
    int prime;
    // Get the initial list
    int fd_in = generate_number();

    while (read(fd_in, &prime, sizeof(int)))
    {
        printf("prime %d\n", prime);
        // Update the input of the new parent process
        fd_in = filter(fd_in, prime);
    }
    // Wait for all child processes to finish
    while (wait(0) > 0)
    {
    }

    exit(0);
}