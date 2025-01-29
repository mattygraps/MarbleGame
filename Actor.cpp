#include "Actor.h"
#include "StudentWorld.h"


//Actor Definitions
Actor::Actor(int imageID, double startX, double startY, int HP, StudentWorld* world)
	: GraphObject(imageID, startX, startY), m_World(world), m_HP(HP) {}



void Actor::spaceInFrontofActor(int dir, int& x, int& y) {
	switch (dir) {
	case up:
		y++;
		break;
	case left:
		x--;
		break;
	case down:
		y--;
		break;
	case right:
		x++;
		break;
	default:
		break;
	}
	return;
}

//Agent Definitions
Agent::Agent(int imageID, double startX, double startY, StudentWorld* world, int HP)
	: Actor(imageID, startX, startY, HP, world) {}


bool Agent::moveIfPossible() {
	int x, y;
	switch (getDirection()) {
	case up:
		x = int(getX());
		y = int(getY() + 1);
		break;
	case left:
		x = int(getX() - 1);
		y = int(getY());
		break;
	case down:
		x = int(getX());
		y = int(getY() - 1);
		break;
	case right:
		x = int(getX() + 1);
		y = int(getY());
		break;
	default:
		return false;
	}
	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (x == getWorld()->getContainer(i)->getX() &&
			y == getWorld()->getContainer(i)->getY()) {
			if (!getWorld()->getContainer(i)->allowsAgentColocation()) {
				if (getWorld()->getContainer(i)->bePushedBy(this, x, y)) {
					getWorld()->getContainer(i)->bePushedBy(this, x, y);
					return true;
				}
				else {
					return false;
				}
			}
		}	
	}
	return true;
}


//Avatar Definitions
Avatar::Avatar(double startX, double startY, StudentWorld* world)
	: Agent(IID_PLAYER, startX, startY, world, 20) {
	p = nullptr;
}

void Avatar::doSomething() {
	
	if (!isAlive()) {
		return;
	}	
	int ch;
	int curX, curY;
	if (getWorld()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_UP:
			setDirection(up);
			if (moveIfPossible()) {
				moveTo(getX(), getY() + 1);
			}
			break;	
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (moveIfPossible()) {
				moveTo(getX() - 1, getY());
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (moveIfPossible()) {
				moveTo(getX(), getY() - 1);
			}
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (moveIfPossible()) {
				moveTo(getX() + 1, getY());
			}
			break;
		case KEY_PRESS_SPACE:
			if (m_ammo > 0) {
				curX = int(getX()), curY = int(getY());
				spaceInFrontofActor(getDirection(), curX, curY);
				getWorld()->playSound(shootingSound());
				p = new Pea(getWorld(), curX, curY, getDirection());
				getWorld()->addActor(p);
				p->setDirection(getDirection());
				m_ammo--;
			}
			break;
		case KEY_PRESS_ESCAPE:
			getWorld()->decLives();
			break;
		default:
			break;
		}
	}
}

//Robot Definitions
Robot::Robot(StudentWorld* world, int startX, int startY, int imageID,
	int hitPoints, int score, int startDir)
	: Agent(imageID, startX, startY, world, hitPoints), robot_score(score),
		m_tick(1), robot_p(nullptr) {
	setDirection(startDir);
}

void Robot::damage(int damageAmt) {
	setHitPoints(getHP() - damageAmt);
	if (!isAlive()) {
		setDead();
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(getRobotScore());
	}
}

bool Robot::actionTick() {
	int ticks = (28 - getWorld()->getLevel()) / 4;
	if (ticks < 3) {
		ticks = 3;
	}
	if (m_tick == ticks) {
		m_tick = 1;
		return true;
	}
	m_tick++;
	return false;
}

void Robot::doSomething() {
	if (!isAlive()) { 
		return;
	}
	if (!actionTick()) { return; }
	doDiffRobotSomething();
}

bool Robot::peaCanFire() {
	int avatar_x = int(getWorld()->getAvatar()->getX()), avatar_y = int(getWorld()->getAvatar()->getY());
	int robot_x = int(getX()), robot_y = int(getY());

	switch (getDirection()) {
	case up:
		if (avatar_x == robot_x && avatar_y > robot_y) {
			for (int i = 0; i < getWorld()->getContainerSize(); i++) {
				int actor_x = int(getWorld()->getContainer(i)->getX()), actor_y = int(getWorld()->getContainer(i)->getX());
				if (robot_y <= actor_y && avatar_y >= actor_y &&
					getWorld()->getContainer(i)->stopsPea()) {
					return false;
				}
			}
			return true;
		}
		break;
	case down:
		if (avatar_x == robot_x && avatar_y < robot_y) {
			for (int i = 0; i < getWorld()->getContainerSize(); i++) {
				int actor_x = int(getWorld()->getContainer(i)->getX()), actor_y = int(getWorld()->getContainer(i)->getX());
				if (robot_y >= actor_y && avatar_y <= actor_y &&
					getWorld()->getContainer(i)->stopsPea()) {
					return false;
				}
			}
			return true;
		}
		break;
	case left:
		if (avatar_y == robot_y && avatar_x < robot_x) {
			for (int i = 0; i < getWorld()->getContainerSize(); i++) {
				int actor_x = int(getWorld()->getContainer(i)->getX()), actor_y = int(getWorld()->getContainer(i)->getX());
				if (robot_x > actor_x && avatar_x < actor_x &&
					getWorld()->getContainer(i)->stopsPea()) {
					return false;
				}
			}
			return true;
		}
		break;
	case right:
		if (avatar_y == robot_y && avatar_x > robot_x) {
			for (int i = 0; i < getWorld()->getContainerSize(); i++) {
				int actor_x = int(getWorld()->getContainer(i)->getX()), actor_y = int(getWorld()->getContainer(i)->getX());
				if (robot_x < actor_x && avatar_x > actor_x &&
					getWorld()->getContainer(i)->stopsPea()) {
					return false;
				}
			}
			return true;
		}
		break;
	default:
		break;;
	}
	return false;
}

void Robot::firePea() {
	int curX, curY;
	curX = int(getX()), curY = int(getY());
	spaceInFrontofActor(getDirection(), curX, curY);
	getWorld()->playSound(shootingSound());
	robot_p = new Pea(getWorld(), curX, curY, getDirection());
	getWorld()->addActor(robot_p);
	robot_p->setDirection(getDirection());
}

//RageBot Definitions
RageBot::RageBot(StudentWorld* world, int startX, int startY, int startDir)
	: Robot(world, startX, startY, IID_RAGEBOT, 10, 100, startDir) {}

void RageBot::moveRageBot() {
	if (moveIfPossible()) {
		int curX, curY;
		curX = int(getX()), curY = int(getY());
		spaceInFrontofActor(getDirection(), curX, curY);
		moveTo(curX, curY);
	}
	else {
		switch (getDirection()) {
		case up:
			setDirection(down);
			break;
		case down:
			setDirection(up);
			break;
		case left:
			setDirection(right);
			break;
		case right:
			setDirection(left);
			break;
		default:
			break;
		}
	}
}

void RageBot::doDiffRobotSomething() {
	if (peaCanFire()) {
		firePea();
	}
	else {
		moveRageBot();
	}
}

//VerticalRageBot Definitions
VerticalRageBot::VerticalRageBot(StudentWorld* world, int startX, int startY) 
	: RageBot(world, startX, startY, up) {}

//HorizontalRageBot Definitions
HorizontalRageBot::HorizontalRageBot(StudentWorld* world, int startX, int startY)
	: RageBot(world, startX, startY, left) {}

//TheifBot Definition
ThiefBot::ThiefBot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score)
	: Robot(world, startX, startY, imageID, hitPoints, score, right),
	distanceBeforeTurning(rand() % 5 + 1), curDistance(0), goodieP(nullptr) {}

void ThiefBot::doDiffRobotSomething() {
	//Shoot if meanthiefbot
	doThiefBotSomething();
	//if on a Pickupable Item
	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (getX() == getWorld()->getContainer(i)->getX() &&
			getY() == getWorld()->getContainer(i)->getY() &&
			getWorld()->getContainer(i)->isStealable()) {
			//pick up item (remove from board, play munching sound)
			goodieP = getWorld()->getContainer(i);
			goodieP->setVisible(false);
			getWorld()->playSound(SOUND_ROBOT_MUNCH);
			return;
		}
	}
	//else if distanceWithoutTurning is not met and no obstruction in next square 
	if (curDistance < distanceBeforeTurning && moveIfPossible()) {
		//move to next square
		int nextX = int(getX()), nextY = int(getY());
		spaceInFrontofActor(getDirection(), nextX, nextY);
		moveTo(nextX, nextY);
		distanceBeforeTurning++;
	}
	else {
		//set a new distanceWithoutTurning
		distanceBeforeTurning = 0;
		bool dirArr[4] = { false, false, false, false };
		//pick a new random direction d
		switch (rand() % 3 + 1) {
		case 1:
			setDirection(up);
			dirArr[0] = true;
			break;
		case 2:
			setDirection(left);
			dirArr[1] = true;
			break;
		case 3:
			setDirection(down);
			dirArr[2] = true;
			break;
		case 4:
			setDirection(right);
			dirArr[3] = true;
			break;
		default:
			break;
		}
		while ((dirArr[0] == false || dirArr[1] == false
			|| dirArr[2] == false || dirArr[3] == false)) {
			if (moveIfPossible()) {
				//move to next square
				int nextX = int(getX()), nextY = int(getY());
				spaceInFrontofActor(getDirection(), nextX, nextY);
				moveTo(nextX, nextY);
				distanceBeforeTurning++;
			}
			switch (getDirection()) {
			case up:
				setDirection(left);
				dirArr[1] = true;
				break;
			case left:
				setDirection(down);
				dirArr[2] = true;
				break;
			case down:
				setDirection(right);
				dirArr[3] = true;
				break;
			case right:
				setDirection(up);
				dirArr[0] = true;
				break;
			default:
				break;
			}
		}
	}
}

void ThiefBot::damage(int damageAmt) {
	setHitPoints(getHP() - damageAmt);
	if (!isAlive()) {
		goodieP->moveTo(getX(), getY());
		goodieP->setVisible(true);
		setDead();
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(getRobotScore());
	}
}

//RegularThiefBot Definition
RegularThiefBot::RegularThiefBot(StudentWorld* world, int startX, int startY)
	:ThiefBot(world, startX, startY, IID_THIEFBOT, 5, 10) {}

//MeanThiefBot Definition
MeanThiefBot::MeanThiefBot(StudentWorld* world, int startX, int startY) 
	: ThiefBot(world, startX, startY, IID_MEAN_THIEFBOT, 8, 20) {}

void MeanThiefBot::doThiefBotSomething() {
	if (peaCanFire()) {
		firePea();
	}
}

//ThiefBotFactory Definitions
ThiefBotFactory::ThiefBotFactory(StudentWorld* world, int startX, int startY, ProductType type)
	: Actor(IID_ROBOT_FACTORY, startX, startY, 20, world), 
		m_Type(type), m_ThiefBot(nullptr) {}

void ThiefBotFactory::doSomething() {
	int size = getWorld()->getContainerSize();
	int countBots = 0;
	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (getWorld()->getContainer(i)->isThiefBot() &&
			(getX() - 3) <= getWorld()->getContainer(i)->getX() &&
			(getX() + 3) >= getWorld()->getContainer(i)->getX() &&
			(getY() - 3) <= getWorld()->getContainer(i)->getY() &&
			(getY() + 3) >= getWorld()->getContainer(i)->getY()){
			if (getX() == getWorld()->getContainer(i)->getX() &&
				getY() == getWorld()->getContainer(i)->getY()) {
				return;
			}
			countBots++;
			}
	}
	if (countBots < 3 && (rand() % 50 + 1) == 25) {
		//Add 1:50 chance spawn thiefbot
		switch (m_Type) {
		case REGULAR:
			m_ThiefBot = new RegularThiefBot(getWorld(), int(getX()), int(getY()));
			getWorld()->addActor(m_ThiefBot);
			break;
		case MEAN:
			m_ThiefBot = new MeanThiefBot(getWorld(), int(getX()), int(getY()));
			getWorld()->addActor(m_ThiefBot);
			break;
		default:
			break;
		}
	}
}

//Exit Definitions
Exit::Exit(StudentWorld* world, int startX, int startY) 
	:Actor(IID_EXIT, startX, startY, 20, world) {
	setVisible(false);
}

void Exit::doSomething() {
	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (getWorld()->getContainer(i)->isCrystal()) {
			return;
		}
	}
	if (isVisible() == false) {
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
	}
	if (getX() == getWorld()->getAvatar()->getX() &&
		getY() == getWorld()->getAvatar()->getY() &&
		isVisible()) {
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->increaseScore(2000 + getWorld()->getCurrentLevelBonus());
		getWorld()->advanceToNextLevel();
		getWorld()->setLevelCompleted(true);
	}
}

//Wall Definitions
Wall::Wall(double startX, double startY, StudentWorld* world) 
	: Actor(IID_WALL, startX, startY, 20, world) {}

//Pea Definitions
Pea::Pea (StudentWorld* world, int startX, int startY, int startDir) 
	: Actor(IID_PEA, startX, startY, 20, world) {
	setDirection(right);
}

void Pea::checkToDoDamage(int& x, int& y) {
	if (x == getWorld()->getAvatar()->getX() &&
		y == getWorld()->getAvatar()->getY()) {
		getWorld()->getAvatar()->damage(2);
		setDead();
		return;
	}
	
	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (!getWorld()->getContainer(i)->isAPea() &&
			x == int(getWorld()->getContainer(i)->getX()) &&
			y == int(getWorld()->getContainer(i)->getY()) &&
			getWorld()->getContainer(i)->stopsPea()) {
			if (getWorld()->getContainer(i)->isDamageable()) {
				getWorld()->getContainer(i)->damage(2);
				setDead();
				return;
			}
			else {
 				setDead();
				return;
			}
		}
	}
}

void Pea::doSomething() {
	if (!isAlive()) {
		return;
	}
	int x = int(getX()), y = int(getY());
	checkToDoDamage(x, y);
	if (!isAlive()) {
		return;
	}
	spaceInFrontofActor(getDirection(), x, y);
	moveTo(x, y);
	checkToDoDamage(x, y);
}

//Marble Definitions
Marble::Marble(StudentWorld* world, int startX, int startY)
	: Actor(IID_MARBLE, startX, startY, 10, world) {}

void Marble::damage(int damageAmt) {
	decHitPoints(damageAmt);
}

bool Marble::bePushedBy(Agent* a, int x, int y) {
	if (a->canPushMarbles()) {
		spaceInFrontofActor(a->getDirection(), x, y);
		for (int i = 0; i < getWorld()->getContainerSize(); i++) {
			if (x == getWorld()->getContainer(i)->getX() &&
				y == getWorld()->getContainer(i)->getY() &&
				!getWorld()->getContainer(i)->allowsMarbleColocation()) {
				return false;
			}
		}
		moveTo(x, y);
		return true;
	}
	return false;
	
}

//Pit Definitions
Pit::Pit(StudentWorld* world, int startX, int startY)
	: Actor(IID_PIT, startX, startY, 20, world) {}

void Pit::doSomething() {
	if (!isAlive()) { return; }

	for (int i = 0; i < getWorld()->getContainerSize(); i++) {
		if (getX() == getWorld()->getContainer(i)->getX() &&
			getY() == getWorld()->getContainer(i)->getY() &&
			getWorld()->getContainer(i)->isSwallowable()) {
			getWorld()->getContainer(i)->setDead();
			setDead();
		}
	}
}

//PickupableItem Definitions
PickupableItem::PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score)
	: Actor(imageID, startX, startY, 20, world), item_Score(score) {}

void PickupableItem::doSomething() {
	if (!isAlive()) { return; }
	if (getX() == getWorld()->getAvatar()->getX() &&
		getY() == getWorld()->getAvatar()->getY() &&
		isVisible()) {
		getWorld()->increaseScore(getItemScore());
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		doItemSomething();
	}
}

//Goodie Definitions
Goodie::Goodie(StudentWorld* world, int startX, int startY, int imageID, int score)
	: PickupableItem(world, startX, startY, imageID, score) {}

//ExtraLife Goodie Definitions
ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, int startX, int startY) 
	: Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000) {}

void ExtraLifeGoodie::doItemSomething() {
	getWorld()->incLives();
}

//RestoreHealthGoodie Definitions
RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* world, int startX, int startY) 
	: Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500) {}

void RestoreHealthGoodie::doItemSomething() {
	getWorld()->getAvatar()->setHitPoints(20);
}

//AmmoGoodie Definitions
AmmoGoodie::AmmoGoodie(StudentWorld* world, int startX, int startY) 
	: Goodie(world, startX, startY, IID_AMMO, 100) {}

void AmmoGoodie::doItemSomething() {
	getWorld()->getAvatar()->increaseAmmo();
}

//Crystal Definitions
Crystal::Crystal(StudentWorld* world, int startX, int startY) 
	: PickupableItem(world, startX, startY, IID_CRYSTAL, 50) {}


