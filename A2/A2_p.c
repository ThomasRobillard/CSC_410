#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int shield_power = 50;

int main()
{
    // Starting shield power level

    pid_t pid;
    char *names[] = {"Luke", "Han", "Chewbacca", "Leia"};
    int power[] = {25, 20, 30, 15};

    printf("Millennium Falcon: Initial shield power level: %d%%\n\n", shield_power);

    // Create 4 child processes - 4 different characters adjusting shield power

    // Luke increases the shield power by 25
    // Han increases the shield power by 20
    // Chewbacca increases the shield power by 30
    // Leia increases the shield power by 15

    // Check if process creation failed
    // -----> Write you code here

    for (int i = 0; i < 4; i++)
    {
        pid = fork();
        if (pid < 0) // fork failed
        {
            perror("Fork failed");
            return 1;
        }
        else if (pid == 0)
        {
            // child process
            printf("%s: Adjusting shields...\n", names[i]);
            shield_power += power[i];
            printf("%s: Shield power level now at %d%%\n", names[i], shield_power);
            return 0; // child finished
        }
    }
    // Make parent process wait for all child processes to complete
    // -----> Write you code here
    for (int i = 0; i < 4; i++)
    {
        wait(NULL); // wait for each child to finish
    }

    // Parent process reports final state
    printf("\nFinal shield power level on the Millennium Falcon: %d%%\n", shield_power);
    printf("\nMay the forks be with you!\n");
    return 0;
}
