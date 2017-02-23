/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Kuünik and Adam Gr‰ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#pragma once

#include <string>
#include <type_traits>
#include <cstddef>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <ctime>
#include <stack>
#include "targetver.h"

using std::string;
using std::size_t;
using std::cout;


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
public:
	/* Definitions */
	typedef union
	{
		unsigned all_fields;
		struct
		{
			unsigned last : 1;
			unsigned dest : 22;
			unsigned attr : 8;
			unsigned term : 1;
		} b;
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
	std::ostringstream tempString;

	FILE *lex_file;                     /* lexicon file */
	FILE *aut_file;                     /* automaton file */

	unsigned n_term_trans;         /* number of terminal transitions */
	unsigned n_states;             /* number of states */
	unsigned n_trans;              /* number of transitions */

	void open_dict(char *fname, char *attr);
	void make_automat(void);
	unsigned make_state(transition *state, unsigned state_len);
	unsigned hash_state(transition *state, unsigned state_len);
	int read_string(unsigned char *str);
	void set_io_buffer(FILE *file, size_t size);
	void prepare_tables(void);
	void error(char *message);
	void rewind();
	int currentStatePos;
	/// <summary>Defines whether statistics should be printed on the screen.</summary>
	bool print_statistics;
public:
	/// <summary>Default constructor.</summary>
	/// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
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

	/// <summary>Default destructor.</summary>
	~Automaton()
	{
		fclose(lex_file);
	}

	void read_automat(char *fname);
	void save_automat(char *fname);
	void print_strings();
	void show_stat(double exec_time);
	bool exists(string keyword);
	bool exists2(string keyword);
	unsigned size();

	/* ==== LETTER ITERATOR DEFINITION ==== */

	class AutomatonLetterIterator : public std::iterator<std::random_access_iterator_tag, transition> {

		friend class Automaton<false>;

	protected:
		Automaton<false>& automaton;
		bool shouldEndOnNextIncrement = false;
		unsigned int i;
		transition* automatArray;
		AutomatonLetterIterator(Automaton<false>& automaton) : automaton(automaton) {
			automatArray = (transition *)automaton.automat;
			i = automaton.automat[0].b.dest;
		}
		AutomatonLetterIterator(Automaton<false>& automaton, unsigned int i) : automaton(automaton), i(i) {
			automatArray = (transition *)automaton.automat;
			if (i > automaton.size())
			{
				automaton.error("Error in automaton file.");
			}
		}

	public:
		typedef typename std::iterator<std::random_access_iterator_tag, transition>::difference_type difference_type;

		AutomatonLetterIterator(const AutomatonLetterIterator& other) : automaton(other.automaton), automatArray(other.automatArray), i(other.i) {
			if (i > automaton.size())
			{
				automaton.error("Error in automaton file.");
			}
		}
		AutomatonLetterIterator(const AutomatonLetterIterator& other, unsigned int i) : automaton(other.automaton), automatArray(other.automatArray), i(i) {
			if (i > automaton.size())
			{
				automaton.error("Error in automaton file.");
			}
		}

		AutomatonLetterIterator& operator=(const AutomatonLetterIterator& other) {
			automatArray = other.automatArray;
			i = other.i;
			return *this;
		}

		unsigned char operator*() const {
			return (automatArray + i)->b.attr;
		}

		AutomatonLetterIterator& operator++() {
			++i;
			if (i > automaton.size())
			{
				automaton.error("Error in automaton file.");
			}
			return *this;
		}

		AutomatonLetterIterator operator++(int) {
			return AutomatonLetterIterator(*this, i++);
		}

		AutomatonLetterIterator localBegin() {
			shouldEndOnNextIncrement = false;
			unsigned newPos = (automatArray + i)->b.dest;
			return AutomatonLetterIterator(*this, newPos);
		}

		AutomatonLetterIterator localBeginAntecedent() {
			shouldEndOnNextIncrement = false;
			unsigned newPos = (automatArray + i)->b.dest - 1;
			return AutomatonLetterIterator(*this, newPos);
		}

		bool isEnd() {
			if (i == -1 || i == 0)
			{
				return true;
			}
			if (shouldEndOnNextIncrement)
			{
				shouldEndOnNextIncrement = false;
				return true;
			}
			if ((automatArray + i)->b.last)
			{
				shouldEndOnNextIncrement = true;
			}
			return false;
		}

		bool isRoot() {
			return i == 0;
		}

		bool isLast() {
			return (automatArray + i)->b.last;
		}

		bool isTerm() {
			return (automatArray + i)->b.term;
		}

		AutomatonLetterIterator operator+(const difference_type& n) const {
			return AutomatonLetterIterator(*this, (i + n));
		}

		AutomatonLetterIterator& operator+=(const difference_type& n) {
			i += n;
			return *this;
		}

		reference operator[](const difference_type& n) const {
			return *(automatArray + i + n);
		}

		bool operator==(const AutomatonLetterIterator& other) const {
			return i == other.i;
		}

		bool operator!=(const AutomatonLetterIterator& other) const {
			return i != other.i;
		}

		bool operator<(const AutomatonLetterIterator& other) const {
			return i < other.i;
		}

		bool operator>(const AutomatonLetterIterator& other) const {
			return i > other.i;
		}

		bool operator<=(const AutomatonLetterIterator& other) const {
			return i <= other.i;
		}

		bool operator>=(const AutomatonLetterIterator& other) const {
			return i >= other.i;
		}

		difference_type operator+(const AutomatonLetterIterator& other) const {
			return i + other.i;
		}

		difference_type operator-(const AutomatonLetterIterator& other) const {
			return i - other.i;
		}
	};
	/* ==== LETTER ITERATOR DEFINITION END ==== */

	/* ==== WORD ITERATOR DEFINITION ==== */
	class AutomatonWordIterator : public std::iterator<std::forward_iterator_tag, string> {

		friend class Automaton<false>;

	protected:
		AutomatonLetterIterator letterIterator;

		struct Snapshot {
			AutomatonLetterIterator it;
			int strPos;
		};

		std::stack<Snapshot> snapshotStack;
		unsigned char currentLetter;
		unsigned dest;
		bool isTerm;
		bool isLast;
		int strPos;
		std::ostringstream tempString;
		std::string toPrint;
		transition currentTrans;

		AutomatonWordIterator(AutomatonLetterIterator letterIterator) : letterIterator(letterIterator) {
			snapshotStack.push(Snapshot{ letterIterator, 0 });
		}

	public:
		AutomatonWordIterator(const AutomatonWordIterator& other) : letterIterator(other.letterIterator) {
			snapshotStack.push(Snapshot{ letterIterator, 0 });
		}

		AutomatonWordIterator& operator=(const AutomatonWordIterator& other) {
			letterIterator = other.letterIterator;
			snapshotStack = other.snapshotStack;
			return *this;
		}

		std::string operator*() const {
			return toPrint;
		}

		AutomatonWordIterator operator++(int) {
			while (!snapshotStack.empty())
			{
				do
				{
					Snapshot currentSnapshot = snapshotStack.top();
					snapshotStack.pop();

					/* Params */
					AutomatonLetterIterator currentIt(currentSnapshot.it);
					currentLetter = *currentSnapshot.it;
					strPos = currentSnapshot.strPos;
					isLast = currentIt.isLast();
					/* End Params */

					if (currentIt.isRoot())
					{
						break;
					}

					tempString.seekp(strPos, std::ios_base::beg);
					tempString << currentLetter;

					if (!isLast)
					{
						Snapshot firstNew = Snapshot{ currentIt+1, strPos };
						snapshotStack.push(firstNew);
					}
					Snapshot secondNew = Snapshot{ currentIt.localBegin(), strPos + 1 };
					snapshotStack.push(secondNew);

					if (currentIt.isTerm())
					{
						toPrint = tempString.str().substr(0, tempString.tellp());
						return AutomatonWordIterator(*this);
					}
				} while (!isLast);
			}

			return AutomatonWordIterator(*this);
		}

		bool isEnd() {
			return snapshotStack.empty();
		}
	};
	/* ==== WORD ITERATOR DEFINITION END ==== */

	typedef AutomatonLetterIterator iterator;
	typedef AutomatonWordIterator wordIterator;

	iterator letterBegin() {
		return iterator(*this);
		//return iterator(automat, automat[0].b.dest);
	}

	wordIterator wordBegin() {
		return wordIterator(this->letterBegin());
	}
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
