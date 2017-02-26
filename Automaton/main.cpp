#include "Automaton.h"


using namespace std;
Automaton<false> *listAutomaton;
Automaton<true> *treeAutomaton;
void LoadListAutomat();
void LoadTreeAutomat();
void DisplayMainMenu();
void MainMenu();
void RunListAutomat();
void RunTreeAutomat();
int GetInput()
{
	int choice;
	cin >> choice;
	return choice;
}

void DisplayMainMenu()
{
	cout << "Menu\n";
	cout << "1 - Automaton oparty o liste\n";
	cout << "2  -Automaton oparty o drzewo\n";
	cout << "3 - Wyjscie\n";
	cout << "Wybor: ";
}

void DisplayLoadAutomatonMenu()
{
	cout << "Automaton Menu\n";
	cout << "1 - Wczytaj slownik\n";
	cout << "2 - Zaladuj automat\n";
	cout << "3 - Wstecz\n";
	cout << "Wybor: ";
}

void DisplayAutomatonMenu()
{
	cout << "Automaton Menu\n";
	cout << "1 - Szukaj slowa (ITERATOR SLOW)\n";
	cout << "2 - Szukaj slowa (ITERATOR LITER - INNA IMPLEMENTACJA)\n";
	cout << "3 - Wypisz slowa\n";
	cout << "4 - Zapisz automat\n";
	cout << "5 - Wstecz\n";
}

void LoadListAutomat()
{

	int choice = 0;
	string fileName;
	do
	{
		cout << "Wybor: ";
		system("cls");
		choice = GetInput();
		if (choice == 1)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> fileName;
			listAutomaton = new Automaton<false>((char *)fileName.c_str());
			DisplayAutomatonMenu();
			RunListAutomat();
		}
		else if (choice == 2)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> fileName;
			listAutomaton = new Automaton<false>();
			listAutomaton->read_automat((char *)fileName.c_str());
			DisplayAutomatonMenu();
			RunListAutomat();
		}

	} while (choice != 3);
	DisplayMainMenu();
	MainMenu();
}

void RunListAutomat()
{

	int choice = 0;
	string input;

	do
	{
		cout << "Wybor: ";
		system("cls");
		choice = GetInput();
		if (choice == 1)
		{
			std::cout << "szukane slowo: ";
			std::cin >> input;
			bool result = listAutomaton->exists(input);
			std::cout << std::endl;
			if (result)
			{
				std::cout << "Istnieje" << std::endl;
			}
			else
			{
				std::cout << "NIE Istnieje" << std::endl;
			}
		}
		else if(choice == 2)
		{
			std::cout << "szukane slowo: ";
			std::cin >> input;
			bool result = listAutomaton->exists(input);
			std::cout << std::endl;
			if (result)
			{
				std::cout << "Istnieje" << std::endl;
			}
			else
			{
				std::cout << "NIE Istnieje" << std::endl;
			}
		}
		else if (choice == 3)
		{
			for (Automaton<false>::AutomatonWordIterator it = listAutomaton->wordBegin(); !it.isEnd(); it++)
			{
				std::cout << *it << std::endl;
			}
		}
		else if (choice == 4)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> input;
			listAutomaton->save_automat((char *)input.c_str());
		}
	} while (choice != 5);

	DisplayLoadAutomatonMenu();
	LoadListAutomat();
}

void LoadTreeAutomat()
{

	int choice = 0;
	string fileName;
	do
	{
		cout << "Wybor: ";
		system("cls");
		choice = GetInput();
		if (choice == 1)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> fileName;
			treeAutomaton = new Automaton<true>((char *)fileName.c_str());
			DisplayAutomatonMenu();
			RunTreeAutomat();
		}
		else if (choice == 2)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> fileName;
			treeAutomaton = new Automaton<true>();
			treeAutomaton->read_automat((char *)fileName.c_str());
			DisplayAutomatonMenu();
			RunTreeAutomat();
		}

	} while (choice != 3);
	DisplayMainMenu();
	MainMenu();
}

void RunTreeAutomat()
{

	int choice = 0;
	string input;

	do
	{
		cout << "Wybor: ";
		system("cls");
		choice = GetInput();
		if (choice == 1)
		{
			std::cout << "szukane slowo: ";
			std::cin >> input;
			bool result = treeAutomaton->exists(input);
			std::cout << std::endl;
			if (result)
			{
				std::cout << "Istnieje" << std::endl;
			}
			else
			{
				std::cout << "NIE Istnieje" << std::endl;
			}
		}
		else if (choice == 2)
		{
			std::cout << "szukane slowo: ";
			std::cin >> input;
			bool result = treeAutomaton->exists2(input);
			std::cout << std::endl;
			if (result)
			{
				std::cout << "Istnieje" << std::endl;
			}
			else
			{
				std::cout << "NIE Istnieje" << std::endl;
			}
		}
		else if (choice == 3)
		{
			treeAutomaton->print_strings(treeAutomaton->letterBegin(), 0);
		}
		else if (choice == 4)
		{
			std::cout << "nazwa pliku: ";
			std::cin >> input;
			treeAutomaton->save_automat((char *)input.c_str());
		}
	} while (choice != 5);

	DisplayLoadAutomatonMenu();
	LoadTreeAutomat();
}


void MainMenu()
{
	int choice = 0;
	do
	{
		system("cls");
		choice = GetInput();
		switch (choice) {
		case 1:
			DisplayLoadAutomatonMenu();
			LoadListAutomat();
			break;
		case 2:
			DisplayLoadAutomatonMenu();
			LoadTreeAutomat();
			break;
		case 3:
			cout << "Goodbye!";
			break;

		default:
			break;
		}
	} while (choice != 3);
}

int main(int argc, char *argv[])
{

	DisplayMainMenu();
	MainMenu();

	system("PAUSE");
	return EXIT_SUCCESS;
}
