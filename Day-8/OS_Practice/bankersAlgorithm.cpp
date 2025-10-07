#include <iostream>
using namespace std;

class Process{
  int pid;
  int resources;
  int* allocate_resources;
  int* resources_need;
  int* max_resources_need;
public:
  Process(){
    this->pid = 0;
    this->resources = 0;
  }
  Process(int pid,int resources){
    this->pid = pid;
    this->resources = resources;
    this->allocate_resources = new int[resources];
    this->resources_need = new int[resources];
    this->max_resources_need = new int[resources];
  }
  void allocateResources();
  void maximumResourcesRequired();
  void calculateResourcesRequired();
  int getResources();
  int getPid();
  int* getResourcesNeed();  
  ~Process(){
    delete[] allocate_resources;
    delete[] resources_need;
    delete[] max_resources_need;
  }
};

int Process::getPid(){
  return this->pid;
}

int Process::getResources(){
  return this->resources;
}

// getter for resources_need
int* Process::getResourcesNeed(){
  return this->resources_need;
}

void Process::allocateResources(){
  cout<<"\nPlease allocate resources for Process Id: "<<this->pid<<" as specified below: "<<endl;
  for(int i=0;i<this->resources;i++){
    cout<<"Please allocate Resource "<<i+1<<": ";
    int a;
    cin>>a;
    this->allocate_resources[i] = a;
  }
  cout<<"Resources allocated to Process Id: "<<this->pid<<" successfully\n";
}

void Process::maximumResourcesRequired(){
  cout<<"\nPlease provide maximum need of resources required to run Process Id: "<<this->pid<<" as specified below: "<<endl;
  for(int i=0;i<this->resources;i++){
    cout<<"Please provide maximum resource "<<i+1<<" need: ";
    int a;
    cin>>a;
    if(a<allocate_resources[i]){
      cout<<"Allocated Resources cannot be more than the maximum resources required, please try again";
      i--;
    }
    else this->max_resources_need[i] = a;
  }
  cout<<"Maximum Resources need information taken for Process Id: "<<this->pid<<" successfully\n";
}

void Process::calculateResourcesRequired(){
  for(int i=0;i<this->resources;i++){
    this->resources_need[i] = max_resources_need[i]-allocate_resources[i];
  }
  cout<<"Resources needed to run Process Id: "<<this->pid<<" calculated successfully and displayed below:- \n";
  for(int i=0;i<this->resources;i++){
    cout<<"Resources required for Resource "<<i+1<<" : "<<resources_need[i]<<endl;
  }
}

void bankersAlgo(Process** P,int n){
  int maxResources = -1;
  for(int i=0;i<n;i++){
    if(maxResources<P[i]->getResources()){
      maxResources = P[i]->getResources();
    }
  }
  int* availableResources = new int[maxResources];
  cout<<"Please enter total resources available to CPU for Resource Allocation to Process as specified below "<<endl;
  for(int i=0;i<maxResources;i++){
    cout<<"Enter Resource "<<i+1<<" available: ";
    int j;
    cin>>j;
    if(j<0){
      cout<<"Resources available cannot be negative, please try again\n";
      i--;
    }else{
      availableResources[i] = j;
    }
  }
  cout<<"Resources available information taken successfully\n";

  int processDoneCount = 0;

  for(int iter=0; iter<n; iter++){
    bool progress = false;

    for(int i=0;i<n;i++){
      int resCount = P[i]->getResources();
      bool canRun = true;
      int* need = P[i]->getResourcesNeed();

      for(int j=0;j<resCount;j++){
        if(need[j] > availableResources[j]){
          canRun = false;
          break;
        }
      }

      if(canRun){
        for(int j=0;j<resCount;j++)
          availableResources[j] += need[j];  // release resources after process completion

        processDoneCount++;
        progress = true;
        cout<<"Process "<<P[i]->getPid()<<" finished. Updated available resources.\n";
      }
    }

    if(!progress) break;  // agar is iteration me koi process complete nahi hua
  }

  if(processDoneCount == n)
    cout<<"\nSystem is in Safe State.\n";
  else
    cout<<"\nSystem is NOT in Safe State. Deadlock may occur for some processes.\n";

  delete[] availableResources;
}

Process** initializeProcess(int n){
  Process** P = new Process*[n];
  for(int i=0;i<n;i++){
    cout<<"Please enter the total resources required for Process Id: "<<i+1<<": ";
    int j;
    cin>>j;
    Process* newProcess = new Process(i+1,j);
    newProcess->allocateResources();
    newProcess->maximumResourcesRequired();
    newProcess->calculateResourcesRequired();
    P[i] = newProcess;
  }
  return P;
}


int main(){
  cout<<"Welcome to the World of Programming\n";
  cout<<"This program is based on Bankers Algorithm\n";
  int n;
  cout<<"Please enter the number of Processes: ";
  cin>>n;
  Process** P = initializeProcess(n);
  bankersAlgo(P, n);
  for(int i = 0; i < n; i++)
    delete P[i]; 
  delete[] P;           

  return 0;
}
