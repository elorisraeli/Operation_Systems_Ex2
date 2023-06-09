#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define PERMISSIONS 0644

int main(int argc, char *argv[])
{
    int input_fd, output_fd;
    ssize_t bytes_read, bytes_written;
    char buffer[BUFFER_SIZE];
    bool verbose = false, force = false;
    int opt;

    while ((opt = getopt(argc, argv, "vf")) != -1)
    {
        switch (opt)
        {
        case 'v':
            verbose = true;
            break;
        case 'f':
            force = true;
            break;
        default:
            fprintf(stderr, "Usage: %s <source_file> <destination_file> [-v] [-f]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind + 2 != argc)
    {
        printf("%d\n", 1);
        fprintf(stderr, "Usage: %s <source_file> <destination_file> [-v] [-f]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_fd = open(argv[optind], O_RDONLY);
    if (input_fd == -1)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    int open_flags = O_CREAT | O_WRONLY;
    if (force)
    {
        open_flags |= O_TRUNC;
    }
    else
    {
        open_flags |= O_EXCL;
    }

    output_fd = open(argv[optind + 1], open_flags, PERMISSIONS);
    if (output_fd == -1)
    {
        if (verbose)
        {
            if (errno == EEXIST)
            {
                printf("target file exists\n");
            }
            else
            {
                printf("general failure\n");
            }
        }
        else
        {
            printf("%d\n", 1);
        }
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0)
    {
        bytes_written = write(output_fd, buffer, bytes_read);
        if (bytes_written != bytes_read)
        {
            if (verbose)
            {
                printf("general failure\n");
            }
            else
            {
                printf("%d\n", 1);
            }
            close(input_fd);
            close(output_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(input_fd);
    close(output_fd);

    if (verbose)
    {
        printf("success\n");
    }
    else
    {
        printf("0\n");
    }

    return 0;
}
