#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

enum PlayerType {OGRE,HUMAN,ELF,WIZARD};
enum SlotType {LEVEL_GROUND,HILL,CITY};
enum AttackType {NEAR, DISTANT, MAGIC};

struct Player newOgre(char playerName[100]);
struct Player newHuman(char playerName[100]);
struct Player newElf(char playerName[100]);
struct Player newWizard(char playerName[100]);
void createPlayers();
void populateSlots();
void addPlayersToRandomSlot();
void beginGame();
const char* getSlotName(enum SlotType slotType);
const char* getPlayerTypeName(enum PlayerType playerType);
void attackMode(struct Player * player);
void alterStatsBasedOnPosition(struct Player * player);
bool attackPlayer(struct Player * attacker, struct Player * attacked, enum AttackType attackType);
int checkRightOutOfBounds(int column, int row);
int checkLeftOutOfBounds(int column, int row);
int checkTopOutOfBounds(int column, int row);
int checkBottomOutOfBounds(int column, int row);
int getUniqueSlotHash(int column, int row);
struct Slot getSlot(int slotHash);
void updatePlayerSlotLocation(struct Player * player, int slotId);
void moveToAdjacentSlot(struct Player * player);
void doNearAttack(struct Player * player);
void doMagicAttack(struct Player * player);
void nearAttack(struct Player * attacker, struct Player * attacked);
int generateId();
void doDistantAttack(struct Player *player);
void distantAttack(struct Player * attcker, struct Player * attacked);
void magicAttack(struct Player * attacker, struct Player * attacked);
bool isWithinRangeForDistantAttack(struct Slot currentPlayerSlot, struct Slot enemyPlayerSlot);
void checkForWinner();
void printStatsOfPlayers();
bool giveDetailsOfPlayerAndChooseToAttack(struct Player * player, struct Player * enemyPlayer, enum AttackType attackType);

struct Player{
	int id;
	enum PlayerType type;
	char name[100];
	double lifePoints;
	int smartness;
	int strength;
	int magicSkills;
	int luck;
	int dexterity;
	int slotId;
	bool isDefeated;
};


struct Slot
{
	int id;
	int row;
	int column;
	int leftSlotId;
	int rightSlotId;
	int topSlotId;
	int bottomSlotId;
	enum SlotType type;
};

struct Player players[6];
struct Slot slots[7][7];
int numberOfPlayers;
int numberOfSlots;

int main()
{
	setvbuf(stdout, NULL, _IONBF, 0); //Need to have this for printing to eclipse console

	createPlayers();

	populateSlots();

	addPlayersToRandomSlot();

	beginGame();

}

void beginGame(){
	//Will loop indefinitely till the user quits, or if there's only one player left
	while(1)
	{
		printStatsOfPlayers();

		for(int i = 0; i < numberOfPlayers; i++){
			struct Player * player = &players[i];
			if(player->lifePoints <= 0){
				continue;
			}
			char playerChoice;
			printf("\nIt is %s's turn. You are in position [%d,%d].\nDo you want to move to an adjacent slot [a], attack [b], or quit [c]? ", players[i].name, getSlot(players[i].slotId).column, getSlot(players[i].slotId).row);

			scanf("%s", &playerChoice);
			switch(playerChoice){
			case 'a': moveToAdjacentSlot(&players[i]); break;
			case 'b': attackMode(&players[i]); break;
			case 'c': exit(1);
			default: printf("Typed %c", playerChoice);
			}
			checkForWinner();
		}

	}
}

void printStatsOfPlayers(){
	printf("\n-------Current Stats-----\n");
	for(int j = 0; j < numberOfPlayers; j++){
		struct Player player = players[j];
		printf("Player: %s, Type: %s, Position: [%d,%d], Position Type: %s , lifepoints: %f, Strength: %d, Magic Skills: %d, Luck: %d, Dexterity: %d, Smartness: %d\n",
				player.name, getPlayerTypeName(player.type),
				getSlot(player.slotId).column, getSlot(player.slotId).row,
				getSlotName(getSlot(player.slotId).type), player.lifePoints,
				player.strength, player.magicSkills, player.luck, player.dexterity, player.smartness);
	}
	printf("------End Current Stats-----\n\n");
}

void moveToAdjacentSlot(struct Player * player){
	struct Slot currentSlot = getSlot(player->slotId);
	char slotChoice;

	printf("You can move ");
	//print slot positions
	if(currentSlot.topSlotId != -1){
		printf("up [u] ");
	}
	if (currentSlot.bottomSlotId != -1){
		printf("down [d] ");
	}
	if (currentSlot.leftSlotId != -1){
		printf("left [l] ");
	}

	if (currentSlot.rightSlotId != -1){
		printf("right [r] :");
	}

	scanf("%s", &slotChoice);

	printf("You've selected %c\n", slotChoice);
	switch(slotChoice){
	case 'u':
		updatePlayerSlotLocation(player, currentSlot.topSlotId);
		break;
	case 'd':
		updatePlayerSlotLocation(player, currentSlot.bottomSlotId);
		break;
	case 'l':
		updatePlayerSlotLocation(player, currentSlot.leftSlotId);
		break;
	case 'r':
		updatePlayerSlotLocation(player, currentSlot.rightSlotId);
		break;
	default: printf("Invalid choice, skipping turn\n");
	}
}

void updatePlayerSlotLocation(struct Player * player, int slotId)
{
	if(slotId == -1){
		printf("Invalid choice, skipping turn\n");
		return;
	}

	printf("Moving player to position [%d,%d]\n", getSlot(slotId).column, getSlot(slotId).row);
	player->slotId = slotId;
}

void createPlayers(){
	printf("How many players do you want to create [Min 2, Max 6]? ");
	scanf("%d", &numberOfPlayers);

	if(numberOfPlayers > 6 || numberOfPlayers < 2){
		printf("Please enter a valid number");
		exit(0);
	}

	//Player creation is here.
	//User enters name and type, then the values for each type are generated
	int i;
	for(i = 0; i < numberOfPlayers; i++){
		char playerName[100];
		int playerType;
		struct Player player;

		printf("Enter name for Player %d : ", i + 1);
		scanf("%s", playerName);
		printf("Enter type for %s [0 = Ogre, 1 = Human, 2 = Elf, 3 = Wizard] : ", playerName);
		scanf("%d", &playerType);

		switch(playerType){
		case OGRE:
			player = newOgre(playerName);
			break;
		case HUMAN:
			player = newHuman(playerName);
			break;
		case ELF:
			player = newElf(playerName);
			break;
		case WIZARD:
			player = newWizard(playerName);
			break;
		default:
			printf("Please choose a valid type");
			exit(0);
		}
		players[i] = player;
	}
}

int generateId(){
	static int id = 1;
	return id++;
}

struct Player newOgre(char playerName[100]){
	struct Player player;
	strcpy(player.name,playerName);
	player.id = generateId();
	player.lifePoints = 100.0;
	player.type = OGRE;
	player.magicSkills = 0;
	player.smartness = rand() % 20;
	player.strength = (rand() % 20) + 80;
	player.dexterity = (rand() % 20) + 80;
	player.luck = rand() % (50 - player.smartness);
	return player;

}

struct Player newHuman(char playerName[100]){
	struct Player player;
	player.id = generateId();
	strcpy(player.name,playerName);
	player.lifePoints = 100.0;
	player.type = HUMAN;
	int totalPoints = 300;

	player.magicSkills = rand() % 100;
	totalPoints -= player.magicSkills;

	player.smartness = rand() % 100;
	totalPoints -= player.smartness;

	if(totalPoints < 100 && totalPoints > -1){
		player.strength = (rand() % totalPoints);
		totalPoints -= player.strength;
		if(totalPoints < 0){
			totalPoints = 0;
		}
	} else {
		player.strength = (rand() % 100);
	}

	if(totalPoints < 100 && totalPoints > -1){
		player.dexterity = (rand() % totalPoints);
		totalPoints -= player.dexterity;
		if(totalPoints < 0){
			totalPoints = 0;
		}
	} else {
		player.dexterity = (rand() % 100);
	}

	if(totalPoints < 100 && totalPoints > -1){
		player.luck = (rand() % totalPoints);
		totalPoints -= player.luck;
		if(totalPoints < 0){
			totalPoints = 0;
		}
	} else {
		player.luck = (rand() % 100);
	}

	if(totalPoints < 100 && totalPoints > -1){
		player.magicSkills = (rand() % totalPoints);
		totalPoints -= player.magicSkills;
		if(totalPoints < 0){
			totalPoints = 0;
		}
	} else {
		player.magicSkills = (rand() % 100);
	}

	return player;

}

struct Player newElf(char playerName[100]){
	struct Player player;
	player.id = generateId();
	strcpy(player.name,playerName);
	player.lifePoints = 100.0;
	player.type = ELF;
	player.magicSkills = (rand() % 30) + 50;
	player.smartness = (rand() % 30) + 70;
	player.strength = rand() % 50;
	player.dexterity = (rand() % 20) + 80;
	player.luck = (rand() % 40) + 60;
	return player;
}

struct Player newWizard(char playerName[100]){
	struct Player player;
	player.id = generateId();
	strcpy(player.name,playerName);
	player.lifePoints = 100.0;
	player.type = WIZARD;
	player.magicSkills = (rand() % 80) + 20;
	player.smartness = (rand() % 10) + 90;
	player.strength = (rand() % 20);
	player.dexterity = (rand() % 100);
	player.luck = (rand() % 50) + 50;
	return player;
}

const char* getPlayerTypeName(enum PlayerType playerTypes)
{
	switch (playerTypes)
	{
	case OGRE: return "Ogre";
	case WIZARD: return "Wizard";
	case HUMAN: return "Human";
	case ELF: return "Elf";
	default: return "";
	}
}

const char* getSlotName(enum SlotType slotType)
{
	switch (slotType)
	{
	case LEVEL_GROUND: return "Level Ground";
	case HILL: return "Hill";
	case CITY: return "City";
	default: return "";
	}
}

//Generate the slots using a random number generator
void populateSlots(){
	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 7; j++){
			struct Slot slot;
			slot.id = getUniqueSlotHash(i, j);
			slot.column = i;
			slot.row = j;
			slot.leftSlotId = checkLeftOutOfBounds(i - 1, j);
			slot.rightSlotId = checkRightOutOfBounds(i + 1, j);
			slot.topSlotId = checkTopOutOfBounds(i, j - 1);
			slot.bottomSlotId = checkBottomOutOfBounds(i, j + 1);
			slot.type = rand() % 3;
			slots[i][j] = slot;
		}
	}
}

int getUniqueSlotHash(int column, int row)
{
	return column * 17 + row * 37;
}

struct Slot getSlot(int slotHash)
{
	for(int i = 0; i < 7; i++)
	{
		for(int j = 0; j < 7; j++)
		{
			if(slots[i][j].id == slotHash)
			{
				return slots[i][j];
			}
		}
	}

	struct Slot slot;
	slot.row = -1;
	slot.column = -1;
	return slot;

}

int checkLeftOutOfBounds(int column, int row)
{
	if(column < 0){
		return -1;
	}

	return getUniqueSlotHash(column, row);
}

int checkRightOutOfBounds(int column, int row)
{
	if (column > 6){
		return -1;
	}
	return getUniqueSlotHash(column, row);
}

int checkTopOutOfBounds(int column, int row)
{
	if (row < 0){
		return -1;
	}
	return getUniqueSlotHash(column, row);
}

int checkBottomOutOfBounds(int column, int row)
{
	if (row > 6){
		return -1;
	}
	return getUniqueSlotHash(column, row);
}

void addPlayersToRandomSlot(){
	int i;
	for(i = 0; i < numberOfPlayers; i ++){
		players[i].slotId = getUniqueSlotHash(rand() % 7, rand() % 7);
	}
}

void attackMode(struct Player * player){
	char attackChoice;
	printf("You have chosen to attack!\n");
	printf("You can do a Near Attack[n], a Distant Attack[d] or a Magic Attack[m] : ");
	scanf(" %c", &attackChoice);

	switch (attackChoice){
	case 'n': doNearAttack(player); break;
	case 'd': doDistantAttack(player); break;
	case 'm': doMagicAttack(player); break;
	}
}

void doMagicAttack(struct Player * player){

	if(player->smartness + player->magicSkills > 150){
		for(int i = 0; i < numberOfPlayers; i++){
			struct Player * enemyPlayer = &players[i];

			if(enemyPlayer->id != player->id  && !enemyPlayer->isDefeated){
				bool hasAttacked = giveDetailsOfPlayerAndChooseToAttack(player, enemyPlayer, 2);

				if(hasAttacked){
					break;
				}
			}
		}

	} else {
		printf("Player's stats are not high enough to do a magic attack\n");
	}
}

//Checks for players whose lifepoints are less than or equal to 0
//These players are removed from the game
//Once there's only one player remaining, the winner is printed
//And the game ends
void checkForWinner(){
	int numberOfLosers = 0;
	for(int i = 0; i < numberOfPlayers; i++){
		if(players[i].lifePoints <= 0){
			if(!players[i].isDefeated){
				printf("player %s has been defeated!\n", players[i].name);
				players[i].isDefeated = true;
			}
			numberOfLosers++;
		}
	}

	if(numberOfLosers == numberOfPlayers - 1){
		printf("We have a winner! The winner is... ");
		struct Player winner;
		for(int i = 0; i < numberOfPlayers; i++){
			if(players[i].lifePoints > 0 && !players[i].isDefeated){
				winner = players[i];
			}
		}
		printf(" %s!\n", winner.name);
		exit(0);
	}
}


bool isAdjacentToPlayer(struct Slot currentPlayerSlot, int enemyPlayerSlotId){

	//Get all the slot hash surrounding player
	int slotHashAbovePlayer = currentPlayerSlot.topSlotId;
	int slotHashLeftOfPlayerPlayer = currentPlayerSlot.leftSlotId;
	int slotHashBelowPlayer = currentPlayerSlot.bottomSlotId;
	int slotHashRightOfPlayer = currentPlayerSlot.rightSlotId;
	int currentSlotOfPlayer = currentPlayerSlot.id;

	return enemyPlayerSlotId == slotHashAbovePlayer
			|| enemyPlayerSlotId == slotHashLeftOfPlayerPlayer
			|| enemyPlayerSlotId == slotHashBelowPlayer
			|| enemyPlayerSlotId == slotHashRightOfPlayer
			|| enemyPlayerSlotId == currentSlotOfPlayer;
}

void doNearAttack(struct Player * player)
{
	//loop through all the players in the players array
	for(int i = 0; i < numberOfPlayers; i++){
		struct Slot currentPlayerSlot = getSlot(player->slotId);

		struct Player * enemyPlayer = &players[i];
		int enemyPlayerSlotId = enemyPlayer->slotId;
		int enemyPlayerId = enemyPlayer->id;

		if(isAdjacentToPlayer(currentPlayerSlot, enemyPlayerSlotId) && enemyPlayerId != player->id && !enemyPlayer->isDefeated){
			bool hasAttacked = giveDetailsOfPlayerAndChooseToAttack(player, enemyPlayer, 0);

			if(hasAttacked){
				break;
			}
		}
	}
}

bool giveDetailsOfPlayerAndChooseToAttack(struct Player * player, struct Player * enemyPlayer, enum AttackType attackType){
	char playerChoice;
	printf("There is an enemy near you. Details -> Name : %s, Type: %s, Slot: %s\n", enemyPlayer->name, getPlayerTypeName(enemyPlayer->type), getSlotName(getSlot(enemyPlayer->slotId).type));
	printf("Do you want to attack this player? y/n : ");
	scanf(" %c", &playerChoice);
	switch(playerChoice){
	case 'y' : attackPlayer(player, enemyPlayer, attackType); return true;
	case 'n' : return false;
	}

	return false;
}

void doDistantAttack(struct Player * player)
{

	//loop through all the players in the players array
	for(int i = 0; i < numberOfPlayers; i++){
		struct Slot currentPlayerSlot = getSlot(player->slotId);
		struct Player * enemyPlayer = &players[i];
		int enemyPlayerSlotId = enemyPlayer->slotId;

		if(isWithinRangeForDistantAttack(currentPlayerSlot, getSlot(enemyPlayerSlotId)) && !enemyPlayer->isDefeated){
			bool hasAttacked = giveDetailsOfPlayerAndChooseToAttack(player, enemyPlayer, 1);

			if(hasAttacked){
				break;
			}
		}
	}
}

bool isWithinRangeForDistantAttack(struct Slot currentPlayerSlot, struct Slot enemyPlayerSlot){

	int rowDistance = abs(currentPlayerSlot.row - enemyPlayerSlot.row);
	int columnDistance = abs(currentPlayerSlot.column - enemyPlayerSlot.column);

	if(rowDistance > 1 && columnDistance < 5){
		return true;
	} else if (columnDistance > 1 && columnDistance < 5){
		return true;
	}

	return false;
}

//This is where the players stats are changed based on the terrain they currently are on
void alterStatsBasedOnPosition(struct Player * player){

	printf("Altering stats for %s based on slot type...\n", player->name);

	if(getSlot(player->slotId).type == HILL){
		printf("On a Hill slot. ");
		if(player->dexterity < 50){
			player->strength -= 10;
			if(player->strength < 0){
				player->strength = 0;
			}
			printf("%s's strength decreased by 10 to %d.\n", player->name, player->strength);
		} else {
			player->strength += 10;
			if (player->strength > 100){
				player->strength = 100;
			}
			printf("%s's strength increased by 10 to %d.\n", player->name, player->strength);
		}
	} else if(getSlot(player->slotId).type == CITY){
		printf("On a City slot. ");
		if(player->smartness <= 50){
			player->dexterity -= 10;
			if(player->dexterity < 0){
				player->dexterity = 0;
			}
			printf("%s's strength decreased by 10 to %d.\n", player->name, player->strength);
		} else {
			player->magicSkills += 10;
			if (player->magicSkills > 100){
				player->magicSkills = 100;
			}
			printf("%s's magic skills increased by 10 to %d.\n", player->name, player->strength);
		}
	} else if(getSlot(player->slotId).type == LEVEL_GROUND){
		printf("On a Level Ground slot. %s's stats are unchanged.\n", player->name);
	}
}

//This is where the life points go down for the attacker/attacked based on strength
bool attackPlayer(struct Player * attacker, struct Player * attacked, enum AttackType attackType){
	alterStatsBasedOnPosition(attacker);
	alterStatsBasedOnPosition(attacked);

	printf("%s attacked %s!\n Attackers lifePoints = %f, attacked lifepoints = %f (Before)\n", attacker->name, attacked->name, attacker->lifePoints, attacked->lifePoints);

	switch(attackType){
	case NEAR: nearAttack(attacker, attacked); break;
	case DISTANT: distantAttack(attacker, attacked); break;
	case MAGIC : magicAttack(attacker, attacked); break;
	}
	printf("%s's lifePoints = %f, %s's lifepoints = %f (After)\n", attacker->name, attacker->lifePoints, attacked->name, attacked->lifePoints);
	return true;
}


void magicAttack(struct Player * attacker, struct Player * attacked){
	double damage = (0.5 * attacker->magicSkills) + (0.2 * attacker->smartness);
	printf("Attacking %s for %f\n", attacked->name, damage);
	attacked->lifePoints -= damage;
	if(attacked->lifePoints < 0){
		attacked->lifePoints = 0;
	}
}

void nearAttack(struct Player * attacker, struct Player * attacked){
	if(attacked->strength <= 70){
		printf("Attacking for %f\n", (0.5 * attacker->strength));
		attacked->lifePoints -= (0.5 * attacker->strength);
	} else {
		printf("Attacker is injured. Lifepoints drop by %f\n", (0.3 * attacked->strength));
		attacker->lifePoints -= (0.3 * attacked->strength);
	}
	if(attacked->lifePoints < 0){
		attacked->lifePoints = 0;
	}
}

void distantAttack(struct Player * attacker, struct Player * attacked){
	if(attacked->dexterity >= attacker->dexterity){
		printf("Dexterity of %s is higher than %s's. Attacking for 0\n", attacked->name, attacker->name);
	} else {
		printf("Attacking for %f\n", (0.3 * attacker->strength));
		attacked->lifePoints -= (0.3 * attacker->strength);
	}
	if(attacked->lifePoints < 0){
		attacked->lifePoints = 0;
	}
}
