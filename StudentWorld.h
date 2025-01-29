#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <iostream>
using namespace std;
#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Avatar;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual void cleanUp();
  ~StudentWorld();

  virtual int init();
  virtual int move();

  string levelToString();
  void removeDeadGameObjects();

  void setLevelCompleted(bool level) { levelCompleted = level; }
  bool isLevelCompleted() const; 

  //Getters and Setters
  int getContainerSize() { return m_Container.size(); }
  Actor* getContainer(int a) { return m_Container[a]; }
  int getCurrentLevelBonus() const { return currentLevelBonus; }
  void addActor(Actor* actor) { m_Container.push_back(actor); }
  Avatar* getAvatar() { return m_Avatar; }

  //Display
  void setDisplayText();
  string formatDisplayText(int score, int level, int lives, double health, int ammo, unsigned int bonus);


private:
	Avatar* m_Avatar;
	vector <Actor*> m_Container;
    int currentLevelBonus = 1000;
	bool levelCompleted;

};

#endif // STUDENTWORLD_H_
