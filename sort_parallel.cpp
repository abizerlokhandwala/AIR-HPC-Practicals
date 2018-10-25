#include<iostream>
#include<stdlib.h>
#include<vector>
#include<omp.h>
using namespace std;

void bubble_seq(vector<int>a, int n){
    int cnt=1;
    while(1){
        int f=0;
        for(int i=0;i<n-cnt;i++){
            if(a[i]>a[i+1]){
                swap(a[i],a[i+1]);
                f=1;
            }
        }
        cnt++;
        if(f==0){
            break;
        }
    }
}

void bubble_parallel(vector<int>a, int n){
    int f=0;
    for(int j=0;;j++){
        if((j&1)==0){
            f=0;
        }
        #pragma omp parallel for reduction(+:f)
        for(int i=(j&1);i<n-1;i+=2){
            if(a[i]>a[i+1]){
                swap(a[i],a[i+1]);
                f++;
            }
        }
        if(j&1){
            if(f==0){
                break;
            }
        }
    }
    // for(int i=0;i<n;i++){
    //     cout<<a[i]<<" ";
    // }
}

void merge(vector<int>&a, int l, int mid, int r){
    int i=0,j=0,k=l;
    int sz1 = mid-l+1;
    int sz2 = r-mid;

    int arr1[sz1], arr2[sz2];

    for(int i=0;i<sz1;i++){
        arr1[i] = a[l+i];
    }
    for(int i=0;i<sz2;i++){
        arr2[i] = a[mid+i+1];
    }

    while(i<sz1 && j<sz2){
        if(arr1[i]<arr2[j]){
            a[k] = arr1[i];
            k++;
            i++;
        }else{
            a[k] = arr2[j];
            k++;
            j++;
        }
    }
    while(i<sz1){
        a[k] = arr1[i];
        i++;
        k++;
    }
    while(j<sz2){
        a[k] = arr2[j];
        j++;
        k++;
    }
}

void merge_seq(vector<int>&a, int l, int r){
    if(l<r){
        int mid=(l+r)/2;
        merge_seq(a,l,mid);
        merge_seq(a,mid+1,r);
        merge(a,l,mid,r);
    }
}

void merge_parallel(vector<int>&a, int l, int r){
    if(l<r){
        int mid=(l+r)/2;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                merge_seq(a,l,mid);
            }

            #pragma omp section
            {
                merge_seq(a,mid+1,r);
            }
        }
        merge(a,l,mid,r);
    }
}

int main(){
    srand(time(NULL));
    int n=10000;
    omp_set_num_threads(2);
    vector<int>a(n);
    for(int i=0;i<n;i++){
        a[i]=rand()%100;
    }
    double st = omp_get_wtime();
    bubble_seq(a,n);
    double en = omp_get_wtime();
    cout<<"Bubbleseq: "<<en-st<<"\n";
    st = omp_get_wtime();
    bubble_parallel(a,n);
    en = omp_get_wtime();
    cout<<"Bubbleparll: "<<en-st<<"\n";
    vector<int>temp(a.begin(),a.end());
    st = omp_get_wtime();
    merge_seq(temp,0,n-1);
    en = omp_get_wtime();
    cout<<"Mergeseq: "<<en-st<<"\n";
    st = omp_get_wtime();
    merge_parallel(a,0,n-1);
    en = omp_get_wtime();
    cout<<"Mergeparll: "<<en-st<<"\n";
}
