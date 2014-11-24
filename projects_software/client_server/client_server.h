#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H
/**
Library defining client ad server classes
*/

#include <string>
#include <ctime>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BACKPORT 5 //Backport constant

#define DEF_PORT 3490  // the port users will be connecting to
#define DEF_BUF_SIZE 256 // Default buffer size
#define DEF_SERVER_HOST "127.0.0.1" //Default server host

//Default log messages
#define RECI_LOG "Message was recieved: "
#define SEND_LOG "Message was sent: "

enum log_type {ERROR, INFO};
/**
* Class <CS> is super class for <client> and <server> classes
* Implements and defines:
* <log_out> - logging function
* <clear_buffer> - clearing recieve buffer function
* <send_message> - Sending message function
* <recieve_message> - Recieving message function
*/
class CS
{
protected:
	int _socket; /*!< socket for network */
	struct sockaddr_in serv_addr_ip4; /*!< server address. IPv4 format*/
	uint16_t port; /*!< network port */

	char* rec_buffer; /*!< buffer poiter */
    uint64_t buf_size; /*!< network port */
    bool connected; /*!< boolean property. Defines when connection is established */

	std::string name; /*!< Name property showed in log out */

	/**
    * Logging function.
    * @param type is log type parameter.
    * @param message is what it will print out.
    */
	void log_out(log_type type, std::string message){
		time_t     now = time(0);
    	struct tm  tstruct = *localtime(&now);
    	char       buf[80];
    	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		std::cout << name << "-"<< type <<"|";
		std::cout <<buf<<"|";
		std::cout << message;
		std::cout << "\n";
	}
	/**
    * Function clears buffer
    */
	void clear_buffer(){
		memset(rec_buffer, 0, buf_size);
	}

public:
	/**
    * CS constructor.
    * allocates buffer and set netwerk port.
    * @param _buf_length is a length of buffer.
    * @param _port is a network port.
    */
	CS(uint64_t _buf_length, uint16_t _port){
		buf_size = _buf_length;
		rec_buffer = (char*)calloc(buf_size, sizeof(char));
		port = _port;
		
	}
	/**
    * CS destructor.
    * frees buffer allocated memory
    */
	~CS(){
		free(rec_buffer);
	};

	/**
    * Send message through network. Works only if connection established
    * @param message is a message to send.
    */
	void send_message(std::string message){
		if(!connected){
			log_out(INFO, "Trying to send while not connected");
			return;
		}

		uint32_t sen_size = 0;
		sen_size = write(_socket, message.c_str(), message.size());
		if (sen_size == message.size())
			log_out(INFO,"Message was succesfull sent");
		else
			log_out(ERROR,"Message was NOT sent correctly");
	}

	/**
    * Recieve message through network. Works only if connection established
    * @return recieved message.
    */
	std::string recieve_message(){
		if(!connected){
			log_out(ERROR, "Trying to recieve while not connected");
			return std::string();
		}

		clear_buffer();

		if(_socket < 0){
			log_out(ERROR, "Corrupted connection");
			return std::string();
		}

		int rec_size = read(_socket, rec_buffer, buf_size);
		if (rec_size >= 0){
			log_out(INFO, "Message was succesfull recieved");
			return std::string(rec_buffer);
		}else{
			log_out(ERROR, "Cannot read correct number of bytes");
			return std::string();
		}
	}
};

/**
* Class <Server> is class defines server functionalty.
* LIMITATIONS: Now support only one client connected
* Defines:
* <wait_for_client> - waiting for client cnnection function
* <prepare_socket()> - creating socket function
*/
class Server: public CS
{
private:
	struct sockaddr_in cli_addr_ip4; /*!< Client address. IPv4 format*/

	/**
    * Create and prepare socket for incoming connection
    */
    void prepare_socket();

public:
	/**
    * Server constructor.
    * Set <name> log property in "Server" and prepare socket
    * @param _buf_length is a length of buffer.
    * @param _port is a network port.
    */
	Server(uint64_t __buf_length = DEF_BUF_SIZE, uint16_t __port = DEF_PORT): 
										CS(__buf_length, __port)
	{
		name = std::string("SERVER");
		prepare_socket();
	}

	/**
    * Server destructor.
    */
	~Server(){};

	/**
    * Wait for incoming connection
    * Freezing server execution until incoming connection will be established
    */
	int wait_for_client();
};

/**
* Class <Client> is class defines client functionalty
* Defines:
* <connect_to_server> - establish connections to the server
*/
class Client: public CS
{
public:
	/**
    * Client constructor.
    * Set <name> log property in "Client" and prepare socket
    * @param _buf_length is a length of buffer.
    * @param _port is a network port.
    */
	Client(uint64_t __buf_length = DEF_BUF_SIZE, uint16_t __port = DEF_PORT): 
										CS(__buf_length, __port)
	{
		name = std::string("CLIENT");
	}
	~Client(){};

	/**
    * Establish connection to the server
    * @param hostname is name for host where server is.
    * @return 0 if connection was established and -1 if error
    */
	int connect_to_server(std::string hostname = std::string(DEF_SERVER_HOST));
};

#endif