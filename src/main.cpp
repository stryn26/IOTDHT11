#include <Arduino.h>
// library untuk DHT11
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// library untuk firebase
//#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>

// untuk koneksi ke wifi yang akan digunakan

#ifndef STASSID
  #define STASSID "OPPO A53"
  #define STAPSK "12345678"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

// mendefinisikan key projek
#define AUTH "tDyDM9u1R4RE17DCx1y8Qvd7KkNO52BZcPlssgk1"

/* mendefinisikan projek ID */
#define HOST "ardxandrmontemp-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com 

//define fdbo 
FirebaseData fbdo;

//  define firebase auth untuk melakukan authentikasi data
FirebaseAuth auth;

FirebaseConfig config;

String path = "/DHT11";
unsigned long dataMillis = 0;
int count = 0;

#define DHTPIN D5     // pin yang digunakan
#define DHTTYPE DHT11 // tipe dht yang digunakan

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  
  Serial.println(F("Monitoring Suhu Lingkungan"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("mencoba koneksi ke Wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Terkoneksi dengan IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.host = HOST;

  // mendefinisikan rtdb url
  config.signer.tokens.legacy_token = AUTH;  

  Firebase.begin(&config,&auth);

  Firebase.reconnectWiFi(true);
}

void loop()
{
  if (millis()- dataMillis>500){
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // menampilkan data dalam celcius

    if (isnan(h) || isnan(t))
    {
      Serial.println(F("Gagal Mendapatkan data dari sensor!"));
      return;
    }

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    

    // transaksi firebase
    if (Firebase.setFloat(fbdo,path + "/data/temperature",t)){
      Serial.println("PASSED");
      Serial.println("PATH : "+fbdo.dataPath());
      Serial.println("TYPE : "+fbdo.dataType());
      Serial.println("ETag : "+fbdo.ETag());
      Serial.print("VALUE : ");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON : "+fbdo.errorReason());
    }
    Serial.println("----------------------------------");
    if (Firebase.setFloat(fbdo,path + "/data/kelembapan",h)){
      Serial.println("PASSED");
      Serial.println("PATH : "+fbdo.dataPath());
      Serial.println("TYPE : "+fbdo.dataType());
      Serial.println("ETag : "+fbdo.ETag());
      Serial.print("VALUE : ");
    }else {
      Serial.println("FAILED");
      Serial.println("REASON : "+fbdo.errorReason());
    }
    Serial.println("----------------------------------");
  }
}
