#ifndef TpyxaTerminal_h
#define TpyxaTerminal_h

//#include "Arduino.h"
//#include <WiFi.h>

#define MAXofTELNET 32

// Внутренние типы клиентов
#define CLIENT_TYPE_UNDEFINED		0
#define CLIENT_TYPE_TEXT			1
#define CLIENT_TYPE_BASE			2
#define CLIENT_TYPE_GRAPHIC			3
#define CLIENT_TYPE_DISCONNECTED	-1


#define WiFi_CLOSED      0
#define WiFi_LISTEN      1
#define WiFi_SYN_SENT    2
#define WiFi_SYN_RCVD    3
#define WiFi_ESTABLISHED 4
#define WiFi_FIN_WAIT_1  5
#define WiFi_FIN_WAIT_2  6
#define WiFi_CLOSE_WAIT  7
#define WiFi_CLOSING     8
#define WiFi_LAST_ACK    9
#define WiFi_TIME_WAIT   10

// Время принудительного отключения клиента
#define CLIENT_TIMEOUT 180000
// Время таймаута отправки 1 символа
#define WRITE_TIMEOUT 1000

class TpyxaTerminal : public Print {
	public:
		TpyxaTerminal(uint16_t port = 23); // Конструктор с портом по умолчанию 23

		void begin();
		void toDisplay(char *text);
		void toDisplay(const char *text);
		void toAll(char *text);
		void toAll(const char *text);
		void toBase(char *text);
		void toBase(const char *text);
		// Устанавливает текущий вывод на 1 соотв клиента
		int toText();
		int toDisplay();
		int toBase();

		
		
		//void terminal();
		void task_handler();
		void task_handler(bool exeCom);
		
		//terminalClear
		void clearBuffers();
		
		// Реализация Print
		size_t write(uint8_t byte) override;
		size_t write(const uint8_t *buffer, size_t size) override;
		
		// Отладка в Serial
		void debugON();
		void debugOFF();
		int nClient(); // Количесво клиентов
		char *typeClient(); // Типы клиентов наглядно

		int Connect(const char *ip, int port);
		int Connect(IPAddress ip, int port);
		
		void SetClient(int ClientIndex); // Установка текущего клиента
		// Дополнительные пользовательские команды
		int userCommand(String command);			// По умолчанию пустой, нужно переопределять
		typedef int (*UserCommandCallback)(String command);
		void SetCommandCallback(UserCommandCallback callback);
		void SetRole(int role);
		char lc[64]; // Список клиентов по типам

		
	private:
		int _roleClient=-1; // Роль клиента
		int _processing=0; // Обрабатывает ли клиент команды
		void _SendRole(int role);
		UserCommandCallback _UserCommandCallback = nullptr;		// Указатель на функцию доп команд
		int _nCurrentClient=-1; // Номер текущего клиента
		bool _debugSerial=false;
		uint16_t _port=23; // Запомним порт (для клиента)
		IPAddress _host; // Запоминаем хост сервера (для клиента)
		WiFiServer _telnetServer;
		bool client=false; // Если true, то это клиент, а не сервер

		WiFiClient		_ClientTelnet[MAXofTELNET];
		WiFiClient		_ClientSolo; // Переменная для клиентского подключения
		int				_ClientType[MAXofTELNET]; // 0-неопределен 1-текстовый терминал 2-графичекий терминал
		unsigned long	_ClientTime[MAXofTELNET]; // Время последнего обмена
		
		//void terminal_start3();
		void _acceptNewClient();
		
		// void processCommand(String command, int tc);
		void _processCommand(String command);
		void _freeDisconnectedSlots();
		void _freeSlot(int i);
		void _addClient(int i, WiFiClient &newC);

};

#endif // TpyxaTerminal_h
