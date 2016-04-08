#include <gmmproc/gir/xml.hh>

#include <iostream>

int main ()
{
  Gmmproc::Gir::CInclude inc;

  std::cout << inc.get<Gmmproc::Gir::Names::Name> () << std::endl;
}
