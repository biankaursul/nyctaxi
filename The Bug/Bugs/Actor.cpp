#include "Actor.h"
#include "StudentWorld.h"

#include <cstdio>
#include <cmath>
using namespace std;

//==================implementation of Actor================================
Actor::Actor(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* sWorld)
: GraphObject(imageID, startX, startY, dir, depth), m_stdworld(sWorld)
{
}

StudentWorld* Actor::getStudentWorld()
{
    return m_stdworld;
}
//==========================================================================

int Pheromone::decideIID(int colony)        // decide the imageID for a colony (different for different colony)
{
    switch (colony) {
        case 0:
            return IID_PHEROMONE_TYPE0;
            break;
        case 1:
            return IID_PHEROMONE_TYPE1;
            break;
        case 2:
            return IID_PHEROMONE_TYPE2;
            break;
        case 3:
            return IID_PHEROMONE_TYPE3;
        default:
            return INVALID_COLONY_NUMBER;
            break;
    }
}
int Ant::decideIID(int colony)              // decide the imageID for a colony (different for different colony)

{
    switch (colony) {
        case 0:
            return IID_ANT_TYPE0;
            break;
        case 1:
            return IID_ANT_TYPE1;
            break;
        case 2:
            return IID_ANT_TYPE2;
            break;
        case 3:
            return IID_ANT_TYPE3;
        default:
            return INVALID_COLONY_NUMBER;
            break;
    }
}


//==================implementation of Insect===============================
Insect::Insect(int imageID, int startX, int startY, Direction dir, int energy, int sleepCycle,
               int stunCycle, StudentWorld* sWorld)
: EnergyHolder(imageID, startX, startY, dir, 1, energy, sWorld), m_oldx(startX), m_oldy(startY),
m_sleepCycle(sleepCycle), m_stunCycle(stunCycle), m_sleepTick(0)
{}


bool Insect::asleep() {
    if (m_sleepTick == 0){
        m_sleepTick = m_sleepCycle;
        return false;
    }
    m_sleepTick--;
    return true;
}

void Insect::getStunned(){
    if (m_oldx != getX() || m_oldy != getY()) {
        cout << "Was at " << m_oldx << "," << m_oldy << ", now stunned at "
        << getX() << "," << getY() << endl;
        m_sleepTick += m_stunCycle;
    }
}

void Insect::recordCurrentCoord() {
    m_oldx = getX();
    m_oldy = getY();
}

void Insect::moveTo(int newx, int newy) {
    recordCurrentCoord();
    getStudentWorld()->actorMove(m_oldx, m_oldy, newx, newy, this);
    GraphObject::moveTo(newx, newy);
}


void Insect::getBitten(int amount) {
    if (!alive()) return;
    printf("Insect %0llx at %d %d is bitten by %d\n",
           (long long)this, getX(), getY(), amount);
    if (getEnergy() <= amount) {
        turnToFood();
    }
    decreaseEnergyBy(amount);
}

void Insect::turnToFood() {
    getStudentWorld()->addFood(getX(), getY(), 100);
}

bool Insect::eatFood(int amount) {
    int eaten = getStudentWorld()->eatFood(getX(), getY(), amount);
    increaseEnergyBy(eaten);
    return eaten > 0;
}


bool Insect::checkStatus()
{
    // lost one hit point
    if (!alive()) {
        return false;
    }
    decrementEnergy();
    if (!alive()) {
        // turn to food because of natural death
        turnToFood();
        return false;
    }
    recordCurrentCoord();       //keep track of oldX and oldY before actually move
    if (asleep()) {
        return false;
    }
    return true;
}


bool Insect::attemptMove(GraphObject::Direction r)  // try if the insect can move in direction r
{
    switch (r)
    {
        case GraphObject::up:
            if(getStudentWorld()->findObstacle(getX(),getY()+1))
                return false;
            else return true;
            break;
        case GraphObject::right:
            if(getStudentWorld()->findObstacle(getX()+1,getY()))
                return false;
            else return true;
            break;
        case GraphObject::down:
            if(getStudentWorld()->findObstacle(getX(),getY()-1))
                return false;
            else return true;
            break;
        case GraphObject::left:
            if(getStudentWorld()->findObstacle(getX()-1,getY()))
                return false;
            else return true;
            break;
        default:
            return false;
            break;
    }
}

bool Insect::moveForward()
{
    Direction dir = getDirection();
    if(attemptMove(dir))        // if the insect can move in direction dir, then do the move
    {
        switch(dir)
        {
            case GraphObject::up:
                moveTo(getX(), getY()+1);
                break;
            case GraphObject::right:
                moveTo(getX()+1, getY());
                break;
            case GraphObject::down:
                moveTo(getX(), getY()-1);
                break;
            case GraphObject::left:
                moveTo(getX()-1, getY());
                break;
            default:
                break;
        }
        return true;
    }
    return false;
}

//==================implementation of Grasshopper======================
void Grasshopper::regularMove()
{
    if (getDistance() ==0)
    {
        setDirection(static_cast<GraphObject::Direction>(randInt(1, 4)));
        setDistance(randInt(2, 10));
    }
    
    if (moveForward()) {
        decrementDistance();
    } else {
        setDistance(0);
    }
}

void Grasshopper::decrementDistance()
{
    m_distance--;
}

int Grasshopper::getDistance()
{
    return m_distance;
}

void Grasshopper::setDistance(int x)
{
    m_distance = x;
}

BabyGrasshopper::BabyGrasshopper(int startX, int startY, StudentWorld* sWorld)
: Grasshopper(IID_BABY_GRASSHOPPER, startX, startY,
         500, 2, 2, sWorld)
{
}

void BabyGrasshopper::getPoisoned()
{
    printf("Baby hopper %0llx at %d %d got poisoned by 150!\n",     // for debug use
           (long long)this, getX(), getY());
    getBitten(150);
}


void BabyGrasshopper::doSomething()
{
    if (!checkStatus()) return;
    if(getEnergy() < 1600)
    {
        if (eatFood(200)) {
            // if eaten some, go to sleep 50% of the time
            if (randInt(0,1)) return;
        }
        regularMove();
    } else {
        // kill self and spawn an adult in place of it
        printf("baby hopper %0llx at %d %d, about to transform!\n",
               (long long)this, getX(), getY());
        decreaseEnergyBy(getEnergy());
        Actor* alias = new AdultGrasshopper(getX(), getY(), getStudentWorld());
        getStudentWorld()->addNextTick(getX(), getY(), alias);
    }
}

AdultGrasshopper::AdultGrasshopper(int startX, int startY, StudentWorld* sWorld)
: Grasshopper(IID_ADULT_GRASSHOPPER, startX, startY,
              1600, 2, 0, sWorld)
{
    }

bool AdultGrasshopper::tryAndJump() {
    int x = getX(), y = getY();
    for (int dx = -10; dx <= 10; dx++) {
        for (int dy = -10; dy <= 10; dy++) {
            if (dx * dx + dy * dy > 100) continue;
            if (!getStudentWorld()->findObstacle(x+dx, y+dy)) {
                moveTo(x+dx,y+dy);
                printf("Adult hopper %0llx at %d %d about to jump to %d %d",
                       (long long)this, x, y, x+dx, y+dy);
                return true;
            }
        }
    }
    return false;
}

void AdultGrasshopper::doSomething() {

    if (!checkStatus()) return;
    
  
    if (randInt(0, 2) == 0) {
        // one in three chance it will bite
        if (getStudentWorld()->biteOneAt(getX(), getY(), 50, this)) {
            printf("Adult hopper %0llx at %d %d shall bite.\n",
                   (long long)this, getX(), getY());
            return;
        }
    } else {
        // 1/10 chance try to jump to somewhere within radius of 10
        if (randInt(0, 9) == 0 && tryAndJump()) {
            return;
        }
        // if there's food, eat it, and rest with 1/2 chance
        if (eatFood(200) && randInt(0,1)) {
            return;
        }
        regularMove();
    }
}

void AdultGrasshopper::getBitten(int amount)
{
    Insect::getBitten(amount);
    if (!alive()) return;
    // 50% chance it will retaliate
    if (randInt(0,1)) return;
    printf("Adult hopper %0llx at %d %d shall retaliate!\n",
           (long long)this, getX(), getY());
    getStudentWorld()->biteOneAt(getX(), getY(), 50, this);
}



//==================implementation of WaterPool======================

void WaterPool::doSomething()
{
    getStudentWorld()->stunAllStunnableAt(getX(), getY());
}

//==================implementation of Poison======================

void Poison::doSomething()
{
    getStudentWorld()->poisonAllPoisonableAt(getX(), getY());
}


// ================= ANT!!!!!!!!!!! =======================

Ant::Ant(int startX, int startY, int colony, Compiler* compiler, StudentWorld* sWorld)
:Insect(decideIID(colony), startX, startY,
        static_cast<GraphObject::Direction>(randInt(1, 4)), 1500, 0, 2, sWorld),
m_food(0), m_ic(0), m_rand(0), m_blocked(false), m_bitten(false), m_compiler(compiler),
m_colony(colony)
{
}
void Ant::turnToFood()
{
    //getStudentWorld()->decrementAntNumber(m_colony);
    Insect::turnToFood();
}

void Ant::getBitten(int amount)
{
    Insect::getBitten(amount);
    m_bitten = true;
}

void Ant::getPoisoned()
{
    printf("Ant %0llx of colony %d at %d %d got poisoned by 150!\n",
           (long long)this%10000, m_colony, getX(), getY());
    Insect::getBitten(150);
}

void Ant::doSomething() {
    char dir[][10] = {"", "up", "right", "down", "left"};
    if (!checkStatus()) return;
    // fetch the next command pointed to by the instruction counter
    Compiler::Command c;
    bool debug = (long long)this % 15 == 0;
    if (debug) {
        printf("Ant %0llx (C%d) running command: \n",
           (long long)this%10000, m_colony);
    }
    for (int nCommands = 0; nCommands < 10; nCommands++)
    {
        m_compiler->getCommand(m_ic, c);
        if (c.opcode == Compiler::invalid) {
            turnToFood();
            decreaseEnergyBy(getEnergy());
            return;
        }
        
        if (debug) {
            printf("\tline %d, (%d,%d)facing(%s): do %s\n",
            m_ic, getX(), getY(), dir[getDirection()], c.text.c_str());
        }
        
        switch (c.opcode)
        {
            case Compiler::moveForward: {
                if (moveForward()) {
                    m_blocked = false;
                    m_bitten = false;
                } else {
                    m_blocked = true;
                }
                break;
            }
            case Compiler::eatFood: {
                if (m_food >= 100) {
                    m_food -= 100;
                    increaseEnergyBy(100);
                } else {
                    increaseEnergyBy(m_food);
                    m_food = 0;
                }
                break;
            }
            case Compiler::dropFood: {
                getStudentWorld()->addFood(getX(), getY(), m_food);
                m_food = 0;
                break;
            }
            case Compiler::bite: {
                getStudentWorld()->biteEnemyAt(getX(), getY(), 15, m_colony);
                break;
            }
            case Compiler::pickupFood: {
                int capacity = 1800 - m_food;
                int taken = getStudentWorld()->eatFood(getX(), getY(), capacity);
                m_food += taken;
                break;
            }
            case Compiler::emitPheromone: {
                getStudentWorld()->increasePheromoneAt(getX(), getY(), 200, m_colony);
                break;
            }
            case Compiler::faceRandomDirection: {
                setDirection(static_cast<Direction>(randInt(1, 4)));
                break;
            }
            case Compiler::rotateClockwise: {
                switch (getDirection()) {
                    case up: setDirection(right); break;
                    case right: setDirection(down); break;
                    case down: setDirection(left); break;
                    case left: setDirection(up); break;
                    default: break;
                }
                break;
            
            }
            case Compiler::rotateCounterClockwise: {
                switch (getDirection()) {
                    case up: setDirection(left); break;
                    case left: setDirection(down); break;
                    case down: setDirection(right); break;
                    case right: setDirection(up); break;
                    default: break;
                }
                break;
            }
            case Compiler::generateRandomNumber: {
                int op1 = stoi(c.operand1);
                m_rand = (op1 == 0) ? 0 : randInt(0, op1 - 1);
                break;
            }
            case Compiler::goto_command: {
                int op1 = stoi(c.operand1);
                m_ic = op1;
                break;
            }
            case Compiler::if_command: {
                do_if(stoi(c.operand1), stoi(c.operand2));
            }
        }
        if (c.opcode != Compiler::goto_command && c.opcode != Compiler::if_command) {
            m_ic++;
        }
        if (c.opcode != Compiler::goto_command && c.opcode != Compiler::if_command
            && c.opcode != Compiler::generateRandomNumber) {
            // make sure at most one external change of state happens during a tick
            return;
        }
    }
}

void Ant::do_if(int op1, int op2) {
    bool status = false;
    int dx[] = {0, 0, 1, 0, -1};
    int dy[] = {0, 1, 0, -1, 0};
    
    // the dx, dy values correspond to up, right, down, left
    switch (op1) {
        case Compiler::last_random_number_was_zero:
            status = m_rand == 0; break;
        case Compiler::i_am_carrying_food:
            status = (m_food > 0); break;
        case Compiler::i_am_hungry:
            status = getEnergy() <= 25; break;
        case Compiler::i_am_standing_with_an_enemy:
            status = getStudentWorld()->checkEnemyAt(getX(), getY(), m_colony); break;
        case Compiler::i_am_standing_on_food:
            status = getStudentWorld()->getFoodAt(getX(), getY())->getEnergy() > 0; break;
        case Compiler::i_am_standing_on_my_anthill:
            status = getStudentWorld()->checkAntHillAt(getX(), getY(), m_colony);
            break;
        case Compiler::i_smell_pheromone_in_front_of_me: {
            int x = getX() + dx[getDirection()],
            y = getY() + dy[getDirection()];
            status = getStudentWorld()->checkPheromoneAt(x, y, m_colony);
            break;
        }
        case Compiler::i_smell_danger_in_front_of_me: {
            int x = getX() + dx[getDirection()],
            y = getY() + dy[getDirection()];
            status = getStudentWorld()->checkDangerousAt(x, y, m_colony);
            break;
        }
        case Compiler::i_was_bit:
            status = m_bitten; break;
        case Compiler::i_was_blocked_from_moving:
            status = m_blocked; break;
        default: break;
    }
    if (status) {
        m_ic = op2;
    } else {
        m_ic++;
    }
}

void AntHill::doSomething() {
    decrementEnergy();
    if (!alive()) return;
    StudentWorld* sw = getStudentWorld();
    int eaten = sw->eatFood(getX(), getY(), 10000);
    if (eaten > 0) {
        increaseEnergyBy(eaten);
        return;
    }
    if (getEnergy() >= 2000) {
        // produce a new ant
        Actor* ant = new Ant(getX(), getY(), m_colony, m_compiler, sw);
        sw->addNextTick(getX(), getY(), ant);
        decreaseEnergyBy(1500);
        getStudentWorld()->incrementAntNumber(m_colony);
    }
}









