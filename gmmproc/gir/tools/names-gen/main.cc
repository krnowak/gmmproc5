#include <gmmproc/xml/xml.hh>

#include <boost/algorithm/string.hpp>

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cctype>

namespace Xml = Gmmproc::Xml;

namespace
{

class NamesWalker : public Xml::Walker
{
public:
  NamesWalker ()
    : names {}
  {}

  std::vector<std::string>&&
  steal ()
  {
    return std::move (names);
  }

private:
  virtual bool node (Xml::Node& node,
                     int) override
  {
    if (node.name () == "name")
    {
      auto attribute = node.attribute ("name");
      if (!attribute)
      {
        throw std::runtime_error ("no 'name' attribute found in 'name' tag");
      }
      names.push_back (attribute->value ());
    }
    return true;
  }

  virtual bool postprocess_node (Xml::Node&,
                                 int) override
  {
    return true;
  }

  std::vector<std::string> names;
};

Xml::Document
get_cleaned_up_document (std::string const& path)
{
  Xml::Document doc (path);
  auto root_tag = doc.root_tag ();

  if (!root_tag)
  {
    std::ostringstream oss;

    oss << "document " << path << " has no contents\n";
    throw std::runtime_error (oss.str ());
  }

  auto short_tag = root_tag->child ("short");
  auto long_tag = root_tag->child ("long");

  if (short_tag)
  {
    root_tag->remove (*short_tag);
  }
  if (long_tag)
  {
    root_tag->remove (*long_tag);
  }
  return doc;
}

class CxxNames
{
public:
  CxxNames (std::vector<std::string>&& n)
    : names {std::move (n)}
  {}

  std::string
  get_cxx_code ()
  {
    std::ostringstream oss;

    write_header (oss);
    write_names (oss);
    write_footer (oss);

    return oss.str ();
  }

private:
  void
  write_header (std::ostringstream& oss)
  {
    oss <<
      "// this file is generated, do not edit\n"
      "#include <gmmproc/utils/str.hh>\n"
      "\n"
      "namespace Gmmproc\n"
      "{\n"
      "\n"
      "namespace Gir\n"
      "{\n"
      "\n"
      "namespace Names\n"
      "{\n"
      "\n";
  }

  void
  write_names (std::ostringstream& oss)
  {
    for (auto const& name : names)
    {
      oss << "using " << get_cxx_name (name) << " = Utils::StaticString<" << get_exploded_name (name) << ">;\n";
    }
  }

  std::string
  get_cxx_name (std::string const& name)
  {
    namespace ba = boost::algorithm;

    std::vector<std::string> parts;

    ba::split (parts, name, ba::is_any_of ("-:"));
    for (auto& p : parts)
    {
      if (!p.empty ())
      {
        p[0] = std::toupper (p[0]);
      }
    }

    return ba::join (parts, "");
  }

  std::string
  get_exploded_name (std::string const& name)
  {
    namespace ba = boost::algorithm;

    std::vector<std::string> parts;

    parts.reserve (name.size ());
    for (auto c : name)
    {
      parts.emplace_back (1, c);
    }

    return "'" + ba::join (parts, "', '") + "'";
  }

  void
  write_footer (std::ostringstream& oss)
  {
    oss <<
      "\n"
      "} // namespace Names\n"
      "\n"
      "} // namespace Gir\n"
      "\n"
      "} // namespace Gmmproc\n";
  }

  std::vector<std::string> names;
};

void
generate_names_code (std::vector<std::string>&& names)
{
  CxxNames cxx {std::move (names)};

  std::cout << cxx.get_cxx_code ();
}

} // anonymous namespace

int
main (int argc,
      char** argv)
{
  if (argc < 2)
  {
    std::cerr << "expected a path to gir.xml\n";
    return 1;
  }
  try
  {
    auto doc = get_cleaned_up_document (argv[1]);
    NamesWalker walker;

    walker.walk (doc);
    generate_names_code (walker.steal ());
  }
  catch (std::exception const& ex)
  {
    std::cerr << ex.what () << "\n";
    return 1;
  }
}
