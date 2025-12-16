// Библиотека удобной связи ардуино-ардуино через вайфай посредством протокола телнет
// Написано Зуйковым И.О. "Tpyxa"
// Это пример простого сервера, который ожидает команды управления цветом светодиода
// Команды могут быть или простыми типа "RED", "GREEN", "LEFT", "RIGHT"
// Или с параметрами например "XMove 123", "YMove -24", "Temperature -12" и т.д.

#include <WiFi.h>
#include <WiFiUdp.h>
#include <TpyxaTerminal.h>

const char *ssid = "TERMINAL";
const char *password = "12345678";

TpyxaTerminal ttServer; // Создаём объект
unsigned long t=0;

int myUserCommand(String command){
    if (command.startsWith("BLACK")){
        // встроенный RGB светодиод Для полноразмерной ESP32-S3 с двумя USB
        // Измените на нужную вам реакцию
        rgbLedWrite(48, 0, 0, 0);
		// Эти и ниже закомментированные строки для версии с клиентом-текстовым логгером
        //ttServer.toText(); // Устанавливается текущий вывод на текстовый терминал
        //ttServer.print("Set black led#");
    }
    else if (command.startsWith("RED")){
        rgbLedWrite(48, 64, 0, 0);
        //ttServer.toText(); // Устанавливается текущий вывод на текстовый терминал
        //ttServer.print("Set красный led#");
    }
    else if (command.startsWith("GREEN")){
        rgbLedWrite(48, 0, 64, 0);
        //ttServer.toText(); // Устанавливается текущий вывод на текстовый терминал
        //ttServer.print("Set green led#");
    }
    // Пример команды с параметром, например MOVE 100
    else if (command.startsWith("MOVE")){
        int m = command.substring(5).toInt(); // С символа  идет нужное нам число
        // Тут нужная вам обработка
    }
    else
	{
        //ttServer.toText(); // Устанавливается текущий вывод на текстовый терминал
        //ttServer.print(command);
		return(0);
	}
    return(1);
}

void setup() {
  Serial.begin(115200); delay(2000);

  // Поднимаем WiFi точку
  WiFi.mode(WIFI_AP); WiFi.softAP( ssid, password );
  Serial.print("AP IP: ");  Serial.println(WiFi.softAPIP());
  Serial.println("WiFi done");
  
  // ttServer.debugON(); // Отладка телнета (на Serial)
  ttServer.begin(); // Старт телнет-сервера
  ttServer.SetCommandCallback(myUserCommand); // Регистрация функции-обработчика команд
}
 
void loop() {  
  ttServer.task_handler(true); // Обработчик телнета

  // Для примера с текстовым терминалом - отсылка сообщения каждые 10 секунд
  if((millis()-t)>10000)
  {
        t=millis();
        //ttServer.toText(); // Устанавливается текущий вывод на текстовый терминал
        //ttServer.print("Посылка на текстовый терминал#");
  }
}
