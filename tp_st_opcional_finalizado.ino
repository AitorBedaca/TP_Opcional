#define LED 25
#include <SPI.h>
#include <Wire.h>

#include <ESP32Time.h>
#include "time.h"
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define SCREEN_WIDTH 128
#define SCREEN_HIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HIGHT, &Wire, -1);

int estado = 1;

void pedir_lahora(void);
void setup_rtc_ntp(void);
struct tm timeinfo;
ESP32Time rtc;
float temp;


int Triego = 0;
int Hriego = 0;

int prox_est;

bool finalizado = false;
const int BTN1_PIN = 34;
const int BTN2_PIN = 35;

long ahora = 0;
long intervalo;
long last_ahora = 0;
long tiempoActual;
long unsigned int timestamp; // hora
long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;
const char *servidor = "south-america.pool.ntp.org";

const char* red = "ORT-IoT";
const char* clave = "OrtIOTnew22$2";
int flag = 0;

int THriego = 0;
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  Serial.begin (9600);
  Serial.println("Iniciando...");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Inicializamos el oled
  display.clearDisplay(); // Borrar imagen en el OLED


  display.setTextSize(1); // Tamaño del texto
  display.setTextColor(WHITE); // Definir color del texto (WHITE-BLACK)


  initWiFi();
  setup_rtc_ntp();
}

void loop() {
  ahora = millis();

  Serial.println(estado);

  bool regando = false;
  int estado_btn1 = digitalRead (BTN1_PIN);
  int estado_btn2 = digitalRead (BTN2_PIN);

  pedirHora();
  int hora = timeinfo.tm_hour;
  int minuto = timeinfo.tm_min;


  Serial.println(estado_btn1);
  Serial.println(estado_btn2);

  switch (estado) {
    case 1:

      estado_btn1 = digitalRead (BTN1_PIN);
      estado_btn2 = digitalRead (BTN2_PIN);


      if ((hora == Hriego) && (finalizado == false)) {
        digitalWrite(LED, HIGH);
        regando = true;
        intervalo = ahora - last_ahora;
        if (flag == 0) {
          last_ahora = millis();
          flag = 1;
        }

        if (intervalo > Triego * 1000) {
          finalizado = true;
          flag = 0;
          digitalWrite(LED, LOW);

        }
      }

      if (hora < Hriego) {
        finalizado = false;

      }
      else {
        regando = false;
      }
      if (regando == true) {


      }
      display.clearDisplay();
      display.setCursor(5, 5);
      display.println("tiempo:         ");
      display.setCursor(60, 5);
      display.print(hora);
      display.print(":");
      display.print(minuto);
      display.print(".");
      display.setCursor(5, 20);
      display.print("duracion:");
      display.print(Triego);
      display.setCursor(5, 45);
      display.print("hora de riego :");
      display.print(Hriego);
      display.setCursor(5, 65);

      if (regando == false) {
        display.println("no regando");
      }
      else {
        display.println("regando");
      }
      display.display();

      if (regando == true) {

      }


      if ((estado_btn2 == 0) && (estado_btn1 == 0)) {
        estado = 4;
        prox_est = 2;
      }


      break;

    case 2:

      estado_btn1 = digitalRead (BTN1_PIN);
      estado_btn2 = digitalRead (BTN2_PIN);
      if (estado_btn1 == 0) {
        prox_est = 6;
        estado = 4;

      }
      if ((estado_btn2 == 0) && (Triego != 0)) {

        prox_est = 5;
        estado = 4;

      }


      if ((estado_btn2 == 0) && (estado_btn1 == 0)) {
        estado = 4;
        prox_est = 3;

      }

      display.clearDisplay();
      display.setCursor(5, 20);
      display.print("duracion:");
      display.print(Triego);
      display.display();

      break;


    case 3:


      estado_btn1 = digitalRead (BTN1_PIN);
      estado_btn2 = digitalRead (BTN2_PIN);
      Serial.println(estado_btn1);
      Serial.println(estado_btn2);
      display.clearDisplay();
      display.setCursor(5, 20);
      display.print("hora de riego :");
      display.print(Hriego);
      display.display();

      if ((estado_btn1 == 0) && (Hriego < 24)) {

        estado = 4;
        prox_est = 7;


      }
      if ((estado_btn2 == 0) && (Hriego  > 0)) {
        estado = 4;
        prox_est = 8;

      }



      if ((estado_btn2 == LOW) && (estado_btn1 == LOW)) {
        estado = 4;
        prox_est = 1;
        Serial.println("entro al if");

      }

      break;

    case 4:

      if ((estado_btn2 == 1) && (estado_btn1 == 1)) {
        estado = prox_est;
      }


      break;

    case 5:


      if (estado_btn1 == 0) {

        prox_est = 3;
        estado = 4;


      }

          if ((estado_btn2 == 1) && (Triego != 0)) {
            Triego = Triego - 30;
            prox_est = 2;
            estado = 4;
          }

        
      
      break;
    case 6:

      if (estado_btn2 == 0) {

        prox_est = 3;
        estado = 4;


      }


          if (estado_btn1 == 1) {
            Triego = Triego + 30;
            prox_est = 2;
            estado = 4;
          }

        
      
      break;

    case 7:

      if (estado_btn2 == 0) {


        estado = 9;


      }

          if (estado_btn1 == 1) {

            prox_est = 3;
            estado = 4;
            Hriego = Hriego + 1;
          }

        
      
      break;

    case 8:
      if (estado_btn1 == 0) {

        estado = 9;
      }
 
        if (estado_btn2 == 1) {


          prox_est = 3;
          estado = 4;
          Hriego = Hriego - 1;
          

        }
      
      break;

  

  case 9 :


Serial.println("Volviendo");
      if ((estado_btn2 == 1) && (estado_btn1 == 1)) {
        estado = 1;
      }


      break;


}
}
void initWiFi() {
  WiFi.begin(red , clave);
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup_rtc_ntp()
{
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, servidor);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);
}

void pedirHora()
{
  if (!getLocalTime(&timeinfo))
  {

    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();

  }
  else
  {

    timestamp = time(NULL);

  }
}
