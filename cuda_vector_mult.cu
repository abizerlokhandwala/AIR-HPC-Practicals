%%cu
#include<iostream>
#include<cuda.h>
#define THREADS_PER_BLOCK 256
using namespace std;

void seq(int v1[], int v2[], int n, int m){
    int v3[m]={0};
    clock_t st = clock();
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            v3[i]+=v1[j]*v2[j*m+i];
        }
    }
    clock_t en = clock();
    cout<<"Sequential vector-matrix multiplication: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    int sum=0;
    for(int i=0;i<m;i++){
        sum+=v3[i];
    }
    cout<<sum<<"\n";
}

__global__ void multVector(int *d1_in, int *d2_in, int *d_out, int n, int m){
    int ind = blockDim.x*blockIdx.x + threadIdx.x;
    if(ind<m){
        d_out[ind]=0;
        for(int i=0;i<n;i++){
            d_out[ind]+= d1_in[i]*d2_in[i*m+ind];
        }
    }
}

void parll(int v1[], int v2[], int n, int m){
    int *d1_in, *d2_in, *d_out;
    int h_out[m];
    cudaMalloc(&d1_in,sizeof(int)*n);
    cudaMalloc(&d2_in,sizeof(int)*n*m);
    cudaMalloc(&d_out,sizeof(int)*m);
    cudaMemcpy(d1_in,v1,sizeof(int)*n,cudaMemcpyHostToDevice);
    cudaMemcpy(d2_in,v2,sizeof(int)*n*m,cudaMemcpyHostToDevice);
    clock_t st = clock();
    multVector<<<1+(m-1)/THREADS_PER_BLOCK,THREADS_PER_BLOCK>>>(d1_in,d2_in,d_out,n,m);
    clock_t en = clock();
    cout<<"Parallel vector-matrix multiplication: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    cudaMemcpy(h_out, d_out, sizeof(int)*m,cudaMemcpyDeviceToHost);
    int sum=0;
    for(int i=0;i<m;i++){
        sum+=h_out[i];
    }
    cout<<sum<<"\n"; //to confirm if the elements are the same
    cudaFree(d1_in);
    cudaFree(d2_in);
    cudaFree(d_out);
}

int main(){
    srand(time(NULL));
    int n=1000;
    int m=1000;
    int v1[n];
    int v2[n*m];
    for(int i=0;i<n;i++){
        v1[i]=rand()%100;
    }
    for(int i=0;i<n*m;i++){
        v2[i]=rand()%100;
    }
    seq(v1,v2,n,m);
    parll(v1,v2,n,m);
}
