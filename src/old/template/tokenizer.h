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

#ifndef PROC_TEMPLATE_TOKENIZER_H
#define PROC_TEMPLATE_TOKENIZER_H

// standard
#include <list>
#include <memory>
#include <string>

// template
#include "token.h"

namespace Proc
{

namespace Template
{

class Tokenizer
{
public:
  typedef std::list<Token> TokensList;
  typedef std::shared_ptr<TokensList> TokensListPtr;

  Tokenizer ();
  ~Tokenizer ();

  void set_file (const std::string& file_path);
  void set_contents (const std::string& contents);

  TokensListPtr tokenize ();

  void reset ();
private:
  struct TokenizerImpl;
  std::shared_ptr<TokenizerImpl> m_pimpl;
};

} // namespace Template

} // namespace Proc

#endif // PROC_TEMPLATE_TOKENIZER_H
