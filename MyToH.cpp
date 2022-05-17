#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>
using namespace std;

//========
// Gabriel Eller 
//========

#define NUM_RINGS  3
#define NUM_PEGS  3
#define L_WEIGHT  1.8
#define M_WEIGHT  .9
#define S_WEIGHT  .3

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
        return (rep == "" ?  "[empty]" : rep);
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
      record.push_back(this);
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
      //(sum of weights on peg) / (# of rings on peg)
      float val = 0;
      float inter = 0;
      for(int i = 0; i < pegs.size(); i++){
        if(pegs[i].size() != 0){
          for(int j = 0; j < pegs[i].size(); j++){
            inter += convertChar(pegs[i].peek(j));
          }
         //last peg gets more weight, moreso if L is in place 
          if(i == NUM_PEGS - 1){
            //end condition; two rings that are not small must mean 
            //S [empty] ML state has been reached
            //-1 indicates that f() for this state should be 0, taken immediately
            if (pegs[i].size() > 1 && pegs[i].peek(0) != 'S'){
              return -1;
            }
            val += inter * pegs[i].size(); 
            continue;
          }
          val += inter / pegs[i].size();
          inter = 0;
        }
      }
      hDb = (val > 3) ? 0 : 3 - val;
      return (val > 3) ? 0 : 3 - val;
    }
        
    float f(){
      fDb = h() + g;
      return (h() == -1) ? 0 : h() + g;
    }

    string generateString(){
      string rep = "";
      for(int i = 0; i < NUM_PEGS; i++){
        rep += pegs[i].generate();
      }
      //cout << rep << endl;
      this->rep = rep;
      return rep;
    }
    
    bool isFinished(){
      return pegs[NUM_PEGS - 1].size() == NUM_RINGS;
    }

    int g = 0;
    vector< State::MyStack > pegs;
    bool isVisited = false;
    string rep;
    list<State*> record;
    //debugger variables for easier viewing 
    float hDb;
    float fDb;
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
    if(i == 0){
      cout << "\tf = " << s.f();
      cout << " g = " << s.g;
      cout << " h = " << s.h();
    }
    cout << endl;
  }
  cout << endl;
}

/**
pre-condition: it is assumed this is only called on a legal state
  **/
static bool generateStates(State* current, vector<State*>* frontier, int pegPos){
  State* dummy;
  char c = current->pegs[pegPos].peek(0);
  for(int i = 0; i < NUM_PEGS; i++){
    dummy = new State(current);
    if(i != pegPos && dummy->isLegalMove(i, c)){
      
      dummy->moveDisk(pegPos, i);
      if(!states.count(dummy->generateString())){
        dummy->record = current->record;
        frontier->push_back(dummy);
        printState(dummy);
        dummy->record.push_back(dummy);
        if(dummy->isFinished()){
          current->record = dummy->record;
          current->g += 1;
          return true;
        }
        states.emplace(dummy->generateString(), true);
      }
    }
  }
  return false;
}

static bool generateFrontier(State* current, vector<State*>* frontier){
  //first find pegs with disks that have not been generated 
  int debugger = 0;
  cout << "\n\t\texpanding on state:" << endl;
  printState(current);
  cout << "attempting to creating frontier state: " << endl;
  for(int i = 0; i < NUM_PEGS; i++){
    if(current->pegs[i].size() > 0 && !current->isVisited){
      if(generateStates(current, frontier, i)){
        return true;
      }
    }
  }
  current->isVisited = true;
  return false;
}

static void printRecord(list<State*>* record){
  for(list<State*>::iterator it = record->begin(); it != record->end(); it++){
    printState(*it);
  }
}

//returns index of frontier node to expand upon next
static int selectNode(vector<State*>* frontier){
  int index = 0;
  float eval = 4;
  for(int i = 0; i < frontier->size(); i++){
    if (!frontier->at(i)->isVisited && 
          states.count(frontier->at(i)->generateString()) != 0 &&
          frontier->at(i)->f() < eval) {
      index = i; 
      eval = frontier->at(i)->f();
    }
  }
  return index;
}

int main() {
  float eval;
  int index = 0;
  bool isSolved = false;
  eval = 4;
  //cout << "main is starting" << endl;
  vector<State*> frontier;
  State start;
  states.emplace(start.generateString(), true);
  cout << "\t\tstart state: \n";
  printState(start);  
  generateFrontier(&start, &frontier);
  State* dummy = &start;
  State* prev;
  while(!isSolved){
    index = selectNode(&frontier);
    dummy = frontier[index];
    eval = dummy->f();
    if(generateFrontier(dummy, &frontier)){
      isSolved = true; 
      //dummy->record.push_back(dummy);
      cout << "Solved in " << dummy->g << " moves\n";
      cout << "solution path: " << endl;
      printRecord(&dummy->record);
      break;
    }
    frontier.erase(frontier.begin() + index);
    prev = dummy;
    //reset evaluation and index for next iteration
    eval = frontier[0]->f();
    index = 0;
    }
  return 0;
}