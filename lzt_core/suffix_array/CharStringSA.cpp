#include <string.h>
#include <iosfwd>
#include <string>
#include <algorithm>

#include "CharStringSA.h"
#include "debug/StackTrace.h"

CharStringSA::CharStringSA(const char* s) {
    // set str = s + '$'
    L = strlen(s); L++;
    str = new char[L+1];
    strcpy(str, s);
    str[L] = 0;
    str[L-1] = '|';
    createSA();
}

CharStringSA::~CharStringSA() {
    delete [] sarray;
    delete [] lcp;
    delete [] str;
}

void CharStringSA::printSA(string fileName) {
    fstream file(fileName.c_str(), ios_base::out);
    // print the string
    file << str << endl << endl;
    // print suffixes and lcps
    for (int i = 0; i < L; ++i) {
        // print current suffix index, lcp and then the suffix
        file << setw(3) << i << setw(3) << sarray[i] << setw(3) << lcp[i] << " "
                << str + sarray[i] << endl;
    }
    file.close();
}

void CharStringSA::createSA() {
    // allocate and init array
    sarray = new int[L];
    for (int i = 0; i < L; ++i) sarray[i] = i;
    // sort suffixes lexicographically
    IndexCompare cmp(str);
    sort(sarray, sarray + L, cmp);

    // allocate and calculate lcp array
    lcp = new int[L];

    lcp[0] = 0;
    for (int i = 1; i <= L - 1; ++i)
        lcp[i] = calcLCP(str + sarray[i-1], str + sarray[i]);
}

// Calculate longest common prefix of two strings.
int CharStringSA::calcLCP(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != 0 && s2[i] != 0 && s1[i] == s2[i]) i++;
    
    return i;
}

void CharStringSA::computeLCPInt(bool details = true) {
    stack<TLcpInt> intStack;   
    TLcpInt init(0, 0, TLcpInt::UNDEF);
    intStack.push(init);
    // L is the length of string with terminating char appended
    for (int i = 1; i < L; ++i) {
        int lb = i - 1;

        if (details) {
            printStack(intStack);
            cout << "i: " << i << " lcp[i]: " << lcp[i] <<  endl;
        }

        while (lcp[i] < intStack.top().lcp) {
            if (details) {
                cout << "   lcp[i] (" << lcp[i]
                     << ") < top.lcp (" << intStack.top().lcp << ")" <<  endl;
            }

            intStack.top().rb = i - 1;
            TLcpInt ival = intStack.top();
            intStack.pop();
            // report interval;
            cout << "   interval: " << ival.toString() << endl;
            lb = ival.lb;
        }
        if (details) cout<<"end-while"<<endl;

        if (lcp[i] > intStack.top().lcp) {
            if (details) {
                cout << "   lcp[i] (" << lcp[i]
                     << ") > top.lcp (" << intStack.top().lcp << ")" <<  endl;
            }

            TLcpInt ival(lcp[i], lb, TLcpInt::UNDEF);
            intStack.push(ival);

            if (details) cout<<"   pushed: " << ival.toString() << endl;
        }
        
        if (details) cout << endl;
    }
}

// Compute longest previous factor array
void CharStringSA::computeLPF() {
    // ignoring special end-character
    L--;
    int* lpf = new int[L];
    sarray[L] = -1;
    lcp[L] = 0;
    stack<int> st; vector<int> buff;
    st.push(0);
    
    for (int i = 1; i <= L; ++i) {
        int top = st.top();

        cout << "i: " << i << " top: " << top << endl
                << "SA[i]: " << sarray[i]  << " LCP[i]: " << lcp[i]
                << " SA[top]: " << sarray[top] << " LCP[top]: " << lcp[top] << endl;
        cout << "stack: ";

        while (st.empty() == false) {
            cout << st.top() << " ";
            buff.insert(buff.begin(), st.top()); st.pop();
        }
        cout << endl;
        for (int j = 0; j < buff.size(); ++j) st.push(buff[j]);
        buff.clear();

        bool first = true;
        while ( ( st.empty() == false ) && (
                ( sarray[i] < sarray[top] ) ||
                ( sarray[i] > sarray[top] && lcp[i] <= lcp[top] ) ) ) {

            if (!first) {
                cout << "...while iter > 1" << endl;
                cout << "i: " << i << " top: " << top << endl
                    << "SA[i]: " << sarray[i]  << " LCP[i]: " << lcp[i]
                    << " SA[top]: " << sarray[top] << " LCP[top]: " << lcp[top] << endl;                                
            } else first = false;

            if (sarray[i] < sarray[top]) {
                cout << "case: SA[i] < SA[top]  " << endl;
                lpf[sarray[top]] = max(lcp[top], lcp[i]);
                lcp[i] = min(lcp[top], lcp[i]);
                cout << "LPF[SA[top]] = " << lpf[sarray[top]]
                        << " LCP[i] = " << lcp[i] << endl;
            }
            else {
                cout << "case: sarray[i] > sarray[top] && lcp[i] <= lcp[top] " << endl;
                lpf[sarray[top]] = lcp[top];
                cout << "LPF[SA[top]] = " << lpf[sarray[top]] << endl;
            }

            st.pop();

            if (st.empty() == false) top = st.top();
            else break;            
        }
        
        if (i < L) {
            st.push(i);
            cout << "push i: " << i << endl;
        }

        cout << endl;
    }


    sarray[L] = L;
    L++;
}

void CharStringSA::printStack(const stack<TLcpInt>& stck) {
    stack<TLcpInt> tempStck(stck);
    while (tempStck.empty() == false) {
        cout << tempStck.top().toString() << " ";
        tempStck.pop();
    }
    cout << endl;
}