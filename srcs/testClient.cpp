#include "Client.hpp"
#include <iostream>

int main()
{
    Client client(42);

    // Test 1 : Message complet d'un coup
    std::cout << "\n=== TEST 1 ===" << std::endl;
    client.appendToBuffer("NICK alice\r\n");
    std::vector<std::string> msgs = client.extractMessages();
    std::cout << "Messages extraits: " << msgs.size() << std::endl;
    for (size_t i = 0; i < msgs.size(); ++i)
        std::cout << "  [" << i << "] " << msgs[i] << std::endl;

    // Test 2 : Message fragmenté
    std::cout << "\n=== TEST 2 ===" << std::endl;
    client.appendToBuffer("USER ali");
    msgs = client.extractMessages();
    std::cout << "Messages extraits: " << msgs.size() << std::endl;

    client.appendToBuffer("ce 0 * :Alice\r\n");
    msgs = client.extractMessages();
    std::cout << "Messages extraits: " << msgs.size() << std::endl;
    for (size_t i = 0; i < msgs.size(); ++i)
        std::cout << "  [" << i << "] " << msgs[i] << std::endl;

    // Test 3 : Plusieurs messages d'un coup
    std::cout << "\n=== TEST 3 ===" << std::endl;
    client.appendToBuffer("NICK bob\r\nUSER bob 0 * :Bob\r\n");
    msgs = client.extractMessages();
    std::cout << "Messages extraits: " << msgs.size() << std::endl;
    for (size_t i = 0; i < msgs.size(); ++i)
        std::cout << "  [" << i << "] " << msgs[i] << std::endl;

    return 0;
}