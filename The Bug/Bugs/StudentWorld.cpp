#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <cstring>
using namespace std;
bool operator<(const Coord &a, const Coord&b)
{
    if(a.x < b.x)
        return true;
    else if(a.x == b.x)
    {
        if(a.y < b.y)
            return true;
        else return false;
    }
    else return false;
}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir), currentTick(0)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}


int StudentWorld::init()
{
    currentTick = 2000;
    std::string s = getFieldFilename();

    vector<string> fileNames = getFilenamesOfAntPrograms();
    m_hillCount = (int)fileNames.size();
    for (int i = 0; i < m_hillCount; i++) {
        m_compilers[i] = new Compiler;
        antCount[i] = 0;
        antTickCount[i] = 0;
        string error;
        if ( ! m_compilers[i]->compile(fileNames[i], error)) {
            cout << "Error opening file #" << i << " " << fileNames[i] << ":" <<
            error << endl;
            setError(fileNames[0] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
    }
    
    if(fld.loadField(s))
        return GWSTATUS_LEVEL_ERROR;
    for(int i = 0; i!= VIEW_HEIGHT; i++)    // add dynamically allocated variables
    {
        for(int j = 0; j != VIEW_WIDTH; j++)
        {            
            switch(fld.getContentsOf(j, i))
            {
                case Field::grasshopper:
                {
                    Actor* ptr = new BabyGrasshopper(j, i, this);
                    addActor(j, i, ptr);
                    break;
                }
                case Field::rock:
                {
                    Actor* ptr = new Pebble(j, i, this);
                    addActor(j, i, ptr);
                    break;
                }
                case Field::food:
                {
                    Actor* ptr = new Food(j, i, 6000, this);
                    addActor(j, i, ptr);
                    break;
                }
                case Field::water:
                {
                    Actor* ptr = new WaterPool(j, i, this);
                    addActor(j, i, ptr);
                    break;
                }
                case Field::poison:
                {
                    Actor* ptr = new Poison(j, i, this);
                    addActor(j, i, ptr);
                    break;
                }
                case Field::anthill0:
                    m_anthills[0] = new AntHill(j, i, 0, m_compilers[0], this); break;
                case Field::anthill1:
                    m_anthills[1] = new AntHill(j, i, 1, m_compilers[1], this); break;
                case Field::anthill2:
                    m_anthills[2] = new AntHill(j, i, 2, m_compilers[2], this); break;
                case Field::anthill3:
                    m_anthills[3] = new AntHill(j, i, 3, m_compilers[3], this); break;
                case Field::empty:
                {
                    break;
                }
            }
        }
    }
    for (int i = 0; i < m_hillCount; i++) {
        addActor(m_anthills[i]->getX(), m_anthills[i]->getY(), m_anthills[i]);
    }
    
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::actorMove(int oldx, int oldy, int newx, int newy, Actor* actor)
{
    Entry entry1(actor, oldx, oldy);
    Entry entry2(actor, newx, newy);
    m_movefrom.push(entry1);
    m_moveto.push(entry2);
}

void StudentWorld::addNextTick(int x, int y, Actor* actor)
{
    m_moveto.push(Entry(actor, x, y));
}

void StudentWorld::addActor(int x, int y, Actor* actor)
{
    m_arr[x][y].push_back(actor);
    insertActiveCoord(x, y);
}

void StudentWorld::addFood(int x, int y, int amount)
{
    EnergyHolder* food = getFoodAt(x, y);
    food->increaseEnergyBy(amount);
}

EnergyHolder* StudentWorld::getFoodAt(int x, int y)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++){
        Actor* actor = *itr;
        if (actor->isEdible()) {
            EnergyHolder* eh = dynamic_cast<EnergyHolder*>(actor);
            return eh;
        }
    }
    EnergyHolder* actor = new Food(x, y, 0, this);
    addActor(x, y, actor);
    return actor;
}

int StudentWorld::eatFood(int x, int y, int amount)
{
    EnergyHolder* food = getFoodAt(x, y);
    int energy = food->getEnergy();
    if (amount > energy) {
        food->decreaseEnergyBy(energy);
        return energy;
    } else {
        food->decreaseEnergyBy(amount);
        return amount;
    }
}


void StudentWorld::removeActor(int x, int y, Actor* actor)
{
    m_arr[x][y].remove(actor);
    if (m_arr[x][y].empty())
        m_activeCoord.erase(Coord(x, y));
}

void StudentWorld::updateDisplay()
{
    int wn = getWinningAntNumber();
    char text[1000];
    sprintf(text, "Ticks:%5d -", currentTick);
    for (int i = 0; i < m_hillCount; i++) {
        sprintf(text+strlen(text), " %s: %02d",
                (m_compilers[i]->getColonyName() +
                ((i == wn) ? "*" : "")).c_str(),
                antCount[i]);
    }
    sprintf(text+strlen(text), "\n");
    string text_str = text;
    setGameStatText(text_str);
}

int StudentWorld::getWinningAntNumber()
{
    int winner = -1, winningNumber = -1, winningTick = -1;
    for (int i = 0; i < m_hillCount; i++) {
        if (antCount[i] > winningNumber ||
            (antCount[i] == winningNumber &&
             antTickCount[i] > winningTick)) {
                winner = i;
                winningNumber = antCount[i];
                winningTick = antTickCount[i];
            }
    }
    return winner;
}

int StudentWorld::move()
{
    updateTickCount();
    
    cout << "======== Tick " << currentTick << "============\n"; // for debug use
    
    std::set<Coord>::iterator itr1;
    
    for(itr1 = m_activeCoord.begin(); itr1 != m_activeCoord.end(); itr1++) // iterate through active coordinate and ask the active actors to do something
    {
        actorlist& currCell = m_arr[itr1->x][itr1->y];
        
        for(actorlist::iterator curr_itr = currCell.begin(); curr_itr != currCell.end(); curr_itr++)
        {
            Actor* actor = *curr_itr;
            actor->doSomething();
        }
    }

    while (!m_movefrom.empty()) {                   // update the data structure, actors which have moved
        Entry entry = m_movefrom.front();
        removeActor(entry.x, entry.y, entry.actor);
        m_movefrom.pop();
    }
    while (!m_moveto.empty()) {
        Entry entry = m_moveto.front();
        addActor(entry.x, entry.y, entry.actor);
        m_moveto.pop();
    }
    
    removeDeadSimulationObjects();  // update the data structure, remove actors which have died
    
    updateDisplay();
    
    if(SimulationOver()) {
        int winner = getWinningAntNumber();
        if (antCount[winner] <= 5)
            return GWSTATUS_NO_WINNER;
        setWinner(m_compilers[winner]->getColonyName());
        return GWSTATUS_PLAYER_WON;


    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    std::set<Coord>::iterator itr1;
    for(itr1 = m_activeCoord.begin(); itr1 != m_activeCoord.end(); itr1++)  // iterate through active coord and delete the dynamically allocated variables
    {
        actorlist& a = m_arr[itr1->x][itr1->y];
        actorlist::iterator itr2;
        for(itr2 = a.begin(); itr2 != a.end(); itr2++)
        {
            delete *itr2;
        }
        a.clear();
    }
    m_activeCoord.clear();  // important because don't want delete nullptr, will cause crush

    for (int i = 0; i < m_hillCount; i++) {
        delete m_compilers[i];
    }
    m_hillCount = 0;
    
}

void StudentWorld::insertActiveCoord(int x, int y)
{
    Coord cd(x,y);
    m_activeCoord.insert(cd);
}
void StudentWorld::updateTickCount()
{
    currentTick--;
}

//void StudentWorld::updateDataStruct(std::list<Actor*>& list1, std::list<Actor*>::iterator& q, std::list<Actor*>& list2, int newX, int newY)
//{
//    Actor* point = *q;
//    list1.erase(q);
//    list2.push_back(point);
//}

void StudentWorld::removeDeadSimulationObjects()
{
    
    for(set<Coord>::iterator itr1 = m_activeCoord.begin(); itr1 != m_activeCoord.end(); itr1++)
    {
        actorlist& list = m_arr[itr1->x][itr1->y];
        for(actorlist::iterator itr = list.begin(); itr != list.end(); )
        {
            Actor* actor = *itr;
            if (!actor->alive()) {
                itr = list.erase(itr);
                delete actor;
            } else {
                itr++;
            }
        }
    }
}

bool StudentWorld::SimulationOver()
{
    if (currentTick <= 0)
        return true;
    else return false;
}

bool StudentWorld::findObstacle(int x, int y)
{
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return true;
    }
    actorlist& a = m_arr[x][y];
    actorlist::iterator itr;
    for(itr = a.begin(); itr != a.end(); itr++)
    {
        if ((*itr)->isObstacle())
            return true;
    }
    return false;
}

void StudentWorld::stunAllStunnableAt(int x, int y)
{
    actorlist& a = m_arr[x][y];
    actorlist::iterator itr;
    for(itr = a.begin(); itr != a.end(); itr++)
    {
        if ((*itr)->isInsect()) {
            cout << "Try to stun : ";
            (*itr)->getStunned();
        }
    }
}

void StudentWorld::poisonAllPoisonableAt(int x, int y)
{
    actorlist& a = m_arr[x][y];
    actorlist::iterator itr;
    vector<Actor*> insects;
    for(itr = a.begin(); itr != a.end(); itr++)
    {
        if ((*itr)->isInsect())
            (*itr)->getPoisoned();
    }
}

bool StudentWorld::biteEnemyAt(int x, int y, int amount, int colony)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++)
    {
        Actor* a = *itr;
        if (a->isEnemy(colony) && a->alive()) {
            a->getBitten(amount);
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkEnemyAt(int x, int y, int colony)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++)
    {
        Actor *a = *itr;
        if (a->isEnemy(colony) && a->alive()) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkDangerousAt(int x, int y, int colony)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++)
    {
        Actor *a = *itr;
        if (a->isDangerous(colony) && a->alive()) {
            return true;
        }
    }
    return false;
}



bool StudentWorld::biteOneAt(int x, int y, int amount, Actor* self)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++) {
        Actor* a = *itr;
        if (a->isInsect() && a->alive() && a != self) {
            a->getBitten(amount);
            return true;
        }
    }
    return false;
}

void StudentWorld::increasePheromoneAt(int x, int y, int amount, int colony){
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++) {
        Actor* a = *itr;
        if (a->isPheromone(colony)) {
            // Have to do typecasting here
            // because actors can't decrease energy
            EnergyHolder* eh = dynamic_cast<EnergyHolder*>(a);
            int capacity = 768 - eh->getEnergy();
            if (capacity > amount) {
                eh->increaseEnergyBy(amount);
            } else {
                eh->increaseEnergyBy(capacity);
            }
            return;
        }
    }
    Actor* phero = new Pheromone(x, y, colony, amount, this);
    addActor(x, y, phero);
}

bool StudentWorld::checkPheromoneAt(int x, int y, int colony)
{
    actorlist& list = m_arr[x][y];
    for (actorlist::iterator itr = list.begin(); itr != list.end(); itr++)
    {
        Actor *a = *itr;
        if (a->isPheromone(colony) && a->alive()) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkAntHillAt(int x, int y, int colony)
{
   return m_anthills[colony]->getX() == x &&
          m_anthills[colony]->getY() == y;
}

void StudentWorld::incrementAntNumber(int colony) {
    antCount[colony]++;
    antTickCount[colony] = currentTick;
}
//void StudentWorld::decrementAntNumber(int colony) {
//    antCount[colony]--;
//    antTickCount[colony] = currentTick;
//}


