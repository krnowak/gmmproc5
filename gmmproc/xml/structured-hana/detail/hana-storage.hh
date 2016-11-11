#ifndef GMMPROC_XML_STRUCTURED_DETAIL_HANA_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_HANA_STORAGE_HH

namespace Gmmproc::Xml::Structured::Detail
{

class StorageTag
{};

template <typename TypeP...>
class Storage
{
  template <typename IndexP>
  constexpr decltype(auto)
  operator[] (IndexP&& index)
  {
    return storage[index];
  }

private:
  hana::tuple<TypeP...> storage;
};

template <typename TypeP...>
constexpr auto
get_storage_type (StorageTag, hana::type<TypeP>...)
{
  return hana::type_c<Storage<TypeP...>>;
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_HANA_STORAGE_HH
