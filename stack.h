#ifndef STACK_H
#define STACK_H

#include <iomanip>
#include <iostream>

using namespace std;

template<class T, int N = 16>
class Stack {

private:

    T*  data;
    int top;
    int n;

public:

    Stack();

    void push(T t);
    T    pop();

    int  size();
    bool isEmpty();
    bool isFull();

    void print();
};

template<class T, int N>
Stack<T, N>::Stack() {

    data = new T[N];
    top  = -1;
    n    = N;
}

template<class T, int N>
void Stack<T, N>::push(T t) {

    if(size() < n) {

        data[++top] = t;
    } else {

        cerr << "Stack overflow..!" << endl;
        exit(EXIT_FAILURE);
    }
}

template<class T, int N>
T Stack<T, N>::pop() {

    if(size() > 0) {

        return data[top--];
    } else {

        cerr << "Stack underflow..!" << endl;
        exit(EXIT_FAILURE);
    }
}

template<class T, int N>
int Stack<T, N>::size() {
    return (top + 1);
}

template<class T, int N>
bool Stack<T, N>::isEmpty() {
    return (size() == 0);
}

template<class T, int N>
bool Stack<T, N>::isFull() {
    return (size() == n);
}

template<class T, int N>
void Stack<T, N>::print() {

    for(int i = (n - 1); i >= 0; i--) {

        if(i != top) {

            cout << setw(12) << setfill(' ') << " " << setw(12) << setfill('-')
                 << "-" << endl;

            cout << setw(11) << setfill(' ') << i << "|" << setw(12) << right
                 << data[i] << "|" << endl;

            continue;
        }

        cout << setw(12) << setfill(' ') << " " << setw(12) << setfill('-')
             << "-" << endl;

        cout << setw(11) << setfill(' ') << i << "|" << setw(12) << right
             << data[i] << "|"
             << " <- SP" << endl;
    }

    cout << setw(12) << setfill(' ') << " " << setw(12) << setfill('-') << "-"
         << endl;
}
#endif  // STACK_H