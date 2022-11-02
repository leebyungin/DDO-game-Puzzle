//#include"getch.h"     리눅스용
#include<stdio.h>
#include<conio.h>

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

int GAME_STATUS = 1;

/*struct position{
  int x,y;
  }typedef Position;*/

void InitScreen() {
    printf("\033[2J");
    printf("\033[1;1H");
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
    const int EscLines;
    const char* UIEscContents[3];
public:
    UI() :EscLines(3) {
        UIEscContents[0] = "Resume";
        UIEscContents[1] = "Settings";
        UIEscContents[2] = "Quit";
    }
    int GetEscLines() { return EscLines; }
    void Print(int cursor) {
        switch (GAME_STATUS) {
        case -1:
            for (int i = 0; i < EscLines; i++) {
                if (cursor == i)
                    printf("\033[45m");

                printf("%s\n", UIEscContents[i]);
                printf("\033[0m");
            }
            break;
        case -2:
            printf("not yet :p\n");
            break;
        }
    }
    void Select(int cursor) {
        if (GAME_STATUS == -1) {            //press ESC once menu
            switch (cursor) {
            case 0:
                GAME_STATUS = 1;
                break;
            case 1:
                GAME_STATUS = -2;
                break;
            case 2:
                GAME_STATUS = 0;
                break;
            }
        }
    }
};

UI ui;

class Map {
private:
    char map[MAP_Y][MAP_X];
    int RangeCheck(Position<int> a) {
        if (a.GetX() >= 0 && a.GetY() >= 0 && a.GetX() < MAP_X && a.GetY() < MAP_Y)
            return 1;
        return 0;
    }

public:
    Map() {
        for (int i = 0; i < MAP_Y; i++) {
            for (int j = 0; j < MAP_X; j++) {
                map[i][j] = 'x';
            }
        }
    }
    void Print(Position<int> pos) {
        int _x = pos.GetX();
        int _y = pos.GetY();

        for (int y = 0; y < MAP_Y; y++) {
            for (int x = 0; x < MAP_X; x++) {
                if (_x == x && _y == y) {
                    printf("\033[45m%c", map[y][x]);
                    printf("\033[0m");
                }
                else
                    printf("%c", map[y][x]);
            }
            printf("\n");
        }
    }

    void Change(Position<int> a) {
        if (RangeCheck(a)) {
            if (map[a.GetY()][a.GetX()] == 'x')
                map[a.GetY()][a.GetX()] = 'o';
            else if (map[a.GetY()][a.GetX()] == 'o')
                map[a.GetY()][a.GetX()] = 'x';
        }
    }

    char GetTileStatus(Position<int> a) {
        if (RangeCheck(a))
            return map[a.GetY()][a.GetX()];
        return NULL;
    }

};

Map map;

class Actor {
private:
public:
    virtual void mvUp() = 0;
    virtual void mvDown() = 0;
    virtual void mvLeft() = 0;
    virtual void mvRight() = 0;
    virtual void interact() = 0;
    virtual void Print() = 0;
    virtual void InitCursor() { return; }
};
//===========Actor's Childs==========
class UIActor :public Actor {
private:
    int cursor;
    int cursorLimit_ESC;
public:
    UIActor(UI& ui) :cursor(0) {
        cursorLimit_ESC = ui.GetEscLines();
    }
    //==for Command class==
    void mvUp() {
        cursor = RangeChecker<int>(cursor, -1, 0, cursorLimit_ESC);
    }
    void mvDown() {
        cursor = RangeChecker<int>(cursor, 1, 0, cursorLimit_ESC - 1);
    }
    void mvLeft() {
        return;
    }
    void mvRight() {
        return;
    }
    void interact() {
        ui.Select(cursor);
        return;
    }
    //==================
    void InitCursor() {
        cursor = 0;
    }
    void Print() {
        InitScreen();
        //printf("%d\n",cursor);
        ui.Print(cursor);
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

class Command {
public:
    virtual void execute(Actor& actor) = 0;
};

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
        GAME_STATUS = 0;
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
        if (GAME_STATUS == -1) actor.InitCursor();         //open ESC menu. init cursor pos
        if (GAME_STATUS < -1) {                     //current page: esc-Setting. this line must under "... InitCursor()" line
            GAME_STATUS = -1;
        }
        else
            GAME_STATUS *= -1;
    }
};
//==========================================


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

    Command* handleInput() {
        input = _getch();
        fflush(stdin);

        if (isPressed(W)) return buttonW_;
        if (isPressed(A)) return buttonA_;
        if (isPressed(S)) return buttonS_;
        if (isPressed(D)) return buttonD_;
        if (isPressed(ESC)) return buttonESC_;
        if (isPressed(SPACE)) return buttonSPACE_;
        return buttonNothing_;
    }
};

InputHandler input;

int main(void) {
    GameActor player;
    UIActor cursor(ui);
    while (GAME_STATUS) {
        if (GAME_STATUS > 0) {
            player.Print();
            input.handleInput()->execute(player);
        }
        if (GAME_STATUS < 0) {
            cursor.Print();
            input.handleInput()->execute(cursor);

        }
    }
}