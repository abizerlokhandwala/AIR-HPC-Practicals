%%cu
#include<iostream>
#include<cuda.h>
#define THREADS_PER_BLOCK 256
using namespace std;

void seq(int v1[], int v2[], int n, int m){
    int v3[n];
    clock_t st = clock();
    for(int i=0;i<n;i++){
        v3[i]=v1[i]+v2[i];
    }
    clock_t en = clock();
    cout<<"Sequential vector addition: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    int sum=0;
    for(int i=0;i<n;i++){
        sum+=v3[i];
    }
    cout<<sum<<"\n";
}

__global__ void addVector(int *d1_in, int *d2_in, int *d_out, int n){
    int ind = blockDim.x*blockIdx.x + threadIdx.x;
    if(ind<n){
        d_out[ind] = d1_in[ind]+d2_in[ind];
    }
}

void parll(int v1[], int v2[], int n, int m){
    int *d1_in, *d2_in, *d_out;
    int h_out[n];
    cudaMalloc(&d1_in,sizeof(int)*n);
    cudaMalloc(&d2_in,sizeof(int)*n);
    cudaMalloc(&d_out,sizeof(int)*n);
    cudaMemcpy(d1_in,v1,sizeof(int)*n,cudaMemcpyHostToDevice);
    cudaMemcpy(d2_in,v2,sizeof(int)*n,cudaMemcpyHostToDevice);
    clock_t st = clock();
    addVector<<<1+(n-1)/THREADS_PER_BLOCK,THREADS_PER_BLOCK>>>(d1_in,d2_in,d_out,n);
    clock_t en = clock();
    cout<<"Parallel vector addition: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    cudaMemcpy(h_out, d_out, sizeof(int)*n,cudaMemcpyDeviceToHost);
    int sum=0;
    for(int i=0;i<n;i++){
        sum+=h_out[i];
    }
    cout<<sum<<"\n"; //to confirm if the elements are the same
    cudaFree(d1_in);
    cudaFree(d2_in);
    cudaFree(d_out);
}

int main(){
    srand(time(NULL));
    int n=500000;//10 fold speedup
    int m=n;
    int v1[n],v2[m];
    for(int i=0;i<n;i++){
        v1[i]=rand()%100;
        v2[i]=rand()%100;
    }
    seq(v1,v2,n,m);
    parll(v1,v2,n,m);
}
