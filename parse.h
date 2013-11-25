
// This file is provided without any warranty of fitness
// for any purpose. You can redistribute this file
// and/or modify it under the terms of the GNU
// Lesser General Public License (LGPL) as published
// by the Free Software Foundation, either version 3
// of the License or (at your option) any later version.
// (see http://www.opensource.org/licenses for more info)

/*! Function to parse values from any .txt file to a variable
Example:
    string name;
    bool status = parse("file1.txt","Name",name);
    // String corresponding to 'Name' in file 'file1.txt' is parsed to variable 'name'
    // If parsing is successful, then 'status' is true. Otherwise false
Note:
    Define '_DEBUG_PARSE_ENABLE' for printing debug info...
Warnings:
    Operator >> has to be defined for input type 'T' of value.
Author:
    Anver Hisham <anverhisham@gmail.com>
*/

#ifndef PARSE_HEADER_H
#define PARSE_HEADER_H

#include<typeinfo>
#include <sstream>          //added for int to string conversion
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::string;

/*! Following function is similar to system() call, except that it returns terminal output instead of Command status */
std::string exec(std::string cmd) {
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) return "ERROR";
  int length = cmd.length()+1;
  char* buffer = new char [length];
  std::string result = "";
  while(!feof(pipe)) {
      if(fgets(buffer, length, pipe) != NULL)
              result += buffer;
  }
  pclose(pipe);
  delete [] buffer;
  return result;
}


template <class T>
bool parse(string inputFileName,string stringToParse,T &value){

    std::ostringstream convert;
    std::string inputFileNameTemp = inputFileName+".temp"+convert.str();
    std::ostringstream shellScript;
    string cfile=inputFileName; string variablename=stringToParse;

    /*! Following Perl-script does following
    1. Delete all comments/blank-lines, and spaces @ beginning/end/around '='/';'. 2. concat lines ending with '...'
    3. split lines with multiple parses.   4. And print into inputFileNameTemp file ..    */
    shellScript<<"cp -f "<<inputFileName<<" "<<inputFileNameTemp<<"; perl -i -pe 'BEGIN{undef $/;} s/\\R?\\h*\\/\\*\\X*?\\*\\/\\h*//smg, s/(\\R)*\\h*\\/\\/[[:print:]]*//smg, s/^\\h*\\R//smg, s/\\h*=\\h*/=/sg, s/\\h*\\;\\h*/\\;/sg, s/\\h*\\R\\h*/\\n/smg, s/\\.\\.\\.\\s*\\n//smg, s/\\;([^;\\n]*=)/\\;\\n\\1/g, s/\\;\\h*$//g, s/\\\"\\h*$//g  ' "<<inputFileNameTemp<<";";
    //! Now grep the only right side of '=' by removing beginning/trailing '=','\s','"','\n'
    shellScript<<"grep -P '^\\s*"<<stringToParse<<"' "<<inputFileNameTemp<<" |grep -Po '=.*' |grep -Po '[^= \"].*' |grep -Po '.*[^\\010; \"]' |head -1 |perl -ne 'chomp and print'";
   // cout<<"shellScript.str() = "<<shellScript.str()<<endl;
    //! Now store the output of shell-command to shellScriptOutput
    std::stringstream shellScriptOutput;  shellScriptOutput<<exec(shellScript.str());
    //! Remove the temp-file (which got created by removing C-comments in input-file)
    cout<<exec(std::string("rm -f ")+inputFileNameTemp);
    //! If parse-value is blank, then return false..
    if( shellScriptOutput.str()=="") {
        shellScriptOutput >> value;
    #ifdef _DEBUG_PARSE_ENABLE
        cout<<"cfile = "<<cfile<<"\t variablename = "<<variablename<<"\t value = "<<value<<endl;
    #endif
        return false;
    }

    //! Move the shellScriptOutput to value
    bool valueTemp;
    if(typeid(value).name()==typeid(valueTemp).name()) {        //! If parse value expected is bool,,
        const char* s1="true";
        const char* s2="1";
        if( (shellScriptOutput.str().compare(s1)==0 ) || (shellScriptOutput.str().compare(s2)==0 ))
           valueTemp=true;
        else
           valueTemp=false;
        T* temp = reinterpret_cast<T*>(&valueTemp);
        value = *temp;
    }
    else
        shellScriptOutput >> value;

    #ifdef _DEBUG_PARSE_ENABLE
        cout<<"cfile = "<<cfile<<"\t variablename = "<<variablename<<"\t value = "<<value<<endl;
    #endif
    return true;

}



#endif // PARSE_HEADER_H
