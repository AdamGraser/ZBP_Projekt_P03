#include "Automaton.h"

int main(int argc, char **argv)
{
    clock_t t1, t2;

    if (argc == 4)
	{
        if (!strcmp(argv[1], "-m"))
        { /* make a new automaton */

			Automaton<false> *a = new Automaton<false>(argv[3], argv[2], false);
			//unsigned char *text1 = (unsigned char *)"Radom";
			//bool test1 = a->exists(text1);

			//if (test1)
			//{
			//	std::cout << "yep1" << std::endl;
			//}
			//else
			//{
			//	std::cout << "Nope1" << std::endl;
			//}

			//unsigned char *text2 = (unsigned char *)"zbita";
			//bool test2 = a->exists(text2);

			//if (test2)
			//{
			//	std::cout << "yep2" << std::endl;
			//}
			//else
			//{
			//	std::cout << "Nope2" << std::endl;
			//}

			//std::cout << " -----------" << std::endl;
			//std::cout << "List String:" << std::endl;
			//std::cout << " -----------" << std::endl;
			//a->print_strings(26, 0);
			std::cout << std::endl;
			std::cout << " -------" << std::endl;
			std::cout << "Iterator:" << std::endl;
			std::cout << " -------" << std::endl;
			
			//a->print_strings();

			for (Automaton<false>::AutomatonWordIterator it = a->wordBegin(); !it.isEnd(); it++)
			{
				std::cout << *it << std::endl;
			}


			// CUSTOM WORD SEARCH

			std::string keyword;

			const char* searchWord;
			unsigned char currentLetter;
			bool found;

			while (true)
			{
				std::cin >> keyword;
				searchWord = keyword.c_str();
				found = false;
				for (Automaton<false>::AutomatonLetterIterator<transition> it = a->begin(); !it.isEnd(); it++)
				{
					currentLetter = (unsigned char)it->b.attr;
					if (currentLetter == *searchWord)
					{
						searchWord++;
						if ((unsigned)*searchWord == 0 && it->b.term == 1)
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