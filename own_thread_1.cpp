#include <iostream>

#include <stdlib.h> // for exit()
#include <unistd.h> // for clone(), getpid()
#include <sys/wait.h> // for waitpid()
#include <sys/syscall.h> // for SYS_gettid

int thread_func(void*)
{
    std::cerr << "New thread created, pid: " << getpid() << std::endl;
    std::cerr << "Parent pid: " << getppid() << std::endl;
    std::cerr << "Thread id: " << syscall(SYS_gettid) << std::endl;
    sleep(2);
    return 0;
}

int main()
{
    char stack[1024*1024]; // allocate 1MB stack
    char *stack_top = stack + sizeof(stack); // stack top is the highest address of the allocated memory

    // Create the new thread
    pid_t pid = clone(thread_func, stack_top, CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID,
                      NULL, NULL);

    /*
        CLONE_VM - child thread will share the same memory space as the parent.
        CLONE_SIGHAND - child thread will share the signal handlers with the parent.
        CLONE_FS - child thread will share the file system information (such as the current working directory)
        CLONE_FILES - child thread will share file descriptors with the parent.
        CLONE_SYSVSEM - child thread will share System V semaphore operations.
        CLONE_PARENT_SETTID - Passes the thread ID of the child thread to the parent process.
        CLONE_CHILD_CLEARTID - Clears the child’s thread ID when it exits
    */

    if (pid == -1)
    {
        std::cerr << "clone failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    int status{0};
    // Wait for child thread to exit
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status))
    {
        std::cerr << "Child thread exited with status: " << WEXITSTATUS(status);
    }

    return 0;
}
