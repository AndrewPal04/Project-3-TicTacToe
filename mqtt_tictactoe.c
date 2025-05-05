#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

int main() {
    int currentPlayer = 1;
    char key;
    char command[256];
    const char* broker_ip = "34.82.67.83"; // Replace with your GCP IP

    printf("Press 1–9 to make a move\n");
    printf("Press Q to quit\n\n");

    while (1) {
        printf("[Player %d] > ", currentPlayer);
        key = _getch();
        printf("%c\n", key);

        if (key == 'q' || key == 'Q') {
            break;
        } else if (key >= '1' && key <= '9') {
            char topic[32];
            snprintf(topic, sizeof(topic), "player%d/move", currentPlayer);

            snprintf(command, sizeof(command),
                     "mosquitto_pub -h %s -t %s -m \"%c\"",
                     broker_ip, topic, key);

            printf("Sending move %c to %s\n", key, topic);
            system(command);

            // Switch players automatically
            currentPlayer = (currentPlayer == 1) ? 2 : 1;
        } else {
            printf("Invalid input. Use 1–9 to move, or Q to quit.\n");
        }
    }

    printf("Game ended.\n");
    return 0;
}
