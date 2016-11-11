#include "test.hh"

#include <gmmproc/xml/xml.hh>

#include <sstream>

int main ()
{
  std::stringstream ss {"<one><two/></one>"};
  Xml::Bundle bundle {ss};
  auto doc_view = bundle.document ();
  Doc document {doc_view};
  //auto one = document.get<TagOne> ();
  //auto two = one.get<TagTwo> ();
  auto two = document.get<TagTwo> ();

  (void) two;
}
