#include "data.h"

static int s_data[MAX_ENTRIES];

int data_reload_steps() {
  if(TEST) {
    // Use dummy test data
    for(int i = 0; i < MAX_ENTRIES; i++) {
      s_data[i] = i * i;
    }
    return MAX_ENTRIES;
  }

  // Clear old data
  for(int i = 0; i < MAX_ENTRIES; i++) {
    s_data[i] = 0;
  }

  time_t end = time(NULL);
  time_t start = time_start_of_today();

  // Last 15 minutes may not be available
  // start -= (15 * SECONDS_PER_MINUTE);
  end -= (15 * SECONDS_PER_MINUTE);

  // Check data is available
  if(health_is_available(){
    uint32_t num_records = 0;
    for(int i = 0;i < MAX_ENTRIES; i++){
      s_data[i] = health_get_metric_sum(s_metric);
    }
    //HealthMetricStepCount,HealthMetricActiveSeconds,HealthMetricWalkedDistanceMeters,
    //HealthMetricSleepSeconds,HealthMetricSleepRestfulSeconds,HealthMetricRestingKCalories,HealthMetricActiveKCalories
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "No data available from %d to %d!", (int)start, (int)end);
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "Got %d/%d new entries from the Health API", (int)num_records, MAX_ENTRIES);
  return (int)num_records;
}

int* data_get_steps_data() {
  return s_data;
}

void data_record_last_upload_time() {
  time_t now = time(NULL);
  persist_write_int(PersistKeyLastUploadTime, (int)now);
}

time_t data_get_last_upload_time() {
  return (time_t)persist_read_int(PersistKeyLastUploadTime);
}
