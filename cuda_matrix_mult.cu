%%cu
#include<iostream>
#include<cuda.h>
#define THREADS_PER_BLOCK 256
using namespace std;

void seq(int v1[], int v2[], int n, int m, int k){
    int v3[n*k]={0};
    clock_t st = clock();
    for(int i=0;i<n;i++){
        for(int j=0;j<k;j++){
            for(int l=0;l<m;l++){
                v3[i*k+j] += v1[i*m+l]*v2[l*k+j];
            }
        }
    }
    clock_t en = clock();
    cout<<"Sequential matrix-matrix multiplication: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    int sum=0;
    for(int i=0;i<n*k;i++){
        sum+=v3[i];
        // cout<<v3[i]<<" ";
    }
    cout<<sum<<"\n";
}

__global__ void multMatrix(int *d1_in, int *d2_in, int *d_out, int n, int m, int k){
    int indx = threadIdx.x;
    int indy = threadIdx.y;
    int ind = indy*k+indx;
    //printf("%d %d\n",indy,indx);
    if(ind<n*k){
        d_out[ind] = 0;
        for(int i=0;i<m;i++){
            d_out[ind] += d1_in[indy*m+i]*d2_in[i*k+indx];
        }
    }
}

void parll(int v1[], int v2[], int n, int m, int k){
    int *d1_in, *d2_in, *d_out;
    int h_out[n*k];
    cudaMalloc(&d1_in,sizeof(int)*n*m);
    cudaMalloc(&d2_in,sizeof(int)*m*k);
    cudaMalloc(&d_out,sizeof(int)*n*k);
    cudaMemcpy(d1_in,v1,sizeof(int)*n*m,cudaMemcpyHostToDevice);
    cudaMemcpy(d2_in,v2,sizeof(int)*m*k,cudaMemcpyHostToDevice);
    dim3 threads2D(k,n);
    dim3 blocks(1,1);
    clock_t st = clock();
    multMatrix<<<blocks,threads2D>>>(d1_in,d2_in,d_out,n,m,k);
    clock_t en = clock();
    cout<<"Parallel matrix-matrix multiplication: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
    cudaMemcpy(h_out, d_out, sizeof(int)*n*k,cudaMemcpyDeviceToHost);
    int sum=0;
    for(int i=0;i<n*k;i++){
        sum+=h_out[i];
        // cout<<h_out[i]<<" ";
    }
    cout<<sum<<"\n"; //to confirm if the elements are the same
    cudaFree(d1_in);
    cudaFree(d2_in);
    cudaFree(d_out);
}

int main(){
    srand(time(NULL));
    int n=20;
    int m=50;
    int k=50;
    int v1[n*m];
    int v2[m*k];
    for(int i=0;i<n*m;i++){
        v1[i]=rand()%100;
    }
    for(int i=0;i<m*k;i++){
        v2[i]=rand()%100;
    }
    seq(v1,v2,n,m,k);
    parll(v1,v2,n,m,k);
}
