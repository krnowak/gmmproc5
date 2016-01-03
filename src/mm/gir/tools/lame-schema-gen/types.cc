#include "types.hh"


namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Attribute::Impl
{
private:
  virtual std::unique_ptr<Impl>
  maybe_create_new (Str const& value) = 0;

  virtual Str
  to_string () const = 0;
};

namespace
{

class UndeterminedType;
class BoolType;
class NumericType;
class StringType;

// prereq: first != last
template <typename Iterator>
bool
is_numeric_range (Iterator first, Iterator const &last)
{
  if (*first == '-')
  {
    ++first;
  }
  if (first == last)
  {
    return false;
  }
  return std::find_if (first, last, [](auto c) { return !std::isdigit (c); }) == last;
}

bool
is_string_bool_value (Str const& value)
{
  if ((value == "t") || (value == "y") || (value == "true") || (value == "yes"))
  {
    return std::make_pair (true, true);
  }
  if ((value == "f") || (value == "n") || (value == "false") || (value == "no"))
  {
    return std::make_pair (true, false);
  }
  return std::make_pair (false, false);
}

std::pair<bool, bool>
is_numeric_bool_value (Str const& value)
{
  if (value == "1")
  {
    return std::make_pair (true, true);
  }
  if (value == "0")
  {
    return std::make_pair (true, false);
  }
  return std::make_pair (false, false);
}

// prereq: value.empty () == false
std::unique_ptr<Attribute::Impl>
get_attribute_type (Str const& value)
{
}

class UndeterminedType : public Attribute::Impl
{
private:
  virtual std::unique_ptr<Impl>
  maybe_create_new (Str const& value)
  {
    if (value.empty ())
    {
      return std::make_unique<BoolType> (true);
    }
    if (is_numeric_range (value.cbegin (), value.cend ()))
    {
      auto const bool_desc = is_numeric_bool_value (value);

      if (bool_desc.first)
      {
        return std::make_unique<BoolType> (bool_desc.second);
      }
      return std::make_unique<NumericType> ();
    }

    auto const bool_desc = is_string_bool_value (value);

    if (bool_desc.first)
    {
      return std::make_unique<BoolType> (bool_desc.second);
    }
    return std::make_unique<StringType> ();
  }

  virtual Str
  to_string () const
  {
    return "undetermined";
  }
};

class BoolType : public Attribute::Impl
{
public:
  BoolType (bool explicit_value)
    : implicit_value {!explicit_value}
  {}

private:
  virtual std::unique_ptr<Impl>
  maybe_create_new (Str const& value)
  {
    if (value.empty ())
    {
      return update_with_value (true);
    }
    if (is_numeric_range (value.cbegin (), value.cend ()))
    {
      auto const bool_desc = is_numeric_bool_value (value);

      if (bool_desc.first)
      {
        return update_with_value (bool_desc.second);
      }
      return std::make_unique<NumericType> ();
    }

    auto const bool_desc = is_string_bool_value (value);

    if (bool_desc.first)
    {
      return update_with_value (bool_desc.second);
    }
    return std::make_unique<StringType> ();
  }

  std::unique_ptr<Impl>
  update_with_value (bool explicit_value)
  {
    if (!mixed_implicit_value && (implicit_value == explicit_value))
    {
      mixed_implicit_value = true;
    }
    return nullptr;
  }

  virtual Str
  to_string () const
  {
    if (mixed_implicit_value)
    {
      return "bool";
    }
    if (implicit_value)
    {
      return "bool-t";
    }
    return "bool-f";
  }

  bool implicit_value;
  bool mixed_implicit_value = false;
};

class NumericType : public Attribute::Impl
{
private:
  virtual std::unique_ptr<Impl>
  maybe_create_new (Str const& value)
  {
    if (value.empty ())
    {
      return std::make_unique<StringType> ();
    }
    if (!is_numeric_range (value.cbegin (), value.cend ()))
    {
      return std::make_unique<StringType> ();
    }
    return nullptr;
  }

  virtual Str
  to_string () const
  {
    return "int";
  }
};

class StringType : public Attribute::Impl
{
private:
  virtual std::unique_ptr<Impl>
  maybe_create_new (Str const&)
  {
    return nullptr;
  }

  virtual Str
  to_string () const
  {
    return "string";
  }
};

} // anonymous namespace

Attribute::Attribute (Str const& name)
  : Named {name},
    impl {std::make_unique<UndeterminedType> ()}
{}

void
Attribute::update (Str const& value)
{
  auto new_type = impl->maybe_create_new (value);

  if (new_type)
  {
    impl.swap (new_type);
  }
}

Str
Attribute::to_string () const
{
  return impl->to_string ();
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
