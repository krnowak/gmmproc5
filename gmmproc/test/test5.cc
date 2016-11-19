#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::API
{

// common

template <typename... TypeP>
class NotOverridden
{
public:
  constexpr
  NotOverridden (TypeP&&...)
  {}
};

// storage API

class GetStorageType {};

template <typename StorageTagP, typename... TypeP>
constexpr auto
get_storage_type (StorageTagP&& storage_tag, TypeP... type)
{
  return NotOverridden {GetStorageType {}, storage_tag, type...};
}

} // namespace Gmmproc::Xml::Structured::API

namespace Gmmproc::Xml::Structured::Hana
{

class StorageTag
{};

template <typename... TypeP>
class Storage
{
  template <typename IndexP>
  constexpr decltype(auto)
  operator[] (IndexP index)
  {
    return storage[index];
  }

private:
  boost::hana::tuple<TypeP...> storage;
};

template <typename... HanaTypeP>
constexpr auto
get_storage_type (StorageTag, HanaTypeP...)
{
  return boost::hana::type_c<Storage<typename HanaTypeP::type...>>;
}

} // namespace Gmmproc::Xml::Structured::Hana

struct Foo {};

void
f(Foo) {}

int
main ()
{
  namespace Xmls = Gmmproc::Xml::Structured;
  namespace hana = boost::hana;
  using Xmls::API::get_storage_type;

  Xmls::Hana::StorageTag tag;
  hana::type<int> type;
  f (get_storage_type (tag, type));
}
