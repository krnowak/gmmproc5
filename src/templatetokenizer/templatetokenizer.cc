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
#include <sstream>

// common
#include "simpledfsm.h"
#include "utilities.h"

// template tokenizer
#include "templatetokenizer.h"

namespace Proc
{

struct TemplateTokenizer::TemplateTokenizerImpl
{
  enum States
  {
    STATE_START,
    STATE_SLASH,
    STATE_SINGLE_LINE_COMMENT,
    STATE_MULTI_LINE_ANY,
    STATE_DOXYGEN_MAYBE,
    STATE_DOXYGEN,
    STATE_DOXY_STAR,
    STATE_MULTI_LINE_COMMENT,
    STATE_MC_STAR,
    STATE_WORD,
    STATE_DQ_STRING,
    STATE_DQ_ESCAPE,
    STATE_DQ_IGNORE,
    STATE_SQ_STRING,
    STATE_SQ_ESCAPE,
    STATE_SQ_IGNORE,
    STATE_FQ_STRING,
    STATE_FQ_ESCAPE,
    STATE_FQ_IGNORE,
    STATE_OTHER
  };
  enum Inputs
  {
    INPUT_SLASH,
    INPUT_STAR,
    INPUT_NEWLINE,
    INPUT_DQ,
    INPUT_SQ,
    INPUT_BACKTICK,
    INPUT_ESCAPE,
    INPUT_WORD,
    INPUT_MISC,
    INPUT_OTHER
  };
  typedef Proc::Common::SimpleDFSM<States, Inputs, std::string> GDFSM;
  struct Translator
  {
    TemplateTokenizer::TemplateTokenizerImpl::Inputs operator() (const std::string::const_iterator& iter) const;
  };
  struct EntryFunction
  {
    EntryFunction (TemplateTokenizer::TemplateTokenizerImpl& owner);

    void operator() (const States& state, const Inputs& input, const std::string::const_iterator& input_char);

    TemplateTokenizer::TemplateTokenizerImpl& m_owner;
  };
  struct ExitFunction
  {
    ExitFunction (TemplateTokenizer::TemplateTokenizerImpl& owner);

    void operator() (const States& state, const Inputs& input, const std::string::const_iterator& input_char);

    TemplateTokenizer::TemplateTokenizerImpl& m_owner;
  };

  TemplateTokenizerImpl ();

  TemplateTokenizer::TokensListPtr m_tokens;
  GDFSM m_dfsm;
};

TemplateTokenizer::TemplateTokenizerImpl::TemplateTokenizerImpl ()
: m_tokens (new TemplateTokenizer::TokensList),
  m_dfsm ()
{
  m_dfsm.add_state (STATE_START);
  m_dfsm.add_state (STATE_SLASH);
  m_dfsm.add_state (STATE_SINGLE_LINE_COMMENT);
  m_dfsm.add_state (STATE_MULTI_LINE_ANY);
  m_dfsm.add_state (STATE_DOXYGEN_MAYBE);
  m_dfsm.add_state (STATE_DOXYGEN);
  m_dfsm.add_state (STATE_DOXY_STAR);
  m_dfsm.add_state (STATE_MULTI_LINE_COMMENT);
  m_dfsm.add_state (STATE_MC_STAR);
  m_dfsm.add_state (STATE_WORD);
  m_dfsm.add_state (STATE_DQ_STRING);
  m_dfsm.add_state (STATE_DQ_ESCAPE);
  m_dfsm.add_state (STATE_SQ_STRING);
  m_dfsm.add_state (STATE_SQ_ESCAPE);
  m_dfsm.add_state (STATE_FQ_STRING);
  m_dfsm.add_state (STATE_FQ_ESCAPE);
  m_dfsm.add_state (STATE_OTHER);

  m_dfsm.add_input (INPUT_SLASH);
  m_dfsm.add_input (INPUT_STAR);
  m_dfsm.add_input (INPUT_NEWLINE);
  m_dfsm.add_input (INPUT_DQ);
  m_dfsm.add_input (INPUT_SQ);
  m_dfsm.add_input (INPUT_BACKTICK);
  m_dfsm.add_input (INPUT_ESCAPE);
  m_dfsm.add_input (INPUT_WORD);
  m_dfsm.add_input (INPUT_MISC);
  m_dfsm.add_input (INPUT_OTHER);

  m_dfsm.set_default_next_state_for (STATE_START, STATE_OTHER);
  m_dfsm.set_route (STATE_START, STATE_SLASH, INPUT_SLASH);
  m_dfsm.set_route (STATE_START, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_START, STATE_DQ_STRING, INPUT_DQ);
  m_dfsm.set_route (STATE_START, STATE_SQ_STRING, INPUT_SQ);
  m_dfsm.set_route (STATE_START, STATE_FQ_STRING, INPUT_BACKTICK);
  m_dfsm.set_route (STATE_START, STATE_WORD, INPUT_WORD);
  m_dfsm.set_route (STATE_START, STATE_START, INPUT_MISC);

  m_dfsm.set_default_next_state_for (STATE_SLASH, STATE_OTHER);
  m_dfsm.set_route (STATE_SLASH, STATE_SINGLE_LINE_COMMENT, INPUT_SLASH);
  m_dfsm.set_route (STATE_SLASH, STATE_MULTI_LINE_ANY, INPUT_STAR);
  m_dfsm.set_route (STATE_SLASH, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_SLASH, STATE_DQ_STRING, INPUT_DQ);
  m_dfsm.set_route (STATE_SLASH, STATE_SQ_STRING, INPUT_SQ);
  m_dfsm.set_route (STATE_SLASH, STATE_FQ_STRING, INPUT_BACKTICK);
  m_dfsm.set_route (STATE_SLASH, STATE_WORD, INPUT_WORD);
  m_dfsm.set_route (STATE_SLASH, STATE_START, INPUT_MISC);

  m_dfsm.set_default_next_state_for (STATE_SINGLE_LINE_COMMENT, STATE_SINGLE_LINE_COMMENT);
  m_dfsm.set_route (STATE_SINGLE_LINE_COMMENT, STATE_START, INPUT_NEWLINE);

  m_dfsm.set_default_next_state_for (STATE_MULTI_LINE_ANY, STATE_MULTI_LINE_COMMENT);
  m_dfsm.set_route (STATE_MULTI_LINE_ANY, STATE_DOXYGEN_MAYBE, INPUT_STAR);

  m_dfsm.set_default_next_state_for (STATE_MULTI_LINE_COMMENT, STATE_MULTI_LINE_COMMENT);
  m_dfsm.set_route (STATE_MULTI_LINE_COMMENT, STATE_MC_STAR, INPUT_STAR);

  m_dfsm.set_default_next_state_for (STATE_MC_STAR, STATE_MULTI_LINE_COMMENT);
  m_dfsm.set_route (STATE_MC_STAR, STATE_START, INPUT_SLASH);
  m_dfsm.set_route (STATE_MC_STAR, STATE_MC_STAR, INPUT_STAR);

  m_dfsm.set_default_next_state_for (STATE_DOXYGEN_MAYBE, STATE_DOXYGEN);
  m_dfsm.set_route (STATE_DOXYGEN_MAYBE, STATE_START, INPUT_SLASH);

  m_dfsm.set_default_next_state_for (STATE_DOXYGEN, STATE_DOXYGEN);
  m_dfsm.set_route (STATE_DOXYGEN, STATE_DOXY_STAR, INPUT_STAR);

  m_dfsm.set_default_next_state_for (STATE_DOXY_STAR, STATE_DOXYGEN);
  m_dfsm.set_route (STATE_DOXY_STAR, STATE_START, INPUT_SLASH);
  m_dfsm.set_route (STATE_DOXY_STAR, STATE_DOXY_STAR, INPUT_STAR);

  m_dfsm.set_default_next_state_for (STATE_OTHER, STATE_OTHER);
  m_dfsm.set_route (STATE_OTHER, STATE_SLASH, INPUT_SLASH);
  m_dfsm.set_route (STATE_OTHER, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_OTHER, STATE_DQ_STRING, INPUT_DQ);
  m_dfsm.set_route (STATE_OTHER, STATE_SQ_STRING, INPUT_SQ);
  m_dfsm.set_route (STATE_OTHER, STATE_FQ_STRING, INPUT_BACKTICK);
  m_dfsm.set_route (STATE_OTHER, STATE_WORD, INPUT_WORD);
  m_dfsm.set_route (STATE_OTHER, STATE_START, INPUT_MISC);

  m_dfsm.set_default_next_state_for (STATE_WORD, STATE_OTHER);
  m_dfsm.set_route (STATE_WORD, STATE_SLASH, INPUT_SLASH);
  m_dfsm.set_route (STATE_WORD, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_WORD, STATE_DQ_STRING, INPUT_DQ);
  m_dfsm.set_route (STATE_WORD, STATE_SQ_STRING, INPUT_SQ);
  m_dfsm.set_route (STATE_WORD, STATE_FQ_STRING, INPUT_BACKTICK);
  m_dfsm.set_route (STATE_WORD, STATE_WORD, INPUT_WORD);
  m_dfsm.set_route (STATE_WORD, STATE_START, INPUT_MISC);

  m_dfsm.set_default_next_state_for (STATE_DQ_STRING, STATE_DQ_STRING);
  m_dfsm.set_route (STATE_DQ_STRING, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_DQ_STRING, STATE_START, INPUT_DQ);
  m_dfsm.set_route (STATE_DQ_STRING, STATE_DQ_ESCAPE, INPUT_ESCAPE);

  m_dfsm.set_default_next_state_for (STATE_DQ_ESCAPE, STATE_DQ_STRING);

  m_dfsm.set_default_next_state_for (STATE_SQ_STRING, STATE_SQ_STRING);
  m_dfsm.set_route (STATE_SQ_STRING, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_SQ_STRING, STATE_START, INPUT_SQ);
  m_dfsm.set_route (STATE_SQ_STRING, STATE_SQ_ESCAPE, INPUT_ESCAPE);

  m_dfsm.set_default_next_state_for (STATE_SQ_ESCAPE, STATE_SQ_STRING);

  m_dfsm.set_default_next_state_for (STATE_FQ_STRING, STATE_FQ_STRING);
  m_dfsm.set_route (STATE_FQ_STRING, STATE_START, INPUT_NEWLINE);
  m_dfsm.set_route (STATE_FQ_STRING, STATE_START, INPUT_SQ);
  m_dfsm.set_route (STATE_FQ_STRING, STATE_FQ_ESCAPE, INPUT_ESCAPE);

  m_dfsm.set_default_next_state_for (STATE_FQ_ESCAPE, STATE_FQ_STRING);

  m_dfsm.add_accept_state (STATE_START);
  m_dfsm.add_accept_state (STATE_OTHER);
  m_dfsm.add_accept_state (STATE_WORD);

  m_dfsm.set_start_state (STATE_START);

  m_dfsm.set_input_translator (Translator ());

  m_dfsm.set_entry_callback (EntryFunction (*this));
  m_dfsm.set_exit_callback (ExitFunction (*this));
}

TemplateTokenizer::TemplateTokenizerImpl::Inputs TemplateTokenizer::TemplateTokenizerImpl::Translator::operator() (const std::string::const_iterator& iter) const
{
  const char c (*iter);

  switch (c)
  {
    case '/':
    {
      return INPUT_SLASH;
    }
    case '*':
    {
      return INPUT_STAR;
    }
    case '\n':
    {
      return INPUT_NEWLINE;
    }
    case '"':
    {
      return INPUT_DQ;
    }
    case '\'':
    {
      return INPUT_SQ;
    }
    case '`':
    {
      return INPUT_BACKTICK;
    }
    case '\\':
    {
      return INPUT_ESCAPE;
    }
    case '{':
    case '}':
    case '(':
    case ')':
    case ';':
    case ',':
    {
      return INPUT_MISC;
    }
    default:
    {
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '#'))
      {
        return INPUT_WORD;
      }
      return INPUT_OTHER;
    }
  }
}

TemplateTokenizer::TemplateTokenizerImpl::EntryFunction::EntryFunction (TemplateTokenizer::TemplateTokenizerImpl& owner)
: m_owner (owner)
{}

void TemplateTokenizer::TemplateTokenizerImpl::EntryFunction::operator() (const States& /*state*/, const Inputs& /*input*/, const std::string::const_iterator& /*input_char*/)
{
  
}

TemplateTokenizer::TemplateTokenizerImpl::ExitFunction::ExitFunction (TemplateTokenizer::TemplateTokenizerImpl& owner)
: m_owner (owner)
{}

void TemplateTokenizer::TemplateTokenizerImpl::ExitFunction::operator() (const States& /*state*/, const Inputs& /*input*/, const std::string::const_iterator& /*input_char*/)
{
  
}

TemplateTokenizer::TemplateTokenizer()
: m_pimpl (new TemplateTokenizerImpl)
{}

TemplateTokenizer::~TemplateTokenizer()
{}

void TemplateTokenizer::set_file (const std::string& file_path)
{
  set_contents (Common::Utilities::read_contents (file_path));
}

void TemplateTokenizer::set_contents (const std::string& contents)
{
  m_pimpl->m_dfsm.load_string (contents);
}

TemplateTokenizer::TokensListPtr TemplateTokenizer::tokenize ()
{
  if (!m_pimpl->m_tokens->empty ())
  {
    TokensListPtr empty (new TemplateTokenizer::TokensList);

    m_pimpl->m_tokens.swap (empty);
  }
  m_pimpl->m_dfsm.process_whole ();
  if (!m_pimpl->m_dfsm.is_current_state_accepting ())
  {
    // throw something
  }

  TokensListPtr tokens;

  m_pimpl->m_tokens.swap (tokens);
  return tokens;
}

void TemplateTokenizer::reset ()
{
  {
    TemplateTokenizer::TokensListPtr empty;

    m_pimpl->m_tokens.swap (empty);
  }
  m_pimpl->m_dfsm.reset ();
}

} // namespace Proc
