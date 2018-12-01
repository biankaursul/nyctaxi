#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include <string>
#include <set>
#include <list>
#include <queue>
#include "Actor.h"


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
struct Coord
{
    Coord(int x, int y): x(x), y(y){}
    int x;
    int y;
};
bool operator<(const Coord &a, const Coord&b);
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    void createActor(Actor* ptr, int x, int y);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool findObstacle(int x, int y);
    void stunAllStunnableAt(int x, int y);
    void poisonAllPoisonableAt(int x, int y);
    bool biteOneAt(int x, int y, int amount, Actor* self);
    bool biteEnemyAt(int x, int y, int amount, int colony);
    bool checkEnemyAt(int x, int y, int colony);
    bool checkDangerousAt(int x, int y, int colony);
    bool checkPheromoneAt(int x, int y, int colony);
    bool checkAntHillAt(int x, int y, int colony);
    void increasePheromoneAt(int x, int y, int amount, int colony);
    void incrementAntNumber(int colony);
    //void decrementAntNumber(int colony);
    void actorMove(int oldx, int oldy, int newx, int newy, Actor* actor);
    void addNextTick(int x, int y, Actor* actor);
    EnergyHolder* getFoodAt(int x, int y);
    void addFood(int x, int y, int amount);
    int eatFood(int x, int y, int amount);
  
private:
    void updateDisplay();
    int getWinningAntNumber();
    void removeDeadSimulationObjects();
    void updateTickCount();
    bool SimulationOver();
    void insertActiveCoord(int x, int y);
    void addActor(int x, int y, Actor* actor);
    void removeActor(int x, int y, Actor* actor);

    std::set<Coord> m_activeCoord;

    struct Entry {
        Actor* actor;
        int x;
        int y;
        Entry(Actor* a, int x, int y):actor(a), x(x), y(y){}

    };
    struct Entry_compare {
        bool operator() (const Entry& e1, const Entry& e2) const {
            return e1.actor < e2.actor;
        }
    };
    
    typedef std::list<Actor*> actorlist;
    typedef std::queue<Entry> waitlist;
    typedef waitlist deadlist;
    
    actorlist m_arr[VIEW_WIDTH][VIEW_HEIGHT];
    waitlist m_movefrom, m_moveto;
    deadlist m_dead;
    
    Compiler* m_compilers[4];
    Actor* m_anthills[4];
    int antCount[4];
    int antTickCount[4];
    int m_hillCount;
    int currentTick;
    Field fld;
};

#endif // STUDENTWORLD_H_
