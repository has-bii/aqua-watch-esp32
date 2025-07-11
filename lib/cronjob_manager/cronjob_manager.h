#pragma once

#include <Arduino.h>
#include <NTPClient.h>

// Cron job structure definition
typedef struct
{
    bool isInterval;                 // True if job runs at intervals, false if specific times
    uint16_t seconds;                // For interval jobs: run every X seconds
    uint8_t minutes[4];              // For specific time jobs: array of minutes to run at
    uint8_t numMinutes;              // Number of entries in the minutes array
    void (*callback)();              // Function to call when job should run
    const char *name;                // Name of the job for logging
    unsigned long lastExecutionTime; // Last execution timestamp
} CronJob;

class CronJobManager
{
private:
    CronJob *jobs;
    int jobCount;
    NTPClient *timeClient;
    bool ntpSynchronized;
    unsigned long lastMillisRun;

public:
    CronJobManager(NTPClient *client);
    void begin(CronJob *cronJobs, int numJobs);
    void update();
    void setNtpSynchronized(bool synced);
    bool isNtpSynchronized();
};