#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <mpi/mpi.h>

/******************************************************************************
Connell Henry - Brute Force Program using C and OpenMPI

This is my program using OpenMPI to 'brute-force' a password
Initially, the user will enter a password and see its SALT encryption
After this, the method 'crack' will be called, initially by splitting
the work done accross 4 nodes (the number of nodes I had available to use) in 
order to find an encrypted match: By using this method, the combinatorial explosion 
caused by the encryption of a password of length 7 and a Upper/Lower case possibility 
is split to be more easily solved:
For Example:
To find the total number of combinations needing to be tested for this program is

X ^y
Where X = The Indices covering the number of different characters available for use 
			**(for typical passwords, this is 94 out of 96 possible keys on a standard 
				english keyboard)
and ^y = The password length
	SO
	52 ^7 = 1,028,071,703,000 Possible Combinations 
	
This will be split by:
Node 1: 'A' - 'G', then 'a' - 'g'
Node 2: 'G' - 'M', then 'g' - 'm'
Node 3: 'M' - 'S', then 'm' - 's'
Node 4: 'S' - 'Z', then 's' - 'z'

After initial issues with having MPI_Send and MPI_Recv correctly to implement the desired 'Farmer 
to Worker' method (for efficiency), I instead utilised: MPI_Abort
A function that sends the exit code to the Nodes to stop immediately (used mainly for debugging)
If the entered password comprises of Upper/Lower case chars, the password will be found and 
returned to the terminal

To Compile:
mpicc -o PassWords passWords.c -lcrypt

To Run
mpirun -n 4 ./PassWords
******************************************************************************/

#define SALT "KB"

char encrypted[15]; // Encrypted password
int zero = 0;
char p_plain[7]; // Char array to hold 7 character password
int world_rank; // MPI Node ID

// Time Measurements to record performance of program
    typedef struct arguments
    {
        int begin;
        int stop;
    } arguments_t;

    int time_difference(struct timespec *begin, struct timespec *finish, long long int *difference)
    {
        long long int   ds = finish->tv_sec - begin->tv_sec;
        long long int   dn = finish->tv_nsec - begin->tv_nsec;
            if (dn < 0)
            {
                ds--;
                dn += 1000000000;
            }
        *difference = ds * 1000000000 + dn;
        return !(*difference > 0);
    }
// End of Time Measurements

    void substr(char *dest, char *src, int start, int length)
    {
        memcpy(dest, src + start, length);
        *(dest + length) = '\0';
    }

    void crack(char *salt_and_encrypted)
    {
        int x, y, z;// Loop counters
        int aa, bb, cc, dd; // Loop Counters extended to accept 7 letter Chars
        int count = 0; // The number of combinations explored currently from algorithm
        char salt[3]; // This is a default 2 character string used in hashing the password ("KB")
        char plain[8];	// Char array to hold 7 chars (Normally this is of size '6', but '\' and '0' is added to the end
                        // to ensure the correct comparrison of password encryption - on line below)
        plain[7] = '\0';

        char *enc; // Encrypted version of plain
        char status; // '#' if the current combination is the solution else ' '
        char str[7];
        substr(salt, salt_and_encrypted, 0, 2 );
        printf("Salt is %s\n", salt); // Prints out the current 'SALT' to be appended at the beginning

            if (world_rank == 0)
            { // Node 1

                for (x = 'A'; x <= 'g'; x == 'G' ? x = 'a' : x++)
                {
                    plain[0] = x;
                        for (y = 'A'; y <= 'g'; y == 'G' ? y = 'a' : y++)
                        {
                            plain[1] = y;
                                for (z = 'A'; z <= 'g'; z == 'G' ? z = 'a' : z++)
                                {
                                    plain[2] = z;
                                        for (aa = 'A'; aa <= 'g'; aa == 'G' ? aa = 'a' : aa++)
                                        {
                                            plain[3] = aa;
                                                for (bb = 'A'; bb <= 'g'; bb == 'G' ? bb = 'a' : bb++)
                                                {
                                                    plain[4] = bb;
                                                        for (cc = 'A'; cc <= 'g'; cc == 'G' ? cc = 'a' : cc++)
                                                        {
                                                            plain[5] = cc;
                                                                for (dd = 'A'; dd <= 'g'; dd == 'G' ? dd = 'a' : dd++)
                                                                {
                                                                    plain[6] = dd;

                enc = (char *) crypt(plain, salt);
                count++;
                    if(strcmp(salt_and_encrypted, enc) == 0)
                    {
                        status = '#';
                        printf("Found password: \n%c%-8d%s %s\n", status, count, plain, enc);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                        exit(0);
                    } else {
                        status = ' ';
                    }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        if (world_rank == 1)
        { // Node 2

            for (x = 'G'; x <= 'm'; x == 'M' ? x = 'g' : x++)
            {
                plain[0] = x;
                    for (y = 'G'; y <= 'm'; y == 'M' ? y = 'g' : y++)
                    {
                        plain[1] = y;
                            for (z = 'G'; z <= 'm'; z == 'M' ? z = 'g' : z++)
                            {
                                plain[2] = z;
                                    for (aa = 'G'; aa <= 'm'; aa == 'M' ? aa = 'g' : aa++)
                                    {
                                        plain[3] = aa;
                                            for (bb = 'G'; bb <= 'm'; bb == 'M' ? bb = 'g' : bb++)
                                            {
                                                plain[4] = bb;
                                                    for (cc = 'G'; cc <= 'm'; cc == 'M' ? cc = 'g' : cc++)
                                                    {
                                                        plain[5] = cc;
                                                            for (dd = 'G'; dd <= 'm'; dd == 'M' ? dd = 'g' : dd++)
                                                            {
                                                                plain[6] = dd;

                enc = (char *) crypt(plain, salt);
                count++;
                    if(strcmp(salt_and_encrypted, enc) == 0)
                    {
                        status = '#';
                        printf("Found password: \n%c%-8d%s %s\n", status, count, plain, enc);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                        exit(0);
                    } else {
                        status = ' ';
                    }
                                                            }
                                                    }
                                            }
                                    }
                            }
                  }
           }
    }

    if (world_rank == 2)
    { // Node 3

        for (x = 'M'; x <= 's'; x == 'S' ? x = 'm' : x++)
        {
            plain[0] = x;
                for (y = 'M'; y <= 's'; y == 'S' ? y = 'm' : y++)
                {
                    plain[1] = y;
                        for (z = 'M'; z <= 's'; z == 'S' ? z = 'm' : z++)
                        {
                            plain[2] = z;
                                for (aa = 'M'; aa <= 's'; aa == 'S' ? aa = 'm' : aa++)
                                {
                                    plain[3] = aa;
                                        for (bb = 'M'; bb <= 's'; bb == 'S' ? bb = 'm' : bb++)
                                        {
                                            plain[4] = bb;
                                                for (cc = 'M'; cc <= 's'; cc == 'S' ? cc = 'm' : cc++)
                                                {
                                                    plain[5] = cc;
                                                        for (dd = 'M'; dd <= 's'; dd == 'S' ? dd = 'm' : dd++)
                                                        {
                                                            plain[6] = dd;
	
                enc = (char *) crypt(plain, salt);
                count++;
                    if(strcmp(salt_and_encrypted, enc) == 0)
                    {
                        status = '#';
                        printf("Found password: \n%c%-8d%s %s\n", status, count, plain, enc);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                        exit(0);
                    } else {
                        status = ' ';
                    }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
    }

    if (world_rank == 3)
    { // Node 4

        for (x = 'S'; x <= 'z'; x == 'Z' ? x = 's' : x++)
        {
            plain[0] = x;
                for (y = 'S'; y <= 'z'; y == 'Z' ? y = 's' : y++)
                {
                    plain[1] = y;
                        for (z = 'S'; z <= 'z'; z == 'Z' ? z = 's' : z++)
                        {
                            plain[2] = z;
                                for (aa = 'S'; aa <= 'z'; aa == 'Z' ? aa = 's' : aa++)
                                {
                                    plain[3] = aa;
                                        for (bb = 'S'; bb <= 'z'; bb == 'Z' ? bb = 's' : bb++)
                                        {
                                            plain[4] = bb;
                                                for (cc = 'S'; cc <= 'z'; cc == 'Z' ? cc = 's' : cc++)
                                                {
                                                    plain[5] = cc;
                                                        for (dd = 'S'; dd <= 'z'; dd == 'Z' ? dd = 's' : dd++)
                                                        {
                                                            plain[6] = dd;

                enc = (char *) crypt(plain, salt);
                count++;
                    if(strcmp(salt_and_encrypted, enc) == 0)
                    {
                        status = '#';
                        printf("Found password: \n%c%-8d%s %s\n", status, count, plain, enc);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                        exit(0);
                    } else {
                        status = ' ';
                    }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
    }
    }

    int main(int argc, char **argv) {
        
        // Initialising Time measurements
        struct timespec begin;
        struct timespec finish;
        long long int time_elapsed;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        printf("\n\nTIME START\n\n");

        printf("Enter a 7 letter password that contains only Upper/Lower case letters:");
        scanf("%s", (char *) &p_plain);
        printf("The password that you entered is %s\n", p_plain);
        printf("Your password is %d characters long\n", (int) strlen(p_plain));

        // Initialising MPI
        MPI_Init(&argc, &argv);
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        memcpy(encrypted, crypt(p_plain, SALT), 15);
        printf("When encrypted your password is %s\n", encrypted);
            crack(encrypted); // Method call to initialise Brute Force
        // Finialising MPI
        MPI_Finalize();

        clock_gettime(CLOCK_MONOTONIC, &finish);
        time_difference(&begin, &finish, &time_elapsed);
        printf("\n\nTime taken to run was %0.9lf\n\n", (time_elapsed / 1.0e9));

            return 0;
    }
