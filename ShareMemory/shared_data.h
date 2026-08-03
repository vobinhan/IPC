#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#define SHM_NAME "/demo_shared_memory"

struct SharedData
{
    int counter;
    char message[128];
};

#endif