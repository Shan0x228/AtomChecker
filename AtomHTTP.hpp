#ifndef ATOMHTTP_HPP_
#define ATOMHTTP_HPP_
#pragma warning(disable : 4996)

#ifdef _WIN32
	// Windows
	#include <WinSock2.h>
	#include <ws2tcpip.h>
#else
	// UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <fcntl.h>
	//#include <unistd.h>
	#include <sys/types.h>	
#endif

#include <string>
#include <cassert>
#include <vector>
#include <regex>
#include <algorithm>
#include <chrono>
#include <thread>
#include <codecvt>

namespace Shkolnik
{
	namespace net
	{
		// IP-address regular expression
		static const std::regex url_ip(R"(^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$)");
		// URL        regular expression
		static const std::regex url(R"(^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$)");

		// Объявление класса
		class AtomHTTP
		{
		// Секция содержит необходимые переменные
		private:
			std::wstring serverIP;						// IP сервера
			int serverPort;								// Порт сервера
			std::vector<std::wstring> httpHeaders;		// Вектор с http-хидерами запроса
			std::wstring ResponseBody;					// Тело ответа
			std::wstring ResponseData;					// Полный ответ
			std::wstring RequestBody;					// Тело запроса

			int socketHandle;							// Хендл сокета
			int connectionHandle;						// Id соединения
			sockaddr_in peer;							// Структура соединения

		public:
			// Перечисление с типами запросов
			enum class HTTP
			{
				GET,
				POST
			};

			// Конструктор по умолчанию
			AtomHTTP(const std::wstring &URL, const int &port);

			// Возвращает ip сервера
			std::wstring GetServerIP();

			// Возвращает порт сервера
			int GetServerPort();

			// Подготавливает к запросу
			void SetupRequest(std::wstring PAGE = L"/", AtomHTTP::HTTP req_type = AtomHTTP::HTTP::GET);

			// Добавляет http-хидеры к запросу
			void AddHeader(std::wstring header);

			// Добавляет тело запроса
			void AddRequestBody(std::wstring body);

			// Отправляет запрос серверу и возвращает полный ответ(включая http-хидеры)
			std::wstring SendRequest();

			// Возвращает тело ответа
			std::wstring GetResponseBody();

			// Деструктор
			~AtomHTTP();
		};
	}
}

// Конструктор по умолчанию
Shkolnik::net::AtomHTTP::AtomHTTP(const std::wstring &URL, const int &port)
{
	using namespace std;
	using namespace std::string_literals;

#ifdef _WIN32
	WSADATA wsaData = { 0 };
	if (int iRes = WSAStartup(MAKEWORD(2, 2), &wsaData); iRes != 0)
	throw L"AtomHTTP: can't init WSA!"s;
#endif

	const string URL_C{ begin(URL), end(URL) };

	// Установка порта сервера
	this->serverPort = port;

	// Проверка регулярного выражения ip адреса
	if (regex_match(URL_C, url_ip))
	{
		// Установка ip адреса
		this->serverIP = URL;
	}
	// Проверка регулярного выражения адрес сайта(сервера)
	else if (regex_match(URL_C, url))
	{
		// Получение IP для хоста
		// DEBUG WinSock2: InetNtop()
		string ip = inet_ntoa(*(in_addr*)(gethostbyname(URL_C.c_str())->h_addr));

		// Установка ip адреса
		this->serverIP = wstring(begin(ip), end(ip));
	}
	else
		// Исключение:
		throw L"AtomHTTP: can't resolve this URL!"s;
}

// Возвращает ip сервера
std::wstring Shkolnik::net::AtomHTTP::GetServerIP()
{
	return this->serverIP;
}

// Возвращает порт сервера
int Shkolnik::net::AtomHTTP::GetServerPort()
{
	return this->serverPort;
}

// Подготавливает к запросу
void Shkolnik::net::AtomHTTP::SetupRequest(std::wstring PAGE, AtomHTTP::HTTP req_type)
{
	using namespace std;
	using namespace std::string_literals;

	// Создание сокета
#ifdef _WIN32
	this->socketHandle = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketHandle == INVALID_SOCKET)
		throw L"AtomHTTP: can't create socket!"s;
#else
	this->socketHandle = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketHandle <= 0)
		throw L"AtomHTTP: can't create socket!"s;
#endif
	
	// Заполнение полей структуры соединения
	this->peer.sin_family = AF_INET;
	this->peer.sin_port = htons(this->serverPort);
	this->peer.sin_addr.s_addr = inet_addr(string(begin(this->serverIP), end(this->serverIP)).c_str());

	// Попытка соединения с сервером...
	this->connectionHandle = connect(this->socketHandle, reinterpret_cast<sockaddr*>(&peer), sizeof(this->peer));
	if (this->connectionHandle)
		throw L"AtomHTTP: can't connect to server!"s;

	// Подготовка HTTP-хидеров по умолчанию
	req_type == HTTP::GET ? this->httpHeaders.push_back(L"GET " + PAGE + L" HTTP/1.1\r\n") : this->httpHeaders.push_back(L"POST " + PAGE + L" HTTP/1.1\r\n");
}

// Добавляет http-хидеры к запросу
void Shkolnik::net::AtomHTTP::AddHeader(std::wstring header)
{
	using namespace std;
	using namespace std::string_literals;

	// Проверка структуры соединения
	assert(!this->httpHeaders.at(0).empty() && L"AtomHTTP: first, you need to call SetupRequest()!");

	// Проверка на пустоту хидера
	if (!header.empty())
	{
		// Проверка форматирования...
		if (header.find_last_of(L"\r\n") == std::string::npos)
			header.append(L"\r\n");

		// Добавление хидера в массив
		this->httpHeaders.push_back(header);
	}
}

// Добавляет тело запроса
void Shkolnik::net::AtomHTTP::AddRequestBody(std::wstring body)
{
	this->RequestBody += body;
}

// Отправляет запрос серверу и возвращает полный ответ(включая http-заголовки)
std::wstring Shkolnik::net::AtomHTTP::SendRequest()
{
	using namespace std;
	using namespace std::string_literals;

	// Содержит полное тело запроса
	wstring req{};

	// Добавляем управляющие последовательности для HTTP стандарта
	this->httpHeaders.push_back(L"\r\n");

	// Заполняем переменную HTTP заголовками
	for (const auto x : this->httpHeaders)
		req += x;

	// Добавляем тело запроса(без заголовков)
	if (!this->RequestBody.empty())
		req += this->RequestBody;

	// Отправляем серверу через сокет
	int bytes = send(this->socketHandle, string(begin(req), end(req)).c_str(), req.size() * 2, 0);
	if (bytes <= 0)
		throw L"AtomHTTP: some error in send(...)!"s;

	// Отключаем блокировку сокета
#ifdef _WIN32
	// Windows
	u_long nonblocking_enabled = TRUE;
	ioctlsocket(this->socketHandle, FIONBIO, &nonblocking_enabled);
#else
	// UNIX
	fcntl(this->socket_handle, F_SETFL, O_NONBLOCK);
#endif

	// Общий размер ответа
	int total_size{ 0 };

	// Временный буфер для хранения чанка данных
	string buffer{};
	buffer.resize(1024);

	// TODO...
	while (true)
	{
		// Небольшая пауза...
		this_thread::sleep_for(std::chrono::milliseconds(300));

		// Получение данных от сервера(первый 512 байт)
		int n = recv(this->socketHandle, &buffer[0], 1024, 0);

		// Увеличение общего размера полученных байт
		total_size += n;

		// Проверка - все ли данные получены?
		if (total_size > 0 && n <= 0)
			break;

		// Увеличение чанка данных
		buffer.resize(buffer.size() + 1024);
	}

	// Подгон чанка под реальный размер
	buffer.erase(remove(begin(buffer), end(buffer), 0), end(buffer));

	// Конвертация в UTF-8
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	this->ResponseData = converter.from_bytes(buffer);

	// Форматирование полученных данных
	std::size_t content_pos = this->ResponseData.find(L"\r\n\r\n");
	if (content_pos != std::string::npos)
	{
		this->ResponseBody = this->ResponseData.substr(content_pos + 4);
		this->ResponseBody.erase(remove(begin(this->ResponseBody), end(this->ResponseBody), 0), end(this->ResponseBody));
	}

	// Возврат полного ответа от сервера
	return this->ResponseData;
}

// Возвращает тело ответа
std::wstring Shkolnik::net::AtomHTTP::GetResponseBody()
{
	return this->ResponseBody;
}

// Деструктор
Shkolnik::net::AtomHTTP::~AtomHTTP()
{
}

#endif // ATOMHTTP_HPP_