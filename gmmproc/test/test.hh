#ifndef GMMPROC_TEST_TEST_HH
#define GMMPROC_TEST_TEST_HH

#include <gmmproc/xml/structured/node.hh>

#include <gmmproc/utils/str.hh>

//class TagOne;
class TagTwo;

//using OneName = Gmmproc::Utils::StaticString<'o', 'n', 'e'>;
using TwoName = Gmmproc::Utils::StaticString<'t', 'w', 'o'>;

namespace Xml = Gmmproc::Xml;
namespace Sxml = Gmmproc::Xml::Structured;

//using Doc = Sxml::Document<TagOne>;
//using One = Sxml::Node<Sxml::Basic::Child<Sxml::Basic::Single, TagTwo>>;
using Doc = Sxml::Document<TagTwo>;
using Two = Sxml::Node<>;

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Registry
{
/*
template<>
class NodeInfo<TagOne>
{
public:
  using Name = OneName;
  using Type = One;
};
*/
template<>
class NodeInfo<TagTwo>
{
public:
  using Name = TwoName;
  using Type = Two;
};

}

}

}

}

#endif // GMMPROC_TEST_TEST_HH
