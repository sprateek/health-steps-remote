#include "health.h"

static bool s_health_available;

static void health_handler(HealthEventType event, void *context) {
}

void health_init() {
  s_health_available = health_service_events_subscribe(health_handler, NULL);
  if(!s_health_available) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
}

bool health_is_available() {
  return s_health_available;
}

int health_get_metric_sum(HealthMetric metric) {
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric,
    time_start_of_today(), time(NULL));
  if(mask == HealthServiceAccessibilityMaskAvailable) {
    return (int)health_service_sum_today(metric);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
    return 0;
  }
}

int test_health_score(){
  int steps = health_get_metric_sum(HealthMetricStepCount);
  int active = health_get_metric_sum(HealthMetricActiveSeconds);
  int sleep = health_get_metric_sum(HealthMetricSleepSeconds);
  int cal = health_get_metric_sum(HealthMetricActiveKCalories);
  APP_LOG(APP_LOG_LEVEL_INFO, "Steps count! %d",steps);
  return (steps % 6);
}
