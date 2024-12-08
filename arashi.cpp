#include<iostream>
#include<windows.h>
#include<conio.h>
using namespace std;

/* setups */
void gotoxy(int,int);
char getCharAtxy(short int, short int);
void screenSetup(int,int,int,int,bool);
void setColor(int);

/* screens */
void mainMenu(int,int,int);
void mainScreen();
void gameScreen();
void aboutDeveloperScreen(int,int);

/* game screen functionalities */
void gameRoomGrid();
void gameHeader(int,int);
void gameHeaderCharacter(int,int);

/* player functions */
void playerPrint();
void removePlayer();
void playerJump();
void playerAttack();
void playerRun();
void gravity();

/* enemy functions */
const int maxEnemies = 50;
int enemyX[maxEnemies], enemyY[maxEnemies], enemyType[maxEnemies], enemyDir[maxEnemies], enemyCount = 0;
void generateEnemy();
void printEnemy(int,int,int,int);

int pX = 10, pY = 29; // player coordinates
int camX = 0, camY = 0; // camera coordinates
string activeScreen, gameState, playerState = "idle", playerDir="left"; // player states

main()
{
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
                gameScreen();
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
void gameScreen()
{
    char input;
    system("cls");
    gameRoomGrid();
    playerPrint();
    Sleep(100);
    
    while(true){
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
        else{
            playerState = "idle";
            playerPrint();
        }

        // attack input 
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')){
            playerAttack();
        }
        
        generateEnemy();
        gravity();
        Sleep(5);
    }
}
void playerJump()
{
    int yDestination = pY - 15;
    playerState = "jump"; pY--;
    while(getCharAtxy(pX, pY - 1) == ' ' && getCharAtxy(pX + 14, pY - 1) == ' ' && pY != yDestination ){
        removePlayer(); pY--;
        playerPrint();
        Sleep(2);
    }
    Sleep(50);
    removePlayer();
    playerState = "idle"; 
}
void gravity()
{
    if(getCharAtxy(pX, pY + 7) == ' ' && getCharAtxy(pX + 1, pY + 7) == ' ' && getCharAtxy(pX + 2, pY + 7) == ' ' && getCharAtxy(pX + 3, pY + 7) == ' ' && getCharAtxy(pX + 4, pY + 7) == ' ' && getCharAtxy(pX + 5, pY + 7) == ' ' && getCharAtxy(pX + 6, pY + 7) == ' ' && getCharAtxy(pX + 7, pY + 7) == ' ' && getCharAtxy(pX + 8, pY + 7) == ' ' && getCharAtxy(pX + 9, pY + 7) == ' ' && getCharAtxy(pX + 10, pY + 7) == ' ')
    {
        removePlayer();
        pY++;
        playerPrint();
    }
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
        else if(playerState == "fire")
        {
            gotoxy(pX,pY);  cout << " .    _____  ";
            gotoxy(pX,pY+1);cout << ". .`./__   \\";
            gotoxy(pX,pY+2);cout << "     |/ " <<  (char)248 << "U" <<  (char)248 << "|";
            gotoxy(pX,pY+3);cout << "     (__E3 ) ";
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
        else if(playerState == "fire")
        {
            gotoxy(pX,pY);  cout << " .    _____  ";
            gotoxy(pX,pY+1);cout << ". .`./__   \\";
            gotoxy(pX,pY+2);cout << "     |/ " <<  (char)248 << "U" <<  (char)248 << "|";
            gotoxy(pX,pY+3);cout << "     (__E3 ) ";
            gotoxy(pX,pY+4);cout << "   7 /    <  ";
            gotoxy(pX,pY+5);cout << "  / (__/ __) ";
            gotoxy(pX,pY+6);cout << "     U    U  ";
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
    int x = (temp == 0) ? 89 : 2;
    enemyX[enemyCount] = x;
    enemyY[enemyCount] = 28;
    enemyDir[enemyCount] = temp;
    enemyType[enemyCount] = rand() % 3;

    printEnemy(enemyX[enemyCount], enemyY[enemyCount], enemyDir[enemyCount], enemyType[enemyCount]);
}
void printEnemy(int x, int y, int dir, int type)
{
    if(dir == 0)
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__88 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
        else if(type == 1)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__99 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
        else if(type == 2)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__11 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
    }
    else
    {
        if(type == 0)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__66 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
        else if(type == 1)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__77 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
        else if(type == 2)
        {
            gotoxy(x, y);   cout << "   ____";
            gotoxy(x, y+1); cout << "  /____\\";
            gotoxy(x, y+2); cout << " `|/" <<  (char)248 << " " <<  (char)248 << "|'";
            gotoxy(x, y+3); cout << "  \\_ -_/";
            gotoxy(x, y+4); cout << " (__22 )";
            gotoxy(x, y+5); cout << " /    <";
            gotoxy(x, y+6); cout << "(__/ __)";
            gotoxy(x, y+7); cout << " U    U";
        }
    }
}
void gameRoomGrid()
{
    // Game room grid here
    gotoxy(camX,camY);   cout << "------------------------------------------------------------------------------------------------------------------------";
    gotoxy(camX,camY+1); cout << "|      Room No.      |     Enemies Killed      |     Health   ********************  |    Score   |  Lives : \3 \3 \3 \3 \3  |";
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
    gotoxy(camX,camY+14);cout << ".                                                                                   ===================================.";
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
    gotoxy(camX,camY+26);cout << ".===================================                                                                                   .";
    gotoxy(camX,camY+27);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+28);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+29);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+30);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+31);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+32);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+33);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+34);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+35);cout << ".                                                                                                                      .";
    gotoxy(camX,camY+36);cout << "------------------------------------------------------------------------------------------------------------------------";
    gotoxy(camX,camY+37);cout << "########################################################################################################################";
    gotoxy(camX,camY+38);cout << "########################################################################################################################";
    gotoxy(camX,camY+39);cout << "------------------------------------------------------------------------------------------------------------------------";
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