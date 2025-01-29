#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;
class Agent;
class Pea;

//Actor Def
class Actor: public GraphObject {

public:
	Actor(int imageID, double startX, double startY, int HP, StudentWorld* world);
    virtual~Actor() {}
	
    virtual void doSomething() = 0;
    bool isAlive() const {
        if (m_HP <= 0) {
            return false;
        }
        return true;
    }
    //Set dead
    void setDead() { setHitPoints(0); }
    void decHitPoints(int amt) {  m_HP -= amt; }
    StudentWorld* getWorld() const { return m_World; }
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool allowsMarbleColocation() const { return false; }
    virtual bool countsInFactoryCensus() const { return false; }
    virtual bool stopsPea() const { return true; }
    virtual bool isDamageable() const { return false; }
    virtual void damage(int damageAmt) { m_HP -= damageAmt; }
    virtual bool bePushedBy(Agent* a, int x, int y) { return false; }
    virtual bool isSwallowable() const { return false; }
    virtual bool isStealable() const { return false; }
    virtual int getHP() const { return m_HP; }
    virtual void setHitPoints(int amt) { m_HP = amt; }
    virtual bool tryToBeKilled(int damageAmt) {
        damage(damageAmt);
        if (isAlive()) {
            return true;
        }
        return false;
    }
    virtual bool isAPea() { return false; }
    virtual bool isCrystal() { return false; }
    virtual bool isThiefBot() { return false; }
    void spaceInFrontofActor(int dir, int& x, int& y);



private:
	StudentWorld* m_World;
	int m_HP;
};

//Agent Def
class Agent : public Actor {
public:
	Agent (int imageID, double startX, double startY, StudentWorld* world, int HP);
    bool moveIfPossible();
    virtual bool canPushMarbles() const { return false; }
    virtual bool needsClearShot() const { return true; }
    virtual int shootingSound() const { return SOUND_ENEMY_FIRE; }
    virtual bool isDamagable() const { return true; } //Added by me

private:

};

//Avatar Def
class Avatar : public Agent {
public:
	Avatar(double startX, double startY, StudentWorld* world);
    virtual~Avatar() {}

    //Virtual Functions
    virtual bool allowsAgentColocation() { return true; }
    virtual void doSomething();
    virtual void damage(int damageAmt) { setHitPoints(getHP() - damageAmt);}
    virtual bool canPushMarbles() const { return true; }
    virtual bool needsClearShot() const { return false; }
    virtual int shootingSound() const { return SOUND_PLAYER_FIRE; }

    //Other Functions
    int getHealthPct() const { return (getHP() / 20) * 100; }
    int getAmmo() const { return m_ammo; }
    void restoreHealth() { setHitPoints(20); }
    void increaseAmmo() { m_ammo += 20; }

private:
    int m_ammo = 20;
    Pea* p;
};

//Robot Def
class Robot : public Agent {
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID,
        int hitPoints, int score, int startDir);
    virtual bool allowsAgentColocation() { return true; }
    virtual void doSomething();
    virtual void doDiffRobotSomething() { }
    virtual bool isDamageable() const { return true; }
    virtual bool canPushMarbles() const { return false; }
    virtual bool needsClearShot() const { return true; }
    virtual int shootingSound() const { return SOUND_ENEMY_FIRE; }
    virtual bool isShootingRobot() const { return true; }
    int getRobotScore() { return robot_score; }
    virtual void damage(int damageAmt); 
    bool peaCanFire();
    void firePea();

private:
    bool actionTick();
    int m_tick;
    int robot_score;
    Pea* robot_p;
};

//RageBot Definitions
class RageBot : public Robot {
public:
    RageBot(StudentWorld* world, int startX, int startY, int startDir);
    virtual void doDiffRobotSomething();
    void moveRageBot();

};
//VerticalRageBot Definitions
class VerticalRageBot : public RageBot {
public:
    VerticalRageBot(StudentWorld* world, int startX, int startY);
};

class HorizontalRageBot : public RageBot {
public:
    HorizontalRageBot(StudentWorld* world, int startX, int startY);
};

class ThiefBot : public Robot {
public:
    ThiefBot(StudentWorld* world, int startX, int startY, int imageID,
        int hitPoints, int score);
    virtual void doDiffRobotSomething();
    virtual void doThiefBotSomething() = 0;
    virtual bool countsInFactoryCensus() const { return true; }
    virtual void damage(int damageAmt);
    int getDistanceBeforeTurning() { return distanceBeforeTurning; }
    virtual bool isThiefBot() { return true; }

private:
    int distanceBeforeTurning;
    int curDistance;
    Actor* goodieP;
};

class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(StudentWorld* world, int startX, int startY);
    virtual void doThiefBotSomething() {}
    virtual bool isShootingRobot() const { return false; }
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(StudentWorld* world, int startX, int startY);
    virtual void doThiefBotSomething();
};

class ThiefBotFactory : public Actor {
public:
    enum ProductType { REGULAR, MEAN };

    ThiefBotFactory(StudentWorld* world, int startX, int startY, ProductType type);
    virtual void doSomething();
    virtual bool stopsPea() const { return true; }

private:
    ProductType m_Type;
    ThiefBot* m_ThiefBot;
};

//Exit Def
class Exit : public Actor {
public:
    Exit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; }
    virtual bool allowsMarbleColocation() const {
        if (isVisible()) {
            return false;
        }
        return true;
    }
};

//Wall Def
class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* world);
    virtual void doSomething() { return; } //Need to fix
    virtual bool stopsPea() const { return true; }
};

//Pea Def 
class Pea : public Actor {
public:
    Pea(StudentWorld* world, int startX, int startY, int startDir);
    void checkToDoDamage(int& x, int& y);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; };
    virtual bool isAPea() { return true; }
};

//Marble Definitions
class Marble : public Actor {
public:
    Marble(StudentWorld * world, int startX, int startY);
    virtual void doSomething() {}
    virtual bool isDamageable() const { return true; }
    virtual void damage(int damageAmt);
    virtual bool isSwallowable() const { return true; }
    virtual bool bePushedBy(Agent * a, int x, int y);
};

//Pit Definitions
class Pit : public Actor {
public:
    Pit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsMarbleColocation() const { return true; }
    virtual bool stopsPea() const { return false; }
};


//PickupableItem Defintions
class PickupableItem : public Actor {
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID,
        int score);
    virtual void doSomething();
    virtual void doItemSomething() = 0;
    virtual bool allowsAgentColocation() const { return true; }
    virtual bool allowsMarbleColocation() const { return true; }
    int getItemScore() { return item_Score; }

private:
    int item_Score;
};

//Crystal Definitions
class Crystal : public PickupableItem {
public:
    Crystal(StudentWorld* world, int startX, int startY);
    virtual void doItemSomething() {}
    virtual bool isCrystal() { return true; }
};

//Goodie Definitions
class Goodie : public PickupableItem {
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID,
        int score);
    virtual void doItemSomething() = 0;
    virtual bool isStealable() const { return true; }

    // Set whether this goodie is currently stolen.
    void setStolen(bool status) { isStolen = status; }

private:
    bool isStolen = false;
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY);
    virtual void doItemSomething();
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(StudentWorld* world, int startX, int startY);
    virtual void doItemSomething();
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(StudentWorld* world, int startX, int startY);
    virtual void doItemSomething();
};



#endif // ACTOR_H_
