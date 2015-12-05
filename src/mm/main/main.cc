#include <mm/gir/xml.hh>

#include <iostream>

int main()
{
  Mm::Gir::CInclude inc;

  std::cout << inc.get<Mm::Gir::Names::Name> () << std::endl;
}
