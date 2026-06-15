#include<iostream>
#include<windows.h>
#include<conio.h>
#include <mmsystem.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <algorithm>
using namespace std;
#pragma comment(lib, "winmm.lib")

/* setups */
void gotoxy(int,int);
char getCharAtxy(short int, short int);
void screenSetup(int,int,int,int,bool);
void setColor(int);
void playBackgroundMusic();

/* screens */
void mainMenu(int,int,int);
void mainScreen();
void gameRoomCP();
void aboutDeveloperScreen(int,int);

/* game screen functionalities */
void gameHeader(int,int);
void gameHeaderCharacter(int,int);
void updateBoard();

/* player functions */
void playerPrint();
void removePlayer();
void playerJump();
void playerAttack();
void playerRun();
void gravity();
bool shouldJump();
void playerHealth();


int pX = 50, pY = 29; // player coordinates
int camX = 0, camY = 0; // camera coordinates

/* ---- stages, sections & the side-scrolling map ----
   A stage is a wide ASCII field loaded from stages\stageN.txt. The field is
   FIELD_ROWS tall and any multiple of VIEW_W wide; the screen shows one VIEW_W
   slice (a "section"). Walking off the left/right edge flips to the neighbouring
   section, so the world can be wider than the screen.                         */
const int VIEW_W     = 120;   // visible width (one section)
const int FIELD_ROWS = 37;    // play rows; console row = field row + 3
const int GROUND_TOP = 28;    // sprite-top Y of anything standing on the ground
const int STAGE_FILES = 3;    // number of stage*.txt layouts we cycle through

string fieldRows[FIELD_ROWS]; // the whole wide field for the current stage
int fieldWidth   = VIEW_W;    // width of the loaded field
int sectionCount = 1;         // how many VIEW_W slices the field holds
int section      = 0;         // which slice is on screen
int stage        = 1;         // current stage (endless: 1, 2, 3, ...)
int stageKillTarget = 5;      // kills needed to clear the current stage

void loadStage(int s);
void buildDefaultField(int s);
void saveField(int idx);
void renderField();
void resetActors();
void startStage(int s);
bool changeSection(int delta);
void banner(const string &msg);

/* enemy functions */
const int maxEnemies = 12;
int enemyX[maxEnemies], enemyY[maxEnemies], enemyType[maxEnemies], enemyDir[maxEnemies], enemyState[maxEnemies], enemyCount = 0, activeEnemies = 0;
bool isEnemyAlive[maxEnemies];
void generateEnemy();
void printEnemy(int,int,int,int,int);
void removeEnemy(int,int,int,int,int);
void enemies(int);
void killEnemy(int);
void destroyEnemy(int);          // kill + erase + update score/kills/counters
int  enemyHitIndex(int x, int y);// alive enemy whose body box covers (x,y), else -1
void meleeAttack();              // resolve a sword swing against nearby enemies
int  freeEnemySlot();            // reusable (dead) enemy slot, else -1

int bulletX[200], bulletY[200], bulletCount = 0, bulletDir[200], bulletOwner[200];
bool isBulletActive[200];
const int maxBullets = 200;

void generateBullet(string, int, int, int);
void eraseBullet(int x, int y);
void printBullet(int x, int y);
void moveBullet();
void makeBulletInactive(int idx);
int  freeBulletSlot();           // index of a reusable inactive bullet, else -1
bool hitsPlayer(int x, int y);   // does (x,y) fall inside the player's body box

DWORD lastShotTime = 0;          // for fire-rate cooldown



string activeScreen, gameState, playerState = "idle", playerDir="left"; // player states

int coins = 100, currentLevel = 1, currentRoom = 0, enemiesKilled = 0, health = 100, score = 0, bulletsLeft = 100, lives = 3, currentRoomKills = 0;
bool isGameOver = false, isWin = false;

main()
{
    screenSetup(105,40,120,40, FALSE);
    playBackgroundMusic();
    mainScreen();
}
void playBackgroundMusic()
{
    // Resolve audios\bg.wav relative to the executable so it plays no matter
    // what the current working directory is when the game is launched.
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    string dir = exePath;
    size_t slash = dir.find_last_of("\\/");
    dir = (slash == string::npos) ? "." : dir.substr(0, slash);
    string wav = dir + "\\audios\\bg.wav";

    // SND_FILENAME = treat the string as a path; ASYNC = don't block; LOOP = repeat.
    if(!PlaySound(wav.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP))
    {
        // Fall back to a path relative to the current directory.
        PlaySound("audios\\bg.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
}
void mainScreen()
{
    system("cls");
    int option = 0;
    gameHeaderCharacter(5,5);
    gameHeader(22,3);
    gameHeaderCharacter(70,5);
    mainMenu(3,15,option);
    gotoxy(50,16);cout <<"---- Instructions ----";
    gotoxy(50,18);cout << "Press ["<< (char)24 <<"] or [W] For Upword Navigation";
    gotoxy(50,19);cout << "Press ["<< (char)25 <<"] or [S] For Downword Navigation";
    gotoxy(50,20);cout << "Press [SPACE] or [Q] For Selecting Option";
    while(true){
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')){
            if(option < 6){
                option++;
            }
            mainMenu(3,15,option);
            cout << option;
        }
        else if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')){
            if(option > 0){
                option--;
            }
            mainMenu(3,15,option);
            cout << option;
        }
        else if(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState('Q')){
            if(option == 0){
                gameRoomCP();
            }else if(option == 5){
                Sleep(200);
                aboutDeveloperScreen(3,15);
            }
            else if(option == 6){
                exit(0);
            }
        }
        Sleep(200);
    }
}
void mainMenu(int x,int y,int option)
{
    char op[7] = {' ',' ',' ',' ',' ',' ',' '};
    if(option == 0)
        op[option] = '+';
    else if(option == 1)
        op[option] = '+';
    else if(option == 2)
        op[option] = '+';
    else if(option == 3)
        op[option] = '+';
    else if(option == 4)
        op[option] = '+';
    else if(option == 5)
        op[option] = '+';
    else if(option == 6)
        op[option] = '+';

    gotoxy(x,y);cout << "------ Menu ------";
    gotoxy(x,++y);cout << " ";
    gotoxy(x,++y);cout << "["<<op[0]<<"] New Game";
    gotoxy(x,++y);cout << "["<<op[1]<<"] Continue Game";
    gotoxy(x,++y);cout << "["<<op[2]<<"] Shop";
    gotoxy(x,++y);cout << "["<<op[3]<<"] Story of Game";
    gotoxy(x,++y);cout << "["<<op[4]<<"] Select Your Character";
    gotoxy(x,++y);cout << "["<<op[5]<<"] About Developer";
    gotoxy(x,++y);cout << "["<<op[6]<<"] Exit Game";
}
void gameRoomCP()
{
    int count = 0;
    isGameOver = false;
    system("cls");
    startStage(1);                  // load stage 1, draw the field, place the player
    Sleep(100);

    while(!isGameOver){
        if(GetAsyncKeyState('P')){
            while(true)
            {
                gotoxy(50, 4); cout << "[PAUSED]";
                if (GetAsyncKeyState('R'))
                {
                    gotoxy(50, 4); cout << "[RESUMED]";
                    break;
                }
            }
            Sleep(90);
            gotoxy(50, 4); cout << "         ";
        }
        else if(GetAsyncKeyState(VK_ESCAPE))
        {
            system("cls");
            break;
        }

        // jump input
        if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')){
            playerJump();
        }

        // move right / scroll to the next section at the screen edge
        if(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')){
            playerDir = "right";
            if(pX <= 104 && getCharAtxy(pX+15, pY + 3) == ' ')
                playerRun();
            else if(pX > 104 && section < sectionCount - 1)
                changeSection(+1);
        }
        // move left / scroll to the previous section at the screen edge
        else if(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')){
            playerDir = "left";
            if(pX >= 3 && getCharAtxy(pX-3, pY + 3) == ' ')
                playerRun();
            else if(pX < 4 && section > 0)
                changeSection(-1);
        }

        // attack input
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')){
            playerAttack();
        }

        if(GetAsyncKeyState(VK_SPACE) && bulletsLeft > 0 && GetTickCount() - lastShotTime > 150)
        {
            int dir = (playerDir == "right");
            generateBullet("player", dir, pX, pY);
            lastShotTime = GetTickCount();
        }

        // keep up to 4 enemies on screen until the stage's kill quota is in sight
        if(activeEnemies < 4 && (currentRoomKills + activeEnemies) < stageKillTarget)
        {
            generateEnemy();
        }

        if(count == 15)
        {
            enemies(3);
        }
        else if(count == 25)
        {
            enemies(1);
        }
        else if(count == 40){
            enemies(0);
            count = 0;
        }
        count++;

        if(health <= 0)
        {
            if(lives != 0){
                lives--;
                health = 100;
                updateBoard();
            }
            else
            {
                isGameOver = true;
                isWin = false;
            }
        }

        // stage cleared -> advance forever, raising the difficulty each time
        if(currentRoomKills >= stageKillTarget)
        {
            banner("STAGE " + to_string(stage) + " CLEARED!");
            startStage(stage + 1);
            count = 0;
        }

        gravity();
        moveBullet();
        playerPrint();
        Sleep(1);
    }

    if(isGameOver)
        banner(isWin ? "YOU WIN!" : "GAME OVER  -  Score: " + to_string(score));

    // reset run state so a fresh "New Game" starts clean
    isGameOver = false; isWin = false;
    health = 100; lives = 3; score = 0; coins = 100;
    enemiesKilled = 0; currentRoomKills = 0;
    mainScreen();
}
// One reusable banner in the middle of the screen, then a short pause.
void banner(const string &msg)
{
    int x = (VIEW_W - (int)msg.size()) / 2;
    gotoxy(x - 2, 19); cout << "  " << string(msg.size(), ' ') << "  ";
    gotoxy(x, 20);     cout << msg;
    Sleep(1200);
    gotoxy(x, 20);     cout << string(msg.size(), ' ');
}
// Clear every enemy and bullet (used between sections and stages).
void resetActors()
{
    for(int i = 0; i < maxEnemies; i++) isEnemyAlive[i] = false;
    enemyCount = 0; activeEnemies = 0;
    for(int i = 0; i < maxBullets; i++) isBulletActive[i] = false;
    bulletCount = 0;
}
// Load a stage's field, draw it and drop the player in at the start.
void startStage(int s)
{
    stage = s;
    section = 0;
    currentLevel = stage;
    currentRoom = 1;
    currentRoomKills = 0;
    stageKillTarget = 4 + stage;          // rising difficulty, endlessly
    bulletsLeft = 100 + stage * 20;       // a fresh reload each stage
    resetActors();
    loadStage(stage);
    renderField();
    pX = 50; pY = GROUND_TOP + 1;
    playerState = "idle"; playerDir = "right";
    playerPrint();
    updateBoard();
}
// Flip the visible window to a neighbouring section of the wide map.
bool changeSection(int delta)
{
    int ns = section + delta;
    if(ns < 0 || ns >= sectionCount)
        return false;
    section = ns;
    currentRoom = section + 1;
    resetActors();                         // enemies/bullets belong to the old view
    renderField();
    pX = (delta > 0) ? 6 : 100;            // step in from the edge we arrived at
    pY = GROUND_TOP + 1;
    playerState = "idle";
    playerPrint();
    updateBoard();
    return true;
}
void playerJump()
{
    int yDestination = pY - 15;
    playerState = "jump"; pY--;
    removePlayer();
    Sleep(90);
    while(shouldJump() && pY != yDestination ){
        pY--;
    }
    playerPrint();
    Sleep(90);
    removePlayer();
    playerState = "idle"; 
    gravity();
}
bool shouldJump(){
    for(int i = 0 ; i <= 14 ; i++){
        if(getCharAtxy(pX + i, pY - 1) != ' ')
            return false;
    }
    return true;
}
void gravity()
{
    if(getCharAtxy(pX, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 1, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 2, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 3, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 4, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 5, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 6, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 7, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 8, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 9, pY + 7) != ' ')
        return;
    if(getCharAtxy(pX + 10, pY + 7) != ' ')
        return;
    
    removePlayer();
    pY++;
}
void playerAttack()
{
    removePlayer();
    playerState = "attack";
    if((pX > 94) && (playerDir == "right"))
        pX = 93;
    else if((pX < 14) && (playerDir == "left"))
        pX = 14;
    Sleep(10);
    playerPrint();
    meleeAttack();          // resolve the swing while the sword pose is on screen
    Sleep(150);
    removePlayer();
    playerState = "idle";
    playerPrint();
}
void meleeAttack()
{
    int px = pX + 6;        // player centre column
    for(int i = 0; i < enemyCount; i++)
    {
        if(!isEnemyAlive[i])
            continue;
        // vertical overlap between the enemy's body band and the player's
        if(enemyY[i] + 7 < pY || enemyY[i] > pY + 6)
            continue;
        int cx = enemyX[i] + 4;          // enemy centre column
        bool inReach = (playerDir == "right") ? (cx > px && cx - px <= 22)
                                              : (cx < px && px - cx <= 22);
        if(inReach)
            destroyEnemy(i);             // the blade reaches around a shield up close
    }
}
void playerRun()
{
    removePlayer();
    playerState = "run";
    if(playerDir == "left")
        pX--;
    else
        pX++;
    playerPrint();
    playerState = "idle";
}
void playerPrint()
{
    /* printing player according to there direction and states */
    if(playerDir == "right")
    {
        if(playerState == "idle")
        {
            gotoxy(pX,pY);  cout << " .    _____";
            gotoxy(pX,pY+1);cout << ". .`./__   \\";
            gotoxy(pX,pY+2);cout << "     |/ " <<  (char)248 << "U" <<  (char)248 << "|";
            gotoxy(pX,pY+3);cout << "     (__E3 )";
            gotoxy(pX,pY+4);cout << "   7 /    <";
            gotoxy(pX,pY+5);cout << "  / (__/ __)";
            gotoxy(pX,pY+6);cout << "     U    U";
        }
        else if(playerState == "jump")
        {
            gotoxy(pX,pY);  cout << "      _____  /";
            gotoxy(pX,pY+1);cout << ". . ./__   \\/";
            gotoxy(pX,pY+2);cout << "  _/ |/ "<< (char)248<<"U"<< (char)248<<"|";
            gotoxy(pX,pY+3);cout << "     /  y " << (char)240 << (char)240 << ")";
            gotoxy(pX,pY+4);cout << "    (/\\    |";
            gotoxy(pX,pY+5);cout << "      /  ^\\_)";
            gotoxy(pX,pY+6);cout << "    /(_/  ";
            gotoxy(pX,pY+7);cout << "   /  " ;
        }
        else if(playerState == "run")
        {
            gotoxy(pX,pY);  cout << " .    _____  ";
            gotoxy(pX,pY+1);cout << ". .`./__   \\";
            gotoxy(pX,pY+2);cout << "     |/ " <<  (char)248 << "U" <<  (char)248 << "|";
            gotoxy(pX,pY+3);cout << "     (__33 )";
            gotoxy(pX,pY+4);cout << "     /    <";
            gotoxy(pX,pY+5);cout << "    (__/ __)";
            gotoxy(pX,pY+6);cout << "     U   U";
        }
        else if(playerState == "attack")
        {
            gotoxy(pX,pY);  cout << "  .    _____  ";
            gotoxy(pX,pY+1);cout << " . .`./__   \\";
            gotoxy(pX,pY+2);cout << "      |/ " <<  (char)248 << "U" <<  (char)248 << "|";
            gotoxy(pX,pY+3);cout << "     (" << (char)240 << (char)240 << " " << (char)153 << "  " << (char)239 << " " << (char)196 << (char)180 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 ;
            gotoxy(pX,pY+4);cout << "   7 /    <  ";
            gotoxy(pX,pY+5);cout << "  / (__/ __) ";
            gotoxy(pX,pY+6);cout << "     U    U  "; 
        }
    }
    else if(playerDir == "left")
    {
        if(playerState == "idle")
        {
            gotoxy(pX,pY);  cout << " _____     .";
            gotoxy(pX,pY+1);cout << "/   __\\ .'. .";
            gotoxy(pX,pY+2);cout << "|"<<  (char)248 <<"U"<< (char)248 << " \\|";
            gotoxy(pX,pY+3);cout << "( E3__)  ";
            gotoxy(pX,pY+4);cout << " >    \\ R ";
            gotoxy(pX,pY+5);cout << "(__ \\__) \\";
            gotoxy(pX,pY+6);cout << " U    U";
        }
        else if(playerState == "jump")
        {
            gotoxy(pX,pY);  cout << "\\  _____    ";
            gotoxy(pX,pY+1);cout << " \\/   __\\. . .";
            gotoxy(pX,pY+2);cout << "  |"<< (char)248 <<"U"<< (char)248 << " \\| \\_ ";
            gotoxy(pX,pY+3);cout << " (" << (char)240 << (char)240 << " y  \\";
            gotoxy(pX,pY+4);cout << "  |    /\\)";
            gotoxy(pX,pY+5);cout << " (_/^  \\";
            gotoxy(pX,pY+6);cout << "      \\_)\\";
            gotoxy(pX,pY+7);cout << "          \\" ;
        }
        else if(playerState == "run")
        {
            gotoxy(pX,pY);  cout << " _____     .";
            gotoxy(pX,pY+1);cout << "/   __\\ .'. . ";
            gotoxy(pX,pY+2);cout << "|"<<  (char)248 <<"U"<< (char)248 << " \\|";
            gotoxy(pX,pY+3);cout << "( EE__)";
            gotoxy(pX,pY+4);cout << " >    \\";
            gotoxy(pX,pY+5);cout << "(__ \\__)";
            gotoxy(pX,pY+6);cout << "  U   U";
        }
        else if(playerState == "attack")
        {
            gotoxy(pX-1,pY);  cout << " _____    .";
            gotoxy(pX-1,pY+1);cout << "/   __\\.'. .";
            gotoxy(pX-1,pY+2);cout << "|"<<  (char)248 <<"U"<< (char)248 << " \\|  ";
            gotoxy(pX-13,pY+3);cout << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)195 << (char)196 << " " << (char)239 << "  " << (char)153 << " " << (char)240 << (char)240 << ")";
            gotoxy(pX,pY+4);cout << " >    \\ R";
            gotoxy(pX,pY+5);cout << "(__ \\__) \\";
            gotoxy(pX,pY+6);cout << " U     U  "; 
        }
    }
}
void removePlayer()
{
    gotoxy(pX,pY);cout << "              ";
    if(playerState == "attack"){
        if(playerDir == "left")
            gotoxy(pX-1,pY);
        cout << " ";
    }
    gotoxy(pX,pY+1);cout << "              ";
    if(playerState == "attack"){
        if(playerDir == "left")
            gotoxy(pX-1,pY+1);
        cout << " ";
    }
    gotoxy(pX,pY+2);cout << "             ";
    if(playerState == "attack"){
        if(playerDir == "left")
            gotoxy(pX-1,pY+2);
        cout << " ";
    }
    gotoxy(pX,pY+3);cout << "             ";
    if(playerState == "attack"){
        if(playerDir == "left"){
            gotoxy(pX-13,pY+3);
            cout << "              ";
        }else{
            cout << "            ";
        }
    }
    gotoxy(pX,pY+4);cout << "             ";
    gotoxy(pX,pY+5);cout << "             ";
    gotoxy(pX,pY+6);cout << "           ";
    if(playerState == "jump"){
        cout << "   ";     
        gotoxy(pX,pY+7);cout << "             ";
    }
}
int freeEnemySlot()
{
    for(int i = 0; i < maxEnemies; i++)
        if(!isEnemyAlive[i])
            return i;
    return -1;
}
void generateEnemy()
{
    int e = freeEnemySlot();
    if(e == -1)
        return;                         // all slots busy this frame

    int x;                              // spawn on the ground, away from the player
    do { x = 12 + rand() % 90; } while(x > pX - 18 && x < pX + 18);

    int dir = rand() % 2;
    enemyX[e] = x;
    enemyY[e] = GROUND_TOP;             // stands on the floor (enemyY + 8 == ground row)
    enemyDir[e] = dir;
    enemyType[e] = rand() % 3;
    enemyState[e] = dir;
    isEnemyAlive[e] = true;
    printEnemy(enemyX[e], enemyY[e], enemyDir[e], enemyType[e], enemyState[e]);
    if(e >= enemyCount)
        enemyCount = e + 1;
    activeEnemies++;
}
void printEnemy(int x, int y, int dir, int type, int state)
{
    if(dir)
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" << (char)248 << " " << (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__88 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
        else if(type == 1)
        {
            if(state == 0)
            {
                gotoxy(x, y);   cout << "   ____";
                gotoxy(x, y+1); cout << "  /____\\";
                gotoxy(x, y+2); cout << " `|/" << (char)248 << " " << (char)248 << "|'";
                gotoxy(x, y+3); cout << "  \\_ -_/";
                gotoxy(x - 4, y+4); cout << " (" << (char)240 << (char)240 << " " << (char)153 << " " << (char)239 << " " << (char)196 << (char)180 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 ;
                gotoxy(x, y+5); cout << " /    <"; 
                gotoxy(x, y+6); cout << "(__/ __)";
                gotoxy(x, y+7); cout << " U    U";    
            }
            else
            {
                gotoxy(x, y);   cout << "   ____";
                gotoxy(x, y+1); cout << "  /____\\";
                gotoxy(x, y+2); cout << " `|/" << (char)248 << " " << (char)248 << "|'";
                gotoxy(x, y+3); cout << "  \\_ -_/";
                gotoxy(x, y+4); cout << " (" << (char)240 << (char)240 << " " << (char)153 << " " << (char)239 << " " << (char)196 << (char)180 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 ;
                gotoxy(x, y+5); cout << " /    <"; 
                gotoxy(x, y+6); cout << "(__/ __)";
                gotoxy(x, y+7); cout << " U    U";
            }
        }
        else if(type == 2)
        {
            if(state == 0)
            {            
                gotoxy(x, y);   cout << "   ____";
                gotoxy(x, y+1); cout << "  /____\\   |";
                gotoxy(x, y+2); cout << " `|/" << (char)248 << " " << (char)248 << "|'  |";
                gotoxy(x, y+3); cout << "  \\_ -_/   |";
                gotoxy(x-4, y+4); cout << " (" << (char)240 << (char)240 << " " << (char)153 << " " << (char)239 << " " << (char)196 << (char)180 << (char)196 << (char)196 << (char)196<< (char)196 << "|" << (char)196 << (char)196;
                gotoxy(x, y+5); cout << " /    <    |"; 
                gotoxy(x, y+6); cout << "(__/ __)   |";
                gotoxy(x, y+7); cout << " U    U    |";
            }
            else
            {            
                gotoxy(x, y);   cout << "   ____";
                gotoxy(x, y+1); cout << "  /____\\   |";
                gotoxy(x, y+2); cout << " `|/" << (char)248 << " " << (char)248 << "|'  |";
                gotoxy(x, y+3); cout << "  \\_ -_/   |";
                gotoxy(x, y+4); cout << " (" << (char)240 << (char)240 << " " << (char)153 << "  " << (char)239 << " " << (char)196 << (char)180 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 ;
                gotoxy(x, y+5); cout << " /    <    |"; 
                gotoxy(x, y+6); cout << "(__/ __)   |";
                gotoxy(x, y+7); cout << " U    U    |";
            }
        }
    }
    else
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "  ____";
            gotoxy(x, y+1); cout << " /____\\";
            gotoxy(x, y+2); cout << "`|" <<  (char)248 << " " <<  (char)248 << "\\|'";
            gotoxy(x, y+3); cout << " \\_- _/";
            gotoxy(x, y+4); cout << " ( 88__)";
            gotoxy(x, y+5); cout << "  >    \\";
            gotoxy(x, y+6); cout << " (__ \\__)";
            gotoxy(x, y+7); cout << "  U    U";
        }
        else if(type == 1)
        {
            if(state == 0)
            {
                gotoxy(x, y);   cout << "  ____";
                gotoxy(x, y+1); cout << " /____\\";
                gotoxy(x, y+2); cout << "`|" <<  (char)248 << " " <<  (char)248 << "\\|'";
                gotoxy(x, y+3); cout << " \\_- _/";
                gotoxy(x-7,y+4);cout << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)195 << (char)196 << " " << (char)239 << " " << (char)153 << " " << (char)240 << (char)240 << ")";
                gotoxy(x, y+5); cout << "  >    \\";
                gotoxy(x, y+6); cout << " (__ \\__)";
                gotoxy(x, y+7); cout << "  U    U";
            }
            else
            {
                gotoxy(x, y);   cout << "  ____";
                gotoxy(x, y+1); cout << " /____\\";
                gotoxy(x, y+2); cout << "`|" <<  (char)248 << " " <<  (char)248 << "\\|'";
                gotoxy(x, y+3); cout << " \\_- _/";
                gotoxy(x-10,y+4);cout << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)195 << (char)196 << " " << (char)239 << " " << (char)153 << " " << (char)240 << (char)240 << ")";
                gotoxy(x, y+5); cout << "  >    \\";
                gotoxy(x, y+6); cout << " (__ \\__)";
                gotoxy(x, y+7); cout << "  U    U";
            }
        }
        else if(type == 2)
        {
            if(state == 0)
            {
                gotoxy(x, y);   cout << "     ____";
                gotoxy(x, y+1); cout << "|   /____\\";
                gotoxy(x, y+2); cout << "|  `|" <<  (char)248 << " " <<  (char)248 << "\\|'";
                gotoxy(x, y+3); cout << "|   \\_- _/";
                gotoxy(x-2,y+4);cout << (char)196 << (char)196 << "|" << (char)196 << (char)196 << (char)196<< (char)196 << (char)195 << (char)196 << " " << (char)239 << " " << (char)153 << " " << (char)240 << (char)240 << ")";
                gotoxy(x, y+5); cout << "|    >    \\";
                gotoxy(x, y+6); cout << "|   (__ \\__)";
                gotoxy(x, y+7); cout << "|    U    U"; 
            }
            else
            {
                gotoxy(x, y);   cout << "     ____";
                gotoxy(x, y+1); cout << "|   /____\\";
                gotoxy(x, y+2); cout << "|  `|" <<  (char)248 << " " <<  (char)248 << "\\|'";
                gotoxy(x, y+3); cout << "|   \\_- _/";
                gotoxy(x-9,y+4);cout << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)195 << (char)196 << " " << (char)239 << "  " << (char)153 << " " << (char)240 << (char)240 << ")";
                gotoxy(x, y+5); cout << "|    >    \\";
                gotoxy(x, y+6); cout << "|   (__ \\__)";
                gotoxy(x, y+7); cout << "|    U    U"; 
            }
        }
    }
}
void removeEnemy(int x, int y, int dir, int type, int state)
{
    if(dir)
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "       ";
            gotoxy(x, y+1); cout << "        ";
            gotoxy(x, y+2); cout << "         ";
            gotoxy(x, y+3); cout << "        ";
            gotoxy(x, y+4); cout << "        ";
            gotoxy(x, y+5); cout << "       ";
            gotoxy(x, y+6); cout << "        ";
            gotoxy(x, y+7); cout << "       ";
        }
        else if(type == 1)
        {
            gotoxy(x, y);   cout << "       ";
            gotoxy(x, y+1); cout << "        ";
            gotoxy(x, y+2); cout << "         ";
            gotoxy(x, y+3); cout << "        ";
            if(state == 0)
            {
                gotoxy(x-4, y+4); cout << "                    " ;
            }
            else
            {
                gotoxy(x, y+4); cout << "                     " ;
            }
            gotoxy(x, y+5); cout << "       "; 
            gotoxy(x, y+6); cout << "        ";
            gotoxy(x, y+7); cout << "       ";  
            
        }
        else if(type == 2)
        {
            gotoxy(x, y);   cout << "       ";
            gotoxy(x, y+1); cout << "            ";
            gotoxy(x, y+2); cout << "            ";
            gotoxy(x, y+3); cout << "            ";
            if(state == 0)
            {
                gotoxy(x-4, y+4); cout << "                   " ;
            }
            else
            {
                gotoxy(x, y+4); cout << "                     " ;
            }
            gotoxy(x, y+5); cout << "            "; 
            gotoxy(x, y+6); cout << "            ";
            gotoxy(x, y+7); cout << "            ";
        }
    }
    else
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "      ";
            gotoxy(x, y+1); cout << "       ";
            gotoxy(x, y+2); cout << "        ";
            gotoxy(x, y+3); cout << "       ";
            gotoxy(x, y+4); cout << "        ";
            gotoxy(x, y+5); cout << "        ";
            gotoxy(x, y+6); cout << "         ";
            gotoxy(x, y+7); cout << "        ";
        }
        else if(type == 1)
        {
            gotoxy(x, y);   cout << "      ";
            gotoxy(x, y+1); cout << "       ";
            gotoxy(x, y+2); cout << "        ";
            gotoxy(x, y+3); cout << "       ";
            if(state == 0)
            {
                gotoxy(x-7, y+4); cout << "                     ";
            }
            else
            {
                gotoxy(x-10, y+4); cout << "                     ";
            }
            gotoxy(x, y+5); cout << "        ";
            gotoxy(x, y+6); cout << "         ";
            gotoxy(x, y+7); cout << "        ";
            
        }
        else if(type == 2)
        {
            gotoxy(x, y);   cout << "         ";
            gotoxy(x, y+1); cout << "          ";
            gotoxy(x, y+2); cout << "           ";
            gotoxy(x, y+3); cout << "          ";
            if(state == 0)
            {
                gotoxy(x-2, y+4); cout << "                 ";
            }
            else
            {
                gotoxy(x-9, y+4); cout << "                    ";
            }
            gotoxy(x, y+5); cout << "           ";
            gotoxy(x, y+6); cout << "            ";
            gotoxy(x, y+7); cout << "           ";
        }
    }
}
void enemies(int action)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(isEnemyAlive[i])
        {
            if(action == 0)
            {
                removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                enemyState[i] = !enemyState[i];
                printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
            }
            else if(action == 1)
            {
                char next;
                if(enemyType[i] == 0)
                {
                    if(enemyDir[i])
                        next = getCharAtxy(enemyX[i] + 10, enemyY[i] + 4);
                    else
                        next = getCharAtxy(enemyX[i] - 3, enemyY[i] + 4);
                }
                else if(enemyType[i] == 1)
                {
                    if(enemyDir[i])
                    {
                        if(enemyState[i])
                            next = getCharAtxy(enemyX[i] + 25, enemyY[i] + 4);
                        else
                            next = getCharAtxy(enemyX[i] + 18, enemyY[i] + 4);
                    }
                    else
                    {
                        if(enemyState[i])
                            next = getCharAtxy(enemyX[i] - 11, enemyY[i] + 4);
                        else
                            next = getCharAtxy(enemyX[i] - 8, enemyY[i] + 4);

                        cout << getCharAtxy(enemyX[i] - 15, enemyY[i] + 4);
                    }
                }
                else if(enemyType[i] == 2)
                {
                    if(enemyDir[i])
                    {
                        if(enemyState[i])
                            next = getCharAtxy(enemyX[i] + 22, enemyY[i] + 4);
                        else
                            next = getCharAtxy(enemyX[i] + 18, enemyY[i] + 4);
                    }
                    else
                    {
                        if(enemyState[i])
                            next = getCharAtxy(enemyX[i] - 15, enemyY[i] + 4);
                        else
                            next = getCharAtxy(enemyX[i] - 6, enemyY[i] + 4);
                    }
                }

                if(next != ' ')
                {
                    removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                    enemyDir[i] = !enemyDir[i];
                    printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                }
                else
                {
                    removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);

                    // turn at a ledge, or at the edge of the visible section so an
                    // enemy never wanders off-screen into invalid columns
                    if((getCharAtxy(enemyX[i] , enemyY[i] + 8) != '=' || getCharAtxy(enemyX[i]+8 , enemyY[i] + 8) != '=') || enemyX[i] < 10 || enemyX[i] > 100 ){
                        enemyDir[i] = !enemyDir[i];
                    }
                    if(enemyDir[i])
                        enemyX[i] += 1;
                    else
                        enemyX[i] -= 1;
                    printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                }
            }
            else if(action == 3)
            {
                if(!enemyType[i])
                {
                    if(pY - 1 == enemyY[i])
                    {
                        if(pX < enemyX[i]){
                            removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                            enemyDir[i] = 0;
                            printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                        }
                        else
                        {
                            removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                            enemyDir[i] = 1;
                            printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                        }
                        generateBullet("enemy", enemyDir[i], enemyX[i], enemyY[i]);
                    }
                }
            }
            
        }
    }
}
void killEnemy(int i)
{
    isEnemyAlive[i] = false;
}
// Returns the index of the first ALIVE enemy whose body box covers (x,y), else -1.
// A generous box makes shots land reliably instead of only on a couple of glyphs.
int enemyHitIndex(int x, int y)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(!isEnemyAlive[i])
            continue;
        if(x >= enemyX[i] - 1 && x <= enemyX[i] + 12 &&
           y >= enemyY[i]     && y <= enemyY[i] + 7)
            return i;
    }
    return -1;
}
bool hitsPlayer(int x, int y)
{
    return (x >= pX - 1 && x <= pX + 12 && y >= pY && y <= pY + 6);
}
// One place that kills an enemy and keeps every counter/score/board in sync.
void destroyEnemy(int i)
{
    killEnemy(i);
    removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
    if(activeEnemies > 0) activeEnemies--;
    enemiesKilled++;
    currentRoomKills++;
    score += 10;
    coins += 5;
    updateBoard();
}
int freeBulletSlot()
{
    for(int i = 0; i < maxBullets; i++)
        if(!isBulletActive[i])
            return i;
    return -1;
}
void generateBullet(string type, int dir, int x, int y)
{
    int b = freeBulletSlot();
    if(b == -1)
        return;                  // pool full this frame; drop the shot
    if(type == "player")
    {
        bulletX[b] = dir ? x + 12 : x - 1;
        bulletY[b] = y + 2;
        bulletOwner[b] = 1;
        if(bulletsLeft > 0) bulletsLeft--;
        updateBoard();
    }
    else
    {
        bulletX[b] = dir ? x + 9 : x - 1;
        bulletY[b] = y + 4;
        bulletOwner[b] = 0;
    }
    isBulletActive[b] = true;
    bulletDir[b] = dir;
    if(b >= bulletCount) bulletCount = b + 1;
    printBullet(bulletX[b], bulletY[b]);
}
void printBullet(int x, int y)
{
  gotoxy(x, y);
  cout << "x";
}
void eraseBullet(int x, int y)
{
  gotoxy(x, y);
  cout << " ";
}
void makeBulletInactive(int idx)
{
  isBulletActive[idx] = false;
}
void moveBullet()
{
    for (int j = 0; j < bulletCount; j++)
    {
        if (!isBulletActive[j])
            continue;

        int step = bulletDir[j] ? 1 : -1;
        int nx = bulletX[j] + step;     // cell the bullet is about to enter
        int ny = bulletY[j];

        if(bulletOwner[j])              // ---- player bullet ----
        {
            int hit = enemyHitIndex(nx, ny);
            if(hit != -1)
            {
                eraseBullet(bulletX[j], bulletY[j]);
                makeBulletInactive(j);
                // Type-2 enemies hold a shield on the side they face (their front).
                // A shot arriving from the front (bullet travelling opposite to the
                // way the enemy faces) is deflected; any other hit kills.
                bool shielded = (enemyType[hit] == 2 && bulletDir[j] != enemyDir[hit]);
                if(shielded)
                    printEnemy(enemyX[hit], enemyY[hit], enemyDir[hit], enemyType[hit], enemyState[hit]);
                else
                    destroyEnemy(hit);
                continue;
            }
            if(getCharAtxy(nx, ny) != ' ')   // wall / platform
            {
                eraseBullet(bulletX[j], bulletY[j]);
                makeBulletInactive(j);
                continue;
            }
        }
        else                           // ---- enemy bullet ----
        {
            if(hitsPlayer(nx, ny))
            {
                eraseBullet(bulletX[j], bulletY[j]);
                makeBulletInactive(j);
                if(health > 0) health -= 10;
                updateBoard();
                playerPrint();          // repair the part of the sprite the bullet sat on
                continue;
            }
            if(getCharAtxy(nx, ny) != ' ')
            {
                eraseBullet(bulletX[j], bulletY[j]);
                makeBulletInactive(j);
                continue;
            }
        }

        // clear path: advance one cell
        eraseBullet(bulletX[j], bulletY[j]);
        bulletX[j] = nx;
        printBullet(bulletX[j], bulletY[j]);
    }
}
// Draw the header plus the VIEW_W-wide slice of the field for the current section.
void renderField()
{
    gotoxy(0,0); cout << "------------------------------------------------------------------------------------------------------------------------";
    gotoxy(0,1); cout << "| Level: 6 | Room: 7 | Kills: 8  | Bullets: 6    | Health : !          | Score:  6    | Lives : 8      | Coins:  5     |";
    gotoxy(0,2); cout << "------------------------------------------------------------------------------------------------------------------------";

    int start = section * VIEW_W;
    for(int r = 0; r < FIELD_ROWS; r++)
    {
        string slice;
        if(start < (int)fieldRows[r].size())
            slice = fieldRows[r].substr(start, VIEW_W);
        if((int)slice.size() < VIEW_W)
            slice += string(VIEW_W - slice.size(), ' ');
        gotoxy(0, r + 3);
        cout << slice;
    }
    updateBoard();
}
// Procedurally build a stage's wide field when no .txt layout exists yet.
// Even-numbered stages are two screens wide so the side-scroll is exercised.
void buildDefaultField(int s)
{
    int w = (s % 2 == 0) ? VIEW_W * 2 : VIEW_W;
    for(int r = 0; r <= 32; r++)             // open play rows with edge walls
    {
        string row(w, ' ');
        row[0] = '.'; row[w-1] = '.';
        fieldRows[r] = row;
    }
    fieldRows[33] = string(w, '=');          // ground   (console row 36)
    fieldRows[34] = string(w, '#');          // dirt
    fieldRows[35] = string(w, '#');          // dirt
    fieldRows[36] = string(w, '-');          // bottom border

    // a couple of floating platforms, nudged by the stage number for variety
    int shift = (s * 7) % 24;
    for(int x = 4 + shift; x < 30 + shift && x < w-1; x++)   fieldRows[11][x] = '=';
    for(int x = 72; x < w-2; x++)                            fieldRows[11][x] = '=';
    for(int x = 4; x < 40; x++)                              fieldRows[23][x] = '=';
    for(int x = 78 + shift/2; x < w-2; x++)                  fieldRows[23][x] = '=';
}
// Persist the current field so it becomes a real, editable stage file.
void saveField(int idx)
{
    CreateDirectoryA("stages", NULL);
    char path[64];
    sprintf(path, "stages\\stage%d.txt", idx);
    ofstream o(path);
    if(!o) return;
    for(int r = 0; r < FIELD_ROWS; r++)
        o << fieldRows[r] << "\n";
}
// Load stage s from stages\stageN.txt (cycling through STAGE_FILES layouts).
// If the file is missing it is generated and saved, so stages live on disk.
void loadStage(int s)
{
    int idx = ((s - 1) % STAGE_FILES) + 1;
    char path[64];
    sprintf(path, "stages\\stage%d.txt", idx);

    ifstream f(path);
    if(f)
    {
        for(int r = 0; r < FIELD_ROWS; r++)
        {
            if(!getline(f, fieldRows[r])) fieldRows[r] = "";
            if(!fieldRows[r].empty() && fieldRows[r].back() == '\r')
                fieldRows[r].pop_back();
        }
    }
    else
    {
        buildDefaultField(s);
        saveField(idx);
    }

    // normalise to a whole number of VIEW_W-wide sections
    size_t w = VIEW_W;
    for(int r = 0; r < FIELD_ROWS; r++) w = max(w, fieldRows[r].size());
    if(w % VIEW_W) w += VIEW_W - (w % VIEW_W);
    fieldWidth = (int)w;
    sectionCount = fieldWidth / VIEW_W;

    for(int r = 0; r < FIELD_ROWS; r++)      // pad short rows, keeping floors solid
    {
        char fill = ' ';
        if(r == 33) fill = '=';
        else if(r == 34 || r == 35) fill = '#';
        else if(r == 36) fill = '-';
        if((int)fieldRows[r].size() < fieldWidth)
            fieldRows[r] += string(fieldWidth - fieldRows[r].size(), fill);
    }
}
void aboutDeveloperScreen(int x, int y)
{
    system("cls");
    gameHeaderCharacter(5,5);
    gameHeader(22,3);
    gameHeaderCharacter(70,5);
    gotoxy(x,y);cout << "------ About ------";
    gotoxy(x,++y);cout << " ";
    gotoxy(x,++y);cout << "Game Name           : \t ARASHI";
    gotoxy(x,++y);cout << "Version             : \t 1.0";
    gotoxy(x,++y);cout << "Developer Name      : \t Hafiz Muhammad Moaz";
    gotoxy(x,++y);cout << "Registration Number : \t 2024-CS-23";
    gotoxy(x,++y);cout << "Developer Contact   : \t hafizmoazkhalid@gmail.com";
    gotoxy(x,++y);cout << " ";
    gotoxy(x,++y);cout << " ";
    gotoxy(x,++y);cout << "------ Game Description ------";
    gotoxy(x,++y);cout << " ";
    gotoxy(x,++y);cout << "\tThis ia a story-based Ninja Fighting Game.";
    gotoxy(x,++y);cout << "\tIn this game, you play as Arashi, a former legendary ninja who fights his"; 
    gotoxy(x,++y);cout << "way through the corrupted world to save his kidnapped son from the hand of the";
    gotoxy(x,++y);cout << "shadow devil Orochi. With superior acrobatic and deadly weapons, Arashi is ready";
    gotoxy(x,++y);cout << "to face menacing traps and enemies who have sworn to protect the shadow devil "; 
    gotoxy(x,++y);cout << "Orochi. Ninja Arashi features simple yet addicting gameplay, giving you thrilling";
    gotoxy(x,++y);cout << "moments and an unexpected experience. You can collect gold and  diamond from";
    gotoxy(x,++y);cout << "enemies and the environment in order to keep tracks with the difficulty of the";
    gotoxy(x,++y);cout << "game. Moreover, through traps, lay waste on enemies who try to stop you and rescue"; 
    gotoxy(x,++y);cout << "your son.";

    gotoxy(80,16);cout <<"---- Instructions ----";
    gotoxy(80,18);cout << "Press [SPACE] or [ESC] To Exit";
    while(true){
        if(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_ESCAPE)){
            mainScreen();
        }
        Sleep(200);
    }

}
void gameHeader(int x, int y)
{
    gotoxy(x,y);  cout << "---------------------------------------------" ;
    gotoxy(x,++y);cout << "                                             ";
    gotoxy(x,++y);cout << "                                 .__     .__.";
    gotoxy(x,++y);cout << " _____   .__  ___ _____    ______|  |__  |__|";
    gotoxy(x,++y);cout << " \\__  \\  |  |/___|\\__  \\  /  ___/|  |  \\ |  |";
    gotoxy(x,++y);cout << "  / __ \\_|   /     / __ \\_\\___ \\ |   Y  \\|  |";
    gotoxy(x,++y);cout << " (____  /|  |     (____  /____  >|___|  /|__|";
    gotoxy(x,++y);cout << "      \\/ |__|          \\/     \\/      \\/     ";
    gotoxy(x,++y);cout << "                                             ";
    gotoxy(x,++y);cout << "---------------------------------------------";
}
void gameHeaderCharacter(int x, int y)
{
    gotoxy(x,y);cout << " .    _____  ";
    gotoxy(x,++y);cout << ". .`./__   \\";
    gotoxy(x,++y);cout << "     |/ " <<  (char)248 << "U" <<  (char)248 << "|";
    gotoxy(x,++y);cout << "     (__E3 ) ";
    gotoxy(x,++y);cout << "   7 /    <  ";
    gotoxy(x,++y);cout << "  / (__/ __) ";
    gotoxy(x,++y);cout << "     U    U  ";
}
void updateBoard(){
    gotoxy(9, 1);  cout << "  ";
    gotoxy(19, 1); cout << "  ";
    gotoxy(30, 1); cout << "  ";
    gotoxy(44, 1); cout << "    ";
    gotoxy(60, 1); cout << "          ";
    gotoxy(80, 1); cout << "     ";
    gotoxy(96, 1); cout << "       ";
    gotoxy(112, 1); cout << "       ";

    gotoxy(9, 1); cout << currentLevel;
    gotoxy(19, 1); cout << currentRoom;
    gotoxy(30, 1); cout << currentRoomKills;
    gotoxy(44, 1); cout << bulletsLeft;
    gotoxy(60, 1);
    int tempPerc = (health/100.0) * 100;
    for(int i=0; i < tempPerc/10;i++){
        cout << "#";
    }
    gotoxy(80, 1); cout << score;

    gotoxy(96, 1); 
    for(int i = 0; i < lives; i++)
        cout << "\3";

    gotoxy(112, 1); cout << coins;
    
}
char getCharAtxy(short int x, short int y)
{
  CHAR_INFO ci;
  COORD xy = {0, 0};
  SMALL_RECT rect = {x, y, x, y};
  COORD coordBufSize;
  coordBufSize.X = 1;
  coordBufSize.Y = 1;
  return ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ci, coordBufSize, xy, &rect) ? ci.Char.AsciiChar : ' ';
}
void gotoxy(int x, int y)
{
    COORD coords;
    coords.X = x; coords.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coords);
}
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void screenSetup(int bufferX,int bufferY, int width, int height, bool cursor)
{
    COORD bufferSize;
    bufferSize.X = bufferX;
    bufferSize.Y = bufferY;

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1;
    windowSize.Bottom = height - 1;
    
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);

    RECT rect;
    GetWindowRect(GetConsoleWindow(), &rect);
    int winWidth = rect.right - rect.left;
    int winHeight = rect.bottom - rect.top;

    SetWindowPos(GetConsoleWindow(), 0, 250, 100, winWidth, winHeight, SWP_NOZORDER | SWP_NOACTIVATE);

    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = cursor;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}