#include "Automaton.h"

int main(int argc, char **argv)
{
	clock_t t1, t2;

	if (argc == 4)
	{
		if (!strcmp(argv[1], "-m"))
		{ /* make a new automaton */

			Automaton<false> *a = new Automaton<false>(argv[3], false);

			while (true)
			{
				std::cout << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << "ITERATOR LIST:" << std::endl;
				std::cout << " -------" << std::endl;

				//a->print_strings();

				for (Automaton<false>::AutomatonWordIterator it = a->wordBegin(); !it.isEnd(); it++)
				{
					std::cout << *it << std::endl;
				}

				// CUSTOM WORD SEARCH

				std::string keyword;
				std::string saveFile;
				const char* searchWord;
				unsigned char currentLetter;
				bool found;

				std::cout << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << "SEARCH BY WORD ITERATOR LIST:" << std::endl;
				std::cout << " -------" << std::endl;

				std::cout << std::endl;
				std::cout << "search: ";
				std::cin >> keyword;
				searchWord = keyword.c_str();
				found = a->exists(searchWord);

				if (found)
				{
					std::cout << "FOUND: " << keyword << std::endl;
				}
				if (found == false)
				{
					std::cout << "NOT FOUND: " << keyword << std::endl;
				}

				std::cout << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << "CUSTOM SEARCH BY LETTER ITERATOR LIST:" << std::endl;
				std::cout << " -------" << std::endl;

				std::cout << std::endl;
				std::cout << "search: ";
				std::cin >> keyword;
				searchWord = keyword.c_str();
				found = false;
				for (Automaton<false>::AutomatonLetterIterator it = a->letterBegin(); !it.isEnd(); it++)
				{
					currentLetter = *it;
					if (currentLetter == *searchWord)
					{
						searchWord++;
						if ((unsigned)*searchWord == 0 && it.isTerm())
						{
							found = true;
							break;
						}
						it = it.localBeginAntecedent();
					}
				}

				if (found)
				{
					std::cout << "FOUND: " << keyword << std::endl;
				}
				if (found == false)
				{
					std::cout << "NOT FOUND: " << keyword << std::endl;
				}

				std::cout << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << "SAVE AUTOMAT:" << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << std::endl;
				std::cout << "filename: ";

				std::cin >> saveFile;
				a->save_automat((char *)saveFile.c_str());


				std::cout << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << "READ AUTOMAT:" << std::endl;
				std::cout << " -------" << std::endl;
				std::cout << std::endl;
				std::cout << "filename: ";

				std::cin >> saveFile;
				a->read_automat((char *)saveFile.c_str());

			}
		}
		else
		{
			printf("blad1\n");
		}
		t2 = clock();
	}
	else
	{

		printf("blad2");

	}

	fgetc(stdin);
	return EXIT_SUCCESS;
}