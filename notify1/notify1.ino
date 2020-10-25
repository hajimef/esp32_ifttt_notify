#include <WiFi.h>
#include <HTTPClient.h>
#include "ifttt_root_ca.h"

static const char *ssid = "your_ssid";
static const char *pass = "your_pass";
static const char *eventname = "your_event";
static const char *key = "your_key";

#define SW_PIN 5

void setup() {
  Serial.begin(115200);
  // アクセスポイントに接続する
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // スイッチ用のピンを入力用にする
  pinMode(SW_PIN, INPUT_PULLUP);
  // 時刻を合わせる
  setClock();
}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    yield();
    nowSecs = time(nullptr);
  }
}
void loop() {
  if (digitalRead(SW_PIN) == LOW) {
    Serial.println("Switch pushed");
    // WiFiClientSecureクラスのオブジェクトを生成する
    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
      {
        // IFTTTにアクセスする
        client->setCACert(root_ca);
        HTTPClient https;
        String url = "https://maker.ifttt.com/trigger/";
        url += eventname;
        url += "/with/key/";
        url += key;
        if (https.begin(*client, url)) {
          int status = https.POST("");
          if (status > 0) {
            if (status == HTTP_CODE_OK) {
              // 得られたレスポンスをシリアルモニタに出力する
              String resp = https.getString();
              Serial.print(resp);
            }
            else {
              Serial.print("HTTP Error ");
              Serial.println(status);
            }
          }
          else {
            Serial.println("Post Failed");
          }
          https.end();
        }
        else {
          Serial.print("Connect error");
        }
      }
      delete client;
    }
    delay(100);
  }
}
