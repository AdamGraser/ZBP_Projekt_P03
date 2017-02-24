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


class AbstractAutomaton
{
protected:
	void error(char *message);
public:
	virtual bool exists(string keyword) = 0;
	virtual bool exists2(string keyword) = 0;
	virtual void read_automat(char *fname) = 0;
	virtual void save_automat(char *fname) = 0;
	virtual void print_strings() = 0;
};


/// <summary>
/// Finite-State Automaton, that represents its states in form of lists or complete binary trees.
/// </summary>
template <bool use_tree> class Automaton : public AbstractAutomaton
{
};



/// <summary>
/// Finite-State Automaton, that represents its states in form of lists.
/// </summary>
template <> class Automaton<false> : public AbstractAutomaton
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
protected:
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
template <> class Automaton<true> : public AbstractAutomaton
{
public:
	typedef struct tbucket
	{
		unsigned addr;
		size_t size;
		struct tbucket *next;
	} bucket;
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
	void print_strings();
	bool exists(unsigned char *keyword);
	void test_automat(char *automaton_file);
	void list_automat(char *lexicon_file, char *automaton_file);
	void make_automat(char *lexicon_file, char *automaton_file);
	bool exists(string keyword);
	bool exists2(string keyword);

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
