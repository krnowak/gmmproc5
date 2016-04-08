#include <boost/algorithm/string/classification.hpp> // for is_any_of
#include <boost/algorithm/string/join.hpp> // for join
#include <boost/algorithm/string/split.hpp> // for split

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <cctype>

class CxxNames
{
public:
  CxxNames (std::string const& filename)
    : f {filename}
  {
    if (!f)
    {
      oss << "failed to open file " << filename;
      throw std::runtime_error (oss.str ());
    }
  }

  std::string
  get_cxx_code ()
  {
    oss = std::ostringstream {};
    write_header ();
    write_names ();
    write_footer ();

    return oss.str ();
  }

private:
  void
  write_header ()
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
  write_names ()
  {
    for (std::string name; std::getline (f, name);)
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
  write_footer ()
  {
    oss <<
      "\n"
      "} // namespace Names\n"
      "\n"
      "} // namespace Gir\n"
      "\n"
      "} // namespace Gmmproc\n";
  }

  std::ifstream f;
  std::ostringstream oss;
};

int
main (int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << argv[0] << ": Expected one parameter - a file with names\n";
    return 1;
  }

  try
  {
    CxxNames names {argv[1]};

    std::cout << names.get_cxx_code ();
  }
  catch (std::exception const& ex)
  {
    std::cerr << argv[0] << ": Failed to generate C++ code: " << ex.what () << "\n";
    return 1;
  }
}
