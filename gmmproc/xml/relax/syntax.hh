#include <boost/spirit/home/x3.hpp>

namespace Krnowak
{

namespace Xml
{

namespace RelaxNG
{

namespace Compact
{

enum class Keyword
  {
    Attribute,
    Default,
    DataTypes,
    Div,
    Element,
    Empty,
    External,
    Grammar,
    Include,
    Inherit,
    List,
    Mixed,
    Namespace,
    NotAllowed,
    Parent,
    Start,
    String,
    Text,
    Token
  };

auto const char_ = x3::char_;
auto const nc_name = x3::char_ ("a-zA-Z0-9")

struct Keywords_ : public x3::symbols<Keyword>
{
  Keywords_()
  {
    add ("attribute", Keywords::Attribute,
         "default", Keywords::Default,
         "datatypes", Keywords::DataTypes,
         "div", Keywords::Div,
         "element", Keywords::Element,
         "empty", Keywords::Empty,
         "external", Keywords::External,
         "grammar", Keywords::Grammar,
         "include", Keywords::Include,
         "inherit", Keywords::Inherit,
         "list", Keywords::List,
         "mixed", Keywords::Mixed,
         "namespace", Keywords::Namespace,
         "notAllowed", Keywords::NotAllowed,
         "parent", Keywords::Parent,
         "start", Keywords::Start,
         "string", Keywords::String,
         "text", Keywords::Text,
         "token", Keywords::Token);
  }
};

const Keywords_ keyword;

namespace x3 = boost::spirit::x3;

auto const newline = x3::lit('\n') | '\r' | "\r\n";

auto const literal_segment =
  (x3::lit('"') >> *(char_ - ('"' | newline)) >> '"') |
  (x3::lit('\'') >> *(char_ - ('\'' | newline)) >> '\'') |
  (x3::lit("\"\"\"") >> *(-x3::lit('"') >> -x3::lit('"') >> char_ - ('"' | newline)) >> "\"\"\"") |
  (x3::lit("'''") >> *(-x3::lit('\'') >> -x3::lit('\'') >> char_ - ('\'' | newline)) >> "'''");

auto const literal = literal_segment >> +("~" >> literal_segment);

auto const any_name = x3::lit('*');

auto const ns_name = nc_name >> ":*";

auto const c_name = nc_name >> ":" >> nc_name;

auto const quoted_identifier = x3::lit('\\') >> nc_name;

auto const identifier = (nc_name - keyword) | quoted_identifier;

auto const identifier_or_keyword = idenfier | keyword;

auto const inherit = x3::lit("inherit") >> "=" >> identifier_or_keyword;

auto const namespace_uri_literal = literal | x3::lit("inherit");

auto const any_uri_literal = literal;

auto const data_type_value = literal;

auto const data_type_name = literal;

auto const name = identifier_or_keyword | c_name;

auto const except_name_class = x3::lit('-') >> name_class;

} // namespace Compact

} // namespace RelaxNG

} // namespace Xml

} // namespace Krnowak
