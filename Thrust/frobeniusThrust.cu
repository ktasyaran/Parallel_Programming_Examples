#include <thrust/device_vector.h>
#include <sodium.h>
#include <thrust/transform_reduce.h>

#include <math.h>

#define N 100
#define M 100
#define UPPER_BOUND 40

using namespace std;

struct squar: public unary_function<int,int>

{
	__host__ __device__
	int operator()(const int &x) const{
		return x*x;
	}

};


int main(int argc, char**argv){

	thrust::device_vector <int> vec(N*M);
	
	unsigned long long int result=0;
	for(int i=0;i<N*M;i++){
		vec[i]=randombytes_uniform(UPPER_BOUND);
	}
	
	result=thrust::transform_reduce(vec.begin(),vec.end(),squar(),0,thrust::plus<int>());
	cout<<result<<" "<<sqrt(result);
	return 0;
}