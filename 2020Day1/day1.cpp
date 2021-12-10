#include <fstream>
#include<vector>
#include<iostream>
using namespace std;


int main() {
    ifstream fin("input.txt");
    vector<int> numbers;
    int num;
    if(!fin.is_open()) {
        cout<<"wtf"<<endl;
    }
    while(fin>>num) {
        numbers.push_back(num);
    }
    cout<<numbers.size()<<endl;
    for(int i=0;unsigned(i)<numbers.size();i++) {
        for(int j=i+1; unsigned(j)<numbers.size();j++) {
            for(int k=j+1; unsigned(k) < numbers.size(); k++) {
            if(numbers.at(i)+numbers.at(j)+numbers.at(k) == 2020) {
                cout<<numbers.at(i)*numbers.at(j)*numbers.at(k)<<endl;
                break;
            }
            }
        }
    }
    return 0;
}
