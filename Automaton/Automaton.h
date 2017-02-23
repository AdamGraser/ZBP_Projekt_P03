/* Automaton - C++ version of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Kuünik and Adam Gr‰ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#pragma once

#include <limits.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <time.h>
#include <iterator>
#include "targetver.h"



// -- types

typedef struct tbucket
{
    unsigned addr;
    size_t size;
    struct tbucket *next;
} bucket;



template <bool use_tree> class Automaton
{};



// list
template <> class Automaton<false>
{};



// tree
template <> class Automaton<true>
{
public:
    Automaton(bool print_statistics = true)
        :print_statistics(print_statistics)
    {}

    ~Automaton()
    {
        fclose(lex_file);
        fclose(aut_file);
    }


    // - types

    typedef union
    {
        unsigned all_fields;
        struct
        {
            unsigned llast : 1;
            unsigned rlast : 1;
            unsigned dest : 21;
            unsigned attr : 8;
            unsigned term : 1;
        } b;
    } transition;

    typedef int sizeof_unsigned_int_must_match_sizeof_transition
        [2 * (sizeof(unsigned) == sizeof(transition)) - 1];
    
    template<typename T>
    class AutomatonLetterIterator : public std::iterator<std::random_access_iterator_tag, T>
    {
		friend class Automaton<true>;

    protected:
		unsigned int offset;
		unsigned int i;
		T* automatArray;
		AutomatonLetterIterator(T* automatArray) : automatArray(automatArray), i(0), offset(1) {}
		AutomatonLetterIterator(T* automatArray, unsigned int i) : automatArray(automatArray), i(i), offset(1) {}

    public:
		typedef typename std::iterator<std::random_access_iterator_tag, T>::pointer pointer;
		typedef typename std::iterator<std::random_access_iterator_tag, T>::reference reference;
		typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;

		AutomatonLetterIterator(const AutomatonLetterIterator& other) : automatArray(other.automatArray), i(other.i), offset(other.offset) {}

        AutomatonLetterIterator& operator=(const AutomatonLetterIterator& other)
        {
            automatArray = other.automatArray;
            i = other.i;
            offset = other.offset;
            return *this;
        }

        reference operator*() const
        {
            return *(automatArray + i + offset - 1);
        }

        pointer operator->() const
        {
            return &(*(automatArray + i + offset - 1));
        }

        AutomatonLetterIterator& operator++()
        {
            offset *= 2;
            return *this;
        }

        AutomatonLetterIterator operator++(int)
        {
            AutomatonLetterIterator incremented(this);
            ++incremented;
            return incremented;
        }

        AutomatonLetterIterator localBegin()
        {
            unsigned newPos = (automatArray + i + offset - 1)->b.dest;
            return AutomatonLetterIterator(automatArray, newPos);
        }

        bool isEnd(unsigned char w)
        {
            T node = *(automatArray + i + offset - 1);

            if (w < node.b.attr)
                return node.b.llast;
            
            if (w > node.b.attr)
                return node.b.rlast;

            return false;
        }

        AutomatonLetterIterator operator+(const difference_type& n) const
        {
            AutomatonLetterIterator incremented(this);
            incremented.offset += n;
            return incremented;
        }

        AutomatonLetterIterator& operator+=(const difference_type& n)
        {
            offset += n;
            return *this;
        }

        reference operator[](const difference_type& n) const
        {
            return *(automatArray + i + offset - 1 + n);
        }

        bool operator==(const AutomatonLetterIterator& other) const
        {
            return i == other.i && offset = other.offset;
        }

        bool operator!=(const AutomatonLetterIterator& other) const
        {
            return i != other.i || offset != other.offset;
        }

        bool operator<(const AutomatonLetterIterator& other) const
        {
            return i < other.i || (i == other.i && offset < other.offset);
        }

        bool operator>(const AutomatonLetterIterator& other) const
        {
            return i > other.i || (i == other.i && offset > other.offset);
        }

        bool operator<=(const AutomatonLetterIterator& other) const
        {
            return i < other.i || (i == other.i && offset <= other.offset);
        }

        bool operator>=(const AutomatonLetterIterator& other) const
        {
            return i > other.i || (i == other.i && offset >= other.offset);
        }

        difference_type operator+(const AutomatonLetterIterator& other) const
        {
            return (i + offset) + (other.i + other.offset);
        }

        difference_type operator-(const AutomatonLetterIterator& other) const
        {
            return (i + offset) - (other.i + other.offset);
        }
    };

	typedef AutomatonLetterIterator<transition> iterator;
	typedef AutomatonLetterIterator<const transition> const_iterator;

	
    // - fields

    bool print_statistics;
    transition* a;


    // - methods

    void print_strings(iterator it, int str_pos);
    bool exists(unsigned char *keyword);
    void test_automat(char *automaton_file);
    void list_automat(char *lexicon_file, char *automaton_file);
    void make_automat(char *lexicon_file, char *automaton_file);

    unsigned size()
    {
        return aut_size;
    }

    unsigned max_size()
    {
        return MAX_AUT_SIZE;
    }

	iterator begin() {
		a = automat;
		return iterator(automat, automat[0].b.dest);
	}

	const_iterator begin() const {
		return const_iterator(automat, automat[0].b.dest);
	}

	const_iterator cbegin() const {
		return const_iterator(automat, automat[0].b.dest);
	}

	iterator end() {
		return iterator(automat, aut_size);
	}

	const_iterator end() const {
		return const_iterator(automat, aut_size);
	}

	const_iterator cend() const {
		return const_iterator(automat, aut_size);
	}

private:
    // -- consts

    static const int MAX_STR_LEN = 300;
    static const int MAX_CHARS = 256;
    static const int HT_SIZE = (1 << 20);
    static const int HT_ELEM_SIZE = (1 << 10);
    static const int MAX_AUT_SIZE = (1 << 21);

    // -- fields

    unsigned long n_strings;            /* number of strings */
    unsigned long n_chars;              /* number of characters */

    bucket *hash_table[HT_SIZE];
    bucket *ht_elem[MAX_AUT_SIZE / HT_ELEM_SIZE];
    int ht_next_elem, ht_last_pos;

    transition automat[MAX_AUT_SIZE];   /* the automaton */
    unsigned aut_size;                  /* size of the automaton */
    unsigned start_state;               /* position of the start state */
    transition larval_state[MAX_STR_LEN + 1][MAX_CHARS];
    size_t l_state_len[MAX_STR_LEN + 1];
    int is_terminal[MAX_STR_LEN + 1];
    unsigned char temp_str[MAX_STR_LEN + 1];  /* string for listing */
    transition temp_state[MAX_CHARS + 1];
    int currentStatePos;
    std::ostringstream tempString;

    FILE *lex_file;                     /* lexicon file */
    FILE *aut_file;                     /* automaton file */

    // -- statistics fields

    unsigned n_term_trans;         /* number of terminal transitions */
    unsigned n_states;             /* number of states */
    unsigned n_trans;              /* number of transitions */

    // -- methods

    void open_dict(char *fname, char *attr);
    void save_automat(char *fname);
    void read_automat(char *fname);
    void list_strings(iterator it, int str_pos);
    void test_automat(void);
    bool check_string(unsigned char *str);
    void make_automat(void);
    unsigned make_state(transition *state, unsigned state_len);
    void make_tree(transition *state, int left, int right, unsigned pos, int full);
    unsigned hash_state(transition *state, unsigned state_len);
    int read_string(unsigned char *str);
    void set_io_buffer(FILE *file, size_t size);
    void prepare_tables(void);
    void error(char *message);
    void show_stat(double exec_time);
    void rewind();
};
