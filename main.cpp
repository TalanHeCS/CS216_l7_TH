//Talan He
//Lab 7

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// base class representing a generic creature with strength, health, and a name
class Creature {
protected:
    int strength;
    int health;
    string name;

public:
    // default constructor, sets fields to invalid placeholder values
    Creature() : strength(-1), health(-1), name("invalid") {}

    // constructor that builds a creature from given strength, health, and name
    Creature(int s, int h, const string& n) : strength(s), health(h), name(n) {}

    // virtual destructor, resets fields; virtual so derived class destructors run correctly
    virtual ~Creature() {
        strength = -1;
        health = -1;
        name = "invalid";
    }

    // creates a heap copy of this object as its actual derived type
    // overridden in each subclass so copies keep the correct type
    virtual Creature* clone() const {
        return new Creature(*this);
    }

    // updates all three fields at once
    void setCreature(int s, int h, const string& n) {
        strength = s;
        health = h;
        name = n;
    }

    // accessor for strength
    int getStrength() const { return strength; }

    // accessor for health
    int getHealth() const { return health; }

    // accessor for the plain, unmodified name
    string getRawName() const { return name; }

    // returns the type of creature as a string, overridden by each subclass
    virtual string type() const {
        return "Creature";
    }

    // returns the display name including creature type, overridden by each subclass
    virtual string getName() const {
        return name + " the Creature";
    }

    // calculates random damage based on strength, overridden by subclasses to
    // add their own bonus effects
    virtual int getDamage() const {
        if (strength <= 0) return 0;
        return (rand() % strength) + 1;
    }

    // reduces health by the given damage amount, clamped at 0
    void takeDamage(int d) {
        health -= d;
        if (health < 0) health = 0;
    }

    // randomizes strength and health within a fixed range, used to reset a creature
    void reset() {
        strength = (rand() % 121) + 30; // 30 to 150
        health = (rand() % 121) + 30;
    }

    // builds a formatted string with the creature's name, health, and strength
    virtual string toString() const {
        ostringstream out;
        out << left << setw(20) << getName()
            << setw(10) << health
            << setw(10) << strength;
        return out.str();
    }
};

// demon subclass, has a chance to deal bonus damage on each attack
class Demon : public Creature {
public:
    // default constructor, defers to base class
    Demon() : Creature() {}

    // constructor with values, defers to base class
    Demon(int s, int h, const string& n) : Creature(s, h, n) {}

    // returns a heap copy of this object as a Demon
    Creature* clone() const override {
        return new Demon(*this);
    }

    // identifies this creature's type as "Demon"
    string type() const override {
        return "Demon";
    }

    // returns the display name for a demon
    string getName() const override {
        return name + " the Demon";
    }

    // base damage plus a 15% chance of an extra 40 damage
    int getDamage() const override {
        int damage = Creature::getDamage();
        if ((rand() % 100) < 15) {
            damage += 40;
        }
        return damage;
    }
};

// balrog subclass, a stronger type of demon that attacks twice per turn
class Balrog : public Demon {
public:
    // default constructor, defers to Demon
    Balrog() : Demon() {}

    // constructor with values, defers to Demon
    Balrog(int s, int h, const string& n) : Demon(s, h, n) {}

    // returns a heap copy of this object as a Balrog
    Creature* clone() const override {
        return new Balrog(*this);
    }

    // identifies this creature's type as "Balrog"
    string type() const override {
        return "Balrog";
    }

    // returns the display name for a balrog
    string getName() const override {
        return name + " the Balrog";
    }

    // damage is the sum of two separate demon-style damage rolls
    int getDamage() const override {
        return Demon::getDamage() + Demon::getDamage();
    }
};

// elf subclass, has a small chance to double its damage
class Elf : public Creature {
public:
    // default constructor, defers to base class
    Elf() : Creature() {}

    // constructor with values, defers to base class
    Elf(int s, int h, const string& n) : Creature(s, h, n) {}

    // returns a heap copy of this object as an Elf
    Creature* clone() const override {
        return new Elf(*this);
    }

    // identifies this creature's type as "Elf"
    string type() const override {
        return "Elf";
    }

    // returns the display name for an elf
    string getName() const override {
        return name + " the Elf";
    }

    // base damage with a 1 in 20 chance of being doubled
    int getDamage() const override {
        int damage = Creature::getDamage();
        if ((rand() % 20) == 0) {
            damage *= 2;
        }
        return damage;
    }
};

// cyberelf subclass, a stronger elf with an additional chance of bonus damage
class Cyberelf : public Elf {
public:
    // default constructor, defers to Elf
    Cyberelf() : Elf() {}

    // constructor with values, defers to Elf
    Cyberelf(int s, int h, const string& n) : Elf(s, h, n) {}

    // returns a heap copy of this object as a Cyberelf
    Creature* clone() const override {
        return new Cyberelf(*this);
    }

    // identifies this creature's type as "Cyberelf"
    string type() const override {
        return "Cyberelf";
    }

    // returns the display name for a cyberelf
    string getName() const override {
        return name + " the Cyberelf";
    }

    // elf-style damage plus a 30% chance of an extra 50 damage
    int getDamage() const override {
        int damage = Elf::getDamage();
        if ((rand() % 100) < 30) {
            damage += 50;
        }
        return damage;
    }
};

// manages a dynamically allocated collection of creatures belonging to one army
class Army {
private:
    Creature** members;
    int count;
    string armyName;

    // creates one random creature of a random subtype, using the given name
    Creature* makeRandomCreature(const string& nm) {
        int s = (rand() % 231) + 45; // 45 to 275
        int h = (rand() % 231) + 45;

        int t = rand() % 4;
        if (t == 0) return new Demon(s, h, nm);
        if (t == 1) return new Balrog(s, h, nm);
        if (t == 2) return new Elf(s, h, nm);
        return new Cyberelf(s, h, nm);
    }

    // reads the next name from the file, or generates a placeholder name
    // if the file runs out of lines
    string readName(ifstream& fin, int index) {
        string nm;
        if (getline(fin, nm)) return nm;
        return "Name" + to_string(index + 1);
    }

public:
    // default constructor, creates an empty army with no members
    Army() : members(nullptr), count(0), armyName("none") {}

    // constructor that builds an army of the given size immediately
    Army(int n, const string& nme, const string& fileName) : members(nullptr), count(0), armyName(nme) {
        createArmy(n, nme, fileName);
    }

    // copy constructor, deep-copies each member using clone() so the new
    // army owns its own creature objects
    Army(const Army& other) : members(nullptr), count(0), armyName(other.armyName) {
        count = other.count;
        if (count > 0) {
            members = new Creature*[count];
            for (int i = 0; i < count; i++) {
                members[i] = other.members[i]->clone();
            }
        }
    }

    // assignment operator, frees existing members first, then deep-copies
    // the other army's members; guards against self-assignment
    Army& operator=(const Army& other) {
        if (this != &other) {
            clear();
            armyName = other.armyName;
            count = other.count;
            if (count > 0) {
                members = new Creature*[count];
                for (int i = 0; i < count; i++) {
                    members[i] = other.members[i]->clone();
                }
            }
        }
        return *this;
    }

    // destructor, releases all dynamically allocated creatures
    ~Army() {
        clear();
    }

    // deletes every creature pointer and the array itself, resets state to empty
    void clear() {
        if (members != nullptr) {
            for (int i = 0; i < count; i++) {
                delete members[i];
            }
            delete[] members;
        }
        members = nullptr;
        count = 0;
    }

    // builds an army of n random creatures, pulling names from the given file
    void createArmy(int n, const string& nme, const string& fileName) {
        clear();
        armyName = nme;
        count = n;

        members = new Creature*[count];

        ifstream fin(fileName.c_str());

        for (int i = 0; i < count; i++) {
            string nm = readName(fin, i);
            members[i] = makeRandomCreature(nm);
        }
    }

    // returns the number of creatures in the army
    int size() const {
        return count;
    }

    // returns the army's name
    string getArmyName() const {
        return armyName;
    }

    // returns a pointer to the creature at the given index, or nullptr if out of range
    Creature* getCreature(int i) const {
        if (i < 0 || i >= count) return nullptr;
        return members[i];
    }

    // sums the health of every creature currently in the army
    int totalHealth() const {
        int sum = 0;
        for (int i = 0; i < count; i++) {
            sum += members[i]->getHealth();
        }
        return sum;
    }

    // prints a formatted table listing every creature's name, type, strength, and health
    void printStats() const {
        cout << "\n" << armyName << " Stats\n";
        cout << left
             << setw(20) << "Creature"
             << setw(15) << "Type"
             << setw(12) << "Strength"
             << setw(12) << "Health" << endl;

        for (int i = 0; i < count; i++) {
            cout << left
                 << setw(20) << members[i]->getRawName()
                 << setw(15) << members[i]->type()
                 << setw(12) << members[i]->getStrength()
                 << setw(12) << members[i]->getHealth() << endl;
        }
    }
};

// controls the overall flow of a game session, including setup and battling
class Game {
private:
    // simulates a fight between two creatures, alternating attacks until
    // one of them reaches 0 health
    void duel(Creature* a, Creature* b, const string& armyA, const string& armyB) {
        while (a->getHealth() > 0 && b->getHealth() > 0) {
            int dmgA = a->getDamage();
            int beforeB = b->getHealth();
            b->takeDamage(dmgA);

            cout << a->getName() << " from " << armyA
                 << " attacks " << b->getName() << " from " << armyB
                 << " for " << dmgA
                 << " damage.  " << beforeB << " -> " << b->getHealth() << endl;

            if (b->getHealth() <= 0) break;

            int dmgB = b->getDamage();
            int beforeA = a->getHealth();
            a->takeDamage(dmgB);

            cout << b->getName() << " from " << armyB
                 << " attacks " << a->getName() << " from " << armyA
                 << " for " << dmgB
                 << " damage.  " << beforeA << " -> " << a->getHealth() << endl;
        }
    }

public:
    // runs one full game: gathers setup input, builds both armies, prints
    // their stats, runs duels between matching positions, then reports
    // total health and declares a winner
    void play() {
        string fileName, army1Name, army2Name;
        int army1Size, army2Size;

        cout << "Enter names file: ";
        cin >> fileName;

        cout << "Enter Army #1 name: ";
        cin >> army1Name;
        cout << "Enter Army #1 size: ";
        cin >> army1Size;

        cout << "Enter Army #2 name: ";
        cin >> army2Name;
        cout << "Enter Army #2 size: ";
        cin >> army2Size;

        Army army1(army1Size, army1Name, fileName);
        Army army2(army2Size, army2Name, fileName);

        army1.printStats();
        army2.printStats();

        // only fight as many rounds as the smaller army can support
        int smaller = (army1.size() < army2.size()) ? army1.size() : army2.size();

        cout << "\nBattle Begins\n";
        for (int i = 0; i < smaller; i++) {
            if (army1.getCreature(i)->getHealth() > 0 && army2.getCreature(i)->getHealth() > 0) {
                cout << "\nRound " << (i + 1) << endl;
                duel(army1.getCreature(i), army2.getCreature(i), army1.getArmyName(), army2.getArmyName());
            }
        }

        cout << "\n" << army1.getArmyName() << " total health: " << army1.totalHealth() << endl;
        cout << army2.getArmyName() << " total health: " << army2.totalHealth() << endl;

        // winner is decided by comparing total remaining health across both armies
        if (army1.totalHealth() > army2.totalHealth()) {
            cout << army1.getArmyName() << " wins!\n";
        } else if (army2.totalHealth() > army1.totalHealth()) {
            cout << army2.getArmyName() << " wins!\n";
        } else {
            cout << "Tie game!\n";
        }

        army1.printStats();
        army2.printStats();
    }
};

// options for the main menu
enum Menu {
    PLAY_GAME = 1,
    QUIT_GAME = 2
};

int main() {
    // seeds the random number generator once, using the current time
    srand(static_cast<unsigned int>(time(nullptr)));

    Game game;
    int choice;

    // main program loop; wrapped in a try block to catch allocation failures
    // from the dynamic arrays used inside Army
    try {
        while (true) {
            cout << "\n1. Play Game\n";
            cout << "2. Quit\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                game.play();
            }
            else if (choice == 2) {
                break;
            }
            else {
                cout << "invalid choice\n";
            }
        }
    }
    catch (bad_alloc&) {
        cout << "memory allocation failed";
    }

    return 0;
}
