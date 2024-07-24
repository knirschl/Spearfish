/**
 From coraxlib but for newick -> dm conversion
 */

#ifndef SPEARFISH_IO_NEWICK_HPP
#define SPEARFISH_IO_NEWICK_HPP

#include <fstream>
#include <utility>

/**
 * Lexeme types for the newick parser.
 */
enum spearfish_lexeme_t
{
    OPENING_SQUARE_BRACKET,
    CLOSING_SQUARE_BRACKET,
    OPENING_PARENTHESIS,
    CLOSING_PARENTHESIS,
    COLON,
    SEMICOLON,
    COMMA,
    VALUE,
    END
};

/**
 * A lexer for the newick parser. In general, users should not consume this
 * directly, but instead use the `spearfish_newick_parser_t`.
 */
class spearfish_newick_lexer_t
{
public:
    /**
     * Initializes a lexer for the string `input` in newick format.
     *
     * @param input newick string
     */
    explicit spearfish_newick_lexer_t(std::string input) :
        _input{std::move(input)},
        _current_index{0} {};

    /**
     * Consume next token.
     *
     * @return The consumed token
     */
    spearfish_lexeme_t consume();

    /**
     * Consume tokens until matching token (including). Acts like a discard.
     *
     * @param token_type Token until every other token should be discarded  
     */
    void consume_until(spearfish_lexeme_t token_type)
    {
        while (token_type != consume()) {}
    }

    /**
     * Read the current value.
     *
     * @return A string representation of the value
     */
    std::string consume_value_as_string();

    /**
     * Read the current value and convert it to a double.
     *
     * @return A double representing the value
     * @throws std::runtime_error if conversion fails
     */
    double consume_value_as_double();

    /**
     * Look ahead at next token.
     *
     * @return The token at the next position
     */
    spearfish_lexeme_t peak();

    /**
     * Checks if the current token is the same as `token_type`.
     *
     * @throws std::runtime_error if mismatch
     */
    void expect(spearfish_lexeme_t token_type);

    /**
     * Give the current position of the lexer with a string.
     *
     * @return A description of the lexer's position
     */
    std::string describe_position() const
    {
        std::stringstream builder;
        builder << "position " << _current_index;
        return builder.str();
    }

    /**
     * Checks if end of string is reached.
     *
     * @return True if lexer reached end of string
     */
    bool at_end()
    {
        return _input.size() == _current_index;
    }

private:
    /**
     * Check if `c` is special token.
     *
     * @param c Character to check
     * @return True if `c` is in "[]():;,\0" or end of file*/
    bool is_punct(char c);

    /**
     * Same as `consume()` but return position as well.
     *
     *@return A pair of the read token and its position
     */
    std::pair<spearfish_lexeme_t, size_t> consume_token_pos();

    /**
     * Discard all (white-)space until next token
     */
    void skip_whitespace();

    /**
     * Give a textual description of the `token_type`.
     *
     * @param token_type The token to describe
     * @return A description of the token
     */
    std::string describe_token(spearfish_lexeme_t token_type);

    /* member variables */
    const std::string _input;
    std::string _value;
    size_t      _current_index;
};

/**
 * Parser for a newick encoded tree.
 *
 * Using the following grammar:
 * ```
 * <tree> ::=
 *     <subtree> ";"
 * <subtree> ::=
 *     <leaf> |
 *     <internal>
 * <internal> ::=
 *     "(" <node_set> ")" <node_attrs>
 * <node_set> ::=
 *     <node> |
 *     <node> "," <node_set>
 * <node> ::=
 *     <subtree> <node_attrs>
 * <node_attrs> ::=
 *     <name> <length> <comment>
 * <leaf> ::=
 *     <node_attrs>
 * <length> ::=
 *     ":" <number> |
 *     <empty>
 * <name> ::=
 *     <string> |
 *     <empty>
 * <string> ::=
 *     anything but punctuation
 * <number> ::=
 *     [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
 * <comment> ::=
 *     "[" .* "]" |
 *     <empty>
 * <empty> ::=
 *     ""
 * ```
 */
class spearfish_newick_parser_t
{
public:
    /**
     * Construct a newick parser over the given string.
     *
     * @param input Newick string to be parsed
     */
     explicit spearfish_newick_parser_t(std::string input) :
        _lexer{std::move(input)},
        _leaf_count{0},
        _inner_count{0},
        _edge_count{0} {};

    /**
     * Parse the newick tree which was passed to the constructor.
     */
    void parse()
    {
        parse_tree();
    }

private:
    /**
     * Function corresponding to the rule
     *
     * ```
     * <tree> ::=
     *     <subtree> ";"
     * ```
     */
    void parse_tree();

    /**
     * Function corresponding to the rule
     *
     * ```
     * <subtree> ::=
     *     <leaf> |
     *     <internal>
     * ```
     */
    void parse_subtree();

    /**
     * Function corresponding to the rule
     *
     * ```
     * <internal> ::=
     *     "(" <node_set> ")" <node_attrs>
     *
     * Allocates memory by creating a `corax_unode_t`
     * ```
     */
    void parse_internal(); // creates node

    /**
     * Function corresponding to the rules
     * ```
     * <node_set> ::=
     *     <node> |
     *     <node> "," <node_set>
     * <node> ::=
     *     <subtree> <node_attrs>
     * ```
     */
    void parse_node_set();

    /**
     * Function corresponding to the rule
     * ```
     * <node_attrs> ::=
     *     <name> <length> <comment>
     * ```
     */
    void parse_node_attrs(std::string& name, double& length);

    /**
     * Function corresponding to the rule
     * ```
     * <leaf> ::=
     *     <node_attrs>
     * ```
     *
     * Allocates memory by creating a `corax_unode_t`
     */
    void parse_leaf(); // creates node

    /**
     * Function corresponding to the rule
     * ```
     * <name> ::=
     *     <string> |
     *     <empty>
     * ```
     */
    void parse_name(std::string& name);

    /**
     * Function corresponding to the rule
     * ```
     * <length> ::=
     *     ":" <number> |
     *     <empty>
     * ```
     */
    void parse_length(double& length);

    /**
     * Function corresponding to the rule
     * ```
     * <comment> ::=
     *     "[" .* "]" |
     *     <empty>
     * ```
     */
    void parse_comment();

    /**
     * Function corresponding to the rule
     * ```
     * <string> ::=
     *     anything but punctuation
     * ```
     */
    std::string parse_string();

    /**
     * Function corresponding to the rule
     * ```
     * <number> ::=
     *     [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
     * ```
     */
    double parse_number();

    /* member variables */
    spearfish_newick_lexer_t _lexer;
    size_t               _leaf_count;
    size_t               _inner_count;
    size_t               _edge_count;
};


//
// TODO build matrix & return it
// TODO move code below somewhere else??
// TODO newick.cpp + newick_matrix.cpp with inheritance 
//

/**
 * Wrapper function for spearfish_newick_parser_t::parse()
 */
inline void tree_parse_newick_string(std::string newick_string)
{
    try
    {
        spearfish_newick_parser_t np{std::move(newick_string)};
        np.parse();
    } catch (std::invalid_argument &e)
    {
        //corax_set_error(CORAX_ERROR_INVALID_TREE, e.what());
        //nullptr;
    } catch (std::runtime_error &e)
    {
        //corax_set_error(CORAX_ERROR_NEWICK_SYNTAX, e.what());
        //return nullptr;
    }
}

/**
 * Wrapper function for spearfish_newick_parser_t::parse()
 */
inline void tree_parse_newick(const std::string& filename)
{
    std::ifstream newick_file{filename};
    std::string newick_string{(std::istreambuf_iterator<char>(newick_file)),
                              (std::istreambuf_iterator<char>())};
    return tree_parse_newick_string(newick_string);
}

#endif // SPEARFISH_IO_NEWICK_HPP