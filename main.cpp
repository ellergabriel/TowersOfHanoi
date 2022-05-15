#include <iostream>
#include <vector>
#include <stack>
using namespace std;

//========
// Gabriel Eller 
//========

#define NUM_RINGS  3
#define NUM_PEGS  3
#define L_WEIGHT  2.5
#define M_WEIGHT  1
#define S_WEIGHT  .5

string S_STRING = "  S    ";
string M_STRING = " MMM   ";
string L_STRING = "LLLLL  ";
//#define TOTAL_WEIGHT 4

float convertChar(char c){
  switch(c){
    case 'S':
      return S_WEIGHT;
    case 'M':
      return M_WEIGHT;
    case 'L':
      return L_WEIGHT;
    default:
      return 0;
  }
}

//helper class to store each state of the puzzle
class State{
  public:
    State(){
      pegs.push_back(vector<char> (3, 0));
      pegs.push_back(vector<char> (3, 0));
      pegs.push_back(vector<char> (3, 0));
      pegs[0].push_back('S');
      pegs[0].push_back('M');
      pegs[0].push_back('L');
    }

    State(State* origin){
      pegs = origin->pegs;
      g = origin->g;
    }

    bool isLegalMove(int dest, char disk){
      return (convertChar(disk) < convertChar(pegs[dest].front()));
    }

    bool moveDisk(int origin, int dest, char disk){
      if(isLegalMove(dest, disk)){
        pegs[origin].erase(pegs[origin].begin());
        pegs[dest].emplace(pegs[dest].begin(), disk);
        g++;
        return true;
      }
      return false;
    }

    float h(){
      float val = 0;
      for(int i = 0; i < pegs[NUM_PEGS - 1].size(); i++){
        val += convertChar(pegs[NUM_PEGS - 1][i]);
      }
      return 3 - val;
    }
        
    float f(){
      return h() + g;
    }

    int g = 0;
    vector< vector<char> > pegs;
};

static void printState(State* s){
  int level = 3;
  char c = ' ';
  string empty = "  |    ";
  for(int i = 0; i < NUM_PEGS; i++){
    for(int j = 0; j < NUM_RINGS; j++){
      level = 3 - i;
      c = s->pegs[i][j];
      if(s->pegs[i].size() >= level){
        switch (s->pegs[i][j]){
          case 'S':
            cout << S_STRING;
            break;
          case 'M':
            cout << M_STRING;
            break;
          case 'L':
            cout << L_STRING;
            break;
          default: 
            break;
        }
      } else {
        cout << empty;
      }
    }
  }
}

int main() {
  float eval;
  eval = 0;
  //initialize pegs to SML[empty][empty]
  //pegs[0] = TOTAL_WEIGHT;

  vector<State> frontier;
  State start = new State();
  printState(&start);
  return 0;
}