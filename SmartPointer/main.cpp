#include <iostream>
using namespace std;
class IntPtr{

private:
    int* _ptr;

public:
    IntPtr() {
        cout << "in default" << endl;
        _ptr = new int(0); // or set to nullptr but may cause issues with dereference
    }
    IntPtr(int x){
        cout << "in one-arg" << endl;
        _ptr = new int(x);
    }
    IntPtr(const IntPtr &ip){
        cout << "in copy constructor" << endl;
        _ptr = new int(*ip);
    }
    ~IntPtr(){
        cout << "in destructor" << endl;
        delete _ptr;
    }
    int getInt(){
        return *_ptr;
    }
    void setInt(int x){
        *_ptr = x;
    }
    // may still want to support dereferencing for programmers that like the notation
    // the following will be used in most cases unless a const is required
    int& operator*(){
        cout << "oper*" << endl;
        return *_ptr;
    }

    // the following method will be used when a const is required
    // for example when passing the object by const
    // const as return type required if const used as an l-value mistakenly in const function
    // const on method so this method is prevented from changing _ptr however it can change
    // the value pointed to by _ptr (*_ptr = 3; will work)
    const int& operator* () const{
        cout << "const oper*" << endl;
        return *_ptr;
    }
};

void func(const IntPtr& ip){
    cout << "in func" << endl;
//    cout << "1: " << (*ip)++ << endl;
    cout << "2: " << *ip << endl;
}

int main() {
    IntPtr ip;
    cout << *ip << endl;
    *ip = 12;
    cout << *ip << endl;
    (*ip)++;
    cout << *ip << endl;
    func(ip);
    cout << *ip << endl;
    IntPtr ip2 = ip;
    return 0;
}