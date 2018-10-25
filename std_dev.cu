%%cu
#include<iostream>
#include<cuda.h>
#include<vector>
#include<math.h>
using namespace std;

void seq(vector<double>a, int n){
    double avg=0;
    for(int i=0;i<n;i++){
        avg+=a[i];
    }
    avg/=n;
    // cout<<avg<<"\n";
    double stdv=0;
    for(int i=0;i<n;i++){
        stdv+=(a[i]-avg)*(a[i]-avg);
    }
    stdv/=n;
    stdv=sqrt(stdv);
    cout<<stdv<<"\n";
}

__global__ void summ(double *d_in,double *d_out, int n){
    extern __shared__ double shared_mem[];
    int actind = blockIdx.x*blockDim.x+threadIdx.x;
    int ind = threadIdx.x;
    if(actind<n){
        shared_mem[ind] = d_in[actind];
    }else shared_mem[ind] = 0;
    __syncthreads();
    for(int steps=blockDim.x/2;steps>0;steps=steps/2){
        if(ind<steps){
            shared_mem[ind]+=shared_mem[ind+steps];
        }
        __syncthreads();
    }
    d_out[blockIdx.x] = shared_mem[0];
}

__global__ void subb(double *d_in, int n, double avg){
    int actind = blockIdx.x*blockDim.x+threadIdx.x;
    if(actind<n){
        d_in[actind] = (d_in[actind]-avg)*(d_in[actind]-avg);
    }
}

void parll(vector<double>a, int n){
    double *d_in, *d_out;
    double h_in[n];
    copy(a.begin(),a.end(),h_in);
    cudaMalloc(&d_in,sizeof(double)*n);
    int blocks = 1+(n-1)/256;
    cudaMalloc(&d_out,sizeof(double)*blocks);
    cudaMemcpy(d_in,h_in,sizeof(double)*n,cudaMemcpyHostToDevice);
    int sz=n;
    while(blocks>1){
        summ<<<blocks,256,sizeof(double)*256>>>(d_in,d_out,sz);
        cudaFree(d_in);
        cudaMalloc(&d_in,sizeof(double)*blocks);
        cudaMemcpy(d_in,d_out,sizeof(double)*blocks,cudaMemcpyDeviceToDevice);
        sz=blocks;
        blocks=1+(sz-1)/256;
        cudaFree(d_out);
        cudaMalloc(&d_out,sizeof(double)*blocks);
    }
    summ<<<1,256,sizeof(double)*256>>>(d_in,d_out,sz);
    double h_out[1];
    cudaMemcpy(h_out,d_out,1*sizeof(double),cudaMemcpyDeviceToHost);
    double avg=h_out[0]/n;
    cudaFree(d_in);
    cudaFree(d_out);
    cudaMalloc(&d_in,sizeof(double)*n);
    cudaMemcpy(d_in,h_in,sizeof(double)*n,cudaMemcpyHostToDevice);
    subb<<<1+(n-1)/256,256>>>(d_in,n,avg);
    double fin[n];
    cudaMemcpy(fin,d_in,sizeof(double)*n,cudaMemcpyDeviceToHost);
    cudaFree(&d_in);
    cudaMalloc(&d_in,sizeof(double)*n);
    blocks = 1+(n-1)/256;
    cudaMalloc(&d_out,sizeof(double)*blocks);
    cudaMemcpy(d_in,fin,sizeof(double)*n,cudaMemcpyHostToDevice);
    sz=n;
    while(blocks>1){
        summ<<<blocks,256,sizeof(double)*256>>>(d_in,d_out,sz);
        cudaFree(d_in);
        cudaMalloc(&d_in,sizeof(double)*blocks);
        cudaMemcpy(d_in,d_out,sizeof(double)*blocks,cudaMemcpyDeviceToDevice);
        sz=blocks;
        blocks=1+(sz-1)/256;
        cudaFree(d_out);
        cudaMalloc(&d_out,sizeof(double)*blocks);
    }
    summ<<<1,256,sizeof(double)*256>>>(d_in,d_out,sz);
    cudaMemcpy(h_out,d_out,1*sizeof(double),cudaMemcpyDeviceToHost);
    double ans=sqrt(h_out[0]/n);
    cout<<ans<<"\n";
}

int main(){
    srand(time(NULL));
    int n=100000;
    vector<double>a(n);
    for(int i=0;i<n;i++){
        a[i]=rand()%100;
    }
    seq(a,n);
    parll(a,n);
}
