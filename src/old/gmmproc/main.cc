/* gmmproc5
 *
 * Copyright (C) 2010 Krzesimir Nowak
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

// standard
#include <cstdlib>
#include <iostream>
#include <memory>

// common
#include "dfsmerror.h"
#include "internalerror.h"
#include "syntaxerror.h"

// api
#include "namespace.h"

// defsparser
#include "defsparser.h"

// template
#include "tokenizer.h"

namespace
{

void usage_and_exit ()
{
  std::cerr << "usage: gmmproc --mode MODE --file FILE\n" << "MODE - defs, templates\nFILE - file path\n";
  exit (EXIT_FAILURE);
}

std::string get_val_for_opt (int& iter, int& argc, char**& argv)
{
  if (++iter >= argc)
  {
    usage_and_exit ();
  }
  return argv[iter];
}

std::string get_token_name (Proc::Template::Token::Type type)
{
  switch (type)
  {
    case Proc::Template::Token::TOKEN_WORD:
    {
      return "word";
    }
    case Proc::Template::Token::TOKEN_MULTILINE_COMMENT:
    {
      return "multiline_comment";
    }
    case Proc::Template::Token::TOKEN_SINGLE_LINE_COMMENT:
    {
      return "singleline comment";
    }
    case Proc::Template::Token::TOKEN_DOXYGEN:
    {
      return "doxygen";
    }
    case Proc::Template::Token::TOKEN_MISC:
    {
      return "misc";
    }
    case Proc::Template::Token::TOKEN_DOUBLY_QUOTED_STRING:
    {
      return "doubly-quoted string";
    }
    case Proc::Template::Token::TOKEN_SINGLY_QUOTED_STRING:
    {
      return "singly-linked string";
    }
    case Proc::Template::Token::TOKEN_FUNNY_QUOTED_STRING:
    {
      return "funny-quoted string";
    }
    case Proc::Template::Token::TOKEN_OTHER:
    {
      return "other";
    }
    case Proc::Template::Token::TOKEN_INVALID:
    {
      return "invalid";
    }
  }
  return "unknown";
}

} // namespace

enum MODE
{
  NONE,
  DEFS,
  TEMPLATES
};

int main (int argc, char** argv)
{
  MODE mode (NONE);
  std::string file;

  for (int iter (1); iter < argc; ++iter)
  {
    const std::string opt (argv[iter]);

    if (opt == "--mode" && mode == NONE)
    {
      const std::string val (get_val_for_opt (iter, argc, argv));

      if (val == "defs")
      {
        mode = DEFS;
      }
      else if (val == "templates")
      {
        mode = TEMPLATES;
      }
      else
      {
        usage_and_exit ();
      }
    }
    else if (opt == "--file" && file.empty ())
    {
      file = get_val_for_opt (iter, argc, argv);
    }
    else
    {
      usage_and_exit ();
    }
  }
  if (mode == NONE || file.empty ())
  {
    usage_and_exit ();
  }
  try
  {
    switch (mode)
    {
      case DEFS:
      {
        Proc::Parsers::ApiParsers::DefsParser parser;

        parser.set_file_to_parse (file);
        try
        {
          Proc::Api::NamespacePtr ns (parser.parse());
        }
        catch (Proc::Parsers::SyntaxError& e)
        {
          std::cerr << e.what () << "\n";
          return 1;
        }
        break;
      }
      case TEMPLATES:
      {
        Proc::Template::Tokenizer tokenizer;

        tokenizer.set_file (file);
        try
        {
          Proc::Template::Tokenizer::TokensListPtr tokens (tokenizer.tokenize ());

          if (tokens)
          {
            Proc::Template::Tokenizer::TokensList::const_iterator end (tokens->end ());
            unsigned int token_num (1);

            for (Proc::Template::Tokenizer::TokensList::const_iterator iter (tokens->begin ()); iter != end; ++iter)
            {
              std::cout << "[" << token_num << "] " << iter->get_line () << ": (" << get_token_name (iter->get_type ()) << ")?" << iter->get_contents () << "?\n";
              ++token_num;
            }
          }
        }
        catch (Proc::Common::DFSMError& e)
        {
          std::cerr << e.what () << "\n";
          return 1;
        }
        break;
      }
      default:
      {
        usage_and_exit ();
      }
    }
  }
  catch (Proc::Common::InternalError& e)
  {
    std::cerr << e.what () << "\nPlease make a report of this accident with parsed files attached.\n";
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unexpected exception catched.\n";
    return 1;
  }
  return 0;
}
