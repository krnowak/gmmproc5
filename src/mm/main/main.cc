#include <algorithm> // find_if, sort, partition
#include <functional> // reference_wrapper
#include <iostream>
#include <iterator> // begin, end
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <pugixml.hpp>

using NodeMap = std::unordered_map<std::string, int>;
using StrSet = std::unordered_set<std::string>;

class GirWalker : public pugi::xml_tree_walker
{
public:
  bool
  for_each(pugi::xml_node& node) override
  {
    if (node.type () == pugi::node_element)
    {
      auto const name = get_name (node);
      node_child_names[name];
      all_nodes.insert (name);

      auto range = node.parent ().children (name.c_str ());
      auto occurences = std::distance (std::begin (range), std::end (range));
      auto& nm = node_child_names[get_name (node.parent ())];
      auto old_occurences = nm.find (name);
      if (old_occurences != std::end (nm))
      {
        if (occurences > old_occurences->second)
        {
          old_occurences->second = occurences;
        }
      }
      else
      {
        nm[name] = occurences;
      }
      for (auto const& attr : node.attributes ())
      {
        node_attributes[name].insert (attr.name ());
        all_attributes.insert (attr.name ());
      }
      /*
      if (name == "array" && get_name (node.parent ()) == "type")
      {
        for (auto n = node; n; n = n.parent ())
        {
          std::cout << "  " << n.attribute ("name").value () << std::endl;
        }
      }
      */
      /*
      if (name == "array" || name == "type")
      {
        std::string other_name = (name == "array" ? "type" : "array");
        for (auto const& n : node.parent ().children ())
        {
          if (&n == &node)
          {
            continue;
          }
          if (get_name (n) == other_name)
          {
            for (auto n2 = node; n2; n2 = n2.parent ())
            {
              std::cout << "  " << n2.attribute ("name").value () << std::endl;
            }
          }
        }
      }
      */
      if (name == "record" || name == "union")
      {
        std::string const other_name = (name == "record" ? "union" : "record");
        if (get_name (node.parent ()) == other_name)
        {
          for (auto n = node; n; n = n.parent ())
          {
            std::cout << "  " << n.name () << "(" << n.attribute ("name").value () << ")\n";
          }
        }
      }
    }
    return true;
  }

  std::unordered_map<std::string, NodeMap>&
  get_map ()
  {
    return node_child_names;
  }

  std::unordered_map<std::string, StrSet>&
  get_attrs ()
  {
    return node_attributes;
  }

  StrSet&
  get_all_attrs()
  {
    return all_attributes;
  }

  StrSet&
  get_all_nodes()
  {
    return all_nodes;
  }

private:
  std::string get_name(pugi::xml_node const& node) const
  {
    switch (node.type ()) {
    case pugi::node_document:
      return "docroot";
    case pugi::node_element:
      return node.name ();
    default:
      return "INVALID";
    }
  }

  std::unordered_map<std::string, NodeMap> node_child_names;
  std::unordered_map<std::string, StrSet> node_attributes;
  StrSet all_attributes;
  StrSet all_nodes;
};

void
print_tree (GirWalker& walker)
{
  std::vector<std::string> lines;
  auto const& el_map = walker.get_map ();
  auto const& attr_map = walker.get_attrs ();

  lines.reserve(el_map.size ());
  for (auto const& kv : el_map) {
    using Pair = std::pair<std::string, int>;
    std::ostringstream oss;
    std::vector<Pair> children {std::begin (kv.second), std::end (kv.second)};

    std::sort (std::begin (children), std::end (children),
               [](NodeMap::value_type const& a, NodeMap::value_type const& b) -> bool
               {
                 return a.first < b.first;
               });
    oss << kv.first << ":";
    for (auto const& c : children)
    {
      oss << " " << c.first;
      if (c.second > 1)
      {
        oss << " (" << c.second << ")";
      }
    }
    auto const& attr_iter = attr_map.find (kv.first);

    if (attr_iter != attr_map.cend ())
    {
      std::vector<std::string> attrs {std::begin (attr_iter->second), std::end (attr_iter->second)};
      std::sort (std::begin (attrs), std::end (attrs));
      oss << " |";
      for (auto const& a : attrs)
      {
        oss << " " << a;
      }
    }
    oss << "\n";
    lines.push_back (oss.str ());
  }
  std::sort (std::begin (lines), std::end (lines));
  for (auto const& line : lines)
  {
    std::cout << line;
  }
}

using StrNMPair = std::pair<std::string, NodeMap>;

std::string
remove_loop (std::vector<StrNMPair>& elems,
             std::string const& first_keep,
             std::string const& second_remove)
{
  {
    auto remove_iter = std::find_if (std::begin (elems), std::end (elems),
                                     [&second_remove] (StrNMPair const &p) -> bool
                                     {
                                       return p.first == second_remove;
                                     });
    if (remove_iter != std::end (elems))
    {
      elems.erase (remove_iter);
    }
  }
  std::string new_name {first_keep + "-xor-" + second_remove + "-recursive"};
  std::for_each (std::begin (elems),
                 std::end (elems),
                 [&new_name, &first_keep, &second_remove] (StrNMPair& p)
                 {
                   if (p.first == first_keep)
                   {
                     p.first = new_name;
                     p.second.erase (first_keep);
                     p.second.erase (second_remove);
                   }
                   else if (p.second.erase (first_keep) + p.second.erase (second_remove) > 0)
                   {
                     p.second.insert (NodeMap::value_type (new_name, 0));
                   }
                 });
  return new_name;
}

void
print_leaves_to_root (GirWalker& walker)
{
  std::vector<StrNMPair> elems;
  auto const& el_map = walker.get_map ();
  std::vector<std::string> recs {};
  auto level = 0u;

  elems.reserve (el_map.size ());
  elems.insert (std::end (elems), std::begin (el_map), std::end (el_map));
  recs.push_back (remove_loop (elems, "array", "type"));
  recs.push_back (remove_loop (elems, "record", "union"));
  while (!elems.empty ())
  {
    auto half = std::partition (std::begin (elems),
                                std::end (elems),
                                [] (StrNMPair const& p) -> bool {
                                  return !p.second.empty();
                                });
    if (half == std::end (elems))
    {
      std::cout << "    Leftover nodes:\n";
      std::for_each (std::begin (elems), std::end (elems),
                    [] (StrNMPair const& p)
                    {
                      std::cout << "      " << p.first << "\n";
                    });
      break;
    }
    std::sort (half,
               std::end (elems),
               [] (StrNMPair const& a, StrNMPair const& b) -> bool
               {
                 return a.first < b.first;
               });
    std::for_each (half,
                   std::end (elems),
                   [&elems, &half, &level, &recs] (StrNMPair const& p)
                   {
                     auto const& name = p.first;
                     auto const is_r = [&recs, &name] ()
                     {
                       auto r = std::find (std::begin (recs), std::end (recs), name);
                       if (r != std::end (recs))
                       {
                         recs.erase (r);
                         return true;
                       }
                       return false;
                     }();
                     std::cout << name << " " << level << (is_r ? " (recursive)" : "") << std::endl;
                     std::for_each (std::begin (elems),
                                    half,
                                    [&name] (StrNMPair &p2)
                                    {
                                      p2.second.erase (name);
                                    });
                   });
    elems.erase (half, std::end (elems));
    ++level;
  }
}

void
print_names (GirWalker& walker)
{
  std::vector<std::string> lines;
  auto const& attrs = walker.get_all_attrs ();
  auto const& nodes = walker.get_all_nodes ();

  lines.reserve(attrs.size () + nodes.size ());
  lines.insert(std::end (lines), std::begin (attrs), std::end (attrs));
  lines.insert(std::end (lines), std::begin (nodes), std::end (nodes));
  std::sort (std::begin (lines), std::end (lines));
  for (auto const& line : lines)
  {
    std::cout << line << "\n";
  }
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    std::cerr << "Expected a path to girdir and at least one gir filename (e.g. Gio-2.0.gir)\n";
    return 1;
  }

  auto const girdir = std::string {argv[1]};
  std::queue<std::string> girfiles_to_parse;
  GirWalker walker;
  std::unordered_set<std::string> parsed;

  for (int idx {2}; idx < argc; ++idx)
  {
    auto const girfile = std::string {argv[idx]};

    girfiles_to_parse.push (girdir + '/' + girfile);
  }

  while (!girfiles_to_parse.empty ())
  {
    auto const path = girfiles_to_parse.front ();

    girfiles_to_parse.pop ();

    if (parsed.find (path) != parsed.end ())
    {
      continue;
    }
    std::cout << "parsing " << path << "\n";
    pugi::xml_document doc;
    auto result = doc.load_file (path.c_str (), pugi::parse_default, pugi::encoding_utf8);

    if (!result)
    {
      std::cerr << "Failed to parse - " << result.description () << "\n";
      return 1;
    }

    auto const repo = doc.child ("repository");
    /*
    auto const ns = repo.child ("namespace");
    auto const ns_name = std::string {ns.attribute ("name").value ()};
    auto const ns_version = std::string {ns.attribute ("version").value ()};
    auto const ns_full_name = ns_name + "-" + ns_version;
    */

    for (auto const& include : repo.children ("include"))
    {
      auto const inc_name = std::string {include.attribute ("name").value ()};
      auto const inc_version = std::string {include.attribute ("version").value ()};
      auto const inc_filename = inc_name + "-" + inc_version + ".gir";
      auto const inc_full_path = girdir + "/" + inc_filename;

      girfiles_to_parse.push (inc_full_path);
    }

    doc.traverse (walker);
    parsed.insert (path);
  }
  std::cout << "\nTree\n\n";
  print_tree (walker);
  std::cout << "\nNames\n\n";
  print_names (walker);
  std::cout << "\nOrder\n\n";
  print_leaves_to_root (walker);
  std::cout << "\nFine and dandy\n";
}
