#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

class Waiter {

    public:
    vector<string> inst;
    int rowNum;
    int address;
    string addressReg = "";
    string changeReg = "";

};


unordered_map<int, Waiter> queue;
unordered_map<int, vector<int>> rowSort;

bool isOn;
    vector<string> currInst;
    int clock = 0;
    int relClock =0;
    int rowDelay=0;
    int colDelay=0;
    int updates[2] = {0};
    bool rowDone = false;

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}}; 
    
    int memory[1024][256] = {0};
    int rowBuffer[256] = {0};

    string addressReg = "";
    string changeReg = "";
    int address;
    int rowNum;
    int colNum;
    int currRow = -1;
    bool isFirst = true;
    string regSteps;
    int initialClock;
    bool updated = true;

int bounds(bool isMin) {
    int min = -1;
    int max = -1;
    for (auto& it : queue) {
        if (min == -1) {
            min = it.first;
        }

        else if(it.first < min) {
            min = it.first;
        }

        if (max < it.first) {
            max = it.first;
        }
    }

    if (isMin) {
        return min;
    }
    return max;
}



    void start(Waiter w) {
        currInst = w.inst;
        changeReg  = w.changeReg;
        addressReg = w.addressReg;
        address = w.address;
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        regSteps = "";
        initialClock = clock;
        rowDone = false;
    }


   void initWaiter(vector<string> v) {
        Waiter w;
        w.inst = v;
        w.changeReg  = v.at(1);

        int add;
        if (v.size() == 6) {
            w.addressReg = v.at(4);
            add = regs.at(v.at(4));
        }
        else if (v.size() == 7) {
            w.addressReg = v.at(5);
            add = regs.at(v.at(5))+stoi(v.at(3));
        }
        w.address = add;
        w.rowNum = add/1024;
        
        int max = bounds(false);
        queue[max+1] = w;
        rowSort[w.rowNum].push_back(max+1);
    }

bool currIsDep(vector<string> v) {
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (addressReg == v.at(1) || changeReg == v.at(1) || (currInst.at(0) == "lw" && (changeReg == v.at(3) || changeReg == v.at(5)))){
                return true;
            }
        }

        else if (task == "bne" || task == "beq"){
            if (currInst.at(0) == "lw" && (changeReg == v.at(1) || changeReg == v.at(3))){
                return true;
            }
        }  

        else if (task == "lw" || task == "sw") {
            string c1  = v.at(1);
            string cs = changeReg;
            string a1;
            string as = addressReg;
            if (v.size() == 6) {
                a1 = v.at(4);
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
            }

            if (currInst.at(0) == "lw") {

                if (task == "lw" && (c1 == as || a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || a1 == as)){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || c1 == as || a1 == as)){
                    return true;
                }

            }

        }
 
        return false;
    }

    bool isDep(vector<string> v, Waiter w) {
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (w.addressReg == v.at(1) || w.changeReg == v.at(1) || (w.inst.at(0) == "lw" && (w.changeReg == v.at(3) || w.changeReg == v.at(5)))){
                return true;
            }
        }

        else if (task == "bne" || task == "beq"){
            if (w.inst.at(0) == "lw" && (w.changeReg == v.at(1) || w.changeReg == v.at(3))){
                return true;
            }
        }   

        else if (task == "lw" || task == "sw") {
            string c1  = v.at(1);
            string cs = w.changeReg;
            string a1;
            string as = w.addressReg;
            if (v.size() == 6) {
                a1 = v.at(4);
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
            }

            if (w.inst.at(0) == "lw") {

                if (task == "lw" && (c1 == as || a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || a1 == as)){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || c1 == as || a1 == as)){
                    return true;
                }

            }

        }
        return false;
    }

    int depInRow(vector<string> v, int row){
        for (auto it = rowSort[row].rbegin(); it != rowSort[row].rend(); it++){
            if (isDep(v, queue[*it])){
                return *it;
            }
        }
        return -1;
    }

    vector<int[2]> allDep(vector<string> v) {
        vector<int[2]> ans;
        for ( const auto &it : rowSort ) {
            if (it.first != rowNum) {
                int curr = depInRow(v, it.first); 
                if (curr != -1) {
                    int arr[2] = {it.first,curr};
                    ans.push_back(arr);
                }
            }
        }
        return ans;
    }

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
