// Библиотека удобной связи ардуино-ардуино через вайфай посредством протокола телнет
// Написано Зуйковым И.О. "Tpyxa"
// Например пульт или какой-то датчик
// Команды могут быть или простыми типа "RED", "GREEN", "LEFT", "RIGHT"
// Или с параметрами например "XMove 123", "YMove -24", "Temperature -12" и т.д.
// Соответствующую обработку этих команд смотрите в примерах серверной части
// Это пример сложного клиента, который может как принимать команды, так и отсылать их
// Клиент выполняет роль "текстового терминала - логгера", отображая все нераспознанные команды

#include <WiFi.h>
#include <TpyxaTerminal.h>

// Настройки Wi-Fi
const char* ssid = "TERMINAL";
const char* password = "12345678";

// IP-адрес и порт для подключения
const char* host = "192.168.4.1";
const int port = 23;

TpyxaTerminal ttClient; // Создаём объект клиента

int myUserCommand(String command){
    if (command.startsWith("WHITE")){
        // Тут может быть установка цвета текста
        Serial.println("SetColor: WHITE");
    }
    else
    {
        // Строка тут уже не является известной командой и может
        // быть выведена на текстовый/графический экран в качестве текстового лога
        Serial.println(command);
        return(0);
    } 
    return(1);
}

void setup() {
  Serial.begin(115200); delay(2000);

  // Соединяемся по вайфай к серверу 
  Serial.print("\nConnecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("\nWiFi connected. IP address: "); Serial.println(WiFi.localIP());


  //ttClient.debugON();   // Включаем отладку телнета
  ttClient.Connect(host, port); // Подключаемся к телнет-серверу
  // Установка роли клиента. Пока поддерживается пока только CLIENT_TYPE_TEXT
  // Текстовый терминал. В последствии добавится графический
  ttClient.SetRole(CLIENT_TYPE_TEXT);
  ttClient.SetCommandCallback(myUserCommand); // Регистрация функции-обработчика команд
}

void loop() {

  ttClient.task_handler(); // Обработчик

}