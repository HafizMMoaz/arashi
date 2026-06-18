#include<iostream>
#include<windows.h>
#include<conio.h>
#include <mmsystem.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
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
void drawMainScreen(int);
void gameRoomCP();
void aboutDeveloperScreen(int,int);

/* game screen functionalities */
void gameHeader(int,int);
void gameHeaderCharacter(int,int);
void updateBoard();

/* player functions */
void playerPrint();
void removePlayer();
void restoreField(int sx, int sy, int len);
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

/* ---- endless run ----
   The world is generated one screen at a time. Walking off the right edge
   builds a brand-new screen (ground, trees, clouds, ramps, gems) forever.
   Ramps are raised solid blocks the player jumps onto; their tops are stored
   so enemies can be spawned standing on them.                                */
int  runDistance = 0;         // how many screens travelled (endless progress)
const int MAX_RAMPS = 8;
int  rampX[MAX_RAMPS];        // left column of each ramp
int  rampW[MAX_RAMPS];        // width of each ramp
int  rampTop[MAX_RAMPS];      // field row of each ramp's top solid surface
int  rampCount = 0;           // ramps on the current screen

void loadStage(int s);
void buildDefaultField(int s);
void buildEndlessField();
void startEndless();
void nextScreen();
void saveField(int idx);
void renderField();
void resetActors();
void startStage(int s);
bool changeSection(int delta);
void banner(const string &msg);
void introCutscene();
void storyCutscene();
void drawGirl(int,int);
void drawHeart(int,int);
void drawRing(int,int);
void decorateField(int s);
bool isSolidTile(char c);
bool enemySpaceFree(int idx, int x, int y);
void collectGems();

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

int bulletX[500], bulletY[500], bulletCount = 0, bulletDir[500], bulletOwner[500];
bool isBulletActive[500];
const int maxBullets = 500;

void generateBullet(string, int, int, int);
void eraseBullet(int x, int y);
void printBullet(int x, int y);
void moveBullet();
void makeBulletInactive(int idx);
int  freeBulletSlot();           // index of a reusable inactive bullet, else -1
bool hitsPlayer(int x, int y);   // does (x,y) fall inside the player's body box
bool blockedByEnemy(int newPX, int dir);  // would a step into newPX hit an enemy?

DWORD lastShotTime = 0;          // for fire-rate cooldown
DWORD lastSpawnTime = 0;         // throttles how fast new enemies appear
DWORD lastHitTime = 0;           // cooldown between hits the player can take
DWORD lastRefillTime = 0;        // debounce for the ammo-refill key

const int HIT_DAMAGE = 10;       // health lost per bullet / enemy touch (out of 100)

// Ammo refill: press R to trade coins for bullets (see the play loop).
const int REFILL_COST    = 20;   // coins spent per refill
const int REFILL_AMOUNT  = 50;   // bullets gained per refill
const int MAX_BULLETS_HELD = 999;// cap so the HUD field never overflows



string activeScreen, gameState, playerState = "idle", playerDir="left"; // player states

int coins = 100, currentLevel = 1, currentRoom = 0, enemiesKilled = 0, health = 100, score = 0, bulletsLeft = 100, lives = 3, currentRoomKills = 0;
bool isGameOver = false, isWin = false;

main()
{
    srand((unsigned)GetTickCount());
    screenSetup(VIEW_W, 40, VIEW_W, 40, FALSE);   // buffer == window: no clipping, no scrollbars
    playBackgroundMusic();
    mainScreen();           // the intro now plays when "New Game" is chosen
}

void introCutscene()
{
    system("cls");
    gotoxy(20,6);  cout << "ARASHI";
    gotoxy(20,8);  cout << "A ninja sets out to rescue the girl he loves.";
    gotoxy(20,10); cout << "She has been captured by a dark force.";
    gotoxy(20,11); cout << "He crosses broken tracks, traps, enemies, and ruins to reach her.";
    gotoxy(20,12); cout << "When he wins the final battle, he frees her and they marry.";
    gotoxy(20,14); cout << "Press SPACE to begin.";
    while(!(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_RETURN)))
        Sleep(30);
    while(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_RETURN))
        Sleep(30);
}

void storyCutscene()
{
    system("cls");

    int heroX = 12, girlX = 95, charY = 9;   // sprite top-left positions

    setColor(14);
    gotoxy(48,2);  cout << "------ ARASHI ------";
    setColor(7);

    // The real in-game hero (facing right) and a matching girl (facing left).
    setColor(11); gameHeaderCharacter(heroX, charY);   // real hero sprite
    setColor(13); drawGirl(girlX, charY);              // girl in the same style
    setColor(7);
    Sleep(1100);

    // They walk toward each other to meet in the middle.
    for(int r=0; r<7; r++){
        gotoxy(heroX,   charY+r); cout << "                ";
        gotoxy(girlX-2, charY+r); cout << "              ";
    }
    heroX = 40; girlX = 66;
    setColor(11); gameHeaderCharacter(heroX, charY);
    setColor(13); drawGirl(girlX, charY);
    setColor(7);

    int mx[8]  = {38, 46, 54, 62, 70, 42, 58, 74};   // columns of the floating hearts
    int mph[8] = { 0,  2,  4,  1,  3,  5,  2,  4};    // their starting heights
    const int aTop = 3, aBot = 7;                     // open-air band above the couple
    int aspan = aBot - aTop + 1;
    for(int frame = 0; frame < 16; frame++){
        // the big heart beats: red on even frames, pink on odd
        setColor((frame % 2) ? 12 : 13);
        drawHeart(55, charY);

        // little hearts drift upward through the air above them
        for(int yy = aTop; yy <= aBot; yy++){ gotoxy(36, yy); cout << string(42,' '); }
        for(int i = 0; i < 8; i++){
            int yy = aBot - ((frame + mph[i]) % aspan);
            setColor((i % 2) ? 13 : 12);
            gotoxy(mx[i], yy); cout << (char)3;
        }
        Sleep(160);
    }
    setColor(7);
    for(int yy = aTop;  yy <= aBot;     yy++){ gotoxy(36, yy); cout << string(42,' '); }
    for(int yy = charY; yy <= charY+5;  yy++){ gotoxy(55, yy); cout << "         "; } // clear big heart
    Sleep(300);

    // The enemy attacks.
    setColor(12);
    gotoxy(56, charY+2); cout << " ";                     // the heart breaks away
    gotoxy(54, charY+1); cout << "<* * *";                // shots streak at the hero
    gotoxy(40, charY-2); cout << "                                        ";
    gotoxy(40, charY-2); cout << "An enemy fires at the hero from the shadows!";
    setColor(7);
    Sleep(1200);

    // A cage drops over the girl and traps her.
    setColor(8);
    gotoxy(girlX-2, charY-1); cout << "+-+-+-+-+-+-+-+";
    for(int r=0; r<7; r++){
        gotoxy(girlX-2,  charY+r); cout << "|";
        gotoxy(girlX+11, charY+r); cout << "|";
    }
    gotoxy(girlX-2, charY+7); cout << "+-+-+-+-+-+-+-+";
    setColor(13); drawGirl(girlX, charY);                 // still visible behind the bars
    setColor(7);
    gotoxy(girlX-6, charY+9); cout << "A cage drops and traps her!";
    Sleep(1400);

    // ---- Happy ending: the hero frees her and a wedding ring appears. ----
    system("cls");
    setColor(14);
    gotoxy(48,1);  cout << "==== Happy Ending ====";
    setColor(7);
    gotoxy(20,3);  cout << "The hero fights through every enemy and sets her free.";

    int hy = 16, hHeroX = 46, hGirlX = 61;    // the two reunited, side by side
    setColor(11); gameHeaderCharacter(hHeroX, hy);
    setColor(13); drawGirl(hGirlX, hy);

    // A sparkling wedding ring appears above them, with hearts dancing around.
    drawRing(52, 5);
    int cx[8]  = {42, 50, 58, 66, 74, 46, 62, 70};
    int cph[8] = { 0,  2,  1,  3,  0,  2,  1,  3};
    const int cTop = 12, cBot = 14;           // heart band between ring and couple
    int cspan = cBot - cTop + 1;
    for(int frame = 0; frame < 16; frame++){
        // twinkles around the ring (they self-clear on alternate frames)
        setColor(15);
        gotoxy(50, 6);  cout << ((frame % 2) ? "+" : " ");
        gotoxy(64, 7);  cout << ((frame % 2) ? " " : "+");
        gotoxy(51, 10); cout << ((frame % 2) ? "." : " ");
        // hearts drifting upward between the ring and the couple
        for(int yy = cTop; yy <= cBot; yy++){ gotoxy(40, yy); cout << string(38,' '); }
        for(int i = 0; i < 8; i++){
            int yy = cBot - ((frame + cph[i]) % cspan);
            setColor((i % 2) ? 12 : 13);
            gotoxy(cx[i], yy); cout << (char)3;
        }
        drawRing(52, 5);                      // keep the ring crisp above the hearts
        Sleep(170);
    }
    setColor(7);
    for(int yy = cTop; yy <= cBot; yy++){ gotoxy(40, yy); cout << string(38,' '); }

    setColor(14);
    gotoxy(40, hy+8);  cout << "They marry, and live happily ever after.";
    gotoxy(54, hy+10); cout << (char)3 << "  THE  END  " << (char)3;
    setColor(11);
    gotoxy(50, hy+12); cout << "Press SPACE to return.";
    setColor(7);
    while(!(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_RETURN)))
        Sleep(30);
    while(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_RETURN))
        Sleep(30);
}

void drawGirl(int x, int y)
{
    gotoxy(x,y);   cout << "  _____   ";
    gotoxy(x,y+1); cout << " /__   \\  ";
    gotoxy(x,y+2); cout << " |" << (char)248 << "v" << (char)248 << " \\|  ";
    gotoxy(x,y+3); cout << " (__@_)   ";
    gotoxy(x,y+4); cout << "  /|^|\\   ";
    gotoxy(x,y+5); cout << " //   \\\\  ";
    gotoxy(x,y+6); cout << "  J   L   ";
}

void drawHeart(int x, int y)
{
    gotoxy(x,y);   cout << " *** *** ";
    gotoxy(x,y+1); cout << "*********";
    gotoxy(x,y+2); cout << " ******* ";
    gotoxy(x,y+3); cout << "  *****  ";
    gotoxy(x,y+4); cout << "   ***   ";
    gotoxy(x,y+5); cout << "    *    ";
}

void drawRing(int x, int y)
{
    setColor(11); gotoxy(x,y);   cout << "      __";
    setColor(11); gotoxy(x,y+1); cout << "     /  \\";
    setColor(11); gotoxy(x,y+2); cout << "     \\  /";
    setColor(11); gotoxy(x,y+3); cout << "      \\/";
    setColor(14); gotoxy(x,y+4); cout << "   .-'  '-.";
    setColor(14); gotoxy(x,y+5); cout << "  (        )";
    setColor(14); gotoxy(x,y+6); cout << "   '-.__.-'";
}

void playBackgroundMusic()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    string dir = exePath;
    size_t slash = dir.find_last_of("\\/");
    dir = (slash == string::npos) ? "." : dir.substr(0, slash);
    string wav = dir + "\\audios\\bg.wav";
    if(!PlaySoundA(wav.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP))
    {
        PlaySoundA("audios\\bg.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
}

void drawMainScreen(int option)
{
    system("cls");
    gameHeaderCharacter(5,5);
    gameHeader(22,3);
    gameHeaderCharacter(70,5);
    mainMenu(3,15,option);
    gotoxy(50,16);cout <<"---- Instructions ----";
    gotoxy(50,18);cout << "Press [UP] or [W] For Upword Navigation";
    gotoxy(50,19);cout << "Press [DOWN] or [S] For Downword Navigation";
    gotoxy(50,20);cout << "Press [SPACE] or [Q] For Selecting Option";
}
void mainScreen()
{
    int option = 0;
    drawMainScreen(option);
    while(true){
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')){
            if(option < 6)
                option++;
            mainMenu(3,15,option);
        }
        else if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')){
            if(option > 0)
                option--;
            mainMenu(3,15,option);
        }
        else if(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState('Q')){
            if(option == 0){
                introCutscene();        // story intro plays before a new game
                gameRoomCP();
            }else if(option == 3){
                storyCutscene();
            }else if(option == 5){
                aboutDeveloperScreen(3,15);
            }else if(option == 6){
                exit(0);
            }
            // back from a sub-screen: redraw the menu, then wait for the
            // select key to be released so we don't instantly re-trigger it
            drawMainScreen(option);
            while(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState('Q') || GetAsyncKeyState(VK_RETURN))
                Sleep(30);
        }
        Sleep(120);
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
    startEndless();                 // build the first endless screen, place the player
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
                Sleep(50);
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

        // move right / at the screen edge, run on into a fresh endless screen
        if(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')){
            playerDir = "right";
            if(pX <= 104 && !isSolidTile(getCharAtxy(pX+15, pY + 3)) && !blockedByEnemy(pX + 1, 1))
                playerRun();
            else if(pX > 104)
                nextScreen();       // endless: the world keeps going to the right
        }
        // move left (you can't go back past the left edge of the screen)
        else if(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')){
            playerDir = "left";
            if(pX >= 3 && !isSolidTile(getCharAtxy(pX-3, pY + 3)) && !blockedByEnemy(pX - 1, 0))
                playerRun();
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

        // refill ammo: spend coins for bullets (debounced so a held key buys once)
        if(GetAsyncKeyState('R') && GetTickCount() - lastRefillTime > 300)
        {
            lastRefillTime = GetTickCount();
            if(coins >= REFILL_COST)
            {
                coins -= REFILL_COST;
                bulletsLeft += REFILL_AMOUNT;
                if(bulletsLeft > MAX_BULLETS_HELD) bulletsLeft = MAX_BULLETS_HELD;
                updateBoard();
                gotoxy(50, 4); cout << "+" << REFILL_AMOUNT << " AMMO";
            }
            else
            {
                gotoxy(50, 4); cout << "NEED " << REFILL_COST << " COINS";
            }
            Sleep(120);
            restoreField(50, 4, 14);    // wipe the flash without blanking the field
        }

        // keep at most 2 enemies on screen, and only let a new one appear every
        // 1.5s, so the player isn't swarmed (endless: spawning never stops)
        if(activeEnemies < 2 && GetTickCount() - lastSpawnTime > 1500)
        {
            generateEnemy();
            lastSpawnTime = GetTickCount();
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

        collectGems();

        // Enemy melee: touching an enemy (its sword/body) costs 10% health, but
        // only once every 0.8s so contact drains gradually instead of killing.
        if(GetTickCount() - lastHitTime > 800)
        {
            for(int i = 0; i < enemyCount; i++)
            {
                if(!isEnemyAlive[i])
                    continue;
                bool overlap = !(pX + 12 < enemyX[i] - 1 || pX - 1 > enemyX[i] + 12 ||
                                 pY + 6  < enemyY[i]     || pY     > enemyY[i] + 7);
                if(overlap)
                {
                    health -= HIT_DAMAGE;
                    if(health < 0) health = 0;
                    lastHitTime = GetTickCount();
                    updateBoard();
                    playerPrint();
                    break;
                }
            }
        }

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
    // return to the caller (mainScreen) which redraws the menu
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
// Build one screen of the endless Mario-style world: ground, clouds, trees,
// ramps (raised solid blocks) and gems. Ramp tops are recorded so enemies can
// be placed standing on them.
void buildEndlessField()
{
    int w = VIEW_W;

    for(int r = 0; r <= 32; r++)            // open sky
        fieldRows[r] = string(w, ' ');
    fieldRows[33] = string(w, '=');         // ground surface
    fieldRows[34] = string(w, '#');         // dirt
    fieldRows[35] = string(w, '#');
    fieldRows[36] = string(w, '-');         // bottom border

    fieldWidth   = w;
    sectionCount = 1;
    section      = 0;
    rampCount    = 0;

    // clouds drifting in the sky (decorative, non-solid)
    for(int x = 6; x < w - 10; x += 16 + rand() % 12)
        if(rand() % 2)
        {
            int cy = 2 + rand() % 3;
            fieldRows[cy][x]   = '('; fieldRows[cy][x+1] = '~';
            fieldRows[cy][x+2] = '~'; fieldRows[cy][x+3] = ')';
        }

    // trees along the ground: leafy top above head height, trunk to the ground
    for(int x = 10; x < w - 8; x += 18 + rand() % 12)
        if(rand() % 2)
        {
            fieldRows[25][x+1]='('; fieldRows[25][x+2]='@'; fieldRows[25][x+3]='@'; fieldRows[25][x+4]=')';
            fieldRows[26][x+1]='('; fieldRows[26][x+2]='@'; fieldRows[26][x+3]='@'; fieldRows[26][x+4]=')';
            for(int rr = 27; rr <= 32; rr++){ fieldRows[rr][x+2]='|'; fieldRows[rr][x+3]='|'; }
        }

    // ramps: raised solid blocks at least 4 tall, so the player must jump them.
    // (drawn after the trees so a ramp is always fully solid). Tops are stored.
    for(int x = 26; x < w - 30; x += 24 + rand() % 14)
    {
        if(rand() % 2 == 0 || rampCount >= MAX_RAMPS)
            continue;
        int h   = 4 + rand() % 2;           // 4..5 rows tall
        int top = 33 - h;                   // field row of the ramp's top surface
        int rw  = 14 + rand() % 6;          // 14..19 wide (room to stand on)
        if(x + rw > w - 18) rw = (w - 18) - x;
        if(rw < 12) continue;
        for(int rr = top; rr <= 32; rr++)
            for(int xx = x; xx < x + rw; xx++)
                fieldRows[rr][xx] = '=';
        rampX[rampCount] = x; rampW[rampCount] = rw; rampTop[rampCount] = top;
        rampCount++;
        fieldRows[top - 1][x + rw / 2] = '$';   // a gem rewards the climb
    }

    // a scatter of ground-level gems
    for(int x = 14; x < w - 12; x += 19)
        if(rand() % 2) fieldRows[27][x] = '$';
}
// Begin a fresh endless run from the very first screen.
void startEndless()
{
    stage = 1; section = 0; runDistance = 0;
    currentLevel = 1; currentRoom = 1; currentRoomKills = 0;
    bulletsLeft = 120;
    resetActors();
    buildEndlessField();
    renderField();
    pX = 6; pY = GROUND_TOP + 1;             // enter from the left edge
    playerState = "idle"; playerDir = "right";
    playerPrint();
    updateBoard();
}
// Run off the right edge into a brand-new screen -- the world never ends.
void nextScreen()
{
    runDistance++;
    currentRoom  = runDistance + 1;          // HUD "Room" = how far you've run
    currentLevel = 1 + runDistance / 5;      // difficulty tier creeps up
    resetActors();                           // old screen's enemies/bullets are gone
    buildEndlessField();
    renderField();
    pX = 6; pY = GROUND_TOP + 1;             // step in from the left of the new screen
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
        if(isSolidTile(getCharAtxy(pX + i, pY - 1)))
            return false;
    }
    return true;
}
void gravity()
{
    if(isSolidTile(getCharAtxy(pX, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 1, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 2, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 3, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 4, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 5, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 6, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 7, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 8, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 9, pY + 7)))
        return;
    if(isSolidTile(getCharAtxy(pX + 10, pY + 7)))
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
    setColor(10);               // ninja drawn in bright green
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
    setColor(7);                // back to default for everything else
}
// Repaint the field background across a horizontal run of cells. Erasing a
// sprite this way (instead of blanking with spaces) keeps the solid tiles a
// ramp, platform or the ground are made of intact under it -- so swinging the
// sword or walking across a ramp no longer punches holes through it.
void restoreField(int sx, int sy, int len)
{
    int r    = sy - 3;                    // console row -> field row
    int base = section * VIEW_W + sx;     // leftmost field column shown here
    gotoxy(sx, sy);
    for(int i = 0; i < len; i++)
    {
        int c = base + i;
        char ch = ' ';
        if(r >= 0 && r < FIELD_ROWS && c >= 0 && c < (int)fieldRows[r].size())
            ch = fieldRows[r][c];
        cout << ch;
    }
}
void removePlayer()
{
    restoreField(pX, pY, 14);
    if(playerState == "attack")
        restoreField(playerDir == "left" ? pX - 1 : pX + 14, pY, 1);

    restoreField(pX, pY + 1, 14);
    if(playerState == "attack")
        restoreField(playerDir == "left" ? pX - 1 : pX + 14, pY + 1, 1);

    restoreField(pX, pY + 2, 13);
    if(playerState == "attack")
        restoreField(playerDir == "left" ? pX - 1 : pX + 13, pY + 2, 1);

    restoreField(pX, pY + 3, 13);
    if(playerState == "attack"){
        if(playerDir == "left")
            restoreField(pX - 13, pY + 3, 14);   // sword reaches left of the body
        else
            restoreField(pX + 13, pY + 3, 12);   // sword reaches right of the body
    }

    restoreField(pX, pY + 4, 13);
    restoreField(pX, pY + 5, 13);
    restoreField(pX, pY + 6, 11);
    if(playerState == "jump"){
        restoreField(pX + 11, pY + 6, 3);
        restoreField(pX, pY + 7, 13);
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

    int x, y = GROUND_TOP;
    if(rampCount > 0 && rand() % 2)     // sometimes the enemy stands on a ramp
    {
        int r = rand() % rampCount;
        y = rampTop[r] - 5;             // feet rest on the ramp's top surface
        int room = rampW[r] - 13;
        x = rampX[r] + 1 + (room > 0 ? rand() % room : 0);
    }
    else                                // otherwise on the ground, away from the player
    {
        int attempts = 0;
        do {
            x = 12 + rand() % 90;
            attempts++;
        } while((x > pX - 18 && x < pX + 18) || !enemySpaceFree(-1, x, GROUND_TOP) || attempts < 20);
    }

    int dir = rand() % 2;
    enemyX[e] = x;
    enemyY[e] = y;                      // stands on the floor or a ramp (enemyY+8 == surface)
    enemyDir[e] = dir;
    enemyType[e] = rand() % 3;
    enemyState[e] = dir;
    isEnemyAlive[e] = true;
    printEnemy(enemyX[e], enemyY[e], enemyDir[e], enemyType[e], enemyState[e]);
    if(e >= enemyCount)
        enemyCount = e + 1;
    activeEnemies++;
}
bool enemySpaceFree(int idx, int x, int y)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(i == idx || !isEnemyAlive[i])
            continue;
        if(!(x + 12 < enemyX[i] - 1 || x - 1 > enemyX[i] + 12 || y + 7 < enemyY[i] || y > enemyY[i] + 7))
            return false;
    }
    return true;
}
void printEnemy(int x, int y, int dir, int type, int state)
{
    // colour the three enemy kinds so they read apart at a glance
    setColor(type == 0 ? 12 : (type == 1 ? 14 : 9));   // 0 red, 1 yellow, 2 blue
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
    setColor(7);                // back to default once the enemy is drawn
}
void removeEnemy(int x, int y, int dir, int type, int state)
{
    // Erase by repainting the field background (see restoreField) so an enemy --
    // and especially its outstretched sword on the y+4 row -- doesn't blank the
    // ramp/platform tiles it overlaps.
    if(dir)
    {
        if(type == 0)
        {
            restoreField(x, y,   7);
            restoreField(x, y+1, 8);
            restoreField(x, y+2, 9);
            restoreField(x, y+3, 8);
            restoreField(x, y+4, 8);
            restoreField(x, y+5, 7);
            restoreField(x, y+6, 8);
            restoreField(x, y+7, 7);
        }
        else if(type == 1)
        {
            restoreField(x, y,   7);
            restoreField(x, y+1, 8);
            restoreField(x, y+2, 9);
            restoreField(x, y+3, 8);
            if(state == 0)
                restoreField(x-4, y+4, 20);
            else
                restoreField(x,   y+4, 21);
            restoreField(x, y+5, 7);
            restoreField(x, y+6, 8);
            restoreField(x, y+7, 7);
        }
        else if(type == 2)
        {
            restoreField(x, y,   7);
            restoreField(x, y+1, 12);
            restoreField(x, y+2, 12);
            restoreField(x, y+3, 12);
            if(state == 0)
                restoreField(x-4, y+4, 19);
            else
                restoreField(x,   y+4, 21);
            restoreField(x, y+5, 12);
            restoreField(x, y+6, 12);
            restoreField(x, y+7, 12);
        }
    }
    else
    {
        if(type == 0)
        {
            restoreField(x, y,   6);
            restoreField(x, y+1, 7);
            restoreField(x, y+2, 8);
            restoreField(x, y+3, 7);
            restoreField(x, y+4, 8);
            restoreField(x, y+5, 8);
            restoreField(x, y+6, 9);
            restoreField(x, y+7, 8);
        }
        else if(type == 1)
        {
            restoreField(x, y,   6);
            restoreField(x, y+1, 7);
            restoreField(x, y+2, 8);
            restoreField(x, y+3, 7);
            if(state == 0)
                restoreField(x-7,  y+4, 21);
            else
                restoreField(x-10, y+4, 21);
            restoreField(x, y+5, 8);
            restoreField(x, y+6, 9);
            restoreField(x, y+7, 8);
        }
        else if(type == 2)
        {
            restoreField(x, y,   9);
            restoreField(x, y+1, 10);
            restoreField(x, y+2, 11);
            restoreField(x, y+3, 10);
            if(state == 0)
                restoreField(x-2, y+4, 17);
            else
                restoreField(x-9, y+4, 20);
            restoreField(x, y+5, 11);
            restoreField(x, y+6, 12);
            restoreField(x, y+7, 11);
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
                    if(!enemySpaceFree(i, enemyX[i], enemyY[i]))
                    {
                        if(enemyDir[i]) enemyX[i] -= 1;
                        else enemyX[i] += 1;
                        enemyDir[i] = !enemyDir[i];
                    }
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
// True if a one-step move to newPX would push the player's body into a living
// enemy on the side it's heading toward (dir: 1 = right, 0 = left). Enemies on
// the far side never block, so the player can always back out of contact.
bool blockedByEnemy(int newPX, int dir)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(!isEnemyAlive[i])
            continue;
        bool vOverlap = !(pY + 6 < enemyY[i] || pY > enemyY[i] + 7);
        bool hOverlap = !(newPX + 12 < enemyX[i] - 1 || newPX - 1 > enemyX[i] + 12);
        if(vOverlap && hOverlap)
        {
            if(dir  && enemyX[i] + 6 >= newPX + 6) return true;   // enemy to the right
            if(!dir && enemyX[i] + 6 <= newPX + 6) return true;   // enemy to the left
        }
    }
    return false;
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
  restoreField(x, y, 1);   // repaint the tile the bullet sat on, not a blank
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

        if(nx < 1 || nx >= VIEW_W - 1)  // left the visible play area: retire it
        {
            eraseBullet(bulletX[j], bulletY[j]);
            makeBulletInactive(j);
            continue;
        }

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
            if(isSolidTile(getCharAtxy(nx, ny)))   // wall / platform
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
                health -= HIT_DAMAGE;   // a shot takes 10%, not the whole life
                if(health < 0) health = 0;
                lastHitTime = GetTickCount();
                updateBoard();
                playerPrint();          // repair the part of the sprite the bullet sat on
                continue;
            }
            if(isSolidTile(getCharAtxy(nx, ny)))
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

    decorateField(s);
}

bool isSolidTile(char c)
{
    return c == '.' || c == '=' || c == '#' || c == '-';
}

void decorateField(int s)
{
    int w = fieldWidth;
    for(int x = 8; x < w - 8; x += 28)
    {
        if(rand() % 3 == 0)
        {
            int cloudRow = 4 + rand() % 4;
            fieldRows[cloudRow][x] = '(';
            fieldRows[cloudRow][x + 1] = '_';
            fieldRows[cloudRow][x + 2] = '_';
            fieldRows[cloudRow][x + 3] = ')';
        }
    }

    for(int x = 6; x < w - 6; x += 18)
    {
        if(rand() % 2 == 0)
        {
            fieldRows[GROUND_TOP - 6][x] = '=';
            fieldRows[GROUND_TOP - 6][x + 1] = '=';
            fieldRows[GROUND_TOP - 6][x + 2] = '=';
            fieldRows[GROUND_TOP - 6][x + 3] = '=';
        }
    }

    for(int x = 10; x < w - 10; x += 22)
    {
        if(rand() % 2 == 0)
            fieldRows[GROUND_TOP - 1][x] = '$';
    }

    for(int x = 14; x < w - 14; x += 31)
    {
        if(rand() % 2 == 0)
            fieldRows[GROUND_TOP - 3][x] = '^';
    }
}

void collectGems()
{
    int worldX = section * VIEW_W + pX;
    for(int dy = 0; dy <= 6; dy++)
    {
        for(int dx = 0; dx <= 12; dx++)
        {
            int x = worldX + dx;
            if(x < 0 || x >= fieldWidth)
                continue;
            int row = pY + dy - 3;
            if(row < 0 || row >= FIELD_ROWS)
                continue;
            if(fieldRows[row][x] == '$')
            {
                fieldRows[row][x] = ' ';
                gotoxy(pX + dx, pY + dy); cout << ' ';
                coins += 1;
                score += 2;
                updateBoard();
            }
        }
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
    gotoxy(80,17);cout << "Move    : Arrows / A D";
    gotoxy(80,18);cout << "Jump    : Up / W";
    gotoxy(80,19);cout << "Sword   : Down / S";
    gotoxy(80,20);cout << "Shoot   : Space";
    gotoxy(80,21);cout << "Refill  : R  (" << REFILL_COST << " coins -> " << REFILL_AMOUNT << " ammo)";
    gotoxy(80,22);cout << "Pause   : P  (R resumes)";
    gotoxy(80,24);cout << "Press [SPACE] or [ESC] To Exit";
    while(!(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_ESCAPE)))
        Sleep(30);
    while(GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_ESCAPE))
        Sleep(30);
    // return to the menu, which redraws itself
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
        cout << "+";

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
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    // A console window can never be wider/taller than its buffer, so resize in
    // a safe order: collapse the window first, grow the buffer, then open the
    // window to the exact play size. Buffer == window => no scrollbars, no
    // clipping of the 120-wide field.
    SMALL_RECT minWin = {0, 0, 1, 1};
    SetConsoleWindowInfo(h, TRUE, &minWin);

    COORD bufferSize = { (SHORT)bufferX, (SHORT)bufferY };
    SetConsoleScreenBufferSize(h, bufferSize);

    SMALL_RECT windowSize = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};
    SetConsoleWindowInfo(h, TRUE, &windowSize);

    // Lock the window at the game size: drop the sizing frame and maximize box
    // so it can't be stretched away from the 120x40 layout.
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, style);

    // Position it a little in from the top-left, keeping the size we just set.
    SetWindowPos(hwnd, NULL, 250, 100, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(h, &curInfo);
    curInfo.bVisible = cursor;
    SetConsoleCursorInfo(h, &curInfo);
}