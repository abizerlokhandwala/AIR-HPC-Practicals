//mpicxx knn.cpp
//mpirun -np 10 ./a.out
#include<iostream>
#include<mpi.h>
#include<string.h>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include<math.h>
#include<set>
using namespace std;

class train_instance{
public:
    double r,g,b;
    int skin;
    train_instance(double r, double g, double b, int skin){
        this->r = r;
        this->g = g;
        this->b = b;
        this->skin = skin;
    }
    double calculate(double rr, double gg, double bb){
        return sqrt((rr-r)*(rr-r)+(gg-g)*(gg-g)+(bb-b)*(bb-b));
    }
};

class test_instance{
public:
    double r,g,b;
    test_instance(double r, double g, double b){
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

vector<string> split(string line, char c){
    string temp;
    vector<string>fin;
    for(int i=0;i<line.length();i++){
        if(line[i]!=c){
            temp.push_back(line[i]);
        }else{
            fin.push_back(temp);
            temp.clear();
        }
    }
    if(temp.length()>0){
        fin.push_back(temp);
    }
    return fin;
}

vector<train_instance>train;

int getClass(test_instance obj, int k){
    int skin[3]={0};

    multiset<pair<double,int> >s;
    for(int i=0;i<train.size();i++){
        double dist = train[i].calculate(obj.r,obj.g,obj.b);
        s.insert(make_pair(dist,train[i].skin));
    }
    int count=0;
    for(multiset<pair<double,int> >::iterator itr=s.begin();itr!=s.end();itr++){
        pair<double,int>p = *itr;
        skin[p.second]++;
        count++;
        if(count==k){
            break;
        }
    }
    if(skin[1]>skin[2]){
        return 1;
    }else return 2;
}

int main(){
    MPI_Init(NULL,NULL);
    int world_size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Request request[(world_size-1)*3];
    MPI_Status status[(world_size-1)*3];
    // cout<<rank<<"\n";
    ifstream file("training.txt");
    string line;
    while(getline(file,line)){
        vector<string>tokens = split(line,' ');
        train_instance obj(atof(tokens[0].c_str()),atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()));
        train.push_back(obj);
    }
    file.close();
    int minR=train[0].r;
    int maxR=train[0].r;
    int minG=train[0].g;
    int maxG=train[0].g;
    int minB=train[0].b;
    int maxB=train[0].b;
    for(int i=0;i<train.size();i++){
        minR=min(minR,(int)train[i].r);
        maxR=max(maxR,(int)train[i].r);
        minG=min(minG,(int)train[i].g);
        maxG=max(maxG,(int)train[i].g);
        minB=min(minB,(int)train[i].b);
        maxB=max(maxB,(int)train[i].b);
    }
    for(int i=0;i<train.size();i++){
        train[i].r = (train[i].r-minR)/(maxR-minR);
        train[i].g = (train[i].g-minG)/(maxG-minG);
        train[i].b = (train[i].b-minB)/(maxB-minB);
    }

    int k = sqrt(train.size());

    double start = MPI_Wtime();
    if(rank==0){
        ifstream file2("test_.txt");
        vector<test_instance>test;
        while(getline(file2,line)){
            vector<string>tokens = split(line,' ');
            test_instance obj(atof(tokens[0].c_str()),atof(tokens[1].c_str()),atof(tokens[2].c_str()));
            test.push_back(obj);
        }
        file2.close();
        for(int i=0;i<test.size();i++){
            test[i].r = (test[i].r-minR)/(maxR-minR);
            test[i].g = (test[i].g-minG)/(maxG-minG);
            test[i].b = (test[i].b-minB)/(maxB-minB);
            // cout<<test[i].r<<"\n";
        }
        int index=1;
        for(int i=1;i<test.size();i++){
            double r=test[i].r;
            double g=test[i].g;
            double b=test[i].b;
            MPI_Isend(&r,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,request+index);
            index++;
            MPI_Isend(&g,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,request+index);
            index++;
            MPI_Isend(&b,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,request+index);
            index++;
        }
        int skin = getClass(test[0],k);
        cout<<"Class for test #"<<rank<<" is: "<<skin<<"\n";
    }else{
        double r;
        double g;
        double b;
        int off = (rank-1)*3+1;
        MPI_Irecv(&r,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,request+off);
        MPI_Irecv(&g,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,request+off+1);
        MPI_Irecv(&b,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,request+off+2);

        MPI_Wait(request+off,status+off);
        MPI_Wait(request+off+1,status+off+1);
        MPI_Wait(request+off+2,status+off+2);

        test_instance obj(r,g,b);
        int skin = getClass(obj,k);
        cout<<"Class for test #"<<rank<<" is: "<<skin<<"\n";
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0){
        double end = MPI_Wtime();
        cout<<"Time taken: "<<end-start<<"\n";
    }
    MPI_Finalize();
}
