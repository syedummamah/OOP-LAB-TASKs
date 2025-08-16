#include <iostream>
#include <cstdlib> // for clearing console
using namespace std;

struct Position {
    int x = 0;
    int y = 0;

    void setValues(int xPos, int yPos) {
        x = xPos;
        y = yPos;
    }

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class GameObject {
protected:
    Position position;
public:
    GameObject(int x, int y) { position.setValues(x, y); }
    virtual ~GameObject() {}
    virtual char getDisplayChar() const = 0;
    virtual void handleCollision() const = 0;
    Position& getPosition() { return position; }  
};

class PlayerCar : public GameObject {
public:
    PlayerCar(int x, int y) : GameObject(x, y) {}
    char getDisplayChar() const override { return 'C'; }
    void handleCollision() const override {
        cout << "Collision with an obstacle!"<<endl;
    }
};

class FuelStation : public GameObject {
public:
    FuelStation(int x, int y) : GameObject(x, y) {}
    char getDisplayChar() const override { return 'F'; }
    void handleCollision() const override {
        cout << "Fuel station collision-refueling"<<endl;
    }
};

class Obstacle : public GameObject {
public:
    Obstacle(int x, int y) : GameObject(x, y) {}
    virtual ~Obstacle() {}
};

class Tree : public Obstacle {
public:
    Tree(int x, int y) : Obstacle(x, y) {}
    char getDisplayChar() const override { return 'T'; }
    void handleCollision() const override {
        cout << "Tree collision-damage!"<<endl;
    }
};

class Building : public Obstacle {
public:
    Building(int x, int y) : Obstacle(x, y) {}
    char getDisplayChar() const override { return 'B'; }
    void handleCollision() const override {
        cout << "Building collision-game over!"<<endl;
        exit(0);
    }
};

class Game {
private:
    int numRows, numCols;
    GameObject*** grid;       // why 3d? Understand it by yourself
    Position buildings[5][21];
    Position trees[7];
    Position fuelStations[4];
    PlayerCar* playerCar;

    void clearGrid() {
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                grid[i][j] = nullptr;
            }
        }
    }

    void placeGameObjects() {
        //buildings
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 21; j++) {
                int x = buildings[i][j].x;
                int y = buildings[i][j].y;
                if (x >= 0 && x < numRows && y >= 0 && y < numCols) {
                    grid[x][y] = new Building(x, y);
                }
            }
        }

        //  trees
        for (int i = 0; i < 7; i++) {
            int x = trees[i].x;
            int y = trees[i].y;
            if (x >= 0 && x < numRows && y >= 0 && y < numCols) {
                grid[x][y] = new Tree(x, y);
            }
        }

        // fuel stations
        for (int i = 0; i < 4; i++) {
            int x = fuelStations[i].x;
            int y = fuelStations[i].y;
            if (x >= 0 && x < numRows && y >= 0 && y < numCols) {
                grid[x][y] = new FuelStation(x, y);
            }
        }

        // player car
        int x = playerCar->getPosition().x;
        int y = playerCar->getPosition().y;
        grid[x][y] = playerCar;
    }

public:
    Game(int r, int c) : numRows(r), numCols(c) {
        grid = new GameObject**[numRows];
        for (int i = 0; i < numRows; i++) {
            grid[i] = new GameObject*[numCols];
            for (int j = 0; j < numCols; j++) {
                grid[i][j] = nullptr;
            }
        }

        setBuildingIndexes();
        setTreeIndexes();
        setFuelStationIndexes();
        
        playerCar = new PlayerCar(1, 1);
        placeGameObjects();
    }

    ~Game() {
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (grid[i][j] != playerCar) {
                    delete grid[i][j];
                }
            }
            delete[] grid[i];
        }
        delete[] grid;
        delete playerCar;
    }

    void displayGrid() {
        system("clear"); 
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (grid[i][j] != nullptr) {
                    cout << grid[i][j]->getDisplayChar();
                } else {
                    cout << '.';
                }
                cout << ' ';
            }
            cout << endl;
        }
    }

    void movePlayer(int dx, int dy) {
        Position& current = playerCar->getPosition();
        int newX = current.x + dx;
        int newY = current.y + dy;

        // Checking boundaries
        if (newX < 0 || newX >= numRows || newY < 0 || newY >= numCols) {
            return;
        }

        // Check for collision
        if (grid[newX][newY] != nullptr) {
            grid[newX][newY]->handleCollision();
            return;
        }

        // Moving car
        grid[current.x][current.y] = nullptr;
        current.setValues(newX, newY);
        grid[newX][newY] = playerCar;
    }

    void setBuildingIndexes() {
        int startX = 5;
        int startY = 5;
        for (int i = 0; i < 5; i++) {
            int x = startX;
            int y = startY;
            for (int j = 0; j < 21; j++) {
                buildings[i][j].setValues(x, y);
                y++;
                if ((j + 1) % 7 == 0) {
                    y = startY;
                    x += 1;
                }
            }
            startX += 5;
            startY += 8;
            if (i == 2) {
                startY = 1;
            }
        }
    }

    void setTreeIndexes() {
        trees[0].setValues(2, 20);
        trees[1].setValues(25, 20);
        trees[2].setValues(15, 2);
        trees[3].setValues(18, 17);
        trees[4].setValues(3, 3);
        trees[5].setValues(8, 18);
        trees[6].setValues(28, 2);
    }

    void setFuelStationIndexes() {
        fuelStations[0].setValues(2, 27);
        fuelStations[1].setValues(20, 15);
        fuelStations[2].setValues(28, 19);
        fuelStations[3].setValues(25, 6);
    }
};

int main() {
    Game game(30, 30); // DMA allocated game objects 30x30
    
    char input;
    do {
        game.displayGrid();
        cout << "Moving using WASD keys (Q to quit): ";
        cin >> input;
        
        switch (tolower(input)) {
            case 'w': game.movePlayer(-1, 0); break;
            case 'a': game.movePlayer(0, -1); break;
            case 's': game.movePlayer(1, 0); break;
            case 'd': game.movePlayer(0, 1); break;
        }
    } while (input != 'q' && input != 'Q');

    return 0;
}
