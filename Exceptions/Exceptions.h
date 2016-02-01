// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _Exceptions
#define _Exceptions

#include <exception>
#include <string>
#include <sstream>
using namespace std;

class not_implemented: public exception
    { virtual const char* what() const throw()
        { return "Case not implemented"; }
    };
class invalid_length: public exception
    { virtual const char* what() const throw()
        { return "Invalid length"; }
    };
class invalid_commitment: public exception
    { virtual const char* what() const throw()
        { return "Invalid Commitment"; }
    };
class bad_value: public exception
    { virtual const char* what() const throw()
        { return "Some value is wrong somewhere"; }
    };
class file_error:  public exception
    { string filename;
      public:
      file_error(string m="") : filename(m) {}
      ~file_error()throw() { }
      virtual const char* what() const throw()
        { string ans="File Error : ";
          ans+=filename;
          return ans.c_str();
        }
    };
class end_of_file: public exception
    { virtual const char* what() const throw()
        { return "End of file reached"; }
    };





#endif
