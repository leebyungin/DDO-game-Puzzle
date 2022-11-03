//#define LINUX       //리눅스용
#define WINDOW

#ifdef LINUX
#include"getch.h"
#endif
#ifdef WINDOW
#include<conio.h>
#include<Windows.h>
#endif
#include<stdio.h>

#define SPACE 32
#define W 119           //w
#define A 97            //a
#define S 115           //s
#define D 100           //d
#define QUIT 113        //q
#define ESC 27          //esc
#define LATTER_GAP 32

#define MAP_X 3
#define MAP_Y 3

#define STATE_MENU 1
#define STATE_KEYSETTINGS 2
#define STATE_PLAYGAME -1
#define STATE_QUITGAME 0

class Actor {
private:
public:
    virtual void mvUp() = 0;
    virtual void mvDown() = 0;
    virtual void mvLeft() = 0;
    virtual void mvRight() = 0;
    virtual void interact() = 0;
    virtual void Print() = 0;
    virtual void SetCursor(int n) { return; }
};
class Command {
public:
    virtual void execute(Actor& actor) = 0;
};


class InputHandler {             //user modify key setting
private:
    Command* buttonW_;
    Command* buttonA_;
    Command* buttonS_;
    Command* buttonD_;
    Command* buttonESC_;
    Command* buttonSPACE_;
    Command* buttonNothing_;
    char input;
    char upKey = 'w';
    char downKey = 's';
    char leftKey = 'a';
    char rightKey = 'd';
    char interactKey = 32;
    char escKey = 27;

    int isPressed(char c) {
        if (input == c)
            return 1;
        return 0;
    }
public:
    InputHandler();
    void SetUpKey(char key);
    void SetDownKey(char key);
    void SetLeftKey(char key);
    void SetRightKey(char key);
    void SetInteractKey(char key);
    void setKeys();
    char GetInteractKey();

    Command*handleInput();
};

InputHandler input;

// ================================================
class GameStatus {
private:
    int StatusInfo[10];
    int GAME_STATUS;
public:
    GameStatus():GAME_STATUS(STATE_PLAYGAME){}
    int Get() { return GAME_STATUS; }
    void Set(int n) { GAME_STATUS = n; }
    void operator*= (int n) { GAME_STATUS *= n; }
};
GameStatus GAME_STATUS;

/*struct position{
  int x,y;
  }typedef Position;*/

void InitScreen() {
#ifdef LINUX            //윈도우 미지원
    printf("\033[2J");
    printf("\033[1;1H");
#endif
#ifdef WINDOW
    system("cls");
    printf("\033[1;1H");
#endif
}

template <class T>
class Position {
private:
    T x, y, temp;
    T BottomX, BottomY, TopX, TopY;

public:
    Position(T X, T Y, T BX, T TX, T BY, T TY) :x(X), y(Y), BottomX(BX), BottomY(BY), TopX(TX), TopY(TY) {}
    T GetX() { return x; }
    T GetY() { return y; }

    int MoveX(T mvX) {
        temp = x + mvX;
        if (temp >= BottomX) {
            if (temp <= TopX) {
                x = temp;
                return 0;
            }
            x = TopX;
            return 1;
        }
        x = BottomX;
        return -1;
    }
    int MoveY(T mvY) {
        temp = y + mvY;
        if (temp >= BottomY) {
            if (temp <= TopY) {
                y = temp;
                return 0;
            }
            y = TopY;
            return 1;
        }
        y = BottomY;
        return -1;
    }
    int MoveXY(T mvX, T mvY) {
        MoveX(mvX);
        MoveY(mvY);
    }
};

template <class T>
T RangeChecker(T num, T adder, T bottom, T top) {                   //change to multi use function( type Position )
    T temp = num + adder;
    if (temp <= top)
        if (temp >= bottom)
            return temp;
    return num;
}


class UI {
private:
    void UI_Clear() {
        printf("\033[2J");
        printf("\033[1;1H");
    }
    int limits;
    int setting;
    const int MenuLines;
    const int KeyLines;
    const char* UIEscContents[3];
    const char* UIKeys[5];

    char keys[5];
    char keysTemp[5];
    int idx;
public:
    UI() :MenuLines(3),KeyLines(5),limits(0),setting(0),idx(0) {
        UIEscContents[0] = "Resume";
        UIEscContents[1] = "Settings";
        UIEscContents[2] = "Quit";

        UIKeys[0] = "↑ :";
        UIKeys[1] = "↓ :";
        UIKeys[2] = "← :";
        UIKeys[3] = "→ :";
        UIKeys[4] = "Interact :";

        keysTemp[0] = keys[0]='w';
        keysTemp[1] = keys[1] ='s';
        keysTemp[2] = keys[2] ='a';
        keysTemp[3] = keys[3] ='d';
        keysTemp[4] = keys[4] =SPACE;

    }
    int RangeCheck(int cursor){
        if (setting == 1) { //key edit mode
            return 0;
        }
        if (cursor >= 0 && cursor < limits&&setting!=2)
            return 1;
        if (setting == 2 && cursor >= 0 && cursor<2) {
            return 1;
        }
        return 0;
    }
    int GetEscLines() { return MenuLines; }
    int GetSetting() {
        return setting;
    }
    void Print(int cursor) {
        switch (GAME_STATUS.Get()) {

        case STATE_MENU:                //menu page
            limits = MenuLines;

            for (int i = 0; i < MenuLines; i++) {
                if (cursor == i)
                    printf("\033[45m");
                printf("%s\n", UIEscContents[i]);
                printf("\033[0m");
            }
            break;

        case STATE_KEYSETTINGS:         //key setting page
            limits = KeyLines;
            for (int i = 0; i < KeyLines; i++) {
                if (cursor == i&&setting==0) {
                    printf("\033[45m");
                }
                printf("%s", UIKeys[i]);
                printf("\033[0m");

                if (setting == 1&&cursor==i) {
                    printf("\033[45m");
                    printf("%c → ",keys[i]);
                }
                printf("%c\n",keysTemp[i] );
                printf("\033[0m");
            }

            if (setting == 2) {
                printf("\n===fix===\n");
                printf("%c → %c\n", keys[idx], keysTemp[idx]);
                if(cursor==0)
                    printf("\033[45m");
                printf("yes");
                printf("\033[0m");
                printf("    ");
                if (cursor == 1)
                    printf("\033[45m");
                printf("no");
                printf("\033[0m");
            }
            break;

        }
    }
    void Select(int cursor) {

        if (GAME_STATUS.Get() == STATE_MENU) {            //now you on the menu

            switch (cursor) {
            case 0:
                GAME_STATUS.Set(STATE_PLAYGAME);
                break;
            case 1:
                GAME_STATUS.Set(STATE_KEYSETTINGS);
                break;
            case 2:
                GAME_STATUS.Set(STATE_QUITGAME);
                break;
            }
        }else if (GAME_STATUS.Get() == STATE_KEYSETTINGS) {      //now you on the key settings
           if (setting == 0) {
                setting = 1;
            }
        }

        if (setting == 1 && GAME_STATUS.Get() == STATE_KEYSETTINGS) {   //키 변경 중
            char temp = keys[cursor];
            idx = cursor;
             do{
                keysTemp[cursor] = temp;
                InitScreen();
                Print(cursor);
                temp = _getch();
             } while (temp != input.GetInteractKey());
            setting = 2;
        }else if (setting == 2) { //cursor값이 cursorX가 넘어와야 함
            Print(cursor);
            if (cursor == 0) {//설정
                keys[idx] = keysTemp[idx];
                input.SetUpKey(keys[0]);
                input.SetDownKey(keys[1]);
                input.SetLeftKey(keys[2]);
                input.SetRightKey(keys[3]);
                input.SetInteractKey(keys[4]);
            }
            if (cursor == 1) {//설정 취소
                keysTemp[idx] = keys[idx];
            }
            setting = 0;
        }
    }
    int STATECANCHANGE() {
        if (setting != 2)
            return 1;
    }
    void BreakAll() {
        setting = 0;
    }
};

UI ui;

class Map {
private:
    int map[MAP_Y][MAP_X];
    int RangeCheck(Position<int> a) {
        if (a.GetX() >= 0 && a.GetY() >= 0 && a.GetX() < MAP_X && a.GetY() < MAP_Y)
            return 1;
        return 0;
    }
    int RangeCheck(int x,int y) {
        if (x >= 0 && y >= 0 && x < MAP_X && y < MAP_Y)
            return 1;
        return 0;
    }

public:
    Map() {
        for (int i = 0; i < MAP_Y; i++) {
            for (int j = 0; j < MAP_X; j++) {
                map[i][j] = 0;
            }
        }
    }
    void Print(Position<int> pos) {
        int _x = pos.GetX();
        int _y = pos.GetY();

        for (int y = 0; y < MAP_Y; y++) {
            for (int x = 0; x < MAP_X; x++) {
                if (_x == x && _y == y) {
                    printf("\033[45m%");
                    if(map[y][x]==0)
                        printf("□");
                    if(map[y][x]==1)
                        printf("■");
                    printf("\033[0m");
                }
                else {
                    //printf("%c", map[y][x]);
                    if (map[y][x] == 0)
                        printf("□");
                    if (map[y][x] == 1)
                        printf("■");
                }
            }
            printf("\n");
        }
    }

    void Reverse(int x,int y) {
        if (RangeCheck(x,y)) {
            if (map[y][x] == 0)
                map[y][x] = 1;
            else if (map[y][x] == 1)
                map[y][x] = 0;
        }
    }
    void Change(Position<int>a) {
        Reverse(a.GetX(),a.GetY());
        Reverse(a.GetX() - 1, a.GetY());
        Reverse(a.GetX()+1, a.GetY());
        Reverse(a.GetX(), a.GetY()-1);
        Reverse(a.GetX(), a.GetY()+1);
    }

    char GetTileStatus(Position<int> a) {
        if (RangeCheck(a))
            return map[a.GetY()][a.GetX()];
        return NULL;
    }

};

Map map;

/*class Actor {
private:
public:
    virtual void mvUp() = 0;
    virtual void mvDown() = 0;
    virtual void mvLeft() = 0;
    virtual void mvRight() = 0;
    virtual void interact() = 0;
    virtual void Print() = 0;
    virtual void SetCursor(int n) { return; }
};*/
//===========Actor's Childs==========
class UIActor :public Actor {
private:
    int cursor;
    int cursorX;
public:
    UIActor() :cursor(0),cursorX(0) {
    }
    //==for Command class==
    void mvUp() {
        //cursor = RangeChecker<int>(cursor, -1, 0, cursorLimit_ESC);
        if(ui.RangeCheck(cursor-1)) //it can move?
            cursor--;
    }
    void mvDown() {
        //cursor = RangeChecker<int>(cursor, 1, 0, cursorLimit_ESC - 1);
        if (ui.RangeCheck(cursor + 1))  //it can move?
            cursor++;
    }
    void mvLeft() {
        if (ui.RangeCheck(cursorX - 1))
            cursorX--;
        return;
    }
    void mvRight() {
        if (ui.RangeCheck(cursorX +1))
            cursorX++;
        return;
    }
    void interact() {           //=========================방금 변경한 것
        if (ui.GetSetting() != 2)
            ui.Select(cursor);
        else if (ui.GetSetting() == 2)
            ui.Select(cursorX);
        return;
    }
    //==================
    void SetCursor(int n) {
        cursor = n;
    }
    void Print() {
        InitScreen();
        //printf("%d\n",cursor);
        if (ui.GetSetting() != 2)
            ui.Print(cursor);
        else if (ui.GetSetting() == 2)
            ui.Print(cursorX);
    }
};

class GameActor :public Actor {
private:
    Position<int>position;
public:
    GameActor() :position(0, 0, 0, MAP_X - 1, 0, MAP_Y - 1) {}
    //==for Command class==
    void mvUp() {
        position.MoveY(-1);
    }
    void mvDown() {
        position.MoveY(1);
    }
    void mvLeft() {
        position.MoveX(-1);
    }
    void mvRight() {
        position.MoveX(1);
    }
    void interact() {
        map.Change(position);
    }
    //=====================
    void Print() {
        InitScreen();
        //printf("x: %d y: %d\n",position.GetX(),position.GetY());
        map.Print(position);
    }
    const Position<int> GetPosition() {
        return position;
    }
};
//=======================================

/*class Command {
public:
    virtual void execute(Actor& actor) = 0;
};*/

//=============Command's Childs========
/*class Command_UI{
  public:
  virtual void open(UI& ui)=0;
  };*/

class RightCommand : public Command {
public:
    virtual void execute(Actor& actor) { actor.mvRight(); }
};
class LeftCommand :public Command {
public:
    virtual void execute(Actor& actor) { actor.mvLeft(); }
};
class UpCommand :public Command {
public:
    virtual void execute(Actor& actor) { actor.mvUp(); }
};
class DownCommand :public Command {
public:
    virtual void execute(Actor& actor) { actor.mvDown(); }
};
class InteractCommand :public Command {
public:
    virtual void execute(Actor& actor) {
        actor.interact();
        //map.Change(actor.GetPosition());              //
    }
};
class quitCommand :public Command {
public:
    virtual void execute(Actor& actor) {
        GAME_STATUS.Set(0);
    }
};
class EmptyCommand :public Command {
public:
    virtual void execute(Actor& actor) {
        return;
    }
};

class ESCCommand :public Command {
public:
    virtual void execute(Actor& actor) {
        if (ui.STATECANCHANGE()) {
            if (GAME_STATUS.Get() == STATE_MENU) actor.SetCursor(0);  //when ESC menu opend, init cursor pos

            if (GAME_STATUS.Get() == STATE_KEYSETTINGS) {                     //current page: keySetting. this line must under "... InitCursor()" line
                GAME_STATUS.Set(STATE_MENU);
                ui.BreakAll();
                actor.SetCursor(1);
            }
            else
                GAME_STATUS *= -1;
        }
    }
};
//==========================================


/*class InputHandler {             //user modify key setting
private:
    Command* buttonW_;
    Command* buttonA_;
    Command* buttonS_;
    Command* buttonD_;
    Command* buttonESC_;
    Command* buttonSPACE_;
    Command* buttonNothing_;
    char input;

    char upKey='w';
    char downKey = 's';
    char leftKey = 'a';
    char rightKey = 'd';
    char interactKey = 32;
    char escKey = 27;

    int isPressed(char c) {
        if (input == c)
            return 1;
        return 0;
    }
public:
    InputHandler() {
        buttonW_ = new(UpCommand);
        buttonA_ = new(LeftCommand);
        buttonS_ = new(DownCommand);
        buttonD_ = new(RightCommand);
        buttonSPACE_ = new(InteractCommand);
        buttonNothing_ = new(EmptyCommand);

        buttonESC_ = new(ESCCommand);
    }
    void SetUpKey(char key) { upKey = key; }
    void SetDownKey(char key) { downKey = key; }
    void SetLeftKey(char key) { leftKey = key; }
    void SetRightKey(char key) { rightKey = key; }
    void SetInteractKey(char key) { interactKey = key; }
    void setKeys() {
    }

    Command* handleInput() {
#ifdef LINUX
        input = getch();
#endif
#ifdef WINDOW
        input = _getch();
#endif

        fflush(stdin);

        if (isPressed(upKey)) return buttonW_;
        if (isPressed(leftKey)) return buttonA_;
        if (isPressed(downKey)) return buttonS_;
        if (isPressed(rightKey)) return buttonD_;
        if (isPressed(escKey)) return buttonESC_;
        if (isPressed(interactKey)) return buttonSPACE_;
        return buttonNothing_;
    }
};
InputHandler input;
*/
//=================================================================추가부


InputHandler::InputHandler() {
        buttonW_ = new(UpCommand);
        buttonA_ = new(LeftCommand);
        buttonS_ = new(DownCommand);
        buttonD_ = new(RightCommand);
        buttonSPACE_ = new(InteractCommand);
        buttonNothing_ = new(EmptyCommand);

        buttonESC_ = new(ESCCommand);
    }
    void InputHandler::SetUpKey(char key) { upKey = key; }
    void InputHandler::SetDownKey(char key) { downKey = key; }
    void InputHandler::SetLeftKey(char key) { leftKey = key; }
    void InputHandler::SetRightKey(char key) { rightKey = key; }
    void InputHandler::SetInteractKey(char key) { interactKey = key; }
    void InputHandler::setKeys() {
    }
    char InputHandler::GetInteractKey() {
        return interactKey;
    }

    Command* InputHandler::handleInput() {
#ifdef LINUX
        input = getch();
#endif
#ifdef WINDOW
        input = _getch();
#endif

        fflush(stdin);

        if (isPressed(upKey)) return buttonW_;
        if (isPressed(leftKey)) return buttonA_;
        if (isPressed(downKey)) return buttonS_;
        if (isPressed(rightKey)) return buttonD_;
        if (isPressed(escKey)) return buttonESC_;
        if (isPressed(interactKey)) return buttonSPACE_;
        return buttonNothing_;
    }
// ====================================================================여까지




int main(void) {
    GameActor player;
    UIActor cursor;
    printf("게임 규칙: 화면에 나타나는 9개의 □를 ■으로 바꿔야 합니다.\n이동키: W,A,S,D\n상호작용: SPACE\n설정: ESC\n\n\n시작하려면 상호작용 키를 누르세요");
    do {} while (_getch() != SPACE);
    while (GAME_STATUS.Get()) {
        if (GAME_STATUS.Get()==STATE_PLAYGAME) {
            player.Print();
            input.handleInput()->execute(player);
        }
        if (GAME_STATUS.Get()>=STATE_MENU) {
            cursor.Print();
            input.handleInput()->execute(cursor);

        }
    }
}