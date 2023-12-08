#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char ** argv) {
    int fd;
    char buf[256];
    int n;

    // IP address and Port of Pi
    const char* host = "172.20.10.7";
    const int port = 12345;

    // Create a socket object
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialize the server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(host);
    serverAddress.sin_port = htons(port);

    // Bind the socket to the address and port
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Listen for incoming connections (1 connection at a time)
    listen(serverSocket, 1);
    std::cout << "Server listening on " << host << ":" << port << std::endl;

    // Accept a connection from a client
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen);

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::cout << "Connection from " << clientIP << std::endl;

    // Receive and parse messages
    char buffer[1024];

    // Open the Port. We want read/write, no "controlling tty" status, 
    // and open it no matter what state DCD is in
    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);  
    //open mbed's USB virtual com port
    if (fd == -1) {  
        perror("open_port: Unable to open /dev/ttyACM0 - ");
        return(-1);
    }

    // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that	
    fcntl(fd, F_SETFL, 0);
    //Linux Raw serial setup options
    struct termios options; 
    tcgetattr(fd,&options);   //Get current serial settings in structure
    cfsetspeed(&options, B9600);   //Change a only few
    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    cfmakeraw(&options);
    tcsetattr(fd,TCSANOW,&options);    //Set serial to new settings
    sleep(1);

    buf[1] = '\r'; // end of line
    buf[2] = 0; // end of string


    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;  // Break the loop if no more data is received
        }

        buffer[bytesRead] = '\0';  // Null-terminate the received data
        std::string message(buffer);

        // Parse the received message into two floats and two integers
        std::istringstream iss(message);
        float axis1, axis3;
        int button6, button7;
        char comma;

        if (!(iss >> axis1 >> comma >> axis3 >> comma >> button6 >> comma >> button7)) {
            std::cerr << "Error parsing message: " << message << std::endl;
            continue;
        }

        // Process the values as needed
        //std::cout << "Axis 1: " << axis1 << ", Axis 3: " << axis3
        //          << ", Button 6: " << button6 << ", Button 7: " << button7 << std::endl;
        signed char char_motor_left = static_cast<signed char>(axis1 * -127.0);
        signed char char_motor_right = static_cast<signed char>(axis3 * -127.0);
        signed char char_servo;
        if(button6 || button7) {
            if(button6 )char_servo = static_cast<signed char>(-1);
            else char_servo = static_cast<signed char>(1);
        }
        else char_servo = static_cast<signed char>(0);

        //char input = std::getchar();
        //std::string send(1, input);

        std::cout << "char_motor_left: " << static_cast<int>(char_motor_left)
                    << ", char_motor_right: " << static_cast<int>(char_motor_right)
                    << ", char_servo: " << static_cast<int>(char_servo) << std::endl;


        char char_array[3];
        char_array[0] = char_motor_left;
        char_array[1] = char_motor_right;
        char_array[2] = char_servo;

        n = write(fd, char_array, 3);  		
        if (n < 0) {
            perror("Write failed - ");
            return -1;
        }
        read(fd,buf,1); //read echo character
        printf("Came back from mbed: %s\n\r",buf);   //print in terminal window
    }

    // Don't forget to clean up and close the port
    tcdrain(fd);
    close(fd);
    return 0;
}
