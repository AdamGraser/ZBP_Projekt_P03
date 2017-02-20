/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Ku�nik and Adam Gr�ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#pragma once

#include <string>
#include <type_traits>
#include <cstddef>
#include <iostream>
#include <ctime>
#include "targetver.h"

using std::string;
using std::size_t;
using std::cout;

/* Definitions */

typedef union
{
	unsigned all_fields;
	struct
	{
#ifdef USE_TREE
		unsigned llast : 1;
		unsigned rlast : 1;
		unsigned dest : 21;
#else
		unsigned last : 1;
		unsigned dest : 22;
#endif
		unsigned attr : 8;
		unsigned term : 1;
	} b;
#ifdef USE_INCLUSION
	struct
	{
		unsigned last : 1;
		unsigned dest_attr_term : 31;
	} d;
#endif
} transition;

typedef int sizeof_unsigned_int_must_match_sizeof_transition
[2 * (sizeof(unsigned) == sizeof(transition)) - 1];

typedef struct tbucket
{
	unsigned addr;
	size_t size;
	struct tbucket *next;
} bucket;


/* End definitions */

/// <summary>
/// Finite-State Automaton, that represents its states in form of lists or complete binary trees.
/// </summary>
template <bool use_tree> class Automaton
{ };



/// <summary>
/// Finite-State Automaton, that represents its states in form of lists.
/// </summary>
template <> class Automaton<false>
{
private:
	/// <summary>Maximal lexicon string length.</summary>
	static const int max_str_len = 300;
	/// <summary></summary>
	static const int max_chars = 256;
	/// <summary></summary>
	static const int ht_size = (1 << 20);
	/// <summary></summary>
	static const int ht_elem_size = (1 << 10);
	/// <summary></summary>
	static const int max_aut_size = (1 << 22);


	unsigned long n_strings;            /* number of strings */
	unsigned long n_chars;              /* number of characters */

	bucket *hash_table[ht_size];
	bucket *ht_elem[max_aut_size / ht_elem_size];
	int ht_next_elem, ht_last_pos;

	transition automat[max_aut_size];   /* the automaton */
	unsigned aut_size;                  /* size of the automaton */
	unsigned start_state;               /* position of the start state */
	transition larval_state[max_str_len + 1][max_chars];
	size_t l_state_len[max_str_len + 1];
	int is_terminal[max_str_len + 1];
	unsigned char temp_str[max_str_len + 1];  /* string for listing */

	FILE *lex_file;                     /* lexicon file */
	FILE *aut_file;                     /* automaton file */

	unsigned n_term_trans;         /* number of terminal transitions */
	unsigned n_states;             /* number of states */
	unsigned n_trans;              /* number of transitions */

	void open_dict(char *fname, char *attr);
	void save_automat(char *fname);
	void read_automat(char *fname);
	void list_strings(unsigned pos, int str_pos);
	void test_automat(void);
	int check_string(unsigned char *str);
	void make_automat(void);
	unsigned make_state(transition *state, unsigned state_len);
	unsigned hash_state(transition *state, unsigned state_len);
	int read_string(unsigned char *str);
	void set_io_buffer(FILE *file, size_t size);
	void prepare_tables(void);
	void error(char *message);
	void show_stat(double exec_time);


public:
    /// <summary>Defines whether statistics should be printed on the screen.</summary>
    bool print_statistics;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    Automaton(char *dictFileName, char *automatFileName, bool print_statistics = true)
        :print_statistics(print_statistics)
	{
		open_dict(dictFileName, "r");
		make_automat();
		save_automat(automatFileName);
		read_automat(automatFileName);
	}

    /// <summary>Default destructor.</summary>
    ~Automaton()
    {
		fclose(lex_file);
	}

	bool exists(unsigned char *keyword);
};



/// <summary>
/// Finite-State Automaton, that represents its states in form of complete binary trees.
/// </summary>
template <> class Automaton<true>
{
public:
    /// <summary>Defines whether statistics should be printed on the screen.</summary>
    bool print_statistics;
    /// <summary>Maximal lexicon string length.</summary>
    int max_str_len;
    /// <summary></summary>
    int max_chars;
    /// <summary></summary>
    int ht_size;
    /// <summary></summary>
    int ht_elem_size;
    /// <summary></summary>
    int max_aut_size;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    /// <param name="max_str_len">Maximal lexicon string length.</param>
    /// <param name="max_chars"></param>
    /// <param name="ht_size"></param>
    Automaton(bool print_statistics = true, int max_str_len = 300, int max_chars = 256, int ht_size = (1 << 20), int ht_elem_size = (1 << 10),
              int max_aut_size = (1 << 21))
        :print_statistics(print_statistics), max_str_len(max_str_len), max_chars(max_chars), ht_size(ht_size), ht_elem_size(ht_elem_size), max_aut_size(max_aut_size)
    {}

    /// <summary>Default destructor.</summary>
    ~Automaton()
    {}
};
