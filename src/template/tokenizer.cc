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

// template
#include "tokenizer.h"

namespace Proc
{

namespace Template
{

struct Tokenizer::TokenizerImpl
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
    STATE_SQ_STRING,
    STATE_SQ_ESCAPE,
    STATE_FQ_STRING,
    STATE_FQ_ESCAPE,
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
  class Translator
  {
  public:
    Tokenizer::TokenizerImpl::Inputs operator() (const std::string::value_type& c) const;
  };
  class EntryFunction
  {
  public:
    EntryFunction (Tokenizer::TokenizerImpl& owner);

    void operator() (const Tokenizer::TokenizerImpl::GDFSM& dfsm);

  private:
    void handle_word_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm);
    void handle_other_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm);
    void handle_string_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm, States string_state);
    void handle_slash_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm);
    void handle_comment_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm);
    void handle_start_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm);

    void set_begin_to_current ();

    Tokenizer::TokenizerImpl& m_owner;
  };
  class ExitFunction
  {
  public:
    ExitFunction (Tokenizer::TokenizerImpl& owner);

    void operator() (const Tokenizer::TokenizerImpl::GDFSM& dfsm);

  private:
    Tokenizer::TokenizerImpl& m_owner;
  };

  TokenizerImpl ();

  Tokenizer::TokensListPtr m_tokens;
  std::string::const_iterator m_token_begin;
  std::string::const_iterator m_maybe_token_begin;
  unsigned int m_current_line;
  unsigned int m_token_begin_line;
  States m_previous_state;
  GDFSM m_dfsm;
};

Tokenizer::TokenizerImpl::TokenizerImpl ()
: m_tokens (new Tokenizer::TokensList),
  m_token_begin (),
  m_current_line (1),
  m_token_begin_line (1),
  m_previous_state (STATE_START),
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

Tokenizer::TokenizerImpl::Inputs Tokenizer::TokenizerImpl::Translator::operator() (const std::string::value_type& c) const
{
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

Tokenizer::TokenizerImpl::EntryFunction::EntryFunction (Tokenizer::TokenizerImpl& owner)
: m_owner (owner)
{}

void Tokenizer::TokenizerImpl::EntryFunction::operator() (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  States state (dfsm.get_current_state ());

  switch (state)
  {
    case STATE_SLASH:
    {
      handle_slash_state (dfsm);
      break;
    }
    case STATE_START:
    {
      handle_start_state (dfsm);
      break;
    }
    case STATE_WORD:
    {
      handle_word_state (dfsm);
      break;
    }
    case STATE_OTHER:
    {
      handle_other_state (dfsm);
      break;
    }
    case STATE_DQ_STRING:
    case STATE_SQ_STRING:
    case STATE_FQ_STRING:
    {
      handle_string_state (dfsm, state);
      break;
    }
    default:
    {
      break;
    }
  }
  if (dfsm.get_current_input () == INPUT_NEWLINE)
  {
    ++m_owner.m_current_line;
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_word_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  switch (m_owner.m_previous_state)
  {
    case STATE_SLASH:
    case STATE_OTHER:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_OTHER, m_owner.m_token_begin_line, std::string(m_owner.m_token_begin, dfsm.get_current_raw_string_iterator ())));
      set_begin_to_current ();
      break;
    }
    case STATE_START:
    {
      set_begin_to_current ();
      break;
    }
    case STATE_WORD:
    {
      // nothing
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_other_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  switch (m_owner.m_previous_state)
  {
    case STATE_WORD:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_WORD, m_owner.m_token_begin_line, std::string(m_owner.m_token_begin, dfsm.get_current_raw_string_iterator ())));
      set_begin_to_current ();
      break;
    }
    case STATE_START:
    {
      set_begin_to_current ();
      break;
    }
    case STATE_OTHER:
    case STATE_SLASH:
    {
      // nothing
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_string_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm, States string_state)
{
  const std::string::const_iterator current (dfsm.get_current_raw_string_iterator ());

  switch (m_owner.m_previous_state)
  {
    case STATE_SLASH:
    case STATE_OTHER:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_OTHER, m_owner.m_token_begin_line, std::string(m_owner.m_token_begin, current)));
      set_begin_to_current ();
      break;
    }
    case STATE_WORD:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_WORD, m_owner.m_token_begin_line, std::string(m_owner.m_token_begin, current)));
      set_begin_to_current ();
      break;
    }
    case STATE_START:
    {
      set_begin_to_current ();
      break;
    }
    case STATE_DQ_STRING:
    case STATE_DQ_ESCAPE:
    {
      if (string_state != STATE_DQ_STRING)
      {
        // shouldn't happen
        // throw something.
      }
      break;
    }
    case STATE_SQ_STRING:
    case STATE_SQ_ESCAPE:
    {
      if (string_state != STATE_SQ_STRING)
      {
        // shouldn't happen
        // throw something.
      }
      break;
    }
    case STATE_FQ_STRING:
    case STATE_FQ_ESCAPE:
    {
      if (string_state != STATE_FQ_STRING)
      {
        // shouldn't happen
        // throw something.
      }
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_slash_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  const std::string::const_iterator current (dfsm.get_current_raw_string_iterator ());

  switch (m_owner.m_previous_state)
  {
    case STATE_WORD:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_WORD, m_owner.m_token_begin_line, std::string(m_owner.m_token_begin, current)));
      set_begin_to_current ();
      break;
    }
    case STATE_START:
    {
      set_begin_to_current ();
      break;
    }
    case STATE_OTHER:
    {
      m_owner.m_maybe_token_begin = current;
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_comment_state (const Tokenizer::TokenizerImpl::GDFSM& /* dfsm */)
{
  switch (m_owner.m_previous_state)
  {
    case STATE_SLASH:
    {
      m_owner.m_token_begin = m_owner.m_maybe_token_begin;
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::handle_start_state (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  const std::string::const_iterator current (dfsm.get_current_raw_string_iterator ());
  bool check_current_input (false);

  switch (m_owner.m_previous_state)
  {
    case STATE_SINGLE_LINE_COMMENT:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_SINGLE_LINE_COMMENT, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_MC_STAR:
    case STATE_DOXYGEN_MAYBE:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_MULTILINE_COMMENT, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_DOXY_STAR:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_DOXYGEN, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_DQ_STRING:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_DOUBLY_QUOTED_STRING, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_SQ_STRING:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_SINGLY_QUOTED_STRING, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_FQ_STRING:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_FUNNY_QUOTED_STRING, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current + 1)));
      break;
    }
    case STATE_WORD:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_WORD, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current)));
      check_current_input = true;
      break;
    }
    case STATE_OTHER:
    {
      m_owner.m_tokens->push_back (Token (Token::TOKEN_OTHER, m_owner.m_token_begin_line, std::string (m_owner.m_token_begin, current)));
      check_current_input = true;
      break;
    }
    case STATE_START:
    {
      check_current_input = true;
      break;
    }
    default:
    {
      // should not happen.
      // throw something.
    }
  }
  if (check_current_input)
  {
    switch (dfsm.get_current_input ())
    {
      case INPUT_NEWLINE:
      case INPUT_MISC:
      {
        m_owner.m_tokens->push_back (Token (Token::TOKEN_MISC, m_owner.m_current_line, std::string (1, *(dfsm.get_current_raw_string_iterator ()))));
        break;
      }
      default:
      {
        // should not happen.
        // throw something.
      }
    }
  }
}

void Tokenizer::TokenizerImpl::EntryFunction::set_begin_to_current ()
{
  m_owner.m_token_begin = m_owner.m_dfsm.get_current_raw_string_iterator ();
  m_owner.m_token_begin_line = m_owner.m_current_line;
}

Tokenizer::TokenizerImpl::ExitFunction::ExitFunction (Tokenizer::TokenizerImpl& owner)
: m_owner (owner)
{}

void Tokenizer::TokenizerImpl::ExitFunction::operator() (const Tokenizer::TokenizerImpl::GDFSM& dfsm)
{
  m_owner.m_previous_state = dfsm.get_current_state ();
}

Tokenizer::Tokenizer()
: m_pimpl (new TokenizerImpl)
{}

Tokenizer::~Tokenizer()
{}

void Tokenizer::set_file (const std::string& file_path)
{
  set_contents (Common::Utilities::read_contents (file_path));
}

void Tokenizer::set_contents (const std::string& contents)
{
  m_pimpl->m_dfsm.load_raw_string (contents);
}

Tokenizer::TokensListPtr Tokenizer::tokenize ()
{
  if (!m_pimpl->m_tokens->empty ())
  {
    TokensListPtr empty (new Tokenizer::TokensList);

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

void Tokenizer::reset ()
{
  {
    Tokenizer::TokensListPtr empty;

    m_pimpl->m_tokens.swap (empty);
  }
  m_pimpl->m_dfsm.reset ();
}

} // namespace Template

} // namespace Proc
