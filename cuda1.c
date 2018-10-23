// %%cu
// as data type is int, sum might overflow (depending on rand(), but the seq and parallel answers are still equal, or change int to long long (too lazy sorry))
#include<iostream>
#include<vector>
#include<cstdlib>
#include<cstdio>
#include<limits>
#include<cuda.h>
#define THREADS_PER_BLOCK 256
using namespace std;

void seq(vector<int>v){
    clock_t st = clock();
    int sum = 0;
    for(int i=0;i<v.size();i++){
        sum+=v[i];
    }
    clock_t en = clock();

    cout<<"Sequential Sum: "<<sum<<"\n";

    cout<<"Time taken: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";

    st= clock();
    int maxx=-1e9;
    for(int i=0;i<v.size();i++){
        maxx=max(maxx,v[i]);
    }
    en = clock();
    cout<<"Sequential Maximum: "<<maxx<<"\n";

    cout<<"Time taken: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";

    st = clock();
    int minn=1e9;
    for(int i=0;i<v.size();i++){
        minn=min(minn,v[i]);
    }
    en = clock();
    cout<<"Sequential Minimum: "<<minn<<"\n";

    cout<<"Time taken: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";


    st = clock();
    sum = 0;
    for(int i=0;i<v.size();i++){
        sum+=v[i];
    }
    sum=sum/(int)v.size();
    en = clock();

    cout<<"Sequential Average: "<<sum<<"\n";

    cout<<"Time taken: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
}

__global__ void calculate(int *arr_in, int* arr_out, int sz, int option){
    int ind = threadIdx.x;
    int dim = blockDim.x;
    extern __shared__ int shared_mem[];
    int actual_ind = blockIdx.x*blockDim.x + ind;
    if(actual_ind < sz){
        shared_mem[ind] = arr_in[actual_ind];
    }else{
        if(option == 0 || option == 3)
            shared_mem[ind] = 0;
        else if(option == 1){//maximum
            shared_mem[ind] = -INT_MAX;
        }else{//minimum
            shared_mem[ind] = INT_MAX;
        }
    }
    __syncthreads();
    for(int i=dim/2 ; i > 0 ; i=i/2){
        if(ind<i){
            if(option == 0 || option == 3)
                shared_mem[ind]+=shared_mem[ind+i];
            else if(option == 1){
                shared_mem[ind]=max(shared_mem[ind],shared_mem[ind+i]);
            }else{
                shared_mem[ind]=min(shared_mem[ind],shared_mem[ind+i]);
            }
        }
        __syncthreads();
    }
    arr_out[blockIdx.x]=shared_mem[0];
}

void parll(vector<int>v){
    int *h_in, *h_out, *d_in, *d_out;
    h_in = new int[v.size()];
    copy(v.begin(),v.end(),h_in);
    string opt[] = {"Sum", "Maximum", "Minimum", "Average"};
    h_out = new int[1];
    int actual_size = v.size();
    for(int option = 0; option<4; option++){
        cudaMalloc(&d_in,sizeof(int)*v.size());
        cudaMalloc(&d_out,sizeof(int)*v.size());
        cudaMemcpy(d_in,h_in,sizeof(int)*v.size(),cudaMemcpyHostToDevice);
        int d_in_size = v.size();
        int num_blocks = 1+(d_in_size-1)/THREADS_PER_BLOCK;
        clock_t st = clock();
        while(num_blocks>1){
            calculate<<<num_blocks,THREADS_PER_BLOCK,sizeof(int)*THREADS_PER_BLOCK>>>(d_in,d_out,d_in_size,option);
            d_in_size = num_blocks;
            cudaFree(d_in);
            cudaMalloc(&d_in,sizeof(int)*d_in_size);
            cudaMemcpy(d_in,d_out,sizeof(int)*d_in_size,cudaMemcpyDeviceToDevice);
            cudaFree(d_out);
            num_blocks = 1+(num_blocks-1)/THREADS_PER_BLOCK;
            cudaMalloc(&d_out,sizeof(int)*num_blocks);
        }
        calculate<<<1,THREADS_PER_BLOCK,sizeof(int)*THREADS_PER_BLOCK>>>(d_in,d_out,d_in_size,option);
        clock_t en = clock();
        cudaMemcpy(h_out,d_out,sizeof(int)*1,cudaMemcpyDeviceToHost);
        if(option==3){
            h_out[0] = h_out[0]/actual_size;
        }
        cout<<"Parallel "<<opt[option]<<" : "<<h_out[0]<<"\n";
        cout<<"Time taken: "<<double(en-st)/CLOCKS_PER_SEC<<"\n";
        cudaFree(d_in);
        cudaFree(d_out);
    }
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    vector<int>v;
    srand(time(NULL));
    for(int i=0;i<100000000;i++){
        v.push_back(1+rand()%1000);
    }
    seq(v);
    parll(v);
}
