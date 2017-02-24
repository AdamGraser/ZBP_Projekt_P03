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
#include <stack>
#include "targetver.h"

using std::string;
using std::ostringstream;
using std::cout;
using std::endl;
using std::cin;



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
    Automaton(char *dictFileName, bool print_statistics = true)
        :print_statistics(print_statistics)
    {
        open_dict(dictFileName, "r");
        n_strings = 0;
        n_chars = 0;
        make_automat();
        rewind();
    }

    Automaton(bool print_statistics = true)
        :print_statistics(print_statistics)
    {
        n_strings = 0;
        n_chars = 0;
    }

    ~Automaton()
    {
        fclose(lex_file);
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

    /* ==== LETTER ITERATOR DEFINITION ==== */
    
    class AutomatonLetterIterator : public std::iterator<std::random_access_iterator_tag, transition>
    {
		friend class Automaton<true>;

    protected:
        Automaton<true>& automaton;
		unsigned int offset;
		unsigned int i;
        transition* automatArray;

		AutomatonLetterIterator(Automaton<true>& automaton) : automaton(automaton), offset(1)
        {
            automatArray = (transition *) automaton.automat;
            i = automaton.automat[0].b.dest;
        }

		AutomatonLetterIterator(Automaton<true>& automaton, unsigned int i) : automaton(automaton), i(i), offset(1)
        {
            automatArray = (transition *) automaton.automat;

            if (i > automaton.size())
            {
                automaton.error("Error in automaton file.");
            }
        }

    public:
		typedef typename std::iterator<std::random_access_iterator_tag, transition>::difference_type difference_type;

		AutomatonLetterIterator(const AutomatonLetterIterator& other) : automaton(other.automaton), automatArray(other.automatArray), i(other.i), offset(other.offset)
        {
            if (i > automaton.size())
            {
                automaton.error("Error in automaton file.");
            }
        }

        AutomatonLetterIterator(const AutomatonLetterIterator& other, unsigned int i) : automaton(other.automaton), automatArray(other.automatArray), i(i), offset(1)
        {
            if (i > automaton.size())
            {
                automaton.error("Error in automaton file.");
            }
        }

        AutomatonLetterIterator& operator=(const AutomatonLetterIterator& other)
        {
            automatArray = other.automatArray;
            i = other.i;
            offset = other.offset;
            return *this;
        }

        unsigned char operator*() const
        {
            return (automatArray + i + offset - 1)->b.attr;
        }

        AutomatonLetterIterator& operator++()
        {
            offset *= 2;

            if (i > automaton.size())
            {
                automaton.error("Error in automaton file.");
            }

            return *this;
        }

        AutomatonLetterIterator operator++(int)
        {
            AutomatonLetterIterator incremented(*this);
            ++incremented;
            return incremented;
        }

        AutomatonLetterIterator localBegin()
        {
            unsigned newPos = (automatArray + i + offset - 1)->b.dest;
            return AutomatonLetterIterator(*this, newPos);
        }

        bool isEnd(unsigned char w)
        {
            transition node = *(automatArray + i + offset - 1);

            if (w < node.b.attr)
                return node.b.llast;
            
            if (w > node.b.attr)
                return node.b.rlast;

            return false;
        }

        bool isRoot()
        {
            return i == 0;
        }

        bool isTerm()
        {
            return (automatArray + i)->b.term;
        }

        AutomatonLetterIterator operator+(const difference_type& n) const
        {
            AutomatonLetterIterator incremented(*this);
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
            return i == other.i && offset == other.offset;
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

    /* ==== LETTER ITERATOR DEFINITION END ==== */

    /* ==== WORD ITERATOR DEFINITION ==== */

    class AutomatonWordIterator : public std::iterator<std::forward_iterator_tag, string>
    {
        friend class Automaton<true>;

    protected:
        AutomatonLetterIterator letterIterator;

        struct Snapshot
        {
            AutomatonLetterIterator it;
            int strPos;
        };

        std::stack<Snapshot> snapshotStack;
        unsigned char currentLetter;
        unsigned dest;
        bool isTerm;
        bool islLast;
        bool isrLast;
        int strPos;
        ostringstream tempString;
        string toPrint;
        transition currentTrans;

        AutomatonWordIterator(AutomatonLetterIterator letterIterator) : letterIterator(letterIterator)
        {
            snapshotStack.push(Snapshot{ letterIterator, 0 });
        }

    public:
        AutomatonWordIterator(const AutomatonWordIterator& other) : letterIterator(other.letterIterator)
        {
            snapshotStack.push(Snapshot{ letterIterator, 0 });
        }

        AutomatonWordIterator& operator=(const AutomatonWordIterator& other)
        {
            letterIterator = other.letterIterator;
            snapshotStack = other.snapshotStack;
            return *this;
        }

        string operator*() const
        {
            return toPrint;
        }

        AutomatonWordIterator operator++(int)
        {
            //while (!snapshotStack.empty())
            //{
            //    do
            //    {
            //        Snapshot currentSnapshot = snapshotStack.top();
            //        snapshotStack.pop();

            //        /* Params */
            //        AutomatonLetterIterator currentIt(currentSnapshot.it);
            //        currentLetter = *currentSnapshot.it;
            //        strPos = currentSnapshot.strPos;
            //        isLast = currentIt.isLast();
            //        /* End Params */

            //        if (currentIt.isRoot())
            //        {
            //            break;
            //        }

            //        tempString.seekp(strPos, std::ios_base::beg);
            //        tempString << currentLetter;

            //        if (!isLast)
            //        {
            //            Snapshot firstNew = Snapshot{ currentIt + 1, strPos };
            //            snapshotStack.push(firstNew);
            //        }
            //        Snapshot secondNew = Snapshot{ currentIt.localBegin(), strPos + 1 };
            //        snapshotStack.push(secondNew);

            //        if (currentIt.isTerm())
            //        {
            //            toPrint = tempString.str().substr(0, tempString.tellp());
            //            return AutomatonWordIterator(*this);
            //        }
            //    } while (!isLast);
            //}

            return AutomatonWordIterator(*this);
        }

        bool isEnd()
        {
            return snapshotStack.empty();
        }
    };

    /* ==== WORD ITERATOR DEFINITION END ==== */

	typedef AutomatonLetterIterator iterator;
    typedef AutomatonWordIterator wordIterator;

	
    // - fields

    bool print_statistics;
    transition* a;


    // - methods

    void print_strings(iterator it, int str_pos);
    bool exists(unsigned char *keyword);

    unsigned size()
    {
        return aut_size;
    }

    unsigned max_size()
    {
        return MAX_AUT_SIZE;
    }

	iterator letterBegin() {
		a = automat;
		return iterator(*this, automat[0].b.dest);
	}

    wordIterator wordBegin()
    {
        return wordIterator(this->letterBegin());
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
    ostringstream tempString;

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
