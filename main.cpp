#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
using namespace std;

class Card {
public:
    Card() : face(""), suit('\0') {};
    Card(string face, char suit) : face(face), suit(suit) {};
    const string getFace();
    const char getSuit();
private:
    string face;
    char suit;
};

class Player {
public:
    Player(int selfIndex, int takeFrom, vector<Card> hand) :
            selfIndex(selfIndex), takeFrom(takeFrom), hand(hand), discardIndex(-1) {};
    virtual void chooseDiscard(int);
    void replenish();
    vector<Card> getHand();
    const void printHand();
protected:
    static  Card discardList[];

    int selfIndex;   	//index within player vector, starting with 0 for the human
    int takeFrom;       //player supplying new Card
    vector<Card> hand;  //holds 7 cards

    Card discard;       //card being discarded
    int discardIndex;   //position within hand of card being discarded

    virtual ~Player() {};
private:

};

class AutoPlayer: public Player {
public:
    AutoPlayer(int selfIndex, int takeFrom, vector<Card> hand) :
            Player(selfIndex, takeFrom, hand) {};
    void chooseDiscard(int);
private:
};

void createShuffledDeck(vector<Card>&);
void initializePlayers(vector<Player *>&, vector<Card>&);
bool isGameOver(vector<Player*>&);

Card Player::discardList[5];

int main() {
    vector<Card> deck;
    vector<Player *> players;

    srand(time(NULL));
    createShuffledDeck(deck);
    initializePlayers(players, deck);

    int discardIndex;
    while(!isGameOver(players)) {
        cout << "Your hand" << endl;
        players[0]->printHand();
        cout << "  \t\t";
        for(size_t i = 0; i < 7; i ++)
            cout << (char)(97 + i) << "\t";
        cout << endl;

        do {
            cout << "cmd: ";
            string s;
            getline(cin, s);
            discardIndex = (int)s[0] - 97;

            if(s.length() != 1)
                cout << "Invalid choice" << endl;
            else if (discardIndex == -34) {
                for (size_t i = 0; i < players.size(); i++) {
                    players[i]->printHand();
                }
            } else if(discardIndex > -1 && discardIndex < 7)
                break;
            else
                cout << "Invalid choice" << endl;
        } while(true);

        players[0]->chooseDiscard(discardIndex);
        for(size_t i = 1; i < players.size(); i ++) {
            players[i]->chooseDiscard(-1);
        }

        for(size_t i = 0; i < players.size(); i ++) {
            players[i]->replenish();
        }

    }
}

bool isGameOver(vector<Player*>& players) {
    bool gameOver = false;
    for(size_t i = 0; i < players.size(); i ++) {
        bool sameSuit = true;
        for(size_t j = 1; j < players[0]->getHand().size(); j ++) {
            if(players[i]->getHand()[0].getSuit() != players[i]->getHand()[j].getSuit())
                sameSuit = false;
        }
        if(sameSuit) {
            gameOver = true;
            cout << "Player " << i << " is a winner!" << endl;
        } else
            sameSuit = false;
    }

    if(gameOver) {
        cout << "At the end of the game the players had:" << endl;
        for(size_t i = 0; i < players.size(); i ++) {
            cout << i << ": \t\t";
            for(size_t j = 0; j < players[0]->getHand().size(); j ++) {
                Card jCard = players[i]->getHand()[j];
                cout << jCard.getSuit() << jCard.getFace() << "\t";
            }
            cout << endl;
        }
    }

    return gameOver;
}

void initializePlayers(vector<Player *>& players, vector<Card>& deck) {
    vector<Card> temp;
    temp.assign(deck.begin(), deck.begin() + 7);
    deck.erase(deck.begin(), deck.begin() + 7);

    players.push_back(new Player(0, 4, temp));

    for(size_t i = 1; i <= 4; i ++) {
        temp.assign(deck.begin(), deck.begin() + 7);
        deck.erase(deck.begin(), deck.begin() + 7);
        players.push_back(new AutoPlayer(i, i - 1, temp));
    }
}

void createShuffledDeck(vector<Card>& deck) {
    char suit;
    string face;
    for (int i = 0; i < 52; i++) {
        switch (i % 4) {
            case (0):
                suit = 'S';
                break;
            case (1):
                suit = 'H';
                break;
            case (2):
                suit = 'D';
                break;
            case (3):
                suit = 'C';
        }

        switch (i % 13) {
            case (12):
                face = "K";
                break;
            case (11):
                face = "Q";
                break;
            case (10):
                face = "J";
                break;
            default:
                face = to_string((i + 1) % 13);

        }

        deck.push_back(Card(face, suit));
    }
    random_shuffle(deck.begin(), deck.end());
}

const string Card::getFace() {
    return face;
}

const char Card::getSuit() {
    return suit;
}

vector<Card> Player::getHand() {
    return hand;
}

const void Player::printHand() {
    cout << selfIndex << ":\t\t";
    for (size_t i = 0; i < hand.size(); i++) {
        Card iCard = hand[i];
        cout << iCard.getSuit() << iCard.getFace() << "\t";
    }
    cout << endl;
}

void Player::replenish() {
    Card* card = &discardList[selfIndex];
    cout << "\t\tPlayer " << selfIndex << " gave " << (selfIndex + 1) % 5 << " " << card->getSuit() << card->getFace() << endl;
    hand.insert(hand.begin() + discardIndex, discardList[takeFrom]);
}

void Player::chooseDiscard(int discardIndex) {
    this->discardIndex = discardIndex;
    discardList[selfIndex] = hand[discardIndex];
    hand.erase(hand.begin() + discardIndex);
}

void AutoPlayer::chooseDiscard(int discardIndex) {

    vector<int> chars(4, 0);

    for (size_t i = 0; i < hand.size(); i++) {
        char cardSuit = hand[i].getSuit();
        if (cardSuit == 'S')
            chars[0]++;
        else if (cardSuit == 'H')
            chars[1]++;
        else if (cardSuit == 'D')
            chars[2]++;
        else
            chars[3]++;
    }

    int lowestIndex = -1;
    for (size_t i = 0; i < chars.size(); i++) {
        if (chars[i] != 0) {
            if (lowestIndex == -1) {
                lowestIndex = i;
            } else if (chars[i] < chars[lowestIndex])
                lowestIndex = i;
        }
    }

    char discardSuit;
    if (lowestIndex == 0)
        discardSuit = 'S';
    else if (lowestIndex == 1)
        discardSuit = 'H';
    else if (lowestIndex == 2)
        discardSuit = 'D';
    else
        discardSuit = 'C';

    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i].getSuit() == discardSuit) {
            this->discardIndex = i;
            break;
        }
    }

    discardList[selfIndex] = hand[this->discardIndex];
    hand.erase(hand.begin() + this->discardIndex);
}