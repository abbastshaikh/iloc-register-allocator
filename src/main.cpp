#include <Scanner.hpp>
#include <Parser.hpp>
#include <Renamer.hpp>
#include <iostream>
#include <cstring>

void help () {
   std::cout << "Command Syntax: 412alloc [-h] [-x <filename>] [<k> <filename>]" << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << "   -h: Print this help menu." << std::endl;
   std::cout << "   -x <filename>: Scans, parses, and performs renaming on the ILOC block in <filename> and prints the results to stdout." << std::endl;
   std::cout << "   <k> <filename>: Scans, parses, and allocates the ILOC block in <filename> to <k> registers and prints the results to stdout." << std::endl;
}

void rename (std::string filename) {

   try {
      Scanner scanner (filename);

      try {
         Parser parser (scanner);
         InternalRepresentation rep = parser.parse();

         Renamer renamer;
         renamer.rename(rep);

         for (Operation op: rep.operations){
            std::cout << op.printVR() << std::endl;
         }
      } catch (ParseFailedException& e) {
         std::cerr << "Due to syntax errors, run terminates." << std::endl;
      }
   } catch (FileNotFoundException& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
   }
}

int main (int argc, char *argv[]) {

   if (argc < 2) {
      std::cerr << "ERROR: Must provide command line arguments." << std::endl;
      return -1;
   }

   if (!strcmp(argv[1], "-h")){
      help();
   } else if (!strcmp(argv[1], "-x")){
      if (argc == 2) {
         std::cerr << "ERROR: Expected filename after -x." << std::endl;
         return -1;
      }
      rename(argv[2]);
   } else {
      std::cerr << "ERROR: Unsupported command line syntax." << std::endl;
      return -1;
   }

   return 0;
}
