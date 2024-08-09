#include <exception>
#include <iterator>
#include <sstream>
#include <string>

#include "newick.hpp"

/**
 * NEWICK LEXER
 */

spearfish_lexeme_t spearfish_newick_lexer_t::consume()
{
    auto token{peak()};
    if (token == VALUE)
    {
      // we have a value, so we need to scan until we have found punctuation, or
      // the end of the string
      auto start_itr{_input.begin() + _current_index};
      auto end_itr{start_itr};
      while (char tmp = _input[_current_index])
      {
          if (is_punct(tmp))
          {
            break;
          }
          end_itr++;
          _current_index++;
      }

      _value = std::string{start_itr, end_itr};
      while (std::isspace(*(_value.end() - 1)))
      {
          _value.resize(_value.size() - 1);
      }
      return token;
    } else
    {
        _current_index++;
        skip_whitespace();
        return token;
    }
}

std::string spearfish_newick_lexer_t::consume_value_as_string()
{
    std::string tmp;
    std::swap(tmp, _value);
    return tmp;
}

double spearfish_newick_lexer_t::consume_value_as_double()
{
    auto d_str{consume_value_as_string()};
    size_t pos{0};
    double val{std::stod(d_str, &pos)};
    if (pos != d_str.size())
    {
        throw std::runtime_error{std::string("Double conversion failed around")
                                + describe_position()};
    }
    return val;
}

// TODO peak, is_punc very similar, do only one switch / if?
spearfish_lexeme_t spearfish_newick_lexer_t::peak()
{
    size_t tmp_index{_current_index};
    char current_char{_input[tmp_index++]};
    if (is_punct(current_char))
    {
        switch (current_char)
        {
            case '[':
                return OPENING_SQUARE_BRACKET;
            case ']':
                return CLOSING_SQUARE_BRACKET;
            case '(':
                return OPENING_PARENTHESIS;
            case ')':
                return CLOSING_PARENTHESIS;
            case ':':
                return COLON;
            case ';':
                return SEMICOLON;
            case ',':
                return COMMA;
            case 0:
            case EOF:
                return END;
            default:
                throw std::runtime_error{"The punctuation was unrecognized"};
        }
    }
    else
    {
        return VALUE;
    }
}

void spearfish_newick_lexer_t::expect(spearfish_lexeme_t token_type)
{
    auto ret = consume_token_pos();
    if (ret.first != token_type)
    {
      throw std::runtime_error{
          std::string("Got the wrong token type at position ")
          + std::to_string(ret.second + 1) + ". Was expecting "
          + describe_token(token_type) + " but got "
          + describe_token(ret.first)};
    }
}

bool spearfish_newick_lexer_t::is_punct(char c)
{
    return c == '[' || c == ']' || c == '(' || c == ')' || c == ':' || c == ';'
          || c == ',' || c == 0 || c == EOF;
}

std::pair<spearfish_lexeme_t, size_t> spearfish_newick_lexer_t::consume_token_pos()
{
    auto start_index{_current_index};
    auto token{consume()};
    return {token, start_index};
}

void spearfish_newick_lexer_t::skip_whitespace()
{
  while (_current_index < _input.size())
  {
    if (!std::isspace(_input[_current_index]))
    {
        break;
    }
    _current_index++;
  }
}

std::string spearfish_newick_lexer_t::describe_token(spearfish_lexeme_t token_type)
{
  switch (token_type)
  {
    case OPENING_SQUARE_BRACKET:
        return {"opening square bracket"};
    case CLOSING_SQUARE_BRACKET:
        return {"closing square bracket"};
    case OPENING_PARENTHESIS:
        return {"opening parenthesis"};
    case CLOSING_PARENTHESIS:
        return {"closing parenthesis"};
    case COLON:
        return {"colon"};
    case SEMICOLON:
        return {"semicolon"};
    case COMMA:
        return {"comma"};
    case END:
        return {"end of input"};
    case VALUE:
        return {"either a identifier or a number"};
    default:
        return {"unknown token"};
  }
}

/**
 * NEWICK PARSER
 */

void spearfish_newick_parser_t::parse_tree()
{
    try // TODO possible via return value?
    {
        parse_subtree();
        if (_lexer.peak() != SEMICOLON)
        {
            parse_leaf();
        } else
        {
            // TODO check obs in else korrekt ist oder ohne else immer ausgeführt werden sollte
            // We overcounted, because we assume there is an "upper" branch still
            _edge_count--;
            // inner_count--;
        }
        _lexer.expect(SEMICOLON);
        if (!_lexer.at_end())
        {
            throw std::runtime_error{
                "There were extra characters after the tree was parsed"};
        }
    } catch (...)
    {
        // TODO free memory matrix, r, ...???
        throw;
    }
}

void spearfish_newick_parser_t::parse_subtree()
{
  _edge_count++;

  if (_lexer.peak() == OPENING_PARENTHESIS)
  {
    parse_internal();
  } else
  {
    parse_leaf();
  }
}

void spearfish_newick_parser_t::parse_internal()
{
    _lexer.expect(OPENING_PARENTHESIS);
    size_t start_leaf{_leaf_count}; // first leaf in subtree => start index in matrix
    parse_node_set();
    // TODO singleton subtree check (corax)???
    
    _lexer.expect(CLOSING_PARENTHESIS);
    
    std::string name;
    double length;
    parse_node_attrs(name, length); // TODO dont need name?
    /* TODO
    matrix.add(length, start_leaf, _leaf_count);
    _r.add(length, start_leaf, _leaf_count);
    */
    
    _inner_count++;
}

void spearfish_newick_parser_t::parse_node_set()
{
    parse_subtree();
    if (_lexer.peak() == COMMA)
    {
        _lexer.consume();
        parse_node_set();
    }
}

void spearfish_newick_parser_t::parse_node_attrs(std::string& name, double& length)
{
    parse_name(name);
    parse_comment();
    parse_length(length);
    parse_comment();
}

void spearfish_newick_parser_t::parse_leaf()
{
    std::string name;
    double length;
    parse_node_attrs(name, length);
    if (name.empty()) // TODO correct check (nullptr/empty/...)
    {
        throw std::runtime_error{
            std::string{"Found a leaf without a name around "}
            + _lexer.describe_position()};
    }
    /* TODO
    // _leaf_count == 0 => _r = ()
    //       => push 0 or nothing depending on implementation of matrix
    matrix.push_row(_r + length);
    _r.push_back(length);
    */ 
    _leaf_count++;
}

void spearfish_newick_parser_t::parse_name(std::string& name)
{
    if (_lexer.peak() == VALUE)
    {
        _lexer.consume();
        name = parse_string();
    }
}

void spearfish_newick_parser_t::parse_length(double& length)
{
    if (_lexer.peak() == COLON)
    {
        _lexer.consume();
        _lexer.expect(VALUE);
        length = parse_number();
    }
}

void spearfish_newick_parser_t::parse_comment()
{
    if (_lexer.peak() == OPENING_SQUARE_BRACKET)
    {
        _lexer.consume();
        _lexer.consume_until(CLOSING_SQUARE_BRACKET);
    }
}

std::string spearfish_newick_parser_t::parse_string()
{
  return _lexer.consume_value_as_string();
}

double spearfish_newick_parser_t::parse_number()
{
  return _lexer.consume_value_as_double();
}
