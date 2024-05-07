#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include <string.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50



char currentUsername[MAX_USERNAME_LENGTH];

#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
}snake;


// 结构体定义用户
struct User {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
};



//全局变量//
time_t startTime;
int duration;

int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。

//声明全部函数//
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
//void endgame(int endgamestatus, int score);

void gamestart();

void showUserLog();
void recordUserLog(int userID, char* username,time_t startTime,int duration, int score);
int userID_counter = 0; // 用户ID计数器
// 函数声明
void registerUser();
void loginUser();
void showusername();
int isUsernameTaken(const char *username);
int validateUser(const char *username, const char *password);
void enterGame();


// 用户注册函数
void registerUser() {
    struct User newUser;

    // 输入用户名
    printf("输入用户名： ");
    scanf("%s", newUser.username);

    // 检查用户名是否已被使用
    if (isUsernameTaken(newUser.username)) {
        printf("用户名已经被使用，请重新选择。\n");
        return 0;
    }

    // 输入密码
    printf("输入密码: ");
    scanf("%s", newUser.password);

    // 保存用户信息到文件
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    fprintf(file, "%s %s\n", newUser.username, newUser.password);
    fclose(file);

    printf("注册成功!\n");
}

// 用户登录函数
void loginUser() {
   // char username[100];
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    // 输入用户名和密码
    printf("用户名: ");
    scanf("%s", username);
    printf("密码: ");
    scanf("%s", password);

    // 验证用户信息
    if (validateUser(username, password)) {
        printf("登陆成功!\n");
        strcpy(currentUsername, username); // 更新全局变量
        enterGame();
        return currentUsername;
    } else {
        printf("用户名或密码错误，请重试。\n");
        free(username);
        return NULL;
    }
}
void showusername(){
    // FILE *file = fopen("users.txt", "r");

    if(strlen(currentUsername) > 0) {
        printf("***%s正在游戏中***\n", currentUsername);
    } else {
        printf("请先登录.\n");
    }




}
// 进入游戏
void enterGame() {
    printf("Entering game...\n");
    startTime=time(NULL);
  gamestart();

    // 在这里可以添加进入游戏的代码逻辑
}

// 检查用户名是否已被使用
int isUsernameTaken(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0; // 如果文件不存在，则用户名肯定未被使用
    }

    char storedUsername[MAX_USERNAME_LENGTH];
    while (fscanf(file, "%s", storedUsername) != EOF) {
        if (strcmp(storedUsername, username) == 0) {
            fclose(file);
            return 1; // 用户名已被使用
        }
    }

    fclose(file);
    return 0; // 用户名未被使用
}

// 验证用户信息
int validateUser(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0; // 如果文件不存在，则用户名密码验证失败
    }

    char storedUsername[MAX_USERNAME_LENGTH];
    char storedPassword[MAX_PASSWORD_LENGTH];
    while (fscanf(file, "%s %s", storedUsername, storedPassword) != EOF) {
        if (strcmp(storedUsername, username) == 0 && strcmp(storedPassword, password) == 0) {
            fclose(file);

            return 1; // 用户名密码验证成功
        }
    }

    fclose(file);
    return 0; // 用户名密码验证失败
}

void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood()//随机出现食物
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //保证其为偶数，使得食物能与蛇头对其
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next == NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y) //判断蛇身是否与食物重合
        {
            free(food_1);
            createfood();
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

void gamecircle()//控制游戏
{

     Pos(64,1);
     showusername();
    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64,18);
    printf("按F5显示游戏日志");
    Pos(64, 19);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为1
                }
            }
        }
      else if (GetAsyncKeyState(VK_F5))
        {
    //gamestart();
              showUserLog();
        }
        //time_t startTime=time(NULL);
        Sleep(sleeptime);
        snakemove();
    }
}

void welcometogame()//开始界面
{
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，2 为减速\n");
    Pos(25, 13);
    printf("加速将能得到更高的分数。\n");
    system("pause");
    system("cls");
}

void endgame()//结束游戏
{


    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }

    Pos(24, 13);
    printf("您的得分是%d\n", score);

    extern time_t startTime;
extern int duration;

}


void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}

// 显示游戏用户日志的函数
void showUserLog() {
    system("cls");
    printf("游戏用户日志：\n");
    // 打开游戏用户日志文件
    FILE* logFile = fopen("userlog.txt", "r");
    if (logFile == NULL) {
        printf("无法打开游戏用户日志文件。\n");
        return;
    }
    // 逐行读取日志文件并显示
    char line[100];
    while (fgets(line, sizeof(line), logFile)) {
        printf("%s", line);
    }
    fclose(logFile);
}

// 记录游戏用户日志的函数
void recordUserLog(int userID, char* username,time_t startTime, int duration, int score) {
    // 打开游戏用户日志文件，如果不存在则创建
    FILE* logFile = fopen("userlog.txt", "a+");
    if (logFile == NULL) {
        printf("无法打开游戏用户日志文件。\n");
        return;
    }

    // 获取当前时间
    time_t endTime = time(NULL);
    // 计算游戏时长（秒）
    int playTime = (int)difftime(endTime, startTime);
    // 格式化开始时间和结束时间
    char startStr[20], endStr[20];
    strftime(startStr, sizeof(startStr), "%Y-%m-%d %H:%M:%S", localtime(&startTime));
    strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M:%S", localtime(&endTime));
    // 将日志信息写入文件
    fprintf(logFile, "用户ID：%d, 用户名：%s, 开始时间：%s, 结束时间：%s, 游戏时长：%d秒, 得分：%d\n", userID, username, startStr, endStr, playTime, score);
    fclose(logFile);
}


int main()
{
 int choice;

    do {
        printf("1. 注册\n");
        printf("2. 登录\n");
        printf("输入你的选择: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            default:
                printf("选择无效，请重试。\n");
        }
    } while(choice != 2);


    gamecircle();
    endgame();
    return 0;
}

