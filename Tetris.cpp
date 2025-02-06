#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "sys/ioctl.h"
#include "termios.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include <unistd.h>
#include <cstdlib>


 
class tetramino
{
public:
  std::pair<int, int> coord;
  sf::Color color;
  int selector{0};
  void rotate()
  {
    selector++;
  };
  virtual std::vector<std::vector<int>> getShape() { return std::vector<std::vector<int>>{{1, 1}, {1, 1}}; };
  void update_tetramino_coordinates(std::array<int, 2> dxdy)
  {
    std::pair<int, int> new_coord(coord.first + dxdy[0], coord.second + dxdy[1]);
    coord = new_coord;
  }
};

class board
{
public:
  int nx;
  int ny;
  int taille_cases;
  std::vector<sf::Color> bg;
  sf::RenderWindow window;
  board(int init_nx, int init_ny, int t) : nx{init_nx}, ny{init_ny}, taille_cases{t},
                                           window(sf::VideoMode(init_nx * t, init_ny * t), "TETRIS")
  {
    bg.resize(nx * ny, sf::Color(0, 0, 0));
  }
  void add_tetramino(tetramino &t)
  {
    int x = t.coord.first;
    int y = t.coord.second;
    for (int i = 0; i < t.getShape().size(); i++)
    {
      for (int j = 0; j < t.getShape()[0].size(); j++)
      {
        if (t.getShape()[i][j])
        {
          bg[(y + i) * nx + (x + j)] = t.color;
        }
      }
    }
  }
  void remove_tetramino(tetramino &t)
  {
    int x = t.coord.first;
    int y = t.coord.second;
    for (int i = 0; i < t.getShape().size(); i++)
    {
      for (int j = 0; j < t.getShape()[0].size(); j++)
      {
        if (t.getShape()[i][j])
        {
          bg[(y + i) * nx + (x + j)] = sf::Color(0, 0, 0);
        }
      }
    }
  }
  void printFrame()
  {
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        sf::RectangleShape cell(sf::Vector2f(taille_cases * 0.96, taille_cases * 0.96));
        cell.setPosition(static_cast<float>(i * taille_cases), static_cast<float>(j * taille_cases));
        cell.setFillColor(bg[j * nx + i]);
        window.draw(cell);
      }
    }
  }

  void clearboard()
  {
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        bg[j * nx + i] = sf::Color(0, 0, 0);
      }
    }
  }
  int clearline()
  {
    int lines = 0;
    for (int i = 0; i < ny; i++)
    {
      bool clear = true;
      for (int j = 0; j < nx; j++)
      {
        clear = (clear && (bg[i * nx + j] != sf::Color(0, 0, 0)));
      }
      if (clear)
      {
        lines++;
        for (int l = i; l > 0; l--)
        {
          for (int j = 0; j < nx; j++)
          {
            bg[l * nx + j] = bg[(l - 1) * nx + j];
          }
        }
      }
    }
    return lines;
  }
};

bool canMove(tetramino &t, board &b, int dx, int dy)
{
  for (int i = 0; i < t.getShape().size(); i++)
  {
    for (int j = 0; j < t.getShape()[0].size(); j++)
    {
      if (t.getShape()[i][j] == 1)
      {
        int newX = t.coord.first + j + dx;
        int newY = t.coord.second + i + dy;
        if (newX < 0 || newX >= b.nx || newY < 0 || newY >= b.ny)
          return false;
        if (b.bg[newY * b.nx + newX] != sf::Color(0, 0, 0))
          return false;
      }
    }
  }
  return true;
}

void movement(tetramino &t, board &b, float &acc, float interval)
{
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && acc >= interval)
    if (canMove(t, b, -1, 0))
    {
      t.update_tetramino_coordinates({-1, 0});
      acc = 0;
    }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && acc >= interval)
    if (canMove(t, b, +1, 0))
    {
      t.update_tetramino_coordinates({+1, 0});
      acc = 0;
    }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    if (canMove(t, b, 0, +1))
      t.update_tetramino_coordinates({0, +1});
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && acc >= interval * 3)
  {
    t.rotate();
    if (!canMove(t, b, 0, 0))
    {
      t.selector -= 1;
    }
    acc = 0;
  }
}

class o : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{1, 1},
       {1, 1}}};
  o(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(255, 255, 0);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class i : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{1},
       {1},
       {1},
       {1}},
      {{1, 1, 1, 1}}};
  i(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(0, 255, 255);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class s : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{0, 1, 1},
       {1, 1, 0}},
      {{1, 0},
       {1, 1},
       {0, 1}}};
  s(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(57, 255, 20);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class z : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{1, 1, 0},
       {0, 1, 1}},
      {{0, 1},
       {1, 1},
       {1, 0}}};
  z(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(255, 0, 0);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class l : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{1, 0},
       {1, 0},
       {1, 1}},
      {{0, 0, 1},
       {1, 1, 1}},
      {{1, 1},
       {0, 1},
       {0, 1}},
      {{1, 1, 1},
       {1, 0, 0}}};
  l(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(255, 165, 0);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class j : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{0, 1},
       {0, 1},
       {1, 1}},
      {{1, 1, 1},
       {0, 0, 1}},
      {{1, 1},
       {1, 0},
       {1, 0}},
      {{1, 0, 0},
       {1, 1, 1}}};
  j(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(30, 144, 255);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

class t : public tetramino
{
public:
  std::vector<std::vector<std::vector<int>>> shapes = {
      {{1, 1, 1},
       {0, 1, 0}},
      {{0, 1},
       {1, 1},
       {0, 1}},
      {{0, 1, 0},
       {1, 1, 1}},
      {{1, 0},
       {1, 1},
       {1, 0}}};
  t(std::pair<int, int> init_coord)
  {
    coord = init_coord;
    color = sf::Color(255, 20, 147);
  }
  std::vector<std::vector<int>> getShape()
  {
    return shapes[selector % shapes.size()];
  }
};

tetramino *createRandomPiece(std::pair<int, int> spawnCoord)
{
  int r = rand() % 7;
  switch (r)
  {
  case 0:
    return new o(spawnCoord);
  case 1:
    return new i(spawnCoord);
  case 2:
    return new s(spawnCoord);
  case 3:
    return new z(spawnCoord);
  case 4:
    return new t(spawnCoord);
  case 5:
    return new l(spawnCoord);
  case 6:
    return new j(spawnCoord);
  }
  return new o(spawnCoord);
}

int main()
{  system("mpg123 Tetris99_quiet.mp3 &"); 


  const int nx = 18;
  const int ny = 30;
  const int taille = 15;
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  board b(nx, ny, taille);

  b.window.setFramerateLimit(35);
  b.window.setVerticalSyncEnabled(true);

  int score = 0;
  int highscore = 0;
  sf::Font font;
  font.loadFromFile("Poppins-Regular.ttf");
  sf::Text scoreText;
  scoreText.setFont(font);
  scoreText.setString("Score 0");
  scoreText.setCharacterSize(24);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(10, 12);

  sf::Text highscoreText;
  highscoreText.setFont(font);
  highscoreText.setString("HighScore 0");
  highscoreText.setCharacterSize(24);
  highscoreText.setFillColor(sf::Color::Yellow);
  highscoreText.setPosition(10, 36);

  auto next = createRandomPiece({nx - 3, 1});
  auto t = createRandomPiece({nx / 2, 0});

  sf::Clock clock;
  float accumulator = 0.0;
  float dropInterval = 0.5;
  float moveaccumulator = 0.0;
  const float moveinterval = 0.06;

  while (b.window.isOpen())
  {
    sf::Event event;
    while (b.window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed){
        b.window.close();
        system("pkill mpg123");
        return 0;}
    }
    float dt = clock.restart().asSeconds();
    accumulator += dt;
    moveaccumulator += dt;
    b.remove_tetramino(*t);
    movement(*t, b, moveaccumulator, moveinterval);
    if (accumulator >= dropInterval)
    {
      if (canMove(*t, b, 0, +1))
      {
        t->update_tetramino_coordinates({0, +1});
      }
      else
      {
        if (t->coord.second == 0)
        {

          b.clearboard();
          scoreText.setPosition(4 * taille, 12 * taille);
          scoreText.setString("T'as perdu ?");
          highscoreText.setPosition(4 * taille, 15 * taille);
          highscoreText.setString("arrete !");

          b.window.clear();
          b.printFrame();
          b.window.draw(scoreText);
          b.window.draw(highscoreText);
          b.window.display();

          sleep(2);
          scoreText.setPosition(10, 12);
          highscoreText.setPosition(10, 36);
          if (score > highscore)
          {
            highscore = score;
          }
          score = 0;

          delete t;
          t = next;
          b.remove_tetramino(*t);
          t->coord = {nx / 2, 0};
          next = createRandomPiece({nx - 3, 1});
          int lines = b.clearline();
          dropInterval = 0.5;
          score += lines * lines * 100;
          scoreText.setString("Score " + std::to_string(score));
          highscoreText.setString("HighScore " + std::to_string(highscore));
        }
        else
        {
          b.add_tetramino(*t);
          delete t;
          t = next;
          b.remove_tetramino(*t);
          t->coord = {nx / 2, 0};
          next = createRandomPiece({nx - 3, 1});
          int lines = b.clearline();
          score += lines * lines * 100;
          scoreText.setString("Score " + std::to_string(score));
          dropInterval = 0.01 * (5000 / (score + 100)) + 0.05;
        }
      }
      accumulator = 0.0;
    }
    b.add_tetramino(*t);
    b.add_tetramino(*next);
    b.window.clear();
    b.printFrame();
    b.window.draw(scoreText);
    b.window.draw(highscoreText);
    b.window.display();
  }
  delete t;
  return 0;
}
