#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <string>
using namespace std;

//========
// Gabriel Eller 
//========

#define NUM_RINGS  3
#define NUM_PEGS  3
#define L_WEIGHT  1.5
#define M_WEIGHT  1
#define S_WEIGHT  .5

string S_STRING = "  S    ";
string M_STRING = " MMM   ";
string L_STRING = "LLLLL  ";

unordered_map<string, bool> states;


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
  //inner class with custom stack functionality
  class MyStack{
    public:
      MyStack(){

      }

      char peek(int index){
        if(index < 0){ index = 0; }
        if(top >= 0){
          return stack[top - index];
        } else {
          return ' ';
        }
      }
  
      void push(char c){
        stack.push_back(c);
        top++;
      }
  
      char pop(){
        char c = stack[top];
        //stack[top] = ' ';
        stack.pop_back();
        top--;
        return c;
      }
  
      int size(){
        return top + 1;
      }
  
      float getVal(){
        float val = 0;
        for(int i = 0; i < top + 1; i++){
          val += convertChar(stack[i]);
        }
        return val;
      }

      string generate(){
        string rep = "";
        for(int i = stack.size() - 1; i >= 0; i--){
          rep += stack[i];
        } 
        return (rep == "" ?  "[empty] " : rep + " ");
      }

      int top = -1;
      vector<char> stack;
  };

    State(){
      for(int i = 0; i < NUM_PEGS; i++){
        State::MyStack* holder = new MyStack();
        pegs.push_back(*holder);
      }
      pegs[0].push('L');
      pegs[0].push('M');
      pegs[0].push('S');
    }
    
    State(State* origin){
      pegs = origin->pegs;
      g = origin->g;
    }
    bool isLegalMove(int dest, char disk){
      if(pegs[dest].size() > 0){
        return (convertChar(disk) < convertChar(pegs[dest].peek(0)));
      } else {
        return true;
      }
    }

    bool moveDisk(int origin, int dest){
      char disk = 'c';
      if(isLegalMove(dest, disk)){
        disk = pegs[origin].peek(0);
        pegs[dest].push(disk);
        pegs[origin].pop();
        g++;
        return true;
      }
      return false;
    }

    float h(){
      //rings on last peg determine h
      float val = pegs[NUM_PEGS - 1].getVal();
      return 3 - val;
    }
        
    float f(){
      return h() + g;
    }

    string generateString(){
      string rep = "";
      for(int i = 0; i < NUM_PEGS; i++){
        rep += pegs[i].generate();
      }
      return rep;
    }
    

    int g = 0;
    vector< State::MyStack > pegs;
    bool isVisited = false;
};

static void printState(State s){
  int level = NUM_RINGS;
  int currentSize = -1;
  string empty = "  |    ";
  for(int i = 0; i < NUM_PEGS; i++){
    for(int j = 0; j < NUM_RINGS; j++){
      level = NUM_RINGS - i;
      if(s.pegs[j].size() >= level){
        switch (s.pegs[j].peek(s.pegs[j].size() - level)){
          case 'S':
            cout << S_STRING;
            continue;
            break;
          case 'M':
            cout << M_STRING;
            continue;
            break;
          case 'L':
            cout << L_STRING;
            continue;
            break;
          default: 
            cout << empty;
            break;
        }
      } else {
        cout << empty;
      }

    }
    cout << endl;
  }
  cout << endl;
}

/**
pre-condition: it is assumed this is only called on a legal state
  **/
static void generateStates(State* current, vector<State*>* frontier, int pegPos){
  State* dummy;
  char c = current->pegs[pegPos].peek(0);
  for(int i = 0; i < NUM_PEGS; i++){
    dummy = new State(current);
    if(i != pegPos && dummy->isLegalMove(i, c)){
      //if(dummy->pegs[i].size() == 0){
        //if empty, move disk and add to frontier immediately
        dummy->moveDisk(pegPos, i);
        if(!states[dummy->generateString()]){
          frontier->push_back(dummy);
          printState(dummy);
          //cout << dummy->generateString() << "\n" << endl;
          states.emplace(dummy->generateString(), true);
        //}
      //} else {
       // cout << "NOT EMPTY" << endl;
     // }
      }
    }
  }
  current->isVisited = true;
}

static void generateFrontier(State* current, vector<State*>* frontier){
  //first find pegs with disks that have not been generated 
  int debugger = 0;
  for(int i = 0; i < NUM_PEGS; i++){
    if(current->pegs[i].size() > 0 && !current->isVisited){
      generateStates(current, frontier, i);
    }
  }
}

int main() {
  float eval;
  bool isSolved = false;
  eval = 100;
  cout << "main is starting" << endl;
  vector<State*> frontier;
  State start;
  states.emplace(start.generateString(), true);
  printState(start);
  generateFrontier(&start, &frontier);
  while(!isSolved){
    State* dummy;
    for(int i = 0; i < frontier.size(); i++){
      if(eval > frontier[i]->f()){
        dummy = frontier[i];
        eval = dummy->f();
      }
      //cout << eval << endl;
    }
    generateFrontier(dummy, &frontier);
  }
  return 0;
}