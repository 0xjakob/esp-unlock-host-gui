#include <iostream>
#include <exception>
#include <list>
#include <unistd.h>
#include <string.h>
#include "tclap/CmdLine.h"
#include <string>
#include <map>

#include "command_reader.hpp"
#include "hash_command.hpp"

#define DEVICE "/dev/ttyACM0"

using namespace TCLAP;
using namespace std;

int main (int argc, char** argv) {
    try {
        // Define the command line object.
        CmdLine cmd("Read AT commands from (Quectel) modem", ' ', "0.1");

        // Define a value argument and add it to the command line.
        ValueArg<string> nameArg("p","passphrase","Pass phrase to convert into hash",true,"","string");
        cmd.add(nameArg);

        // Parse the args.
        cmd.parse( argc, argv );
        
        string pass_phrase = nameArg.getValue();

        CommandReader reader(DEVICE);
        std::shared_ptr<HashCommand> cmd_ate(new HashCommand(pass_phrase));
        reader.execute_cmd(cmd_ate);
        string hash = cmd_ate->get_result().get();

        cout << "Result: " << hash << endl;
    } catch (invalid_argument e) {
        cout << e.what() << endl;
    } catch (out_of_range e) {
        cout << e.what() << endl;
    } catch (ReaderException e) {
        cout << "ERROR reading from serial port: " << e.what() << endl;
    }
}
