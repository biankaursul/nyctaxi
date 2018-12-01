#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Compiler.h"
class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* sWorld);
    virtual void doSomething() = 0;
    virtual bool alive() const = 0;
    virtual bool isObstacle() const = 0;
    virtual bool isEdible() const = 0;
    virtual bool isInsect() const = 0;
    virtual bool isDangerous(int colony) const = 0;
    virtual bool isEnemy(int colony) const = 0;
    virtual bool isPheromone(int colony) const = 0;
    virtual void getStunned() {}
    virtual void getPoisoned() {}
    virtual void getBitten(int amount) {}

    
    StudentWorld* getStudentWorld();
private:
    StudentWorld* m_stdworld;
};

class EnergyHolder: public Actor
{
public:
    EnergyHolder(int imageID, int startX, int startY, Direction dir, int depth, int energy, StudentWorld* sWorld)
    : Actor(imageID, startX, startY, dir, depth, sWorld), m_energy(energy)
    {}
    virtual bool isObstacle() const {
        return false;
    }
    virtual bool alive() const {
        return getEnergy() > 0;
    }
    int getEnergy() const {
        return m_energy;
    }
    void decrementEnergy() {
        m_energy--;
    }
    void decreaseEnergyBy(int n) {
        m_energy -= n;
    }
    
    void incrementEnergy() {
        m_energy++;
    }
    void increaseEnergyBy(int n) {
        m_energy+=n;
    }
private:
    int m_energy;
};

class Pheromone : public EnergyHolder
{
public:
    Pheromone(int startX, int startY, int colony, int energy, StudentWorld* sWorld)
    : EnergyHolder(decideIID(colony), startX, startY, right, 2, energy, sWorld),
    m_colony(colony)
    {}
    virtual void doSomething() {
        decrementEnergy();
    }
    virtual bool isInsect() const {
        return false;
    }
    virtual bool isEdible() const {
        return false;
    }
    virtual bool isDangerous(int colony) const {
        return false;
    }
    virtual bool isEnemy(int colony) const {
        return false;
    }
    virtual bool isPheromone(int colony) const {
        return m_colony == colony && alive();
    }
private:
    int decideIID(int colony);
    int m_colony;
};


class Pebble : public Actor
{
public:
    Pebble(int startX, int startY, StudentWorld* sWorld)
    : Actor(IID_ROCK, startX, startY, right, 1, sWorld)
    {}

    virtual bool alive() const {
        return true;
    }
    virtual bool isInsect() const {
        return false;
    }
    virtual bool isObstacle() const {
        return true;
    }
    virtual bool isEdible() const {
        return false;
    }
    virtual bool isDangerous(int colony) const {
        return false;
    }
    virtual bool isEnemy(int colony) const {
        return false;
    }
    virtual bool isPheromone(int colony) const {
        return false;
    }
    virtual void doSomething() {}
};

class Food : public EnergyHolder
{
public:
    Food(int startX, int startY, int energy, StudentWorld* sw)
    : EnergyHolder(IID_FOOD, startX, startY, right, 2, energy, sw)
    {}
    
    virtual bool isInsect() const {
        return false;
    }
    virtual bool isDangerous(int colony) const {
        return false;
    }
    virtual bool isEnemy(int colony) const {
        return false;
    }
    virtual void doSomething() {}
    virtual bool isEdible() const {
        return true;
    }
    virtual bool isPheromone(int colony) const {
        return false;
    }
};

class Insect : public EnergyHolder
{
public:
    Insect(int imageID, int startX, int startY, Direction dir, int energy,
           int sleepCycle, int stunCycle, StudentWorld* sWorld);
    
    virtual void getStunned();
    virtual void getPoisoned() = 0;
    virtual void moveTo(int x, int y);
    virtual void getBitten(int amount);
    virtual bool isInsect() const{
        return true;
    }
    virtual bool isEnemy(int colony) const {
        return true;
    }
    virtual bool isEdible() const {
        return false;
    }
    virtual bool isPheromone(int colony) const {
        return false;
    }
    virtual bool checkStatus();
    
    virtual bool asleep();
    virtual void recordCurrentCoord();
    virtual bool eatFood(int amount);
    virtual void turnToFood();
    
    bool attemptMove(GraphObject::Direction r);
    bool moveForward();

private:
    int m_oldx, m_oldy;
    int m_sleepTick;
    const int m_sleepCycle;
    const int m_stunCycle;
};

class Grasshopper : public Insect{
public:
    Grasshopper(int imageID, int startX, int startY, int energy, int sleepCycle,
                int stunCycle, StudentWorld* sWorld)
    :Insect(imageID, startX, startY, static_cast<GraphObject::Direction>(randInt(1, 4)),
            energy, sleepCycle, stunCycle, sWorld), m_distance(randInt(2,10))
    {}
    virtual bool isDangerous(int colony) const {
        return true;
    }
    void regularMove();
    
private:
    void decrementDistance();
    int getDistance();
    void setDistance(int x);
    int m_distance;
};

class BabyGrasshopper : public Grasshopper
{
public:
    BabyGrasshopper(int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething();
    virtual void getPoisoned();
};

class AdultGrasshopper : public Grasshopper
{
public:
    AdultGrasshopper(int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething();
    virtual void getPoisoned() {
        
    }
    virtual void getBitten(int amount);
private:
    bool tryAndJump();
};


class Triggerable : public EnergyHolder
{
public:
    Triggerable(int imageID, int startX, int startY, StudentWorld* sWorld)
    :EnergyHolder(imageID, startX, startY, right, 2, 1, sWorld)
    {}
    virtual bool isDangerous(int colony) const {
        return true;
    }
    virtual bool isEnemy(int colony) const {
        return false;
    }
    virtual bool isInsect() const { return false;}
    virtual bool isPheromone(int colony) const {
        return false;
    }
    virtual bool isEdible() const { return false;}
};

class WaterPool : public Triggerable
{
public:
    WaterPool(int startX, int startY, StudentWorld* sWorld)
    :Triggerable(IID_WATER_POOL, startX, startY, sWorld)
    {}
    virtual void doSomething();
};

class Poison : public Triggerable
{
public:
    Poison(int startX, int startY, StudentWorld* sWorld)
    :Triggerable(IID_POISON, startX, startY, sWorld)
    {}
    virtual void doSomething();
};


class AntHill : public EnergyHolder
{
public:
    // EnergyHolder(int imageID, int startX, int startY, Direction dir, int depth, int energy, StudentWorld* sWorld)
    AntHill(int startX, int startY, int colony, Compiler* compiler, StudentWorld* sw)
    :EnergyHolder(IID_ANT_HILL, startX, startY, right, 2, 8999, sw),
    m_compiler(compiler), m_colony(colony)
    {}
    virtual void doSomething();
    virtual bool isEdible() const { return false;}
    virtual bool isInsect() const { return false;}
    virtual bool isDangerous(int colony) const { return false;}
    virtual bool isEnemy(int colony) const { return false;}
    virtual bool isPheromone(int colony) const { return false;}
private:
    Compiler* m_compiler;
    int m_colony;
};

class Ant : public Insect
{
public:
    Ant(int startX, int startY, int colony, Compiler* compiler, StudentWorld* sWorld);
    virtual void doSomething();
    virtual void getBitten(int amount);
    virtual bool isEnemy(int colony) const {
        return m_colony != colony;
    }
    virtual bool isDangerous(int colony) const {
        return isEnemy(colony);
    }
    virtual void getPoisoned();
    virtual void turnToFood();
    static int decideIID(int colony);
    
private:
    void do_if(int op1, int op2);
    int m_food, m_ic, m_rand, m_colony;
    bool m_blocked, m_bitten;
    Compiler* m_compiler;
};
#endif // ACTOR_H_
