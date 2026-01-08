#include <ncurses.h> 
#include <unistd.h> 
#include <vector> 
#include <cstdlib> 
#include <ctime> 
#include <algorithm>

using namespace std;

// Directions 
enum Direction { UP, DOWN, LEFT, RIGHT };

// ---------------- Snake ---------------- 
class Snake { private: vector<pair<int,int>> body; // (y, x)
Direction dir;

public: Snake(int startY, int startX) { body.push_back({startY, startX}); body.push_back({startY, startX - 1}); body.push_back({startY, startX - 2}); dir = RIGHT; }

void setDirection(Direction d) {
    // Prevent reversing direction
    if ((dir == UP && d == DOWN) || (dir == DOWN && d == UP) ||
        (dir == LEFT && d == RIGHT) || (dir == RIGHT && d == LEFT))
        return;
    dir = d;
}

pair<int,int> getHead() const {
    return body.front();
}

const vector<pair<int,int>>& getBody() const {
    return body;
}

void move(bool grow = false) {
    pair<int,int> head = body.front();

    switch (dir) {
        case UP:    head.first--; break;
        case DOWN:  head.first++; break;
        case LEFT:  head.second--; break;
        case RIGHT: head.second++; break;
    }

    body.insert(body.begin(), head);
    if (!grow) body.pop_back();
}

bool selfCollision() const {
    pair<int,int> head = body.front();
    for (size_t i = 1; i < body.size(); i++) {
        if (body[i] == head) return true;
    }
    return false;
}

};

// ---------------- Food ---------------- 
class Food { private: int y, x;

public: Food() : y(0), x(0) {}

void spawn(int height, int width, const vector<pair<int,int>>& snakeBody) {
    while (true) {
        y = rand() % (height - 2) + 1;
        x = rand() % (width - 2) + 1;

        if (find(snakeBody.begin(), snakeBody.end(), make_pair(y, x)) == snakeBody.end())
            break;
    }
}

int getY() const { return y; }
int getX() const { return x; }

};

// ---------------- Game ---------------- 
class Game { private: int height, width; int score; bool gameOver;

Snake snake;
Food food;

public: Game(int h, int w) : height(h), width(w), score(0), gameOver(false), snake(h/2, w/2) { food.spawn(height, width, snake.getBody()); }

void drawBorder() {
    for (int x = 0; x < width; x++) {
        mvprintw(0, x, "#");
        mvprintw(height-1, x, "#");
    }
    for (int y = 0; y < height; y++) {
        mvprintw(y, 0, "#");
        mvprintw(y, width-1, "#");
    }
}

void handleInput() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
        case 'w': snake.setDirection(UP); break;
        case KEY_DOWN:
        case 's': snake.setDirection(DOWN); break;
        case KEY_LEFT:
        case 'a': snake.setDirection(LEFT); break;
        case KEY_RIGHT:
        case 'd': snake.setDirection(RIGHT); break;
        default: break;
    }
}

void update() {
    pair<int,int> head = snake.getHead();
    bool grow = false;

    // Check food collision
    if (head.first == food.getY() && head.second == food.getX()) {
        score += 10;
        grow = true;
        food.spawn(height, width, snake.getBody());
    }

    snake.move(grow);
    head = snake.getHead();

    // Wall collision
    if (head.first <= 0 || head.first >= height-1 ||
        head.second <= 0 || head.second >= width-1) {
        gameOver = true;
    }

    // Self collision
    if (snake.selfCollision()) {
        gameOver = true;
    }
}

void draw() {
    clear();
    drawBorder();

    // Draw snake
    for (auto &p : snake.getBody()) {
        mvprintw(p.first, p.second, "O");
    }

    // Draw food
    mvprintw(food.getY(), food.getX(), "*");

    mvprintw(height, 2, "Score: %d", score);
    refresh();
}

void gameOverScreen() {
    clear();
    mvprintw(height/2 - 1, width/2 - 5, "GAME OVER");
    mvprintw(height/2, width/2 - 7, "Final Score: %d", score);
    mvprintw(height/2 + 2, width/2 - 14, "Press R to Restart or Q to Quit");
    refresh();

    nodelay(stdscr, FALSE);
    while (true) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == 'r' || ch == 'R') {
            score = 0;
            gameOver = false;
            snake = Snake(height/2, width/2);
            food.spawn(height, width, snake.getBody());
            nodelay(stdscr, TRUE);
            run();
            return;
        }
    }
}

void run() {
    while (!gameOver) {
        handleInput();
        update();
        draw();
        usleep(120000);
    }
    gameOverScreen();
}

};

// ---------------- main ---------------- 
int main() { srand(time(NULL));

initscr();
noecho();
curs_set(0);
keypad(stdscr, TRUE);
nodelay(stdscr, TRUE);

int height = 25;
int width = 60;

Game game(height, width);
game.run();

endwin();
return 0;

}
