#include <iostream>
#include <vector>
using namespace std;

//========
// Gabriel Eller 
//========

#define NUM_RINGS  3
#define NUM_PEGS  3
#define L_WEIGHT  2.5
#define M_WEIGHT  1
#define S_WEIGHT  .5
//#define TOTAL_WEIGHT 4


//helper class to store each state of the puzzle
class State{
  public:
    State(){}
  
    State(float lastPegs[]){
      for(int i = 0; i < NUM_PEGS; i++){
        pegs[i] = lastPegs[i];
      }
    }
    
    float h(){
      return 3 - pegs[NUM_PEGS - 1];
    }
  
    float f(){
      return h() + g;
    }
  
    float* getPegs(){
      return pegs;
    }


    int g = 0;
  
    float pegs[];
};

bool isLegalMove(State state, int peg, int disk){
  //either peg has larger disks already in place, no disks in place
  //or fringe case of MSL, where weight returns true despite being illegal
  return ( (state.getPegs()[peg] > disk && state.getPegs()[peg] != L_WEIGHT + S_WEIGHT)
           || state.getPegs()[peg] == 0);
}

int main() {
  cout << "FUCK";
  bool isSolved = false;
  float g, h ,f, eval;
  g = h = f = eval = 0;
  float pegs[NUM_PEGS];
  //initialize pegs to SML[empty][empty]
  //pegs[0] = TOTAL_WEIGHT;

  vector<State> frontier;
  cout << 4;
  while(!isSolved){
    for(int i = 0; i < frontier.size(); i++){ 
      eval = (eval > frontier[i].f()) ? frontier[i].f() : eval;
    }
  }
}