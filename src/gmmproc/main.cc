// standard
#include <iostream>
#include <memory>

// common
#include "internalerror.h"
#include "syntaxerror.h"

// api
#include "namespace.h"

// defsparser
#include "defsparser.h"

int main (int argc, char** argv)
{
  std::shared_ptr<Proc::Parsers::ApiParsers::DefsParser> parser (new Proc::Parsers::ApiParsers::DefsParser);

  if (argc != 2)
  {
    std::cerr << "usage: gmmproc file" << std::endl;
    return 1;
  }
  parser->set_file_to_parse (argv[1]);
  try
  {
    Proc::Api::NamespacePtr ns (parser->parse());
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
