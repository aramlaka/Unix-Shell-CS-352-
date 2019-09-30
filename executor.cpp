#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"

int8_t Executor::executeBatchProcess(std::vector<Process> newProcessVector)
{
    // Check to make sure there are actually new processes to execute
    if (newProcessVector.size()  == 0)
    {
        return -1;
    }

    auto processVectorIterator = newProcessVector.begin();

    for(; processVectorIterator != newProcessVector.end(); ++processVectorIterator)
    {
        int processResult = 0;
        if (processVectorIterator->getBackground() == false)
        {
            // This means we have a foreground process
            processResult = executeForegroundProcess(*processVectorIterator);
        }
        else
        {
            // This is a background process
            processResult = executeBackgroundProcess(*processVectorIterator);
        }

        if (processResult != 0)
        {
            // This means we had an error in executing a process, so abandon the batch of processes and return up to the shell
            return processResult;
        }
    }

    // Everything executed as planned, so return a 0
    return 0;
}

int8_t Executor::executeForegroundProcess(Process foregroundProcess)
{
    pid_t pid = fork();

    // Check to make sure the process was correctly forked
    if (pid < 0)
    {
        std::cerr << "\tERROR: Failed to create new process" << std::endl;
        return -1;
    }

    if (pid == 0)
    {
        // We are in the child process now

        // TODO: Extract following code into helper function
        // See if we have an input file to read from
        if (foregroundProcess.getInputFile().size() > 0)
        {
            int file = open(foregroundProcess.getInputFile().c_str(), O_RDONLY);

            // Check to make sure the file was opened properly
            if (file < 0)
            {
                std::cerr << "\tERROR: Failed to open file: " + foregroundProcess.getInputFile() + " ABORTING..." << std::endl;
                exit(-1);
            } 

            else
            {
                // Change our stdin to the file
                dup2(file, STDIN_FILENO);

                // Close the file
                close(file);
            }
            
        }

        // Now check if there is an output file
        if (foregroundProcess.getOuptutFile().size() > 0)
        {
            // TODO: Change for depending on truncation or appending
            int file = open(foregroundProcess.getOuptutFile().c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

            // Check to make sure the file was opened properly
            if (file < 0)
            {
                std::cerr << "\tERROR: Failed to open file: " + foregroundProcess.getInputFile() + " ABORTING..." << std::endl;
                exit(-1);
            }

            else
            {
                // Change the stdout to this file
                dup2(file, STDOUT_FILENO);

                // Close the file
                close(file);
            } 
        }

        // Now we execute the process
        if (foregroundProcess.getCommand().compare("cd") == 0)
        {
            // TODO: Handle cd command
        }
        else if (foregroundProcess.getCommand().compare("clr") == 0)
        {
            execlp("clear", "clear", NULL);
        }
        else if (foregroundProcess.getCommand().compare("dir") == 0)
        {
            // TODO: Handle dir command
        }
        else if (foregroundProcess.getCommand().compare("environ") == 0)
        {
            // TODO: Handle environ command
        }
        else if (foregroundProcess.getCommand().compare("echo") == 0)
        {
            // TODO: Handle echo command
        }
        else if (foregroundProcess.getCommand().compare("help") == 0)
        {
            // TODO: Handle help command
        }
        else
        {
            std::vector<std::string> args = foregroundProcess.getArgs();
            std::vector<char*> cstrings{};

            // Make sure to push back the command first
            char* command = new char[foregroundProcess.getCommand().size() + 1];
            std::copy(foregroundProcess.getCommand().begin(), foregroundProcess.getCommand().end(), command);
            cstrings.push_back(command);
            
            // Needed for a conversion from a vector of strings to a char** for execvp
            for(auto& string: args)
            {
                cstrings.push_back(&string.front());
            }

            // Execute the process
            execvp(foregroundProcess.getCommand().c_str(), cstrings.data());   
        }

        exit(0);
    }

    else
    {
        int status = 0;
        waitpid(pid, &status, 0);

        if (status != 0)
        {
            std::cerr << "\tERROR: Child process produced an error" << std::endl;
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

int8_t Executor::executeBackgroundProcess(Process backgroundProcess)
{
    // TODO: Add background process implementation
    return 0;
}