/******************************************************************************
* Vehicle Telematics Data Logger Sketch for Freematics ONE
* Developed by Stanley Huang <stanleyhuangyc@gmail.com>
* Distributed under GPL v2.0
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include "OBD_dev.h"
//#include <I2Cdev.h>
//#include <MPU9150.h>
#include <SPI.h>
#include "config.h"
#include "datalogger.h"

// logger states
#define STATE_SD_READY 0x1
#define STATE_OBD_READY 0x2
#define STATE_GPS_READY 0x4
#define STATE_MEMS_READY 0x8
#define STATE_SLEEPING 0x20
#define STATE_CONNECTED 0x40

static uint32_t startTime = 0;
#if 0
static uint32_t lastGPSAccess = 0;
static uint32_t lastGPSTime = 0;
static uint32_t lastGPSTimeSent = 0;
#endif
static uint32_t dataCount = 0;

const byte PROGMEM pidTier1[]= {PID_RPM, PID_SPEED, PID_ENGINE_LOAD, PID_THROTTLE};
const byte PROGMEM pidTier2[] = {PID_INTAKE_MAP, PID_INTAKE_TEMP, PID_COOLANT_TEMP};

#define TIER_NUM1 sizeof(pidTier1)
#define TIER_NUM2 sizeof(pidTier2)

int pidValue[TIER_NUM1][MAX_CACHED_ITEM] = {0};
byte pidValueCount[TIER_NUM1] = {0};

int pidValueTier2[sizeof(pidTier2)] = {0};

#if USE_MPU6050
static int16_t ax = 0, ay = 0, az = 0;
static int16_t gx = 0, gy = 0, gz = 0;
static int temp = 0;
#endif

int signal;

//static GPS_DATA gd = {0};

#if USE_MPU6050
MPU6050 accelgyro;
static uint32_t lastMemsDataTime = 0;
#endif

//char gpsline[OBD_RECV_BUF_SIZE] = {0};

typedef enum {
    GPRS_DISABLED = 0,
    GPRS_READY,
    GPRS_HTTP_CONNECTING,
    GPRS_HTTP_READING,
    GPRS_HTTP_ERROR,
} GPRS_STATES;

typedef struct {
  float lat;
  float lon;
  uint8_t year; /* year past 2000, e.g. 15 for 2015 */
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} GSM_LOCATION;

class COBDGSM : public COBDSPI {
public:
    COBDGSM():gprsState(GPRS_DISABLED) { buffer[0] = 0; }
    bool initGSM()
    {
      // check GSM
      uint32_t t = millis();
      do {
        setTarget(TARGET_OBD);
        sendCommand("ATCLRGSM\r", buffer, sizeof(buffer));
        if (sendGSMCommand("ATE1\r") != 0) {
          return true;
        }
        // try turning on GSM
        setTarget(TARGET_OBD);
        if (sendCommand("ATGSMPWR\r", buffer, sizeof(buffer)) > 0) {
          delay(3000);
        }
      } while (millis() - t < 30000);
      return false;
    }
    bool setupGSM(const char* apn)
    {
      Serial.print("REG...");
      while (sendGSMCommand("AT+CREG?\r", 10000, "+CREG: 0,") == 0) {
        Serial.print('.'); 
      }
      sendGSMCommand("AT+CGATT?\r");
      Serial.println(buffer);
      sprintf(buffer, "AT+SAPBR=3,1,\"APN\",\"%s\"\r", apn);
      sendGSMCommand(buffer, 10000);
      Serial.println(buffer);
      sendGSMCommand("AT+SAPBR=1,1\r", 5000);
      Serial.println(buffer);
      sendGSMCommand("AT+SAPBR=2,1\r", 5000);
      Serial.println(buffer);
      sendGSMCommand("ATE0\r");
    }
    int getSignal()
    {
        if (sendGSMCommand("AT+CSQ\r", 500)) {
            char *p = strchr(buffer, ':');
            if (p) {
              p += 2;
              int db = atoi(p) * 10;
              p = strchr(p, '.');
              if (p) db += *(p + 1) - '0';
              return db;
            }
        }
        return -1;
    }
    bool getOperatorName()
    {
        // display operator name
        if (sendGSMCommand("AT+COPS?\r") == 1) {
            char *p = strstr(buffer, ",\"");
            if (p) {
                p += 2;
                char *s = strchr(p, '\"');
                if (s) *s = 0;
                strcpy(buffer, p);
                return true;
            }
        }
        return false;
    }
    void httpUninit()
    {
      sendGSMCommand("AT+HTTPTERM\r");
    }
    bool httpInit()
    {
      if (!sendGSMCommand("AT+HTTPINIT\r", 10000) || !sendGSMCommand("AT+HTTPPARA=\"CID\",1\r", 5000)) {
        gprsState = GPRS_DISABLED;
        return false;
      }
      gprsState = GPRS_READY;
      return true;
    }
    bool httpConnect(const char* cmd)
    {
        if (sendGSMCommand(buffer))
        {
            // Starts GET action
            write("AT+HTTPACTION=0\r");
            gprsState = GPRS_HTTP_CONNECTING;
            bytesRecv = 0;
            checkTimer = millis();
        } else {
            gprsState = GPRS_HTTP_ERROR;
        }
        return false;
    }
    byte httpIsConnected()
    {
        byte ret = checkbuffer(":0,200", ":0,6", 10000);
        if (ret == 1) {
            return 1;
        } else if (ret >= 2) {
            gprsState = GPRS_HTTP_ERROR;
        }
        return false;
    }
    void httpRead()
    {
        write("AT+HTTPREAD\r");
        gprsState = GPRS_HTTP_READING;
        bytesRecv = 0;
        checkTimer = millis();
    }
    bool httpIsRead()
    {
        byte ret = checkbuffer("+HTTPREAD:\r", "Error", 10000) == 1;
        if (ret == 1) {
            bytesRecv = 0;
            sendGSMCommand(0);
            byte n = atoi(buffer);
            char *p = strchr(buffer, '\n');
            if (p) memmove(buffer, p + 1, n);
            buffer[n] = 0;
            gprsState = GPRS_READY;
            return 1;
        } else if (ret >= 2) {
            gprsState = GPRS_HTTP_ERROR;
        }
        return false;
    }
    bool getLocation(GSM_LOCATION* loc)
    {
      if (sendGSMCommand("AT+CIPGSMLOC=1,1\r", 1000)) do {
        char *p;
        if (!(p = strchr(buffer, ':'))) break;
        if (!(p = strchr(p, ','))) break;
        loc->lon = atof(++p);
        if (!(p = strchr(p, ','))) break;
        loc->lat = atof(++p);
        if (!(p = strchr(p, ','))) break;
        loc->year = atoi(++p) - 2000;
        if (!(p = strchr(p, '/'))) break;
        loc->month = atoi(++p);
        if (!(p = strchr(p, '/'))) break;
        loc->day = atoi(++p);
        if (!(p = strchr(p, ','))) break;
        loc->hour = atoi(++p);
        if (!(p = strchr(p, ':'))) break;
        loc->minute = atoi(++p);
        if (!(p = strchr(p, ':'))) break;
        loc->second = atoi(++p);
        return true;
      } while(0);
      return false;
    }
    char buffer[256];
    byte bytesRecv;
    uint32_t checkTimer;
    byte gprsState;
private:
    byte checkbuffer(const char* expected1, const char* expected2 = 0, unsigned int timeout = 2000)
    {
        setTarget(TARGET_OBD);
        write("ATGRD\r");
        delay(10);
        setTarget(TARGET_GSM);
        byte n = receive(buffer + bytesRecv, timeout, sizeof(buffer) - bytesRecv);
        if (n > 0) {
            bytesRecv += n;
            if (bytesRecv >= sizeof(buffer) - 1) {
                // buffer full, discard first half
                bytesRecv = sizeof(buffer) / 2 - 1;
                memcpy(buffer, buffer + sizeof(buffer) / 2, bytesRecv);
            }
            if (strstr(buffer, expected1)) {
                return 1;
            }
            if (expected2 && strstr(buffer, expected2)) {
                return 2;
            }
        }
        return (millis() - checkTimer < timeout) ? 0 : 3;
    }
    byte sendGSMCommand(const char* cmd, unsigned int timeout = 2000, const char* expected = 0)
    {
      uint32_t t = millis();
      if (cmd) {
        setTarget(TARGET_GSM);
        write(cmd);
      }
      do {
        setTarget(TARGET_OBD);
        write("ATGRD\r");
        delay(10);
        setTarget(TARGET_GSM);
        byte n = receive(buffer, timeout);
        if (n > 0) {
          if (strstr(buffer, expected ? expected : "\r\nOK\r\n")) {
            return n;
          }
        }
      } while (millis() - t < timeout);
      return 0;
    }
};

class CTeleLogger : public COBDGSM, public CDataLogger
{
public:
    CTeleLogger():state(0),connErrors(0),channel(0) {}
    void setup()
    {
        begin(7, 6);
        SerialRF.begin(115200);
        SerialRF.print("#GPRS...");
        if (initGSM()) {
            SerialRF.println("OK");
        } else {
            SerialRF.println(buffer);
        }

        setTarget(TARGET_OBD);
        SerialRF.print("#OBD..");
        do {
            SerialRF.print('.');
        } while (!init());
        SerialRF.println("OK");

        state |= STATE_OBD_READY;

#if USE_MPU6050
        Wire.begin();
        accelgyro.initialize();
        if (accelgyro.testConnection()) state |= STATE_MEMS_READY;
#endif

#if 0
        SerialRF.print("#GPS...");
        if (initGPS()) {
            SerialRF.println("OK");
            state |= STATE_GPS_READY;
        } else {
            SerialRF.println("N/A");
        }
        delay(3000);
#endif

        SerialRF.print("#Network...");
        if (setupGSM(APN)) {
            SerialRF.println("OK");
        } else {
            SerialRF.print(buffer);
        }
        
        signal = getSignal();
        SerialRF.print("#SIGNAL:");
        SerialRF.println(signal);

        if (getOperatorName()) {
            SerialRF.print('#');
            SerialRF.println(buffer);
        }
        
        // init HTTP
        SerialRF.print("#HTTP...");
        while (!httpInit()) {
          SerialRF.print('.');
          httpUninit();
          delay(1000);
        }
        SerialRF.println("OK");

        char *p = buffer;
        byte n = sprintf(p, "AT+HTTPPARA=\"URL\",\"%s?CSQ=%d&VIN=", URL_PUSH, signal);
        setTarget(TARGET_OBD);
        getVIN(p, sizeof(buffer) - n);
        strcat(p, "\"\r");
        SerialRF.print('#');
        SerialRF.println(buffer);
        httpConnect(buffer);
        while (!httpIsConnected());
        httpRead();
        while (!httpIsRead());
        SerialRF.println(buffer);
        if (!memcmp(buffer, "OK", 2)) {
          int m = atoi(buffer + 3);
          if (m > 0) {
            channel = m;
            SerialRF.print("#CHANNEL:"); 
            SerialRF.println(n);
            state |= STATE_CONNECTED;
          }
        }
        connErrors = 0;
        delay(1000);
    }
    void loop()
    {
        static byte index = 0;
        static byte index2 = 0;
        uint32_t start = millis();

        // poll OBD-II PIDs
        byte pid = pgm_read_byte(pidTier1 + index);
        int value = 0;
        // send a query to OBD adapter for specified OBD-II pid
        //SerialRF.print(pid, HEX);
        //SerialRF.print('=');
        setTarget(TARGET_OBD);
        if (read(pid, value)) {
            dataTime = millis();
            if (pidValueCount[index] == MAX_CACHED_ITEM) {
               // cache full, free one slot
               for (byte m = 0; m < MAX_CACHED_ITEM - 2; m++) {
                 pidValue[index][m] = pidValue[index][m + 1];
               }
               pidValue[index][MAX_CACHED_ITEM - 1] = value;
            } else {
              pidValue[index][pidValueCount[index]++] = value;
            }
            //SerialRF.println(value);
            // log data to SD card
            logData(0x100 | pid, value);
        } else {
          //SerialRF.println("N/A"); 
        }
        
        if (++index == TIER_NUM1) {
            index = 0;
            if (index2 == TIER_NUM2) {
                index2 = 0;
            } else {
                pid = pgm_read_byte(pidTier2 + index2);
                if (read(pid, value)) {
                  pidValueTier2[index2] = value;
                }
                index2++;
            }
        }

#if 0
        if (state & STATE_GPS_READY) {
            if (millis() - lastGPSAccess > GPS_DATA_INTERVAL) {
                logGPSData();
                lastGPSAccess = millis();
            }
        }
#endif

#if USE_MPU6050
        if (state & STATE_MEMS_READY) {
            processMEMS();
        }
#endif

        if (errors >= 2) {
            reconnect();
        }
        
        processGPRS();
        if (connErrors >= MAX_CONN_ERRORS) {
          // reset GPRS 
          SerialRF.print(connErrors);
          SerialRF.println("#Reset GPRS...");
          setupGSM(APN);
          if (httpInit()) {
            SerialRF.println("OK"); 
          } else {
            SerialRF.println(buffer); 
          }
          connErrors = 0;
        }
    }
private:
    void processGPRS()
    {
        switch (gprsState) {
        case GPRS_READY:
            {
                GSM_LOCATION loc;
                bool hasLoc = false; //gprs.getLocation(&loc);
              
                // generate URL
                char *p = buffer;
                p += sprintf(p, "AT+HTTPPARA=\"URL\",\"%s", URL_PUSH);
                p += sprintf(p, "?id=%u&C=%lu&", channel, ++dataCount);
                for (byte n = 0; n < sizeof(pidTier1); n++) {
                    if (pidValueCount[n] == 0) continue;
                    p += sprintf(p, "%x=", pgm_read_byte(pidTier1 + n));
                    for (byte m = pidValueCount[n] - 1; ;m--) {
                      p += sprintf(p, "%d,", pidValue[n][m]);
                      if (m == 0) break;
                    }
                    *(p - 1) = '&';
                    pidValueCount[n] = 0;
                }
                for (byte n = 0; n < sizeof(pidTier2); n++) {
                    p += sprintf(p, "%x=%d&", pgm_read_byte(pidTier2 + n), pidValueTier2[n]);
                }
#if USE_MPU6050
                p += sprintf(p, "A=%d,%d,%d&G=%d,%d,%d", ax, ay, az, gx, gy, gz);
                //p += sprintf(p, "&T=%d", temp);
#endif
                
                if (hasLoc) {
                  p += sprintf(p, "&GPS=%02u%02u%02u,%ld,%ld", loc.hour, loc.minute, loc.second, loc.lat, loc.lon);
                }
#if 0
                if (gd.time && gd.time != lastGPSTimeSent) {
                    p += sprintf(p, "&GPS=%lu,%ld,%ld,%d,%d,%d", gd.time, gd.lat, gd.lon, gd.alt / 100, (int)gd.speed, gd.sat);
                    lastGPSTimeSent = gd.time;
                }

                char *q = strchr(gpsline, ',');
                if (q) {
                    char *s = strchr(q, '\r');
                    if (s) *s = 0;
                    strcat(p, q + 1);
                }
#endif
                //SerialRF.print("PUSH:");
                //SerialRF.println(gprs.buffer);
                p += sprintf(p, "\"\r");
                httpConnect(buffer);
            }
            break;
        case GPRS_HTTP_CONNECTING:
            if (httpIsConnected()) {
                httpRead();
            }
            break;
        case GPRS_HTTP_READING:
            if (httpIsRead()) {
                SerialRF.print("#HTTP:");
                SerialRF.println(buffer);
                connErrors = 0;
                // ready for next connection
            }
            break;
        case GPRS_HTTP_ERROR:
            SerialRF.println("#HTTP ERROR");
            connErrors++;
            gprsState = GPRS_READY;
            break;
        }
    }
#if USE_MPU6050
    void processMEMS()
    {
        if (dataTime - lastMemsDataTime < ACC_DATA_INTERVAL) {
            return;
        }

        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        dataTime = millis();

        temp = accelgyro.getTemperature();

        ax /= ACC_DATA_RATIO;
        ay /= ACC_DATA_RATIO;
        az /= ACC_DATA_RATIO;
        gx /= GYRO_DATA_RATIO;
        gy /= GYRO_DATA_RATIO;
        gz /= GYRO_DATA_RATIO;

        // log x/y/z of accelerometer
        logData(PID_ACC, ax, ay, az);
        // log x/y/z of gyro meter
        logData(PID_GYRO, gx, gy, gz);
        logData(PID_MEMS_TEMP, temp);

        lastMemsDataTime = dataTime;
    }
#endif
#if 0
    bool logGPSData()
    {
        if (getGPSData(&gd) && gd.lat && gd.lon && gd.time != lastGPSTime) {
            logData(PID_GPS_TIME, gd.time);
            logData(PID_GPS_ALTITUDE, gd.lat);
            logData(PID_GPS_LONGITUDE, gd.lon);
            logData(PID_GPS_ALTITUDE, gd.alt);
            lastGPSTime = gd.time;
            return true;
        }
        return false;
    }
#endif
    void reconnect()
    {
        SerialRF.println("#Sleeping");
        startTime = millis();
        state &= ~STATE_OBD_READY;
        state |= STATE_SLEEPING;
        //digitalWrite(SD_CS_PIN, LOW);
        for (uint16_t i = 0; ; i++) {
            if (init()) {
                int value;
                if (read(PID_RPM, value) && value > 0)
                    break;
            }
        }
        SerialRF.println("#Resuming");
        state &= ~STATE_SLEEPING;
        recover();
        setup();
    }
    byte state;
    byte channel;
    byte connErrors;
};

CTeleLogger logger;

void setup()
{
    logger.initSender();
    logger.setup();
}

void loop()
{
    logger.loop();
}
