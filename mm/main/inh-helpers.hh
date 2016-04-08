#pragma once

struct Stub
{
  struct Yes;
  struct No;
};

class Docable : public Stub
{
  class AbsenceChecker : public Checker
  {
    AbsenceChecker()
      : Checker{std::vector<std::string>{}, std::vector<std::string>{"doc", "doc-deprecated"}}
    {}
  };

  std::string doc;
  std::string doc_deprecated;
};

class CTypable
{
  class AbsenceChecker : public Checker
  {
    AbsenceChecker()
      : Checker{std::vector<std::string>{"c:type"}, std::vector<std::string>{}}
    {}
  };

  std::string c_type;
};

// subclass

template <typename T>
class SubClass;

template <typename T>
class SubClass<typename T::Yes> : public T
{};

template <typename T>
class SubClass<typename T::No> : public typename T::AbsenceChecker
{};

// type list

template <typename Types...>
class TypeList;

class EmptyTypeList;

using AllTypes = TypeList<Docable>;

// prepend

template <typename List, typename T>
class Prepend;

template <typename T, typename List...>
class Prepend<TypeList<List...>, T>
{
public:
  using Type = TypeList<T, List...>;
};

template <typename T>
class Prepend<EmptyTypeList, T>
{
public:
  using Type = Typelist<T>;
};

// convert one

typename <bool yes, typename T>
class ConvertOne;

template <typename T>
class ConvertOne<true, T>
{
  using Type = typename T::Yes;
};

template <typename T>
class ConvertOne<false, T>
{
  using Type = typename T::No;
};

// in list

template <typename T, typename List>
class IsInList;

template <typename T, typename L, typename Types...>
class IsInList<T, TypeList<L, Types...>>
{
public:
  using Type = std::conditional<std::is_same<T, L>::value,
                                std::true_type,
                                IsInList<T, TypeList<Types...>>::type;
  static constexpr bool Value = Type::value;
};

template <typename T, typename L>
class IsInList<T, TypeList<L>>
{
public:
  using Type = std::conditional<std::is_same<T, L>::value,
                                std::true_type,
                                std::false_type>::type;
  static constexpr bool Value = Type::value;
};

// filter generic

template <typename Words, typename List, bool in>
class FilterGeneric;

template <typename List, typename Word, bool in, typename Words...>
class FilterGeneric<TypeList<Word, Words...>, List, in>
{
private:
  using InType = typename IsInList<Word, List>::Type;
  using TmpList = Filter<TypeList<Words...>, List, in>::Type;
public:
  using Type = std::conditional<InType::value == in,
                                typename Prepend<TmpList, Word>::Type,
                                TmpList>::type;
};

template <typename List, typename Word, bool in>
class FilterGeneric<TypeList<Word>, List, in>
{
private:
  using InType = typename IsInList<Word, List>::Type;
public:
  using Type = std::conditional<InType::value == in,
                                TypeList<Word>,
                                EmptyList>::type;
};

// filter

template <typename Words, typename List>
class Filter
{
public:
  using Type = typename FilterGeneric<Words, List, true>::Type;
};

// filter out

template <typename Words, typename List>
class FilterOut
{
public:
  using Type = typename FilterGeneric<Words, List, false>::Type;
};

// yesnoclass

template <bool yes, typename List>
class YesNoClass;

template <bool yes, typename Types...>
class YesNoClass<yes, TypeList<Types...>> : public SubClass<ConvertOne<yes, Types>::Type>::Type...
{};

template <bool yes>
class YesNoClass<yes, EmptyTypeList>
{};

// yes class

template <typename List>
using YesClass = YesNoClass<true, List>;

// no class

template <typename List>
using NoClass = YesNoClass<false, List>;

// convert

template <typename Types>
class Convert : public YesClass<Filter<AllTypes, Types>::Type>, public NoClass<FilterOut<AllTypes, Types>::Type>
{

};

// common

template <typename Types...>
class Common : public Convert<TypeList<Types...>>
{};

template<>
class Common<>
{};
