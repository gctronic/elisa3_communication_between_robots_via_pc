#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "elisa3-lib.h"
#include <stdlib.h>
#include <stdint.h>
#ifdef _WIN32
#include "windows.h"
#endif

#define BLACK_THR 400
#define WHITE_THR 450

int robotAddress[4];

void updateRGB(void)
{
    unsigned int rndNum;
    uint8_t red, green, blue, k;
    static uint8_t colorState = 0;

    switch (colorState)
    {
    case 0: // Red
        red = 50;
        green = 0;
        blue = 0;
        colorState = 1;
        break;
    case 1: // Green
        red = 0;
        green = 50;
        blue = 0;
        colorState = 2;
        break;
    case 2: // Blue
        red = 0;
        green = 0;
        blue = 50;
        colorState = 3;
        break;
    case 3: // Magenta
        red = 50;
        green = 0;
        blue = 50;
        colorState = 4;
        break;
    case 4: // Yellow
        red = 50;
        green = 50;
        blue = 0;
        colorState = 5;
        break;
    case 5: // Yellow
        red = 0;
        green = 50;
        blue = 50;
        colorState = 6;
        break;
    case 6: // White
        red = 50;
        green = 50;
        blue = 50;
        colorState = 0;
        break;
    }
    // printf("new color = %d, %d, %d\r\n", red, green, blue);
    for (k = 0; k < 4; k++)
    {
        setRed(robotAddress[k], red);
        setGreen(robotAddress[k], green);
        setBlue(robotAddress[k], blue);
    }
}

int main(void)
{
    unsigned int k = 0, j = 0;
    uint8_t robotState[4] = {0};
    uint16_t tickCount[4] = {0};
    // received from robot
    unsigned int robGround[4][4];
    // sent to robot
    char robRedLed, robGreenLed, robBlueLed;

    srand(time(NULL));

    // for(k=0; k<4; k++) {
    //     printf("\r\nInsert the robot address %d: ", k);
    //     scanf("%d", &robotAddress[k]);
    // }
    robotAddress[0] = 3302;
    robotAddress[1] = 3345;
    robotAddress[2] = 3338;
    robotAddress[3] = 3344;

    // init the communication with the robots; set the addresses and number of the robots to control
    startCommunication(robotAddress, 4);

    for (k = 0; k < 4; k++)
    {
        enableObstacleAvoidance(robotAddress[k]);
        setLeftSpeed(robotAddress[k], 15);
        setRightSpeed(robotAddress[k], 15);
    }

    while (1)
    {
        getAllGroundFromAll(robGround);
        // printf("%4d\t %4d\t %4d\t %4d\n\r", robGround[0][0], robGround[0][1], robGround[0][2], robGround[0][3]);
        // printf("%4d\t %4d\t %4d\t %4d\n\r", robGround[1][0], robGround[1][1], robGround[1][2], robGround[1][3]);
        // printf("%4d\t %4d\t %4d\t %4d\n\r", robGround[2][0], robGround[2][1], robGround[2][2], robGround[2][3]);
        // printf("%4d\t %4d\t %4d\t %4d\n\r", robGround[3][0], robGround[3][1], robGround[3][2], robGround[3][3]);

        for (k = 0; k < 4; k++) // For all robots
        {
            switch (robotState[k])
            {
            case 0: // Searching for a black line
                if ((robGround[k][0] < BLACK_THR) || (robGround[k][1] < BLACK_THR) || (robGround[k][2] < BLACK_THR) || (robGround[k][3] < BLACK_THR))
                {
                    tickCount[k]++;
                    if (tickCount[k] >= 5) // Avoid false positive
                    {
                        printf("robot %d with grounds: %4d, %4d, %4d, %4d\r\n", robotAddress[k], robGround[k][0], robGround[k][1], robGround[k][2], robGround[k][3]);
                        tickCount[k] = 0;
                        robotState[k] = 1;
                        updateRGB();
                    }
                }
                else
                {
                    tickCount[k] = 0;
                }
                break;
            case 1: // Wait for exiting black line
                if ((robGround[k][0] > WHITE_THR) && (robGround[k][1] > WHITE_THR) && (robGround[k][2] > WHITE_THR) && (robGround[k][3] > WHITE_THR))
                {
                    tickCount[k]++;
                    if (tickCount[k] >= 5) // Avoid false positive
                    {
                        tickCount[k] = 0;
                        robotState[k] = 0;
                    }
                }
                else
                {
                    tickCount[k] = 0;
                }
                break;
            default:
                break;
            }
        }

        usleep(10000); // 100 Hz
    }

    stopCommunication();

    return 0;
}
