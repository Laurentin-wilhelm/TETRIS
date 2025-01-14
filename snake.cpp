#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <thread>
#include <SFML/Graphics.hpp>
#include "sys/ioctl.h"
#include "termios.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"


class tetramino
{public:
  std::string name;
  std::vector<std::pair<int, int>> v_snake;
  virtual void rotate(){};
 

};

class board
{public:
  int nx;
  int ny;
  int taille_cases;
  std::vector<int> bg;
  sf::RenderWindow window;

  board(int init_nx, int init_ny, int taille_cases) : nx{init_nx}, ny{init_ny},taille_cases{taille_cases}, 
      window(sf::VideoMode(init_nx * taille_cases, init_ny * taille_cases), "Exemple : grille SFML")
  {
  window.setFramerateLimit(120);
  bg.resize(nx * ny, 0);
  int i;
  for (i = 0; i < nx; i++)
  {
    bg[i] = 1;
  }
  for (i = 0; i < nx; i++)
  {
    bg[(ny - 1) * nx + i] = 1;
  }
  for (i = 0; i < ny; i++)
  {
    bg[i * nx] = 1;
  }
  for (i = 0; i < ny; i++)
  {
    bg[i * nx + nx - 1] = 1;
  }
  
  

  }

  void add_snake(snake &s)
{
  for(auto &xy :s.v_snake)
  {int x = xy.first;
  int y = xy.second;
  bg[y*nx+x] = 3;
  }
}

void remove_snake(snake &s)
{
 for(auto &xy :s.v_snake)
  {int x = xy.first;
  int y = xy.second;
  bg[y*nx+x] = 0;
  }
}

bool verifyBorder(snake &s)
{
  
  std::pair<int, int> head = s.v_snake[0];
  int snake_len = s.snake_len;
  
    int x = head.first;
    int y = head.second;

    if (x == 0 || y == 0 || x == nx - 1 || y == ny - 1) return false;

    for (int i = 1; i < snake_len-1; i++) {
        if (head == s.v_snake[i]) return false;
    }

    return true;
}
void printFrame()
  {
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {

        sf::RectangleShape cell(sf::Vector2f(taille_cases, taille_cases));
        cell.setPosition(static_cast<float>(i * taille_cases),static_cast<float>(j * taille_cases));
        if (bg[i + j * nx] == 1)
        {
          cell.setFillColor(sf::Color::Black);
        }
        else if (bg[i + j * nx] == 2)
        {
        
        }
        else if (bg[i + j * nx] == 3)
        {
     
        }
        else
        {
          cell.setFillColor(sf::Color::Grey);
        }
        window.draw(cell);
      }
      
    }
  }

 
  

};




namespace internal
{
  // 💀💀💀💀💀💀💀💀 DON'T TOUCH THIS BLOCK 💀💀💀💀💀💀💀💀💀 ///
  int keyEvent();
  void frameSleep(const int &ms);
  const char *cmd_clear = "clear";
  void backgroundClear();
  // 💀💀💀💀💀💀💀💀 DON'T TOUCH THIS BLOCK 💀💀💀💀💀💀💀💀💀 ///
}

std::array<int, 2> snake_movement(char key)
{ 
  std::array<int, 2> u;
  if(key =='z')
  {
    u[0]=0;
    u[1]=-1;
  }
    if(key =='q')
  {
    u[0]=-1;
    u[1]=0;
  }
    if(key =='s')
  {
    u[0]=0;
    u[1]=1;
  }
    if(key =='d')
  {
    u[0]=1;
    u[1]=0;
  }
  return u;
}







float lap = 200;
void startGame( float &lap, snake &s, board &b)
{
  char key;
  std::array<int, 2> dxdy = {1, 0};
  
  while (b.window.isOpen())
  {
    sf::Event event;
    while (b.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            b.window.close();
    }
    internal::frameSleep(lap);
    if (internal::keyEvent())
    {
      std::cin >> key;
      dxdy = snake_movement(key);
    }
    internal::backgroundClear();
    b.add_snake(s);
    b.printFrame();
    b.window.display();
    b.remove_snake(s);
    bool out = b.verifyBorder(s);
    if (out == false)
    {
      std::cerr << "Perdu💀💀" << std::endl;
      exit(1);
    }
    
    s.update_snake_coordinates(dxdy);
    lap = lap * 0.995;
  }
}

int main()
{
  const int nx = 25;
  const int ny = 50;
  const int taille = 15;
  int snake_len = 3;
  snake s(snake_len);


  board b(nx,ny,taille);
  b.printFrame();

  startGame(lap, s, b);
  return 0;
}
// 💀💀💀💀💀💀💀💀 DON'T TOUCH CODE AFTER THIS LINE 💀💀💀💀💀💀💀💀💀💀 ///

namespace internal
{

  static const int STDIN = 0;
  static bool initialized = false;

  // Comment ca va

  int keyEvent()
  {
    if (!initialized)
    {
      termios term;
      tcgetattr(STDIN, &term);
      term.c_lflag &= ~ICANON;
      tcsetattr(STDIN, TCSANOW, &term);
      setbuf(stdin, NULL);
      initialized = true;
    }
    int bytesWaiting;
    // int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
  }

  void frameSleep(const int &ms)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  /////////////

  void backgroundClear()
  {
    int out = system(cmd_clear);
    if (out != 0)
    {
      std::cerr << "clear command failed" << std::endl;
      exit(1);
    }
  }

  

  

}
