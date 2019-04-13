#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <immintrin.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;


void intrinsicCD(void){
__m256i result,cd;
__m256i a,b;
a= _mm256_setr_epi32(0,0,2,0,4,0,6,6);
b= _mm256_setr_epi32(0,0,1,2,3,6,7,8);

cd =_mm256_conflict_epi32(a);
printf("input: (");
for(int i =0;i<8;i++){
int * in = (int*) &a;

  printf("%i ",in[i]);
}
printf(")\n \n");
result =  _mm256_add_epi64(a,b);
int * res = (int*) &cd;
    
}

/**
 * This function performs a conflict detection on a list 
 * of sorted unsigned integers (uint32_t)
 * It returns a bitset that contains a 1 for each conflict 
 * at the same spot. Any further occurence after the first one of a value will be a conflict.
 * example: input {0,0,1,2,2}
 * will generate the following output bitset: {0,1,0,0,1}
 * 
 */
bitset<256> scalarCD(vector<uint32_t> input)
{
bitset<256> result ={0};

uint32_t last;
last = input[0];


for (int i =1; i<input.size();i++){
   
   if(input[i]==last)
   {
       result[i]=1;
   }
    last=input[i];
}
return result;
}
/**
 * This helper function does a conflict detection on input data with an offeset of elements
 * 
 * 
 * 
 * 
 * 
 */
bitset<16> DoCD(vector<uint32_t> in, int offset)
{
    bitset<16> rest ={0};
__m512i b= _mm512_setr_epi32(in[0+offset],in[1+offset],in[2+offset],in[3+offset],in[4+offset],in[5+offset],in[6+offset],in[7+offset],in[8+offset],in[9+offset],in[10+offset],in[11+offset],in[12+offset],in[13+offset],in[14+offset],in[15+offset]);


cout<<endl;
__m512i cd2 =_mm512_conflict_epi32(b);

int * res2 = (int*) &cd2;
//write 1 in return bitset if a conflict occurs
for(int i=0;i<16;i++)
{
	
	
    //if the int is not zero, there has been a conflict (any bit set)
    if(res2[i]!=0)
    rest[i]=1;
   
}
return rest;
}

bitset<256> avxCD(vector<uint32_t> in)
{
    
bitset<256> rest ={0};


//input avx register and conflict detection register
__m512i b,cd2;
//set input values to register



cout <<"BITSET AVXCD MM512: "<<endl;

//write 1 in return bitset if a conflict occurs

for (int k=0;k<16;k++)
{
bitset<16> test = DoCD(in,k*16);
for(int i=0;i<16;i++)
{
rest[k*16+i]=test[i];
cout<<i<<" "<<rest[k*i]<<endl;
}

}






/*
__m512i_u a,cd;
a= _mm512_set_epi32(0,0,1,1,2,1,4,1,3,9,12,9,2,1,4,5);
int * in = (int*) &a;
for(int i=0;i<16;i++){
   cout<<in[i]<<" ";
}
cout<<endl;

cd =_mm512_conflict_epi32(a);
int * out =(int *) &cd;


cout <<"BITSET AVXCD: "<<endl;
bitset<33> bits(cd[0]);

for(int i=0;i<16;i++)
{
    bitset<33> bits(cd[i]);
    
cout <<bits.to_string()<<endl;

}
*/
return rest;
}

/**
 * Helper function to output the contents of a vector<uint32_t>
 * 
 */
void printvec(vector<uint32_t> input)
{
     cout << "(";
     for (uint32_t &t : input){
        cout <<" "<< t <<" " ;
    }
    cout << ")"<<endl;
}


void printvec(vector<int> input, string label)
{
     cout << label<< ": (";
     for (int &t : input){
        cout <<" "<< t <<" " ;
    }
    cout << ")"<<endl;
}
void printvec(vector<uint32_t> input, string label)
{
     cout << label<< ": (";
     for (uint32_t &t : input){
        cout <<" "<< t <<" " ;
    }
    cout << ")"<<endl;
}

/**
 * This program performs a conflict detection on a vector<uint32_t>
 * 
 * 
 * 
 */
int main()
{
    size_t datasize = 256;
    vector<uint32_t> input(224);
    
    //unsorted input data
    
    uint32_t testdata[]={6,8,1,2,1,4,4,1,3,9,12,9,2,14,15,18,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    input.insert(input.begin(),testdata, testdata+32);
    
    //contains a permutatation list. index[i] contains the original position of each input element before sort
    vector<int> index(input.size(),0);
    for(int i=0;i!=index.size();i++){
        index[i]=i;
    }

    
    printvec(input,"input");

    //generate permutation list 
    sort(index.begin(),index.end(),
    [&](const int& a, const int& b) {
        return (input[a] < input[b]);
    }
);
    
    printvec(index, "permutations");

    sort(input.begin(),input.end()); 
    
    printvec(input,"sorted input");
    bitset<256> output ={0};
    
//do conflict detection
bitset<256> result = scalarCD(input);
bitset<256> resultcd =avxCD(input);

cout<< "conflicts: ";
for(int i =0;i<input.size();i++)
{
    cout<<" "<<result[i]<<" ";
}
cout <<endl;
cout<<"conflicts CD: ";
for(int i =0;i<input.size();i++)
{
    cout<<" "<<resultcd[i]<<" ";
}
cout <<endl;

for(int i =0;i<256;i++)
{

output[index[i]]=result[i];
}

cout<< "conflicts sorted: ";
for(int i =0;i<input.size();i++)
{
    cout<<" "<<output[i]<<" ";
}
cout <<endl;
cout <<"number of conflicts: "<<output.count()<<endl;

return 0;
}
