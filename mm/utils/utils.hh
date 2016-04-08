#ifndef MM_UTILS_UTILS_HH
#define MM_UTILS_UTILS_HH

namespace Mm
{

namespace Utils
{

std::string
join (std::vector<std::string> const& v)
{
  if (v.empty ())
  {
    return "";
  }

  std::ostringstream oss;
  std::for_each (std::crbegin (v), std::prev (std::crend (v)),
                 [&oss] (std::string const& str)
                 {
                   oss << str << " -> ";
                 });
  oss << v.front ();

  return oss.str ();
}

} // namespace Utils

} // namespace Mm

#endef // MM_UTILS_UTILS_HH
