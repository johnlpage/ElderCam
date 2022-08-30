/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/



#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "mbedtls/base64.h"
#include <Preferences.h>

/************ Useful Setup Constants *****************/


const char* AtlasAPIEndpoint = "https://data.mongodb-api.com/app/YOURENDPOINTNAME/endpoint/data/v1/action/insertOne";
const char* AtlasAPIKey = "YOURKEYHERE";
const char* ssid = "YOURSSID";
const char* password = "YOURWIFIPASS";

/* This one Works with Atlas OK - extracted with openssl s_client*/

const char* rootCACertificate = \
                                "-----BEGIN CERTIFICATE-----\n" \
                                "MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" \
                                "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
                                "DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" \
                                "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
                                "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" \
                                "AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" \
                                "ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" \
                                "wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" \
                                "LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" \
                                "4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" \
                                "bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" \
                                "sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" \
                                "Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" \
                                "FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" \
                                "SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" \
                                "PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" \
                                "TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" \
                                "SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" \
                                "c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" \
                                "+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" \
                                "ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" \
                                "b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" \
                                "U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" \
                                "MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" \
                                "5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" \
                                "9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" \
                                "WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" \
                                "he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" \
                                "Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" \
                                "-----END CERTIFICATE-----\n";

/****************************************************************************/
/* This converts a buffer to a Stream whcih lets POST send larger data sets */
/* Was the Solution to ESP32 Secure HTTPS LIbrary not working, Constructor takes */
/* And Arduino_JSON object */

class JSONStream: public Stream {
  private:
    uint8_t *buffer;
    size_t buffer_size;
    size_t served;
    int start;
    int end;

  public:
    JSONStream(DynamicJsonDocument &payload ) {
      int jsonlen = measureJson(payload);
      this->buffer = (uint8_t*) heap_caps_calloc(jsonlen + 1, 1, MALLOC_CAP_8BIT);
      this->buffer_size = serializeJson(payload,  this->buffer, jsonlen + 1);
      this->served = 0;
      this->start = millis();
    }
    ~JSONStream() {
      heap_caps_free((void*)this->buffer);
    }

    void clear() {}
    size_t write(uint8_t) {}
    int  available() {
      size_t whatsleft = buffer_size - served;
      if (whatsleft == 0) return -1;
      return whatsleft;
    }
    int peek() {
      return 0;
    }
    void flush() { }
    int read() {}
    size_t readBytes(uint8_t *outbuf, size_t nbytes) {
      //Serial.println(millis()-this->start);
      if (nbytes > buffer_size - served) {
        nbytes = buffer_size - served;
      }
      memcpy(outbuf, buffer + served, nbytes);
      served = served + nbytes;
      return nbytes;
    }

};


/**************************************************************/


// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void configureCameraRaw()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;


  config.frame_size = FRAMESIZE_96X96;
  config.fb_count = 1;


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

}


void configureCamera()
{

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_SXGA;
  config.jpeg_quality = 12;  //0-63 lower number means higher quality
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //Assume USB port on top
  sensor_t * s = esp_camera_sensor_get();
  //s->set_saturation(s, 2);
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
}

const int timerInterval = 1000 * 60 * 1;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent

WiFiClientSecure *client;
HTTPClient https;

bool checkForLight()
{
  Serial.println("Checking if its light");
  Serial.print("Free Heap RAM: ");
  Serial.println(ESP.getFreeHeap());
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  unsigned char* src = fb->buf;
  size_t slen = fb->len;
  long total=0;
  for(int x=0;x<slen;x++) {
   // Serial.println(src[x]);
    total = total + src[x];    
  }
  long ave = total/slen;
  Serial.printf("\n\nAve light = %ld\n\n",ave);
  return (ave > 80);
}

void setup()
{
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  Preferences preferences;

  preferences.begin("espcam", false);
  unsigned int isLight = preferences.getUInt("islight", 0);

  if (isLight == 1) {
    take_photo();
    preferences.putUInt("islight", 0);
    preferences.end();
     ESP.deepSleep(timerInterval * 1000);
  } else {
    configureCameraRaw(); //Set to 96x96 greyscale
    if (checkForLight() == true) {
      preferences.putUInt("islight", 1);
      preferences.end();
      ESP.restart();
    } else {
      Serial.println("Still Dark");
      ESP.deepSleep(timerInterval * 1000);
      //delay(timerInterval); //Wait for light
      //ESP.restart();
    }
  }


}

void take_photo() {
  delay(500);
  configureCamera();
  delay(500);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  setClock();


  client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(rootCACertificate);
  }


  if (!https.begin(*client, AtlasAPIEndpoint)) {  // HTTPS
    Serial.println("Error starting client");
  }

  
  delay(500); //Wait for Camera to wake properly
  sendPhoto();


}


void loop() {
}



const char* base64EncodeImage(camera_fb_t *fb)
{
  /* Base 64 encode the image - this was the simplest way*/
  unsigned char* src = fb->buf;
  size_t slen = fb->len;
  Serial.println(slen);
  size_t dlen = 0;

  int err = mbedtls_base64_encode(NULL, 0 , &dlen, src, slen);

  /* For a larger allocation like thi you need to use capability allocation*/
  const char *dst = (char*) heap_caps_calloc(dlen, 1, MALLOC_CAP_8BIT);
  size_t olen;
  err = mbedtls_base64_encode((unsigned char*)dst, dlen , &olen, src, slen);

  if (err != 0) {
    Serial.printf("error base64 encoding, error %d, buff size: %d", err, olen);
    return NULL;
  }

  return dst;
}



String sendPhoto() {

  /* Capture a JPEG from the camera*/
  Serial.print("Free Heap RAM: ");
  Serial.println(ESP.getFreeHeap());
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  https.addHeader("Content-Type", "application/json");
  https.addHeader("api-key", AtlasAPIKey);


  /* Generate an InsertOne Payload*/
  DynamicJsonDocument payload (1024);
  payload["dataSource"] = "ElderCamDB";
  payload["database"] = "eldercam";
  payload["collection"] = "frames";
  payload["document"]["camname"] = "Eldercam1";

  time_t nowSecs = time(nullptr);

  char datestring[32];

  sprintf(datestring, "%lu000", nowSecs);

  payload["document"]["time"]["$date"]["$numberLong"] = datestring; /*Encode Date() as EJSON*/

  const char* base64Image = base64EncodeImage(fb) ;
  payload["document"]["img"]["$binary"]["base64"] = base64Image; /*Encide as a Binary() */
  payload["document"]["img"]["$binary"]["subType"] = "07";


  JSONStream *buffer = new JSONStream(payload);
  heap_caps_free((void*)base64Image); //Free Base64 as soon as we serialze JSON and save some RAM
  digitalWrite(33, LOW);
  int atime = millis();
  int httpCode = https.sendRequest("POST", buffer, buffer->available());
  digitalWrite(33, HIGH);
  delete buffer;

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTPS] POST ... failed, error: %s\n", https.errorToString(httpCode).c_str());
  }

}


/* No real time clock so set this to let us timestamp docs*/

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}