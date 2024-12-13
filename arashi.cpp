#include<iostream>
#include<windows.h>
#include<conio.h>
#include <mmsystem.h>
using namespace std;
#pragma comment(lib, "winmm.lib")

/* setups */
void gotoxy(int,int);
char getCharAtxy(short int, short int);
void screenSetup(int,int,int,int,bool);
void setColor(int);

/* screens */
void mainMenu(int,int,int);
void mainScreen();
void gameRoomCP();
void aboutDeveloperScreen(int,int);

/* game screen functionalities */
void gameRoomCPGrid();
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

//no. of rooms , num of enemies each room, num of bullets, goalLocation ,enemy locations a(x,y) , b(x,y), c(x,y), d(x,y)
int levels[4][13] = {
   //NR, NE, NB, GX,GY,  AX AY  BX  BY  CX cY dx dy
    {4,  5,  100, 4, 90,  20, 28, 80, 28, 20, 18, 18, 6 },
    {4,  5,  100, 4, 90,  80, 6,  85, 18, 18, 18, 85, 28 },
    {6,  8,  150, 4, 90,  18, 18, 89, 18, 20, 6,  18, 28 },
    {8,  12, 200,4, 90,  89, 28, 29, 28, 18, 18, 85, 18 }
};

/* enemy functions */
const int maxEnemies = 12;
int enemyX[maxEnemies], enemyY[maxEnemies], enemyType[maxEnemies], enemyDir[maxEnemies], enemyState[maxEnemies], enemyCount = 0, activeEnemies = 0;
bool isEnemyAlive[maxEnemies];
void generateEnemy();
void printEnemy(int,int,int,int,int);
void removeEnemy(int,int,int,int,int);
void enemies(int);
void killEnemy(int);

int bulletX[200], bulletY[200], bulletCount = 0, bulletDir[200], bulletOwner[200];
bool isBulletActive[200];

void generateBullet(string, int, int, int);
void eraseBullet(int x, int y);
void printBullet(int x, int y);
void moveBullet();
void makeBulletInactive(int idx);



string activeScreen, gameState, playerState = "idle", playerDir="left"; // player states

int shopProducts[] = {};

int coins = 100, currentLevel = 1, currentRoom = 0, enemiesKilled = 0, health = 100, score = 0, bulletsLeft = 100, lives = 3, currentRoomKills = 0;
bool isGameOver = false, isWin = false;

main()
{
   // PlaySound(TEXT("G:\\University\\Projects\\Game-V-1.0\\audios\\bg.waw"), NULL, SND_ASYNC | SND_LOOP);
    screenSetup(105,40,120,40, FALSE);
    mainScreen();
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
    //PlaySound(TEXT("G:\\University\\Projects\\Game-V-1.0\\audios\\bg.waw"), NULL, SND_ASYNC | SND_LOOP);
    int level = currentLevel;
    int room = currentRoom;
    bulletsLeft = levels[level][2];
    int maxEnemies = levels[level][1];
    int maxRooms = levels[level][0];
    int checkPoint[2] = {levels[level][3], levels[level][4]};
    int count = 0;
    system("cls");
    gameRoomCPGrid();
    playerPrint();
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

        // movements & idle
        if((GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) && pX <= 104 && getCharAtxy(pX+15, pY + 3) == ' '){
            playerDir = "right";
            playerRun();
        }
        else if((GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) && pX >= 3  && getCharAtxy(pX-3, pY + 3) == ' '){
            playerDir = "left";
            playerRun();
        }

        // attack input 
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')){
            playerAttack();
        }

        if(GetAsyncKeyState(VK_SPACE) && bulletsLeft > 0)
        {
            int dir = (playerDir == "right");
            generateBullet("player", dir, pX, pY);
        }
        
        if(activeEnemies < 4 && enemyCount < maxEnemies)
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

        if(health == 0)
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

        

        gravity();
        enemies(2);
        moveBullet();
        playerPrint();
        Sleep(1);

        if(score == 40)
        {
            //isWin = true;
            //isGameOver = true;
        }
    }
    /* if(currentLevel < 4)
    {
        if(isWin)
        {
            if( room < maxRooms)
                currentRoom++;
            else
            {
                currentRoom = 0;
                currentLevel++;
            }
        }
    } */
    mainScreen();
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
    Sleep(150);
    removePlayer();
    playerState = "idle";
    playerPrint();
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
void generateEnemy()
{
    int temp = rand() % 2; // 1 > right > start > x++ || 0 > left > end > x--
    static int a = 5;
    int x = levels[currentLevel][currentRoom + a]; a++;
    enemyX[enemyCount] = x;
    enemyY[enemyCount] = levels[currentLevel][currentRoom + a]; a++;
    enemyDir[enemyCount] = temp;
    enemyType[enemyCount] = rand() % 3;
    enemyState[enemyCount] = temp;
    isEnemyAlive[enemyCount] = true;
    printEnemy(enemyX[enemyCount], enemyY[enemyCount], enemyDir[enemyCount], enemyType[enemyCount], enemyState[enemyCount]);
    enemyCount++;
    activeEnemies++;
    if(a == 13)
        a = 5;
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

                    if((getCharAtxy(enemyX[i] , enemyY[i] + 8) != '=' || getCharAtxy(enemyX[i]+8 , enemyY[i] + 8) != '=') || (enemyType[i] != 0 && (enemyX[i] < 12 || enemyX[i] > 90)) ){
                        enemyDir[i] = !enemyDir[i];
                    }
                    if(enemyDir[i])
                        enemyX[i] += 1;
                    else
                        enemyX[i] -= 1;
                    printEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                }
            }
            else if(action == 2)
            {
                char next;
                if(getCharAtxy(enemyX[i] - 1, enemyY[i] + 3) == 'x'){
                    killEnemy(i);
                    removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                }
                else if(playerState == "attack")
                {
                    for(int x = 0; x < 12; x++)
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            if(getCharAtxy(enemyX[x], enemyY[x]) == (char)195){
                                killEnemy(i);
                                removeEnemy(enemyX[i], enemyY[i], enemyDir[i], enemyType[i], enemyState[i]);
                                break;
                            }
                        }
                    }
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
void generateBullet(string type, int dir, int x, int y)
{
    if(type == "player")
    {
        
        bulletX[bulletCount] = dir ? x + 12 : x - 1;
        bulletY[bulletCount] = y + 2;
        bulletOwner[bulletCount] = 1;
        bulletsLeft--;
        updateBoard();
    }else{
        bulletX[bulletCount] = dir ? x + 9 : x - 1;
        bulletY[bulletCount] = y + 4;
        bulletOwner[bulletCount] = 0;
    }
    isBulletActive[bulletCount] = true;
    bulletDir[bulletCount] = dir; 
    bulletCount++;
    gotoxy(bulletX[bulletCount], bulletY[bulletCount]);
    cout << "x";
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
        if (isBulletActive[j])
        {
            char nextChar; int x = 1;
            if(bulletDir[j]){
                nextChar = getCharAtxy(bulletX[j] + 1, bulletY[j]);
            }else{
                nextChar = getCharAtxy(bulletX[j] - 1, bulletY[j]);
                x = -1;
            }

            if (nextChar != ' ')
            {
                eraseBullet(bulletX[j], bulletY[j]);
                makeBulletInactive(j);
                if(bulletOwner[j])
                {
                    if(nextChar == '/' || nextChar == '\\'){
                        score += 10;
                        updateBoard();
                    }
                }
                else
                {
                    if(nextChar == '(' || nextChar == ')'){
                        health -= 10;
                        updateBoard();
                    }
                }
            }
            else
            {
                eraseBullet(bulletX[j], bulletY[j]);
                bulletX[j] += x;
                printBullet(bulletX[j], bulletY[j]);
            }
        }
    }
}
void gameRoomCPGrid()
{
    // Game room grid here
    gotoxy(camX,camY);   cout << "------------------------------------------------------------------------------------------------------------------------";
    gotoxy(camX,camY+1); cout << "| Level: 6 | Room: 7 | Kills: 8  | Bullets: 6    | Health : !          | Score:  6    | Lives : 8      | Coins:  5     |";
    gotoxy(camX,camY+2); cout << "------------------------------------------------------------------------------------------------------------------------";
    gotoxy(camX,camY+3); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+4); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+5); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+6); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+7); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+8); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+9); cout << ".                                                                                                                      .";
    gotoxy(camX,camY+10);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+11);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+12);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+13);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+14);cout << ".===========================                                         ==================================================.";
    gotoxy(camX,camY+15);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+16);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+17);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+18);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+19);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+20);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+21);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+22);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+23);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+24);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+25);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+26);cout << ".========================================                                     =========================================.";
    gotoxy(camX,camY+27);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+28);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+29);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+30);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+31);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+32);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+33);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+34);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+35);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+36);cout << "========================================================================================================================";
    gotoxy(camX,camY+37);cout << "########################################################################################################################";
    gotoxy(camX,camY+38);cout << "########################################################################################################################";
    gotoxy(camX,camY+39);cout << "------------------------------------------------------------------------------------------------------------------------";
    updateBoard();
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