#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

//StudentWorld Definitions
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath) {
    m_Avatar = nullptr;
    levelCompleted = false;
}

void StudentWorld::cleanUp() {
    //Clear newd pointers in vector
    while (!m_Container.empty()) {
        delete m_Container[0];
        m_Container.erase(m_Container.begin());
    }
    delete m_Avatar; //Destruct avatar
    m_Avatar = nullptr;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::isLevelCompleted() const { return levelCompleted; }

void StudentWorld::removeDeadGameObjects() {
    for (int i = 0; i < m_Container.size(); i++) {
        if (m_Container[i] != nullptr &&
            !m_Container[i]->isAlive()) {
            delete m_Container[i];
            m_Container.erase(m_Container.begin() + i);
        }
    }
}

string StudentWorld::levelToString() {
    if (getLevel() < 10) {
        if (getLevel() == 1) {
            int a = getLevel();
        }
        return "0" + to_string(getLevel());
    }
    else {
        return to_string(getLevel());
    }
}

int StudentWorld::init() {
    //Initialize currentLevelBonus & containerSize
    currentLevelBonus = 1000;
    levelCompleted = false;

    string curLevel = "level" + levelToString() + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found ||
        result == Level::load_fail_bad_format) {
        return GWSTATUS_LEVEL_ERROR;
    }

    for (int x = 0; x <= VIEW_WIDTH - 1; x++) {
        for (int y = 0; y <= VIEW_HEIGHT - 1; y++) {
            Level::MazeEntry item = lev.getContentsOf(x, y);
            switch (item) {
            case Level::player: 
                m_Avatar = new Avatar(x, y, this);
                break;
            case Level::vert_ragebot:
                m_Container.push_back(new VerticalRageBot(this, x, y));
                break;
            case Level::horiz_ragebot:
                m_Container.push_back(new HorizontalRageBot(this, x, y));
                break;
            case Level::wall:
                m_Container.push_back(new Wall(x,y, this));
                break;
            case Level::crystal:
                m_Container.push_back(new Crystal(this, x, y));
                break;
            case Level::ammo:
                m_Container.push_back(new AmmoGoodie(this, x, y));
                break;
            case Level::extra_life:
                m_Container.push_back(new ExtraLifeGoodie(this, x, y));
                break;
            case Level::restore_health:
                m_Container.push_back(new RestoreHealthGoodie(this, x, y));
                break;
            case Level::exit:
                m_Container.push_back(new Exit(this, x, y));
                break;
            case Level::marble:
                m_Container.push_back(new Marble(this, x, y));
                break;
            case Level::pit: 
                m_Container.push_back(new Pit(this, x, y));
                break;
            case Level::thiefbot_factory:
                m_Container.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::ProductType::REGULAR));
                break;
            case Level::mean_thiefbot_factory:
                m_Container.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::ProductType::MEAN));
                break;
            case Level::empty:
                break;
            default:
                break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    setDisplayText();

    //Give each actor a chance to do something
    //Check Avatar health -> doSomething()
    if (m_Avatar->isAlive()) {
        m_Avatar->doSomething();
    }
    else {
        delete m_Avatar;
        m_Avatar = nullptr;
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    for (int i = 0; i < m_Container.size(); i++) {
        if (m_Container[i] != nullptr && m_Container[i]->isAlive()) {
            m_Container[i]->doSomething();
            if (!m_Container[i]->isAlive()) {
                delete m_Container[i];
                m_Container.erase(m_Container.begin() + i);
            }
            if (!m_Avatar->isAlive()) {
                delete m_Avatar;
                m_Avatar = nullptr;
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (isLevelCompleted()) {
                return GWSTATUS_FINISHED_LEVEL; //CHECK FOR COMPLETING LEVEL
            }
        }
        else {
            delete m_Container[i];
            m_Container.erase(m_Container.begin() + i);
        }
    }
    //Remove Newly Dead actors  
    removeDeadGameObjects();
    if (!m_Avatar->isAlive()) {
        delete m_Avatar;
        m_Avatar = nullptr;
        return GWSTATUS_PLAYER_DIED;
    }
    if (currentLevelBonus > 0) {
        currentLevelBonus--;
    }

    //check player/maze location
    if (isLevelCompleted()) {
        return GWSTATUS_FINISHED_LEVEL;
    }
	return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::setDisplayText() {
    int score = getScore();
    int level = getLevel();
    int livesLeft = getLives();
    double health = double(m_Avatar->getHP());
    int ammo = m_Avatar->getAmmo(); //figure out how to keep track of ammo
    unsigned int bonus = getCurrentLevelBonus();

    string s = formatDisplayText(score, level, livesLeft, health, ammo, bonus);
    setGameStatText(s);
}

string StudentWorld::formatDisplayText(int score, int level, int lives, double health, int ammo, unsigned int bonus) {

    double healthBar = (health / 20) * 100;
    int healthPercent = int(healthBar);

    string s = "Score: " + to_string(score) + " Level: " + to_string(level)
        + " Lives: " + to_string(lives) + " Health: " + to_string(healthPercent) + "%"
        + " Ammo: " + to_string(ammo) + " Current Level Bonus: " + to_string(bonus);
    return s;
}
