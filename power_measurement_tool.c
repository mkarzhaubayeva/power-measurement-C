#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

typedef struct Node {
    char descr[50];
    char addr[50];
    char channel[50];
} Node ;

Node getNodes(Node *nodes) {
    return *nodes;
}

//  NOT USED
// struct node getNodesByName(char *name) {
//     for (int i = 0; i < sizeof(nodes); i++) {
//         if (nodes[i].descr == name) {
//             printf("%s, %s, %s", nodes[i].descr, nodes[i].addr, nodes[i].channel);
//         }
//     }
// }

bool pathExists(char *path) {
    DIR* dir = opendir(path);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
    } else {
        /* opendir() failed for some other reason. */
    }
    return false;
}

bool powerSensorsPresent() {
    return pathExists("/sys/bus/i2c/drivers/ina3221x/0-0041/iio:device1/");
}

//  NOT USED
// bool getPowerMode() {
//     char *argv[4] = {"Command", "-c", " nvpmodel -q | grep 'Power Mode' ", NULL};
//     execvp("sh", argv);
//     // from 15 till predposlednii
// }

// what it returns??
int readValue(char *addr, char *channel, char *valType) {
    
    char path[80];
    strcpy(path, "/sys/bus/i2c/drivers/ina3221x/0-");

    char iio_device[] = "1";
    if (strcmp(addr, "0040") == 0)
        strcpy(iio_device, "0");
    else if (strcmp(addr, "0042") == 0)
        strcpy(iio_device, "2");
    else if (strcmp(addr, "0043") == 0)
        strcpy(iio_device, "3");

    strcat(path, addr);
    strcat(path, "/iio:device");
    strcat(path, iio_device);
    strcat(path, "/in_");
    strcat(path, valType);
    strcat(path, channel);
    strcat(path, "_input");
    // printf("%s\n", path);
    FILE *ptr;
    ptr = fopen(path, "r");
    int value = 0;
    fscanf (ptr, "%d", &value);
    // int index = 0;
    // char ch;
    // do {
    //     ch = fgetc(ptr);
    //     values[index] = ch;
    //     index++;
    // } while (ch != EOF);
    // char value[index];
    // for (int i = 0; i < index; i++)
    //     value[i] = values[i];
    // fclose(ptr);
    return value;
}

//  NOT USED
// void getModulePower() {
//     *char ptr = readValue("0041", "0", "power");
//     float value = 0;
//     for (int i = 0; i < sizeof(value); i++)
//         value = value*10 + ptr[i];
//     return value;
// }


// float* getAllValues(char* valTypes, Node* nodes) { //readValue returns FILE *PTR???
//     float values[sizeof(valTypes) * sizeof(nodes)];
//     int count = 0;
//     for (int i = 0; i < sizeof(valTypes); i++) {
//         for (int j = 0; j < sizeof(nodes); j++) {
//             Node data = nodes[j];
//             values[count] = readValue(data.addr, data.channel, &valTypes[i]);
//             count++;
//         }
//     }
//     return values;
// }

void printFullReport() {
    //TABULATE
}

void print(char *addr, char *channel, char *valType) {
    printf("%s, %s, %s", addr, channel, valType);
}

int main(void) {
    Node module_main = {.descr = "module/main", .addr = "0041", .channel = "0"};
    Node module_cpu = {.descr = "module/cpu", .addr = "0041", .channel = "1"};
    Node module_ddr = {.descr = "module/ddr", .addr = "0041", .channel = "2"};
    Node module_gpu = {.descr = "module/gpu", .addr = "0040", .channel = "0"};
    Node module_soc = {.descr = "module/soc", .addr = "0040", .channel = "1"};
    Node module_wifi = {.descr = "module/wifi", .addr = "0040", .channel = "2"};

    Node board_main = {.descr = "board/main", .addr = "0042", .channel = "0"};
    Node board_5v0_io_sys = {.descr = "board/5v0-io-sys", .addr = "0042", .channel = "1"};
    Node board_3v3_sys = {.descr = "board/3v3-sys", .addr = "0042", .channel = "2"};
    Node board_3v3_io_sleep = {.descr = "board/3v3-io-sleep", .addr = "0043", .channel = "0"};
    Node board_1v8_io = {.descr = "board/1v8-io", .addr = "0043", .channel = "1"};
    Node board_3v3_m2 = {.descr = "board/3v3-m.2", .addr = "0043", .channel = "2"};

    Node nodes[12] = {
            module_main,
            module_cpu,
            module_ddr,
            module_gpu,
            module_soc,
            module_wifi,
            board_main,
            board_5v0_io_sys,
            board_3v3_sys,
            board_3v3_io_sleep,
            board_1v8_io,
            board_3v3_m2
    };

    char valTypes[3][20] = { "power", "voltage", "current" };
    char valTypesFull[3][20] = { "power [mW]", "voltage [mV]", "current [mA]" };

    int powers[12];
    int voltages[12];
    int currents[12];

    // For every Node getting 3 values (power, voltage, current)
    for (int i = 0; i < 12; i++) {
        Node data = nodes[i];
        //print(data.addr, data.channel, valTypes[k]);
        powers[i] = readValue(data.addr, data.channel, valTypes[0]);
        voltages[i] = readValue(data.addr, data.channel, valTypes[1]);
        currents[i] = readValue(data.addr, data.channel, valTypes[2]);
    }

    printf("\ndescription\t\t%s\t%s\t%s\n", valTypesFull[0], valTypesFull[1], valTypesFull[2]);

    for (int i = 0; i < 12; i++) {
        Node data = nodes[i];
        //print(data.addr, data.channel, valTypes[k]);
        printf("%s\t\t%d\t\t%d\t\t%d\n", data.descr, powers[i], voltages[i], currents[i]);
    }

    return 0;
}
