#include <mm/gir/tools/lame-schema-gen/schema-data.hh>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace Mm::Gir::Tools::LameSchemaGen;

void
print_sss (Str const& prefix, StrSortedSet const& sss)
{
  std::cout << prefix;
  for (auto const& s : sss)
  {
    std::cout << " " << s;
  }
  std::cout << "\n";
}

StrSortedSet
get_diff (StrSortedSet const& first, StrSortedSet const& second)
{
  StrSortedSet output;

  print_sss ("first:", first);
  print_sss ("second:", second);
  std::set_difference (first.cbegin (), first.cend (),
                       second.cbegin (), second.cend (),
                       std::inserter (output, output.begin ()));
  print_sss ("diff:", output);
  return output;
}

void
process_sets (StrSortedSet&& fresh_set, ShortNode::Sets& sets)
{
  auto iter = sets.begin ();

  for (; iter != sets.cend (); ++iter)
  {
    auto& set = *iter;
    std::cout << "diffing fresh and set\n";
    StrSortedSet only_in_fresh = get_diff (fresh_set, set);

    if (only_in_fresh.empty ())
    {
      std::cout << "nothing new in fresh\n";
      // fresh set has nothing new, nothing to do
      return;
    }

    std::cout << "diffing set and fresh\n";
    StrSortedSet only_in_set = get_diff (set, fresh_set);
    if (only_in_set.empty ())
    {
      std::cout << "nothing new in set, updating it\n";
      // fresh has some new elements, update the set
      set.insert (only_in_fresh.cbegin (), only_in_fresh.cend ());
      print_sss ("updated set:", set);
      break;
    }
    // fresh set and set have exclusive elements, try another set
    std::cout << "trying another set\n";
  }
  if (iter != sets.cend ())
  {
    std::cout << "doing sets cleanup after update\n";
    std::cout << "sets size: " << sets.size() << "\n";
    auto const& updated_set = *iter;
    sets.erase (std::remove_if (sets.begin (), sets.end (),
                                [&updated_set](auto const& set)
                                {
                                  print_sss ("got set:", set);
                                  if (&updated_set == &set)
                                  {
                                    std::cout << "skipping the updated set in cleanup\n";
                                    return false;
                                  }
                                  std::cout << "diffing set and updated set\n";
                                  auto const empty = get_diff (set, updated_set).empty ();
                                  std::cout << "removing: " << empty << "\n";
                                  return empty;
                                }), sets.end ());
    std::cout << "sets size: " << sets.size() << "\n";
    return;
  }
  else
  {
    std::cout << "add fresh as an another set\n";
    sets.push_back (std::move (fresh_set));
  }
}

int
main (int argc, char** argv)
{
  std::ifstream f;
  auto& stream = [argc, argv, &f]() -> std::istream&
    {
      if (argc < 2)
      {
        return std::cin;
      }
      f.open (argv[1]);
      return f;
    }();
  Str str_add;
  ShortNode::Sets sets;

  while (std::getline (stream, str_add))
  {
    StrSortedSet add;
    for (auto c : str_add)
    {
      add.emplace (1, c);
    }
    print_sss ("adding:", add);
    process_sets(std::move (add), sets);
    std::cout << "  sets:\n";
    for (auto const& set : sets)
    {
      print_sss ("    set:", set);
    }
  }
}
