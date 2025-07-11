#include "cronjob_manager.h"

// Constructor - initialize with NTP client reference
CronJobManager::CronJobManager(NTPClient *client)
{
    timeClient = client;
    ntpSynchronized = false;
    lastMillisRun = 0;
    jobs = NULL;
    jobCount = 0;
}

// Set up the job array
void CronJobManager::begin(CronJob *cronJobs, int numJobs)
{
    jobs = cronJobs;
    jobCount = numJobs;
    Serial.print("CronJobManager initialized with ");
    Serial.print(jobCount);
    Serial.println(" jobs");
}

// Update method - call this in loop()
void CronJobManager::update()
{
    unsigned long currentMillis = millis();

    if (ntpSynchronized)
    {
        // Using NTP time
        // Get current time components
        unsigned long epochTime = timeClient->getEpochTime();
        unsigned long currentSeconds = epochTime % 60;
        unsigned long currentMinute = (epochTime / 60) % 60;

        // Check each cron job
        for (int i = 0; i < jobCount; i++)
        {
            if (jobs[i].isInterval)
            {
                // Interval-based job
                if ((epochTime - jobs[i].lastExecutionTime) >= jobs[i].seconds)
                {
                    // Serial.print("Running interval job: ");
                    // Serial.println(jobs[i].name);
                    jobs[i].callback();
                    jobs[i].lastExecutionTime = epochTime;
                }
            }
            else
            {
                // Specific-time job
                unsigned long currentMinuteTimestamp = epochTime / 60;

                // Skip if already executed in this minute
                if (jobs[i].lastExecutionTime == currentMinuteTimestamp)
                {
                    continue;
                }

                // Check if current minute matches any in our list
                for (int j = 0; j < jobs[i].numMinutes; j++)
                {
                    if (currentMinute == jobs[i].minutes[j] && currentSeconds == 0)
                    {
                        // Serial.print("Running scheduled job: ");
                        // Serial.println(jobs[i].name);
                        jobs[i].callback();
                        jobs[i].lastExecutionTime = currentMinuteTimestamp;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // Fallback to millis()-based timing when NTP is not available
        // Only process interval-based jobs in this mode

        // Process each job
        for (int i = 0; i < jobCount; i++)
        {
            if (jobs[i].isInterval)
            {
                // Convert seconds to milliseconds
                unsigned long intervalMs = jobs[i].seconds * 1000;

                // Check if it's time to run this job
                if (currentMillis - jobs[i].lastExecutionTime >= intervalMs)
                {
                    // Serial.print("Running interval job (millis mode): ");
                    // Serial.println(jobs[i].name);
                    jobs[i].callback();
                    jobs[i].lastExecutionTime = currentMillis;
                }
            }
            // Skip time-based jobs when NTP is not synced
        }
    }
}

// Update NTP sync status
void CronJobManager::setNtpSynchronized(bool synced)
{
    ntpSynchronized = synced;

    // If we just switched from non-synced to synced, reset all execution times
    if (synced)
    {
        unsigned long epochTime = timeClient->getEpochTime();
        for (int i = 0; i < jobCount; i++)
        {
            // Reset the last execution time based on mode
            if (jobs[i].isInterval)
            {
                // For interval jobs, set to current time minus 1 second
                // so they'll run soon but not immediately
                jobs[i].lastExecutionTime = epochTime - 1;
            }
            else
            {
                // For time-based jobs, set to 0 so they'll run at the next scheduled time
                jobs[i].lastExecutionTime = 0;
            }
        }
        Serial.println("CronJobManager: Switched to NTP time mode");
    }
    else
    {
        // Just switched to non-synced state
        Serial.println("CronJobManager: Switched to millis() fallback mode");
    }
}

// Get NTP sync status
bool CronJobManager::isNtpSynchronized()
{
    return ntpSynchronized;
}