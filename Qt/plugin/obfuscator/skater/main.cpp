// Zach J. Elko
// 2010
// skater.cpp
//
// I've wanted to make one of these for a while now. I got bored and whipped
// this up in about 3 hours. There are a lot of improvements that can/should
// be made, but it's not bad for the short amount of time put into it.
//
// Basic C/C++ code obfuscator. 
// Could be improved by using regular expressions. 
// Could be expanded by also renaming variables as part of the obfuscation process.
//
// Features: 
//  Strips single/multi line comments
//  Removing leading whitespace
//  Removing line breaks
//  When removing line breaks, it properly handles all preprocessor directives, 
//  'using' declarations and single-line 'else' clauses where the braces have been 
//  omitted. Does not yet support all cases in which braces have been omitted in 
//  single-line conditional expressions.
//  
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

void stripLeadingWhite(std::string& line);
void stripSingleLineComment(std::string& line);
bool containsOnlyWhite(const std::string& line);
void stripMultiLineComment(char* inFile);

const std::string TEMP_FILE = ".skater-temp-file.tmp";

int main(int argc, char *argv[])
{
   if (argc != 3)
   {
      std::cerr << "Usage: ./skater infile outfile\n";
      exit(1);
   }

   // TODO: Do this on the same pass as the rest of the obfuscation
   // to save the extra file I/O
   stripMultiLineComment(argv[1]);

   std::ifstream inFileStream(TEMP_FILE.c_str());
   std::ofstream outFileStream(argv[2]);
   std::string line;
   bool addReturns;
   while (std::getline(inFileStream, line))
   {
      addReturns = false;
      if (!containsOnlyWhite(line))
      {
         stripLeadingWhite(line);
         stripSingleLineComment(line);

         if (line.size() > 0)
         {
            // TODO: Put this into a function to properly
            // handle the exceptional cases
            if (line.find("#") != std::string::npos ||
                    line.find("using") != std::string::npos ||
                    line == "else")
            {
               addReturns = true;
            }
            if (addReturns)
            {
               outFileStream << std::endl << line << std::endl;
            }
            else
            {
               outFileStream << line;
            }
         }
      }
   }
   inFileStream.close();
   outFileStream.close();
   remove(TEMP_FILE.c_str());
   return 0;
}

bool containsOnlyWhite(const std::string& line)
{
   std::string temp = line;
   temp.erase(remove_if(temp.begin(), temp.end(), isspace), temp.end());
   return temp.size() <= 0;
}

void stripLeadingWhite(std::string& line)
{
   if (line.size() > 0)
   {
      int nonWhiteIndex = 0;
      for (int i = 0; i < line.size(); ++i)
      {
         // TODO: remove whitespace in-place to avoid the call to erase()
         if (isspace(line.at(i)))
         {
            ++nonWhiteIndex;
         }
         else
         {
            break;
         }
      }

      line.erase(0, nonWhiteIndex);
   }
}

void stripSingleLineComment(std::string& line)
{
   if (line.size() >= 2)
   {
      int loc = line.find("//");
      if (loc != std::string::npos)
      {
         line.erase(loc);
      }
   }
}

// TODO: Do this on the same pass as the rest of the obfuscation
// to save the extra file I/O

void stripMultiLineComment(char* inFile)
{
   std::ifstream inFileStream(inFile);
   std::ofstream outFileStream(TEMP_FILE.c_str());
   std::string line;
   int startPos;
   int endPos;
   bool multiLine = false;
   while (std::getline(inFileStream, line))
   {
      // TODO: Refactor this! Both sections do the same thing, differently!
      if (multiLine)
      {
         endPos = line.find("*/");
         // If we find the closing comment, erase up through it
         if (endPos != std::string::npos)
         {
            line.erase(0, endPos + 2);
            multiLine = false;
         }
            // Otherwise, erase the entire line
         else
         {
            line.erase();
            multiLine = true;
         }
      }
      else
      {
         startPos = line.find("/*");
         if (startPos != std::string::npos)
         {
            endPos = line.find("*/");
            if (endPos != std::string::npos)
            {
               // We add 2 to the endPos for the */ because find() returns the start
               // of the string, and we need to remove through the end of the string.
               int commentLength = (endPos + 2) - startPos;
               line.erase(startPos, commentLength);
               multiLine = false;
            }
            else
            {
               multiLine = true;
               line.erase();
            }
         }
      }
      if (line.size() > 0)
      {
         outFileStream << line << std::endl;
      }
   }
   inFileStream.close();
   outFileStream.close();
}
