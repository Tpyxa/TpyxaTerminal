#include <Arduino.h>
#include <WiFi.h>
#include <TpyxaTerminal.h>

//WiFiServer telnetServer(23);
//WiFiClient telnetClient[MAXofTELNET];
//int typeTelnet[MAXofTELNET]; // 0-неопределен 1-текстовый терминал 2-графичекий терминал


TpyxaTerminal::TpyxaTerminal(uint16_t port):_telnetServer(port), _port(port){
    for(int i=0;i<MAXofTELNET;i++) { _ClientType[i] = CLIENT_TYPE_DISCONNECTED; _ClientTime[i]=millis(); }
	_debugSerial = false;
	_nCurrentClient = -1; // Текущий клиент пока не определен
	//UserCommandCallback = nullptr; // Нулевой указатель на функцию доп команд
}
// Включение отладки через КОМ-Порт
void TpyxaTerminal::debugON(){ _debugSerial = true; }
void TpyxaTerminal::debugOFF(){ _debugSerial = false; }
		
void TpyxaTerminal::begin(){
	_telnetServer.begin(); // Поднимаем сервер telneta
	if(_debugSerial) Serial.println("TelnetServer start");
	client=false;
	lc[0]='\0';
}
// Подключение нового клиента
void TpyxaTerminal::_acceptNewClient(){
    WiFiClient newClient = _telnetServer.accept();
    // Если есть терминальный клиент
    if (newClient)
    {
        newClient.setTimeout(10); // Таймаут вайфай пакетов
        newClient.setNoDelay(true);

        for(int i=0;i<MAXofTELNET;i++)
        {
			// Ищем пустой слот клиента
            if((!_ClientTelnet[i]) || _ClientType[i]==CLIENT_TYPE_DISCONNECTED)
            {
				_addClient(i, newClient);
                return;
            }
        }
		// Не нашли пустой слот, ищем самый старый
		unsigned long oldslot_time = 0;
		int oldslot_index = -1;
		for(int i=0;i<MAXofTELNET;i++)
		{
			if(_ClientTime[i] > oldslot_time)
			{
				oldslot_index = i;
				oldslot_time = _ClientTime[i];
			}
		}
		if(oldslot_index>-1)
		{
			_freeSlot(oldslot_index);
			_addClient(oldslot_index, newClient);
		}
		else if(_debugSerial) Serial.printf("ERROR: NO FREE SLOT");
    }
}
void TpyxaTerminal::_addClient(int i, WiFiClient &newC){
	//Serial.print("New client "); Serial.println(i);
	_ClientTelnet[i]=newC;
	_ClientTelnet[i].printf("Add Client-%d#", i);
	if(_debugSerial)
	{
		Serial.printf("Add Client-%d IP: %s\n", i, IPAddress(_ClientTelnet[i].remoteIP()).toString().c_str());
		//Serial.printf("MAC: %s\n", i, _ClientTelnet[i].softAPmacAddress());
		
	} // uint32_t remoteIP() localIP()
	
	_ClientType[i] = CLIENT_TYPE_UNDEFINED; // Поначалу тип клиента неизвестен
	_ClientTime[i]=millis();
	
}
// Вывод в разные терминалы
void TpyxaTerminal::toDisplay(char *text){
    //text[strlen(text)] = '#'; text[strlen(text)+1] = 0; 
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_TEXT) // Если есть клиент и он соединен и явл текст терм
		{
            //_ClientTelnet[i].println(text);
			SetClient(i);
			println(text);
		}
}
void TpyxaTerminal::toDisplay(const char *text){
    //text[strlen(text)] = '#'; text[strlen(text)+1] = 0; 
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_TEXT) // Если есть клиент и он соединен и явл текст терм
		{
            //_ClientTelnet[i].println(text);
			SetClient(i);
			println(text);
		}
}
int TpyxaTerminal::toDisplay(){ return(toText()); }
int TpyxaTerminal::toText(){
	int kt=0;
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_TEXT) // Если есть клиент и он соединен и явл текст терм
		{
			SetClient(i);
			kt++;
		}
	if(kt>0) return 1;
	SetClient(-1); return 0;
}
void TpyxaTerminal::toBase(char *text){
    //text[strlen(text)] = '#'; text[strlen(text)+1] = 0; 
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_BASE) // Если есть клиент и он соединен и явл текст терм
		{
            //_ClientTelnet[i].println(text);
			SetClient(i);
			println(text);
		}
}
void TpyxaTerminal::toBase(const char *text){
    //text[strlen(text)] = '#'; text[strlen(text)+1] = 0; 
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_BASE) // Если есть клиент и он соединен и явл текст терм
		{
            //_ClientTelnet[i].println(text);
			SetClient(i);
			println(text);
		}
}
int TpyxaTerminal::toBase(){
	int kt=0;
    for(int i=0;i<MAXofTELNET;i++)
        if (_ClientTelnet[i] && _ClientTelnet[i].connected() && _ClientType[i]==CLIENT_TYPE_BASE) // Если есть клиент и он соединен и явл текст терм
		{
			SetClient(i);
			kt++;
		}
	if(kt>0) return 1;
	SetClient(-1); return 0;
}

int TpyxaTerminal::userCommand(String command){ return 0; }
void TpyxaTerminal::_processCommand(String command) {
	if(_nCurrentClient<0) return;
  command.trim();  // Удаление лишних пробелов в начале и конце строки
  //Serial.printf("Gx=%d Gy=%d CMD: ", Gx, Gy);
  if(_debugSerial) Serial.println(command);
  //toDisplay(command.c_str());
  //telnetClient[tc].print(command.c_str());
  
  if (command.startsWith("ID:TEXT")){
     _ClientType[_nCurrentClient] = CLIENT_TYPE_TEXT; // Переключаем тип клиента
     //_ClientTelnet[_nCurrentClient].println("TextTerm registered#");
	 println("TextTerm registered#");

    // Текстовый терминал может быть только один (пока)
    for(int i=0;i<MAXofTELNET;i++)
        if (i!=_nCurrentClient) // Проверяем всех кроме текущего
            if(_ClientType[i] == CLIENT_TYPE_TEXT) // Нашли фантомный терминал
				_freeSlot(i);
  }
  else if (command.startsWith("ID:BASE")){
     _ClientType[_nCurrentClient] = CLIENT_TYPE_BASE; // Переключаем тип клиента
     //_ClientTelnet[_nCurrentClient].println("BASE registered#");
	 println("BASE registered#");

    // База может быть только одна
    for(int i=0;i<MAXofTELNET;i++)
        if (i!=_nCurrentClient) // Проверяем всех кроме текущего
            if(_ClientType[i] == CLIENT_TYPE_BASE) // Нашли фантомную базу
				_freeSlot(i);
  }
  else if (command.startsWith("PING")) {
    //_ClientTelnet[_nCurrentClient].println("OK#");
	println("OK#");
    //toDisplay("PING OK#");
  }
  else
  {
	
	if(userCommand(command)) return; // Обработка пользовательских команд через переопределение функции userCommand
	if (_UserCommandCallback != nullptr)
	{
		if(_UserCommandCallback(command)) return; // Обработка пользовательских комманд через указатель
	}
	if(_debugSerial) Serial.println("Unknown command");
  }
}

// Бывш terminal
void TpyxaTerminal::task_handler(){ task_handler(false); }
void TpyxaTerminal::task_handler(bool exeCom)
{
	if(client)
	{
		if(!Connect(_host, _port)) return; // Проверка соединения и переконнект
		char localbuf[2];
		
		//if(_ClientSolo.available() > 0) // Есть данные на входе
		//	while(_ClientSolo.available() > 0) _ClientSolo.readBytes(localbuf, 1); // Игнорирование данных
		
		// Клиент обрабатывает команды только через указатель на функцию!!!
		if (_UserCommandCallback != nullptr)
		{
			if(_ClientSolo.available() > 0) // Есть данные на входе
			{
				String command = _ClientSolo.readStringUntil('#');  // Чтение команды до символа
				if(_UserCommandCallback(command)) return; // Обработка пользовательских комманд через указатель
			}
		}
		else // Нет функции - игнорирование данных (например пульт)
		{
			if(_ClientSolo.available() > 0) // Есть данные на входе
				while(_ClientSolo.available() > 0) _ClientSolo.readBytes(localbuf, 1); // Игнорирование данных
		}
		
	}
	else
	{
		_acceptNewClient(); // Ищем новых клиентов
		// Прием команд
		if(exeCom)
		for(int i=0;i<MAXofTELNET;i++)
		{
			if (_ClientTelnet[i] && _ClientTelnet[i].available() > 0)
			{
				String command = _ClientTelnet[i].readStringUntil('#');  // Чтение команды до символа
				SetClient(i);
				if(command.length()>3) _processCommand(command);  // Обработка команды
				_ClientTime[i]=millis();
			}
		}
		_freeDisconnectedSlots();
	}

}
void TpyxaTerminal::_freeDisconnectedSlots(){
    // Освобождение клиентских слотов
    for(int i=0;i<MAXofTELNET;i++)
		 // Если есть клиент
        if (_ClientTelnet[i])
		{
			//auto status = _ClientTelnet[i].status();
			// Смотрим за что бы его выкинуть
			if(!_ClientTelnet[i].connected()) // Не подсоединен
			{
				if(_debugSerial) Serial.print("Not connect: ");
				_freeSlot(i); continue;
			}
			if(_ClientTelnet[i].available() < 0) // Не доступен
			{
				if(_debugSerial) Serial.print("Not available: ");
				_freeSlot(i); continue;
			}
			if((millis()-_ClientTime[i]) > CLIENT_TIMEOUT) // Долго ничего не посылает
			{
				if(_debugSerial) Serial.print("TimeOut: ");
				_freeSlot(i); continue;
			}
			/*
			if(status == WiFi_CLOSED || status == WiFi_CLOSE_WAIT || status == WiFi_TIME_WAIT || status == WiFi_FIN_WAIT_1) // WiFi статус
			{
				if(_debugSerial) Serial.print("WiFi: ");
				_freeSlot(i); continue;
			}
			*/
			
			/*
			uint8_t dummy[1];
			//size_t written = _ClientTelnet[i].write(&dummy, 1); // Блокирует программу
			_ClientTelnet[i].readBytes(dummy, 0);
			if(_ClientTelnet[i].peek() < 0)
			{
				if(_debugSerial) Serial.print("dummy: ");
				_freeSlot(i); continue;
			}
			*/
			//_ClientTelnet[i].println("123#");
		}
}
void TpyxaTerminal::_freeSlot(int i){
	if(_debugSerial) Serial.printf("disconnect %d\n", i);
    if(_ClientTelnet[i]) _ClientTelnet[i].stop();
    _ClientTelnet[i] = WiFiClient(); // Замещаем пустым классом для корректных проверок
    _ClientType[i] = CLIENT_TYPE_DISCONNECTED; // Переключаем тип клиента
}
int TpyxaTerminal::nClient(){
	int k=0;
	for(int i=0;i<MAXofTELNET;i++)
		if(_ClientType[i] != CLIENT_TYPE_DISCONNECTED)
			k++;
	return k;
}
char* TpyxaTerminal::typeClient(){
	int c=0;
	for(int i=0;i<MAXofTELNET;i++)
		switch(_ClientType[i])
	{
		case CLIENT_TYPE_DISCONNECTED:	break;
		case CLIENT_TYPE_UNDEFINED:		lc[c++]='?'; break;
		case CLIENT_TYPE_TEXT:			lc[c++]='T'; break;
		case CLIENT_TYPE_BASE:			lc[c++]='B'; break;
		case CLIENT_TYPE_GRAPHIC:		lc[c++]='G'; break;
		default:						lc[c++]='E'; break;
	}
	lc[c]='\0';
	return lc;
}
// Очистка буферов телнет-клиентов
void TpyxaTerminal::clearBuffers()
{
    char rb[4];
    for(int i=0;i<MAXofTELNET;i++)
    {
        if (_ClientTelnet[i] && _ClientTelnet[i].available() > 0)
        {
            while(_ClientTelnet[i].available() > 0) _ClientTelnet[i].readBytes(rb, 1);
        }
    }
}
size_t TpyxaTerminal::write(uint8_t byte) {
    // Отправляем байт в текущего клиента
    size_t written = 0;

	if(client)
	{
		if(!_ClientSolo.connected()) return 0;
		written += _ClientSolo.write(byte);
	}
	else
	{
		if(_nCurrentClient<0) return 0;
		if(_ClientType[_nCurrentClient]==CLIENT_TYPE_DISCONNECTED) return 0;
		if (!_ClientTelnet[_nCurrentClient]) return 0;
		if(!_ClientTelnet[_nCurrentClient].connected()) return 0;

		unsigned long t=millis();
		written += _ClientTelnet[_nCurrentClient].write(byte);

		// Слишком долгая отправка. Освобождаем слот и убираем текущий индекс
		if((millis()-t)>WRITE_TIMEOUT)
		{
			if(_debugSerial) Serial.printf("Write timeout. Client %d kicked\n", _nCurrentClient);
			_freeSlot(_nCurrentClient);
			_nCurrentClient = -1;
		}
	}
	return written;
}

size_t TpyxaTerminal::write(const uint8_t *buffer, size_t size) {
    // Отправляем блок байт в текущего клиента
    size_t total_written = 0;

	if(client)
	{
		if(!_ClientSolo.connected()) return 0;
		total_written += _ClientSolo.write(buffer, size);
	}
	else
	{
		if(_nCurrentClient<0) return 0;
		if(_ClientType[_nCurrentClient]==CLIENT_TYPE_DISCONNECTED) return 0;
		if (!_ClientTelnet[_nCurrentClient]) return 0;
		if(!_ClientTelnet[_nCurrentClient].connected()) return 0;
		
		unsigned long t=millis();
		total_written += _ClientTelnet[_nCurrentClient].write(buffer, size);

		// Слишком долгая отправка. Освобождаем слот и убираем текущий индекс
		if((millis()-t)>WRITE_TIMEOUT)
		{
			if(_debugSerial) Serial.printf("Write timeout. Client %d kicked\n", _nCurrentClient);
			_freeSlot(_nCurrentClient);
			_nCurrentClient = -1;
		}
	}
    return total_written;
}
// Установка текущего клиента
void TpyxaTerminal::SetClient(int ClientIndex){
	_nCurrentClient = ClientIndex;
}
void TpyxaTerminal::SetCommandCallback(UserCommandCallback callback){
	_UserCommandCallback = callback;
}
int TpyxaTerminal::Connect(const char *ip, int port){
	if(_ClientSolo.connected()) { client = true; return(1); }
	unsigned long t=millis();
	_port = port;
	_host.fromString(ip); // Преобразуем адрес из текста в число
	_ClientSolo.stop();
	while (!_ClientSolo.connect(_host, port) && (millis()-t)<5000) delay(10);
	if(!_ClientSolo.connected())
	{
		if(_debugSerial) Serial.printf("Error connect to %s:%d\n", ip, port);
		return(0);
	}
	_ClientSolo.setTimeout(10);
	_ClientSolo.setNoDelay(true);
	client = true;
	if(_debugSerial) Serial.printf("Telnet connect to %s:%d\n", ip, port);
	_SendRole(_roleClient);
	return(1);
}
int TpyxaTerminal::Connect(IPAddress ip, int port){
	if(_ClientSolo.connected()) { client = true; return(1); }
	unsigned long t=millis();
	_port = port;
	_host = ip;
	_ClientSolo.stop();
	while (!_ClientSolo.connect(_host, port) && (millis()-t)<1000) delay(10);
	if(!_ClientSolo.connected())
	{
		if(_debugSerial) Serial.printf("Error connect to %s:%d\n", ip.toString(), port);
		return(0);
	}
	_ClientSolo.setTimeout(10);
	_ClientSolo.setNoDelay(true);
	client = true;
	if(_debugSerial) Serial.printf("Telnet connect to %s:%d\n", ip.toString(), port);
	_SendRole(_roleClient);
	return(1);
}
void TpyxaTerminal::SetRole(int role){
	_roleClient = role;
	if(!_ClientSolo.connected()) return; // Клиент не соединен. Роль будет переданна позже при переконнекте
	_SendRole(_roleClient);
}
void TpyxaTerminal::_SendRole(int role){
	switch(role)
	{
		case CLIENT_TYPE_TEXT:
			if(_debugSerial) Serial.printf("set ROLE TEXT\n");
			println("ID:TEXT");
			break;
		case CLIENT_TYPE_BASE:
			if(_debugSerial) Serial.printf("set ROLE BASE\n");
			println("ID:BASE");
			break;
		default:
		if(_debugSerial) Serial.printf("Error: Unknown ROLE\n");
	}
}