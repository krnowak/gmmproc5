// standard
#include <iostream>
#include <memory>

// api
#include "namespace.h"

// defsparser
#include "defsparser.h"

int main (int argc, char** argv)
{
  std::auto_ptr<Proc::Parsers::ApiParsers::DefsParser> parser (0);

  if (argc != 2)
  {
    std::cerr << "usage: gmmproc file" << std::endl;
    return 1;
  }
  parser.reset (new Proc::Parsers::ApiParsers::DefsParser (argv[1]));
  try
  {
    std::auto_ptr<Proc::Api::Namespace> ns (parser->parse());
  }
  catch (Proc::Common::InternalError e)
  {
    std::cerr << e.what () << "\nPlease make a report of this accident with parsed files attached." << std::endl;
    return 1;
  }
  catch (Proc::Parsers::SyntaxError e)
  {
    std::cerr << e.what () << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unexpected exception catched." << std::endl;
    return 1;
  }
  return 0;
}
