#include <pugixml.hpp>

#include <algorithm> // find_if, sort, partition
#include <functional> // reference_wrapper
#include <iostream>
#include <iterator> // begin, end
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <cctype>

using StrSet = std::unordered_set<std::string>;
template <typename T>
using StrMap = std::unordered_map<std::string, T>;

struct NodeData
{
  enum class Leaf
  {
    UNDETERMINED,
    NEVER_A_LEAF,
    SOMETIMES_A_LEAF,
    ALWAYS_A_LEAF
  };

  NodeData ()
    : attribute_names {},
      child_names {},
      parent_names {},
      has_text {},
      occurences_per_parent {},
      leaf {Leaf::UNDETERMINED}
  {}

  StrSet attribute_names;
  StrSet child_names;
  StrSet parent_names;

  bool has_text;
  StrMap<std::size_t> occurences_per_parent;
  Leaf leaf;
};

using NodeMap = StrMap<NodeData>;

class GirWalker : public pugi::xml_tree_walker
{
public:
  GirWalker ()
    : nodes {},
      processors {&GirWalker::process_occurences,
                  &GirWalker::process_leaves,
                  &GirWalker::process_attributes,
                  &GirWalker::process_children,
                  &GirWalker::process_parent,
                  &GirWalker::process_text}
  {}

  bool
  for_each (pugi::xml_node& node) override
  {
    if (node.type () == pugi::node_element)
    {
      auto const name = get_name (node);
      auto ins_pair = nodes.insert (NodeMap::value_type {name, NodeData {}});
      auto data_iter = ins_pair.first;

      for (auto&& p : processors)
      {
        p (*this, data_iter, node);
      }
    }
    return true;
  }

  NodeMap&
  get_map ()
  {
    return nodes;
  }

private:
  std::string get_name (pugi::xml_node const& node) const
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

  void
  process_occurences (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    auto parent = node.parent ();
    if (!parent)
    {
      return;
    }
    auto const& name = data_iter->first;;
    auto pname = get_name (parent);
    auto all_siblings = parent.children (name.c_str ());
    auto& opp_map = data_iter->second.occurences_per_parent;
    auto piter = opp_map.find (pname);
    auto count = count_siblings (all_siblings.begin (), all_siblings.end ());

    if (piter == opp_map.end ())
    {
      opp_map.insert (StrMap<int>::value_type {pname, count});
      return;
    }

    if (piter->second < count)
    {
      piter->second = count;
    }
  }

  template <typename It>
  std::size_t
  count_siblings (It b, It const& e)
  {
    std::size_t c {};
    while (b != e)
    {
      ++c;
      ++b;
    }
    return c;
  }

  void
  process_leaves (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    auto is_leaf = is_a_leaf (node);

    switch (data_iter->second.leaf)
    {
    case NodeData::Leaf::UNDETERMINED:
      data_iter->second.leaf = (is_leaf ? NodeData::Leaf::ALWAYS_A_LEAF : NodeData::Leaf::NEVER_A_LEAF);
      break;

    case NodeData::Leaf::NEVER_A_LEAF:
      data_iter->second.leaf = (is_leaf ? NodeData::Leaf::SOMETIMES_A_LEAF : NodeData::Leaf::NEVER_A_LEAF);
      break;

    case NodeData::Leaf::SOMETIMES_A_LEAF:
      break;

    case NodeData::Leaf::ALWAYS_A_LEAF:
      data_iter->second.leaf = (is_leaf ? NodeData::Leaf::ALWAYS_A_LEAF : NodeData::Leaf::SOMETIMES_A_LEAF);
      break;
    }
  }

  bool
  is_a_leaf (pugi::xml_node const& node)
  {
    for (auto n : node.children ())
    {
      if (n.type () == pugi::node_element)
      {
        return false;
      }
    }
    return true;
  }

  void
  process_attributes (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    for (auto attr : node.attributes ())
    {
      data_iter->second.attribute_names.insert (attr.name ());
    }
  }

  void
  process_children (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    for (auto n : node.children ())
    {
      if (n.type () == pugi::node_element)
      {
        data_iter->second.child_names.insert (n.name ());
      }
    }
  }

  void
  process_parent (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    data_iter->second.parent_names.insert (get_name (node.parent ()));
  }

  void
  process_text (NodeMap::iterator& data_iter, pugi::xml_node const& node)
  {
    for (auto c : node.children ())
    {
      if (c.type () == pugi::node_pcdata)
      {
        data_iter->second.has_text = true;
      }
    }
  }

  NodeMap nodes;
  std::vector<std::function<void (GirWalker&, NodeMap::iterator&, pugi::xml_node&)>> processors;
};

void
print_tree (GirWalker& walker)
{
  std::vector<std::string> lines;
  auto const& node_map = walker.get_map ();

  lines.reserve (node_map.size ());
  for (auto const& kv : node_map) {
    auto const& name = kv.first;
    auto& data = kv.second;
    std::vector<std::string> children {std::begin (data.child_names), std::end (data.child_names)};
    std::ostringstream oss;

    std::sort (std::begin (children), std::end (children));
    oss << name << " (";
    switch (kv.second.leaf)
    {
    case NodeData::Leaf::UNDETERMINED:
      oss << "meh, this type should not happen";
      break;

    case NodeData::Leaf::NEVER_A_LEAF:
      oss << "never a leaf";
      break;

    case NodeData::Leaf::SOMETIMES_A_LEAF:
      oss << "sometimes a leaf";
      break;

    case NodeData::Leaf::ALWAYS_A_LEAF:
      oss << "always a leaf";
      break;
    }
    if (kv.second.has_text)
    {
      oss << ", has text";
    }
    oss << "):";
    for (auto const& c : children)
    {
      oss << ' ' << c;

      auto c_iter = node_map.find (c);
      if (c_iter == std::end (node_map))
      {
        continue;
      }
      auto opp_iter = c_iter->second.occurences_per_parent.find (name);
      if (opp_iter == std::end (c_iter->second.occurences_per_parent))
      {
        continue;
      }
      auto const oc = opp_iter->second;

      if (oc < 2)
      {
        continue;
      }
      oss << " (" << oc << ')';
    }

    if (!data.attribute_names.empty ())
    {
      std::vector<std::string> attrs {std::begin (data.attribute_names), std::end (data.attribute_names)};

      std::sort (std::begin (attrs), std::end (attrs));
      oss << " |";
      for (auto const& a : attrs)
      {
        oss << ' ' << a;
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

template <typename Pair>
class UnconstifyKey;

template <template <typename, typename> class Pair, typename Key, typename Value>
class UnconstifyKey<Pair<Key, Value>>
{
public:
  using Type = Pair<std::remove_const_t<Key>, Value>;
};

using NodeMapModPair = UnconstifyKey<NodeMap::value_type>::Type;

std::string
remove_loop (std::vector<NodeMapModPair>& elems,
             std::string const& first_keep,
             std::string const& second_remove)
{
  {
    auto remove_iter = std::find_if (std::begin (elems), std::end (elems),
                                     [&second_remove] (auto const &p) -> bool
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
                 [&new_name, &first_keep, &second_remove] (auto& p)
                 {
                   if (p.first == first_keep)
                   {
                     p.first = new_name;
                     p.second.child_names.erase (first_keep);
                     p.second.child_names.erase (second_remove);
                   }
                   else
                   {
                     auto const erased_first = p.second.child_names.erase (first_keep);
                     auto const erased_second = p.second.child_names.erase (second_remove);

                     if (erased_first > 0 || erased_second > 0)
                     {
                       p.second.child_names.insert (new_name);
                     }
                   }
                 });
  return new_name;
}

void
print_leaves_to_root (GirWalker& walker)
{
  auto const& nodes_map = walker.get_map ();
  std::vector<NodeMapModPair> elems {std::begin (nodes_map), std::end (nodes_map)};
  std::vector<std::string> recs {};
  auto level = 0u;

  recs.push_back (remove_loop (elems, "array", "type"));
  recs.push_back (remove_loop (elems, "record", "union"));
  while (!elems.empty ())
  {
    auto half = std::partition (std::begin (elems),
                                std::end (elems),
                                [] (auto const& p) -> bool {
                                  return !p.second.child_names.empty ();
                                });
    if (half == std::end (elems))
    {
      std::cout << "    Leftover nodes:\n";
      std::for_each (std::begin (elems), std::end (elems),
                    [] (auto const& p)
                    {
                      std::cout << "      " << p.first << "\n";
                    });
      break;
    }
    std::sort (half,
               std::end (elems),
               [] (auto const& a, auto const& b) -> bool
               {
                 return a.first < b.first;
               });
    std::for_each (half,
                   std::end (elems),
                   [&elems, &half, &level, &recs] (auto const& p)
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
                     } ();
                     std::cout << name << " " << level << (is_r ? " (recursive)" : "") << std::endl;
                     std::for_each (std::begin (elems),
                                    half,
                                    [&name] (auto& p2)
                                    {
                                      p2.second.child_names.erase (name);
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
  auto const& nodes_map = walker.get_map ();
  StrSet names;

  for (auto const& kv : nodes_map)
  {
    names.insert (kv.first);
    for (auto const& attr : kv.second.attribute_names)
    {
      names.insert (attr);
    }
  }
  lines.reserve (names.size ());
  lines.insert (std::end (lines), std::begin (names), std::end (names));
  std::sort (std::begin (lines), std::end (lines));
  for (auto const& line : lines)
  {
    std::cout << line << "\n";
  }
}

int
main (int argc, char **argv)
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
