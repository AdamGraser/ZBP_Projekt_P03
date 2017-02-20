/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Ku�nik and Adam Gr�ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#include "Automaton.h"

// PRIVATE

/*
** Open the lexicon file.
*/
void Automaton<false>::open_dict(char *fname, char *attr)
{
	if ((lex_file = fopen(fname, attr)) == NULL)
		error("Cannot open lexicon file.");
	set_io_buffer(lex_file, 8192);
}

/*
** Save the automaton to a file of given name.
*/
void  Automaton<false>::save_automat(char *fname)
{
	if ((aut_file = fopen(fname, "wb")) == NULL)
		error("Cannot open output file.");

	/* create a pseudo state pointing to the start state */
	automat[0].all_fields = start_state;
	if (fwrite(automat, sizeof automat[0], aut_size, aut_file) < aut_size)
		error("Error writing to file.");
	fclose(aut_file);
}

/*
** Read an automaton from a file fname.
*/
void Automaton<false>::read_automat(char *fname)
{
	if ((aut_file = fopen(fname, "rb")) == NULL)
		error("Cannot open input file.");

	aut_size = fread(automat, sizeof automat[0], max_aut_size, aut_file);
	fclose(aut_file);
	if (aut_size >= 2)
	{
		/* create a pseudo state pointing to the start state */
		memcpy(&start_state, &automat[0], sizeof automat[0]);
		automat[0].b.dest = automat[0].all_fields;
		if (start_state < aut_size)
			return;
	}
	error("Error in input file.");
}

/*
** Recursively list all the strings recognized by an automaton,
** beginning at the given position in the automaton and in the
** string.
*/
void Automaton<false>::list_strings(unsigned pos, int str_pos)
{
	int i;

	if (pos == 0)
		return;

	if (pos > aut_size)
		error("Error in automat file.");
	do
	{
		temp_str[str_pos] = (unsigned char)(automat[pos].b.attr);
		if (automat[pos].b.term)
		{
			/* when string terminates at this character write the string */
			for (i = 0; i <= str_pos; i++)
				putc(temp_str[i], lex_file);
			putc('\n', lex_file);
			n_strings++;
			n_chars += str_pos + 2;
		}
		/* execute recursively for all characters in current state */
		list_strings(automat[pos].b.dest, str_pos + 1);
	} while (!(automat[pos++].b.last));
}


/*
** Check if the automaton is correct
** (test all the strings from a lexicon).
*/
void Automaton<false>::test_automat(void)
{
	n_strings = 0;
	n_chars = 0;

	while (read_string(temp_str))
		if (!check_string(temp_str))
			printf("String %s not found!\n", temp_str);
}


/*
** Check if the given string exists in the automaton.
*/
int Automaton<false>::check_string(unsigned char *str)
{
	unsigned pos = 0;
	int i;
	unsigned char w;

	for (i = 0; str[i]; i++)
	{
		/* get pointer to new state */
		pos = automat[pos].b.dest;
		w = str[i];

		if (!pos)
			return 0;

		if (pos > aut_size)
			error("Error in automaton file.");
		/* find current character in state */
		while (automat[pos].b.attr != (unsigned)w)
		{
			if (automat[pos++].b.last)
				return 0;
			if (pos > aut_size)
				error("Error in automaton file.");
		}
	}
	return automat[pos].b.term;
}


/*
** Create the automaton.
*/
void Automaton<false>::make_automat(void)
{
	unsigned char s0[max_str_len + 1] = "";
	unsigned char s1[max_str_len + 1];
	size_t i = 0, p, q;
	transition new_trans;

	new_trans.b.last = 0;
	prepare_tables();   /* prepare data structures for algorithm */

	while ((q = read_string(s1)) != 0)
	{
		/* find common prefix */
		for (p = 0; s1[p] == s0[p]; p++)
			;
		if (s1[p] < s0[p])
			error("Strings in the lexicon file are unsorted.");

		/* emit states for suffix of previous string */
		while (i > p)
		{
			new_trans.b.dest = make_state(larval_state[i], l_state_len[i]);
			new_trans.b.attr = s0[--i];
			new_trans.b.term = is_terminal[i + 1];
			larval_state[i][l_state_len[i]++].all_fields = new_trans.all_fields;
		}

		/* copy suffix of s1 to s0 */
		while (i < q)
		{
			s0[i] = s1[i];
			is_terminal[++i] = 0;
			l_state_len[i] = 0;
		}
		s0[q] = '\0';
		is_terminal[q] = 1;

		if (print_statistics)
		{

			if (n_strings % 65536 == 0)
				printf("%lu strings read\t%u transitions created\n", n_strings, aut_size);
		}
	}
	while (i > 0)
	{
		new_trans.b.dest = make_state(larval_state[i], l_state_len[i]);
		new_trans.b.term = is_terminal[i];
		new_trans.b.attr = s0[--i];
		larval_state[i][l_state_len[i]++].all_fields = new_trans.all_fields;
	}
	start_state = make_state(larval_state[0], l_state_len[0]);
	automat[aut_size].b.dest = start_state;   /* put pseudo state */
}

/*
** Seek an identical state in the automaton
** or create a new state. Return its index.
*/
unsigned Automaton<false>::make_state(transition *state, unsigned state_len)
{
	bucket *ptr;
	int i;
	unsigned pos;
	unsigned hash_addr;

	/* make zero state */
	if (state_len == 0)
		state[state_len++].all_fields = 0;

	state[state_len - 1].b.last = 1;

	/* check if an identical state is in automat */
	hash_addr = hash_state(state, state_len);
	for (ptr = hash_table[hash_addr]; ptr; ptr = ptr->next)
	{
		if (ptr->size == state_len)
		{
			for (i = state_len - 1; i >= 0; i--)
				if (automat[ptr->addr + i].all_fields != state[i].all_fields)
					break;
			if (i < 0)
				return ptr->addr;       /* identical state found */
		}
	}

	if (print_statistics)
	{

		for (i = state_len - 1; i >= 0; i--)
			n_term_trans += state[i].b.term;
	}

	if (aut_size + state_len >= max_aut_size)
		error("The automaton grew too large.");

	/* put state into automat */
	for (i = state_len - 1; i >= 0; i--)
		automat[aut_size + i] = state[i];

	if (ht_next_elem >= ht_elem_size)
	{
		ht_next_elem = 0;
		if ((ht_elem[++ht_last_pos] =
			(bucket *)malloc(sizeof(bucket) * ht_elem_size)) == NULL)
			error("Not enough memory.");
	}
	ptr = &ht_elem[ht_last_pos][ht_next_elem++];

	/* put pointer to the state into hash table */
	ptr->addr = aut_size;
	ptr->size = state_len;
	ptr->next = hash_table[hash_addr];
	hash_table[hash_addr] = ptr;


	pos = aut_size;
	aut_size += state_len;


	if (print_statistics)
	{
		n_states++;
		n_trans += state_len;
	}
	return pos;
}

/*
** Hash function for states.
*/
unsigned Automaton<false>::hash_state(transition *state, unsigned state_len)
{
	unsigned r = 0;
	int i;

	for (i = state_len - 1; i >= 0; i--)
		r += state[i].all_fields;

	return ((r * 324027) >> 13) % ht_size;
}

/*
** Read next string from input file and return its length.
*/
int Automaton<false>::read_string(unsigned char *str)
{
	int c, i;

	for (i = 0; (c = getc(lex_file)) != '\n'; str[i++] = (unsigned char)c)
	{
		if (c == EOF)
			return 0;
		if (i > max_str_len)
			error("Lexicon string too long.");
	}
	str[i] = '\0';
	n_strings++;
	n_chars += i + 1;

	return i;
}


/*
** Set buffer for io stream.
*/
void Automaton<false>::set_io_buffer(FILE *file, size_t size)
{
	if (setvbuf(file, NULL, _IOFBF, size) != 0)
		error("Cannot set input buffer.");
}

/*
** Print an error message and terminate the program.
*/
void Automaton<false>::error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}
/*
** Prepare data structures before program run.
*/
void Automaton<false>::prepare_tables(void)
{
	int i;

	for (i = ht_size - 1; i >= 0; i--)
	{
		hash_table[i] = NULL;
	}

	for (i = max_aut_size / ht_elem_size - 1; i >= 0; i--)
		ht_elem[i] = NULL;
	ht_last_pos = -1;
	ht_next_elem = ht_elem_size;

	aut_size = 0;
	for (i = 0; i < max_str_len + 1; i++)
		l_state_len[i] = 0;
}


/*
** Show some statistics, including execution time.
*/
void Automaton<false>::show_stat(double exec_time)
{
	printf("%lu strings\t%lu characters\n", n_strings, n_chars);
#ifdef PRINT_STATISTICS
	printf("%u states\t%u transitions\t%u terminal transitions\n",
		n_states, n_trans, n_term_trans);
#ifdef USE_INCLUSION
	printf("%u included states\t%u included transitions\n",
		n_states_in, n_trans_in);
#endif
#endif
	printf("Execution time: %.3f seconds\t", exec_time);
	if (exec_time != 0.0)
	{
		printf("\nExecution speed: %.lf wps, %.lf cps\n",
			n_strings / exec_time, n_chars / exec_time);
	}
	printf("Size of the automaton: %u bytes\n", aut_size * sizeof automat[0]);
}

bool Automaton<false>::exists(unsigned char *keyword)
{
	n_strings = 0;
	n_chars = 0;

	if (!check_string(keyword))
		return false;

	return true;
}