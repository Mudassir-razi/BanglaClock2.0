//Timing paramters
int HourOffset = 6;
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

//Time keeping

/*
  1. Connect to the internet
  2. If connected, start time keeping local time
  3. else start time keeping from default time


  4.if at some point, disconnected, keep time from last known local time
  5.else update time after a fixed interval 
*/

void UpdateTime() {
  struct tm _timeInfo;
  if (getLocalTime(&_timeInfo)) {
    timeInfo = _timeInfo;
    Serial.println("Got time");
  }
}

void timeAvailable(struct timeval* t) {
  UpdateTime();
}

void configureTimeKeeper() {
  //callback function
  sntp_set_time_sync_notification_cb(timeAvailable);
  sntp_servermode_dhcp(1);  // (optional)
  configTime(HourOffset * 3600, 0, ntpServer1, ntpServer2);

  timeInfo.tm_year = 2023;
  timeInfo.tm_mon = 8;
  timeInfo.tm_hour = 12;
}
