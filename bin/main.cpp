#include <lib/parsing.h>
#include "lib/id3.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
  ArgumentParser::ArgParser Parser("Main");
  Parser.AddStringArgument('f', "filepath", "Path to mp3 file");
  if (!Parser.Parse(argc, argv)) {
    std::cerr << "Arguments parsing went wrong" << std::endl;
    exit(1);
  };

  Id3 Test(Parser.GetStringValue("filepath"));
}