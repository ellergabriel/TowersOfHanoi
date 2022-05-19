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
#define L_WEIGHT  1.5
#define M_WEIGHT  .8
#define S_WEIGHT  .7

string S_STRING = "  S    ";
string M_STRING = " MMM   ";
string L_STRING = "LLLLL  ";

unordered_map<string, bool> states;

//helper function for getting disk weights
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

//lass to store each state of the puzzle
class State{
  public:
  //inner class with custom stack functionality
  //designed to allow iterating through the stack while maintaining
  //LIFO design 
  class MyStack{
    public:
      MyStack(){}

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

      //generates string representation of the stack
      //concatenated in State class in order to get
      //string representation of the entire State
      string generate(){
        string rep = "";
        for(int i = stack.size() - 1; i >= 0; i--){
          rep += stack[i];
        } 
        return (rep == "" ?  "[empty]" : rep);
      }

      int top = -1;
      vector<char> stack;
  }; //end MyStack inner class

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
      char disk = pegs[origin].peek(0);
      if(isLegalMove(dest, disk)){
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
            val += inter * pegs[i].size(); 
            continue;
          }
          val += inter / (pegs[i].size());
          inter = 0;
        }
      }
      //hDb = (val > 3) ? 0 : 3 - val;
      return (val > 3) ? 0 : 3 - val;
    }
        
    float f(){
      //fDb = h() + g;
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
    //float hDb;
    //float fDb;
};

//Formatted print of each state
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
      //check map if state string has been generated 
      if(!states.count(dummy->generateString())){
        dummy->record = current->record;
        frontier->push_back(dummy);
        printState(dummy);
        dummy->record.push_back(dummy);
        //terminate early if win state has been found
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
  int size = frontier->size();
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
  if(size == frontier->size()){ //no new states added
    cout << "no new states created" << endl;
  }
  return false;
}

//helper function to print record of states reached before input state
static void printRecord(list<State*>* record){
  for(list<State*>::iterator it = record->begin(); it != record->end(); it++){
    printState(*it);
  }
}

//returns index of frontier node to expand upon next
static int selectNode(vector<State*>* frontier){
  int index = 0;
  float eval = frontier->at(0)->f();
  for(int i = 0; i < frontier->size(); i++){
    if (!frontier->at(i)->isVisited && 
          states.count(frontier->at(i)->generateString()) != 0 &&
          frontier->at(i)->f() <= eval) {
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
    if(generateFrontier(dummy, &frontier)){
      isSolved = true; 
      cout << "Solved in " << dummy->g << " moves\n";
      cout << "solution path: " << endl;
      printRecord(&dummy->record);
      break;
    }
    frontier.erase(frontier.begin() + index);
    prev = dummy;
    //reset index for next iteration
    index = 0;
    }
  return 0;
}