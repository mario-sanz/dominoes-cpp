// Mario Sanz, 1ºI
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
using namespace std;

const int Max = 55;
const int PlayersNum = 4;

const bool Hiden = false;
const bool HumanPlayer = true;

struct tTile {
	short int n1;
	short int n2;
};
typedef tTile tTileArray[Max];

struct tTileList {
	tTileArray tiles;
	short int counter;
};
typedef tTileList tPlayersTiles[PlayersNum];

typedef int tPlayersPoints[PlayersNum];

struct tGame {
	tTileList deck;
	tPlayersTiles players;
	tPlayersPoints points;
	short int numPlayers;
	short int maxPoints;
	string lane;
};

// prototypes
short int menu();
string tileToStr(tTile tile);
void displayTable(const tGame& game);
string digitToStr(int digit);
bool canGoLeft(string lane, tTile tile);
bool canGoRight(string lane, tTile tile);
void placeTileLeft(string& lane, tTile tile);
void placeTileRight(string& lane, tTile tile);
void readTileList(ifstream& inFile, tTileList& tileList);
bool readGame(tGame& game);
void writeTileList(ofstream& outFile, const tTileList& tileList);
void writeGame(const tGame& game);
void generate(tTileList& deck, int maxPoints);
void shuffle(tTileList& deck);
bool getTile(tTileList& deck, tTile& tile);
void delTile(tTileList& deck, int index);
int getPoints(const tTileList& player);
bool canPlaceTile(const tTileList& player, string lane);
int whoStarts(const tGame& game, int& index);
bool strategy1(tGame& game, int player);
bool strategy2(tGame& game, int player);
bool strategy3(tGame& game, int player);


int main() {
	srand(time(NULL)); // for the random position of tiles
	int menu_option = 1, op_tileNum, doubleTile_index = 0, startingPlayer, actualPlayer = 0, deckInCount = 0, noPossibleMoves = 0;
	string tileStr, laneStr = "", old_lane, fileName_save, fileName_previous;
	bool placetileYN = false, nextPlayer = false, newRound = true;
	char previousGame, saveGame, playAgainYN;
	ofstream outputFile;
	ifstream inputFile;
	tGame game;

	cout << "Welcome to..." << endl;
	cout << "-------------------" << endl;
	cout << "|  D O M I N O E  |" << endl;
	cout << "-------------------" << endl << endl;

	cout << "Do you want to resume a saved game [Y/N]? ";
	cin.get(previousGame);
	previousGame = toupper(previousGame);
	cout << endl;
	if (previousGame == 'Y') // resume a saved game
	{
		while (!readGame(game))
			cout << "Error when loading the game! Try again..." << endl;
		
		newRound = false;
		actualPlayer = 0;
		nextPlayer = false;
	}
	else // start a new game from 0
	{
		cout << "Please enter the number of players [2-4]: ";
		cin >> game.numPlayers;
		cout << endl;
		while (game.numPlayers < 2 || game.numPlayers > 4)
		{
			cout << "Invalid number! Please enter the number of players [2-4]: ";
			cin >> game.numPlayers;
			cout << endl;
		}
		cout << "Please enter the maximum number for tile squares [6-9]: ";
		cin >> game.maxPoints;
		cout << endl;
		while (game.maxPoints < 6 || game.maxPoints > 9)
		{
			cout << "Invalid number! Please enter the maximum number for tile squares [6-9]: ";
			cin >> game.maxPoints;
			cout << endl;
		}
		for (int i = 0; i < game.numPlayers; i++)
		{
			game.points[i] = 0;
		}
	}

	while ((menu_option != 0 && game.players[actualPlayer].counter > 0 && noPossibleMoves < 3) || newRound == true)
	{
		if (newRound == true)
		{
			newRound = false; // first thing we do is to avoid re-generating the deck when it's not necessary
			// Initialization of counters...
			game.deck.counter = 0;
			for (int i = 0; i < game.numPlayers; i++)
				game.players[i].counter = 0;

			cout << "Generating the deck..." << endl; //prints all possible tiles
			generate(game.deck, game.maxPoints);
			deckInCount = game.deck.counter;

			cout << "Shuffling the deck..." << endl;
			do
			{
				shuffle(game.deck);

				for (int i = 0; i < game.numPlayers; i++)
				{
					for (int j = 0; j < 7; j++)
					{
						getTile(game.deck, game.players[i].tiles[j]);
						game.players[i].counter += 1;
					}
				}
				startingPlayer = whoStarts(game, doubleTile_index);
				if (startingPlayer == -1) // initialization of counters
				{
					game.deck.counter = deckInCount;
					for (int i = 0; i < game.numPlayers; i++)
						game.players[i].counter = 0;
				}
			} while (startingPlayer == -1);

			if (startingPlayer == 0)
			{
				cout << "You open with " << tileToStr(game.players[startingPlayer].tiles[doubleTile_index]) << endl << endl;
				game.lane = tileToStr(game.players[startingPlayer].tiles[doubleTile_index]);
				delTile(game.players[0], doubleTile_index);
				nextPlayer = true;
			}
			else if (startingPlayer >= 1 || startingPlayer <= 3)
			{
				cout << "Machine #" << startingPlayer << " opens with " << tileToStr(game.players[startingPlayer].tiles[doubleTile_index]) << endl << endl;
				game.lane = tileToStr(game.players[startingPlayer].tiles[doubleTile_index]);
				delTile(game.players[startingPlayer], doubleTile_index);
				nextPlayer = true;
			}

			actualPlayer = startingPlayer;

			cout << "Game starts!" << endl;
			cin.ignore();
			cout << "Press any key to continue...";
			cin.get();
			cout << endl << endl;
		}

		if (nextPlayer == true) //checks if we have to advance to the next player or not
		{
			if (actualPlayer == game.numPlayers - 1)
				actualPlayer = 0;
			else
				actualPlayer++;
		}

		displayTable(game); // <--- GAME STARTS

		switch (actualPlayer)
		{
		case 0: // PLAYER
			if (HumanPlayer == true)
			{
				menu_option = menu();
				switch (menu_option)
				{
				case 1:
					cout << "Tile number [1.." << game.players[0].counter << "]: ";
					cin >> op_tileNum;
					while ((op_tileNum > game.players[0].counter) || (op_tileNum < 1))
					{
						cout << "Invalid tile number!" << endl;
						cout << "Tile number [1.." << game.players[0].counter << "]: ";
						cin >> op_tileNum;
					}
					placetileYN = canGoLeft(game.lane, game.players[0].tiles[op_tileNum - 1]);
					if (placetileYN)
					{
						placeTileLeft(game.lane, game.players[0].tiles[op_tileNum - 1]);
						delTile(game.players[0], op_tileNum - 1);
						nextPlayer = true;
						noPossibleMoves = 0;
					}
					else // ...if tile is not placed...
					{
						cout << "This tile can't be positioned in the left!" << endl;
						cin.ignore();
						cout << "Press any key to continue...";
						cin.get();
						nextPlayer = false;
					}
					break;
				case 2:
					cout << "Tile number [1.." << game.players[0].counter << "]: ";
					cin >> op_tileNum;
					while ((op_tileNum > game.players[0].counter) || (op_tileNum < 1))
					{
						cout << "Invalid tile number!" << endl;
						cout << "Tile number [1.." << game.players[0].counter << "]: ";
						cin >> op_tileNum;
					}
					placetileYN = canGoRight(game.lane, game.players[0].tiles[op_tileNum - 1]);
					if (placetileYN)
					{
						placeTileRight(game.lane, game.players[0].tiles[op_tileNum - 1]);
						delTile(game.players[0], op_tileNum - 1);
						nextPlayer = true;
						noPossibleMoves = 0;
					}
					else // ...if tile is not placed...
					{
						cout << "This tile can't be positioned in the right!" << endl;
						cin.ignore();
						cout << "Press any key to continue...";
						cin.get();
						nextPlayer = false;
					}
					break;
				case 3:
					placetileYN = canPlaceTile(game.players[0], game.lane);

					if (!placetileYN)
					{
						if (getTile(game.deck, game.players[0].tiles[game.players[0].counter]))
						{
							game.players[0].counter += 1;
							nextPlayer = false;
						}
						else
						{
							cout << "No more tiles on deck! Next player..." << endl;
							nextPlayer = true;
							noPossibleMoves++;
						}
					}
					else
					{
						cout << "Revise your lane! There are tiles that can be placed!" << endl;
						cin.ignore();
						cout << "Press any key to continue...";
						cin.get();
						nextPlayer = false;
					}

					break;
					// We don't need to write the case 0 and the default because the menu() function will enter a loop until the option selected is 1, 2, 3, or 0, so this 4 are the only possible cases
				}
			}
			else // HumanPlayer == false
			{
				cout << "Player (machine) turn..." << endl;

				if (strategy3(game, 0))
				{
					cout << "Next!" << endl;
					nextPlayer = true;
					noPossibleMoves = 0;
				}
				else
				{
					if (getTile(game.deck, game.players[0].tiles[game.players[0].counter]))
					{
						cout << "New tile..." << endl;
						game.players[0].counter += 1;
						nextPlayer = false;
					}
					else
					{
						cout << "No more tiles on deck! Next player..." << endl;
						nextPlayer = true;
						noPossibleMoves++;
					}
				}

				cout << "Press any key to continue...";
				cin.get();
			}
			
			break;
		case 1: // MACHINE #1
			cout << "Machine #1 turn..." << endl;
			
			if (strategy2(game, 1))
			{
				cout << "Next!" << endl;
				nextPlayer = true;
				noPossibleMoves = 0;
			}
			else
			{
				if (getTile(game.deck, game.players[1].tiles[game.players[1].counter]))
				{
					cout << "New tile..." << endl;
					game.players[1].counter += 1;
					nextPlayer = false;
				}
				else
				{
					cout << "No more tiles on deck! Next player..." << endl;
					nextPlayer = true;
					noPossibleMoves++;
				}
			}
			
			if (HumanPlayer == true) // just for style details
			{
				cout << "Press any key to continue...";
				cin.ignore();
				cin.get();
			}
			else
			{
				cout << "Press any key to continue...";
				cin.get();
			}

			break;
		case 2: // MACHINE #2
			cout << "Machine #2 turn..." << endl;
			
			if (strategy1(game, 2))
			{
				cout << "Next!" << endl;
				nextPlayer = true;
				noPossibleMoves = 0;
			}
			else
			{
				if (getTile(game.deck, game.players[2].tiles[game.players[2].counter]))
				{
					cout << "New tile..." << endl;
					game.players[2].counter += 1;
					nextPlayer = false;
				}
				else
				{
					cout << "No more tiles on deck! Next player..." << endl;
					nextPlayer = true;
					noPossibleMoves++;
				}
			}

			cout << "Press any key to continue...";
			cin.get();

			break;
		case 3: // MACHINE #3
			cout << "Machine #3 turn..." << endl;

			if (strategy2(game, 3))
			{
				cout << "Next!" << endl;
				nextPlayer = true;
				noPossibleMoves = 0;
			}
			else
			{
				if (getTile(game.deck, game.players[3].tiles[game.players[3].counter]))
				{
					cout << "New tile..." << endl;
					game.players[3].counter += 1;
					nextPlayer = false;
				}
				else
				{
					cout << "No more tiles on deck! Next player..." << endl;
					nextPlayer = true;
					noPossibleMoves++;
				}
			}

			cout << "Press any key to continue...";
			cin.get();
			break;
		}

		cout << endl << endl; // Spaces to differenciate one round to another

		if (game.players[actualPlayer].counter == 0 || noPossibleMoves == 3)
		{
			if (game.players[actualPlayer].counter == 0)
			{
				displayTable(game);
				if (actualPlayer == 0)
				{
					cout << setw(50) << right << "-------------------" << endl;
					cout << setw(50) << right << "|  Y O U   W I N  |" << endl;
					cout << setw(50) << right << "-------------------" << endl << endl;
					cin.ignore();
				}
				else if (actualPlayer >= 1 && actualPlayer <= 3)
				{
					cout << setw(50) << right << "----------------------------------" << endl;
					cout << setw(35) << right << "|  M A C H I N E  #" << actualPlayer << "    W I N S  |" << endl;
					cout << setw(50) << right << "----------------------------------" << endl << endl;
				}
			}
			else if (noPossibleMoves == 3)
			{
				cout << endl << "*** No more possible moves for any of the players and there are no more tiles on the deck, so this round is over! ***" << endl << endl;
			}
			
			cout << "Scoring..." << endl;
			// Player's scoring...
			cout << "Player" << setw(24) << right << game.points[0] << setw(2) << "+" << setw(6) << getPoints(game.players[0]) << setw(2) << "=" << setw(6) << (game.points[0] + getPoints(game.players[0])) << endl;
			game.points[0] += getPoints(game.players[0]);
			// Machine's scoring...
			for (int i = 1; i < game.numPlayers; i++)
			{
				cout << "Machine #" << i << setw(20) << right << game.points[i] << setw(2) << "+" << setw(6) << getPoints(game.players[i]) << setw(2) << "=" << setw(6) << (game.points[i] + getPoints(game.players[i])) << endl;
				game.points[i] += getPoints(game.players[i]);
			}

			cout << endl << "Do you want to play another round [Y/N]? ";
			cin.get(playAgainYN);
			playAgainYN = toupper(playAgainYN);

			if (playAgainYN == 'Y')
				newRound = true;
			else
				newRound = false;
		}		
	}

	if (menu_option == 0)
	{
		cout << "Do you want to save the game [Y/N]? ";
		cin.get();
		cin.get(saveGame);
		saveGame = toupper(saveGame);
		cout << endl;

		if (saveGame == 'Y')
			writeGame(game);
	}
	else
		cout << endl << "Thank you for playing!" << endl << endl;

	return 0;
}

short int menu() {
	short int option;
	do
	{
		cout << " ------------------ " << endl;
		cout << "|  O P T I O N S  |" << endl;
		cout << " ------------------ " << endl;
		cout << "1. Place tile to the left" << endl;
		cout << "2. Place tile to the right" << endl;
		cout << "3. Next tile" << endl;
		cout << "0. Exit" << endl << endl;
		cout << "Your option: ";
		cin >> option;

	} while (option < 0 || option > 3);

	return option;
}

string tileToStr(tTile tile) {
	string strTile, leftStr, rightStr;
	leftStr = digitToStr(tile.n1);
	rightStr = digitToStr(tile.n2);
	strTile = "|" + leftStr + "-" + rightStr + "|";
	return strTile;
}

void displayTable(const tGame& game) { 

	cout << " ------------------ " << endl;
	cout << "|    T A B L E    |" << endl;
	cout << " ------------------ " << endl;
	cout << game.lane << endl << endl;
	if (Hiden == true)
	{
		for (int i = 1; i < game.numPlayers; i++) // i = 1 because we don't count the human player
		{
			cout << "[" << setw(3) << right << game.points[i] << "] Machine #" << i << setw(4) << game.players[i].counter << " tile(s)" << endl;
		}
		cout << endl;
	}
	else
	{
		for (int i = 1; i < game.numPlayers; i++) // player
		{
			cout << "[" << setw(3) << right << game.points[i] << "] Machine #" << i;
			if (game.players[i].counter > 0)
				cout << setw(8) << right;

			for (int j = 0; j < game.players[i].counter; j++) // tile of the player
			{
				cout << tileToStr(game.players[i].tiles[j]) << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	
	cout << "[" << setw(3) << right << game.points[0] << setw(15) << left << "] Player";
	for (int i = 0; i < game.players[0].counter; i++)
	{
		cout << tileToStr(game.players[0].tiles[i]) << " ";
	}

	cout << endl << endl;
}

string digitToStr(int digit) { 
	string strDigit;
	strDigit = to_string(digit);
	return strDigit;
}

bool canGoLeft(string lane, tTile tile) {
	bool leftYN;
	if (tile.n1 == lane[1] - '0' || tile.n2 == lane[1] - '0') // - '0' to pass it to ascii code
		leftYN = true;
	
	else
		leftYN = false;
	
	return leftYN;
}

bool canGoRight(string lane, tTile tile) {
	bool rightYN;
	if (tile.n1 == lane[lane.length() - 2] - '0' || tile.n2 == lane[lane.length() - 2] - '0') // - '0' to pass it to ascii code
		rightYN = true;
	
	else
		rightYN = false;
	
	return rightYN;
}

void placeTileLeft(string& lane, tTile tile) {
	short int aux;
	if (tile.n1 == lane[1] - '0') // - '0' to pass it to ascii code
	{
		aux = tile.n2; // we do the changes on the order of the tile numbers
		tile.n2 = tile.n1;
		tile.n1 = aux;
		lane = tileToStr(tile) + lane;
	}
	else if (tile.n2 == lane[1] - '0') // - '0' to pass it to ascii code
	{
		lane = tileToStr(tile) + lane;
	}
}

void placeTileRight(string& lane, tTile tile) {
	short int aux;
	if (tile.n1 == lane[lane.length() - 2] - '0') // - '0' to pass it to ascii code
	{
		lane = lane + tileToStr(tile);
	}
	else if (tile.n2 == lane[lane.length() - 2] - '0') // - '0' to pass it to ascii code
	{
		aux = tile.n1; // we do the changes on the order of the tile numbers
		tile.n1 = tile.n2;
		tile.n2 = aux;
		lane = lane + tileToStr(tile);
	}
}

void readTileList(ifstream& inFile, tTileList& tileList) {
	for (int i = 0; i < tileList.counter; i++)
	{
		inFile >> tileList.tiles[i].n1 >> tileList.tiles[i].n2;
	}
}

bool readGame(tGame& game) {
	bool gameOpenYN = false;
	string fileName_previous;
	ifstream inputFile;

	cout << "Introduce the name of your saved game (without .txt): ";
	cin >> fileName_previous;
	fileName_previous = fileName_previous + ".txt";
	cout << endl;

	inputFile.open(fileName_previous);
	if (inputFile.is_open())
	{
		inputFile >> game.numPlayers >> game.maxPoints >> game.lane >> game.deck.counter;
		readTileList(inputFile, game.deck);
		for (int i = 0; i < game.numPlayers; i++)
		{
			inputFile >> game.players[i].counter;
			readTileList(inputFile, game.players[i]);
			inputFile >> game.points[i];
		}
		gameOpenYN = true;
		inputFile.close();
	}

	return gameOpenYN;
}

void writeTileList(ofstream& outFile, const tTileList& tileList) {
	for (int i = 0; i < tileList.counter; i++)
	{
		outFile << tileList.tiles[i].n1 << " " << tileList.tiles[i].n2 << " ";
	}
}

void writeGame(const tGame& game) {
	string fileName_save;
	ofstream outputFile;

	cout << "Introduce the name of your game (without spaces & without .txt): "; // w/o spaces to avoid file problems
	cin >> fileName_save;
	fileName_save = fileName_save + ".txt";

	outputFile.open(fileName_save);
	if (outputFile.is_open())
	{
		outputFile << game.numPlayers << " " << game.maxPoints << endl;
		outputFile << game.lane << endl;
		outputFile << game.deck.counter << endl;
		writeTileList(outputFile, game.deck);
		for (int i = 0; i < game.numPlayers; i++)
		{
			outputFile << endl << game.players[i].counter << endl;
			writeTileList(outputFile, game.players[i]);
			outputFile << endl << game.points[i];
		}

		cout << endl << "Game saved! Thank you for playing!" << endl << endl;
		outputFile.close();
	}
	else
	{
		cout << "Error at saving the game. Exiting..." << endl << endl;
	}
}

void generate(tTileList& deck, int maxPoints) {
	int position = 0;

	for (int i = 0; i <= maxPoints; i++)
	{
		for (int j = maxPoints; j >= i; j--)
		{
			deck.tiles[position].n1 = i;
			deck.tiles[position].n2 = j;
			cout << tileToStr(deck.tiles[position]);
			position++;
			deck.counter++;
		}
		cout << endl;
	}
}

void shuffle(tTileList& deck) {
	int idx;
	short int tmp1, tmp2;

	for (int i = deck.counter - 1; i >= 0; i--)
	{
		idx = rand() % (i + 1);
		if (idx != i)
		{
			tmp1 = deck.tiles[i].n1;
			tmp2 = deck.tiles[i].n2;
			deck.tiles[i].n1 = deck.tiles[idx].n1;
			deck.tiles[i].n2 = deck.tiles[idx].n2;
			deck.tiles[idx].n1 = tmp1;
			deck.tiles[idx].n2 = tmp2;
		}
	}
}

bool getTile(tTileList& deck, tTile& tile) {
	bool gotTile = false;

	if (deck.counter >= 1)
	{
		tile.n1 = deck.tiles[deck.counter - 1].n1;
		tile.n2 = deck.tiles[deck.counter - 1].n2;
		deck.counter -= 1;
		gotTile = true;
	}
	
	return gotTile;
}

void delTile(tTileList& list, int index) {
	for (int i = index; i < list.counter; i++)
	{
		list.tiles[i].n1 = list.tiles[i + 1].n1;
		list.tiles[i].n2 = list.tiles[i + 1].n2;
	}
	list.counter -= 1;
}

int getPoints(const tTileList& player) {
	int points = 0;
	for (int i = 0; i < player.counter; i++)
	{
		points = points + player.tiles[i].n1 + player.tiles[i].n2;
	}
	return points;
}

bool canPlaceTile(const tTileList& player, string lane) {
	bool placetileYN = false, leftYN = false, rightYN = false;
	int index = 0;

	while ((index < player.counter) && (leftYN == false) && (rightYN == false))
	{
		leftYN = canGoLeft(lane, player.tiles[index]);
		rightYN = canGoRight(lane, player.tiles[index]);
		index += 1;
	} //if one of both variables is true, the program exits the while loop

	// now we have to see if we have exit the for loop because one of them is true or because there are no more tiles
	if ((leftYN == true) || (rightYN == true))
	{
		placetileYN = true;
	}

	return placetileYN;
}

int whoStarts(const tGame& game, int& index) {
	int whoStart = -1, max = -1;

	for (int i = 0; i < game.numPlayers; i++) // player
	{
		for (int j = 0; j < game.players[i].counter; j++) // tile of the player
		{
			if (game.players[i].tiles[j].n1 == game.players[i].tiles[j].n2) // double tile
			{
				if (game.players[i].tiles[j].n1 > max) // higher than the previous double tile found??
				{
					whoStart = i;
					index = j;
					max = game.players[i].tiles[j].n1;
				}
			}
		}
	}

	return whoStart;
}

bool strategy1(tGame& game, int player) {
	bool placedTile = false;
	int tileIndex = 0;

	while (placedTile == false && tileIndex < game.players[player].counter)
	{
		if (canGoLeft(game.lane, game.players[player].tiles[tileIndex]))
		{
			placeTileLeft(game.lane, game.players[player].tiles[tileIndex]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[tileIndex]) << " to the left..." << endl;
			delTile(game.players[player], tileIndex);
		}
		else if (canGoRight(game.lane, game.players[player].tiles[tileIndex]))
		{
			placeTileRight(game.lane, game.players[player].tiles[tileIndex]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[tileIndex]) << " to the right..." << endl;
			delTile(game.players[player], tileIndex);
		}
		else
		{
			tileIndex++;
		}
	}
	return placedTile;
}

bool strategy2(tGame& game, int player) {
	bool placedTile = false;
	int max = -1, value = 0, index = 0;
	
	for (int i = 0; i < game.players[player].counter; i++)
	{
		value = (game.players[player].tiles[i].n1) + (game.players[player].tiles[i].n2);
		if (value > max && (canGoLeft(game.lane, game.players[player].tiles[i]) || canGoRight(game.lane, game.players[player].tiles[i])))
		{
			max = value;
			index = i;
		}
	}

	if (canGoLeft(game.lane, game.players[player].tiles[index]))
	{
		placeTileLeft(game.lane, game.players[player].tiles[index]);
		placedTile = true;
		cout << "Tile " << tileToStr(game.players[player].tiles[index]) << " to the left..." << endl;
		delTile(game.players[player], index);
	}
	else if (canGoRight(game.lane, game.players[player].tiles[index]))
	{
		placeTileRight(game.lane, game.players[player].tiles[index]);
		placedTile = true;
		cout << "Tile " << tileToStr(game.players[player].tiles[index]) << " to the right..." << endl;
		delTile(game.players[player], index);
	}

	return placedTile;
}

bool strategy3(tGame& game, int player) {
	bool placedTile = false;
	int placeable = 0, remainingTiles, previousRemTiles, totalTiles = game.maxPoints + 1, userTiles = 0, index, definitiveIndex = 0, pointToStudy, sumOfPoints, prevSumOfPoints, sumTile = 0, prevSumTile, rightLane, leftLane;

	/*
	The tile that we are interested in putting in the lane is that one which tiles that can be connected with the one we are going to put are most repeated in our lane without taking into account the times that it is repeated in the deck, in other player's lane, what means:
	remainingTiles = totalTiles - userTiles - laneTiles
	Following this formula, remainingTiles gives us the times that a number is found on the rest of the player's lanes or in the deck so, the lower remainingTiles is, the better for us because it's less probable that they have that number, so that it's less probable that they can place tile, so that... We win!!
	IMPORTANT!: Our aim is that the "partner" of the placed number (the other number of the tile) is as much repeated as possible in our lane and as less repeated as possible on other player's lanes, so the pointToStudy will always be the contrary of the placed number (n1 -> n2 ; n2 -> n1)
	*/

	for (int i = 0; i < game.players[player].counter; i++) // we check if there are more than 1 placeable tiles
	{
		if (canGoLeft(game.lane, game.players[player].tiles[i]) || canGoRight(game.lane, game.players[player].tiles[i]))
		{
			placeable++;
			index = i;
		}
	}

	if (placeable > 1)
	{
		previousRemTiles = totalTiles;
		prevSumOfPoints = 0;
		leftLane = game.lane[1] - '0';
		rightLane = game.lane[game.lane.length() - 2] - '0';

		for (int i = 0; i < game.maxPoints; i++) // points
		{
			remainingTiles = totalTiles;
			userTiles = 0;
			sumOfPoints = 0;
			pointToStudy = 0;
			if (i == leftLane || i == rightLane) // first we check if that point is placeable or not
			{
				for (int y = 0; y < game.players[player].counter; y++) // check if we have that number
				{
					sumTile = 0;
					prevSumTile = 0;
					if ((game.players[player].tiles[y].n1 == i)) // If that number is placeable and he have it as n1...
					{ // ...we study its pair of the tile (n2)
						pointToStudy = game.players[player].tiles[y].n2;
						sumTile = game.players[player].tiles[y].n1 + game.players[player].tiles[y].n2;
						userTiles++;
						for (int a = 0; a < game.players[player].counter; a++) // substracts how many times is the pointToStudy in our lane
						{
							if ((pointToStudy == game.players[player].tiles[a].n1) || (pointToStudy == game.players[player].tiles[a].n2))
								remainingTiles--;
						}
					}
					else if ((game.players[player].tiles[y].n2 == i)) // If that number is placeable and he have it as n2...
					{ // ...we study its pair of the tile (n1)
						pointToStudy = game.players[player].tiles[y].n1;
						sumTile = game.players[player].tiles[y].n1 + game.players[player].tiles[y].n2;
						userTiles++;
						for (int a = 0; a < game.players[player].counter; a++) // substracts how many times is the pointToStudy in our lane
						{
							if ((pointToStudy == game.players[player].tiles[a].n1) || (pointToStudy == game.players[player].tiles[a].n2))
								remainingTiles--;
						}
					}

					if (sumTile > prevSumTile)
					{
						sumOfPoints = sumTile;
						index = y;
						prevSumTile = sumTile;
					}
				}

				for (int x = 1; x < (game.lane.length() / 5); x += 5) // substracts how many times is that point in the lane
				{
					if ((pointToStudy == game.lane[x] - '0') || (pointToStudy == game.lane[x + 2] - '0'))
						remainingTiles--;
				}

				if ((userTiles > 0) && (remainingTiles <= previousRemTiles)) // <= because i is always increasing
				{
					definitiveIndex = index;
					// we change the "previous" variables...
					previousRemTiles = remainingTiles;
					prevSumOfPoints = sumOfPoints;
				}
				else if ((userTiles > 0) && (remainingTiles == previousRemTiles))
				{ // if they are equal we place the one with the highest sumOfPoints
					if (sumOfPoints > prevSumOfPoints)
					{
						definitiveIndex = index;
						// we change the "previous" variables...
						previousRemTiles = remainingTiles;
						prevSumOfPoints = sumOfPoints;
					}
				}
			} // if the point "i" is not placeable, we just continue with the next "i"
		}
		// we place the tile...
		if (canGoLeft(game.lane, game.players[player].tiles[definitiveIndex]))
		{
			placeTileLeft(game.lane, game.players[player].tiles[definitiveIndex]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[definitiveIndex]) << " to the left..." << endl;
			delTile(game.players[player], definitiveIndex);
		}
		else if (canGoRight(game.lane, game.players[player].tiles[definitiveIndex]))
		{
			placeTileRight(game.lane, game.players[player].tiles[definitiveIndex]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[definitiveIndex]) << " to the right..." << endl;
			delTile(game.players[player], definitiveIndex);
		}
	}
	else if (placeable == 1) // if we just have 1 placeable tile, we place that tile
	{
		if (canGoLeft(game.lane, game.players[player].tiles[index]))
		{
			placeTileLeft(game.lane, game.players[player].tiles[index]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[index]) << " to the left..." << endl;
			delTile(game.players[player], index);
		}
		else if (canGoRight(game.lane, game.players[player].tiles[index]))
		{
			placeTileRight(game.lane, game.players[player].tiles[index]);
			placedTile = true;
			cout << "Tile " << tileToStr(game.players[player].tiles[index]) << " to the right..." << endl;
			delTile(game.players[player], index);
		}
	}

	return placedTile;
}
