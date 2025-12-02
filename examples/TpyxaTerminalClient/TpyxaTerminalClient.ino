// Библиотека удобной связи ардуино-ардуино через вайфай посредством протокола телнет
// Написано Зуйковым И.О. "Tpyxa"
// Это пример простого клиента, который только отсылает команды и ничего не принимает
// Например пульт или какой-то датчик
// Команды могут быть или простыми типа "RED", "GREEN", "LEFT", "RIGHT"
// Или с параметрами например "XMove 123", "YMove -24", "Temperature -12" и т.д.
// Соответствующую обработку этих команд смотрите в примерах серверной части

#include <WiFi.h>
#include <TpyxaTerminal.h>

#define KEY1 0
#define KEY2 3
#define KEY3 4

// Настройки Wi-Fi
const char* ssid = "TERMINAL";
const char* password = "12345678";

// IP-адрес и порт для подключения
const char* host = "192.168.4.1";
const int port = 23;

TpyxaTerminal ttClient; // Создаём объект клиента

void setup() {
  Serial.begin(115200); delay(2000);

  pinMode(KEY1, INPUT_PULLUP); pinMode(KEY2, INPUT_PULLUP); pinMode(KEY3, INPUT_PULLUP);

  // Соединяемся по вайфай к серверу
  Serial.print("\nConnecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("\nWiFi connected. IP address: "); Serial.println(WiFi.localIP());


  ttClient.debugON();   // Включаем отладку телнета
  ttClient.Connect(host, port); // Подключаемся к телнет-серверу
}

void loop() {

  ttClient.task_handler(); // Обработчик

  // Посылаем команды серверу. # и \n в конце обязательны
  if (digitalRead(KEY1) == 0) ttClient.println("BLACK#");
  if (digitalRead(KEY2) == 0) ttClient.println("RED#");
  if (digitalRead(KEY3) == 0) ttClient.println("GREEN#"); 
}