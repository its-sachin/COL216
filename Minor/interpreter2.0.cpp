#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
using namespace std;

struct node {
    int value;
    struct node *prev;
};

class Stack {
    public:
    struct node* first=NULL;
    void push(int value){
        struct node* a=  (struct node*) malloc (sizeof(struct node));
        a->value=value;
        a->prev=first;
        first=a;
    }
    int pop(){
        if (first==NULL){
            throw "Empty Stack"; 
            return 0;
        }
        else {  
            return first->value;
            first=first->prev;
        }
    }
};

string decToHex(int dec){
    string str="";
    char c;
    if (dec==0){
        str="0";
    }
    while (dec !=0){
        int temp=0;
        temp=dec%16;
        if (temp<10){
            c=temp+48;
        }
        else {
            c=temp+55;
        }
        string s(1,c);
        str=s+str;
        dec= dec /16;
    }
    return str;
}

class MIPS {
    private:
    unordered_map<int, vector<string>> instructions;
    unordered_map<int,int> lineCount;
    int memory[1024][256] = {0};
    int currRow = -1;
    int rowBuffer[256] = {0};
    int clock = 0;
    int rowDelay=0;
    int colDelay=0;
    bool isFirst = true;
    bool updated = true;
    string regSteps = "";
    int updates[2] = {0};

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};   
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0}; 
    
    Stack S;
    public:
    int getRegValue(string s){
        return regs.at(s);
    }
    void feedReg(string s,int value){
        regs.at(s)=value;
    }

    bool isReg(string s){
        if (regs.find(s) == regs.end()) {
            return false;
        }
        return true;
    }

    // ------------instruction handling--------------------------

    vector<string> getInst(int line){
        
        unordered_map<int, vector<string>>:: iterator it = instructions.find(line);
        return (it->second);
    }

    void setInst(int line, vector<string> input){
        instructions[line] = input;
    }

    void setInstLine(int addressInst, int line) {
        lineCount[addressInst] = line;
    }

    bool isInstPos(int line){
        if (line%4 != 0) {
            return false;
        }
        if (line/4+1 <= 0 || line/4+1 > instructions.size()){
            return false;
        }
        return true;
    }

    void setDelay(int rowd, int cold){
        rowDelay = rowd;
        colDelay = cold;
    }

    // -------------memory handling-------------------------

    bool activateRow(int rowNum){
        bool out = false;
        if (rowNum != currRow){
            for (int i=0; i <256; i++){
                if (isFirst == false){
                    memory[currRow][i] = rowBuffer[i];
                }
                rowBuffer[i] = memory[rowNum][i];
            }
            if (isFirst == false) {
                regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + rowDelay) +"\n  DRAM: Writeback Row: " + to_string(currRow);
                clock += rowDelay;
                out = true;
            }
            else {
                out = false;
            }
            isFirst = false;
            regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + rowDelay) +"\n  DRAM: Activate Row: " + to_string(rowNum);
            clock += rowDelay;
            currRow = rowNum;
            updates[0] += 1;
        }
        else {
            out = false;
        }
        return out;
    }

    void finalWrite() {
        for (int i=0;i<256;i++) {
            memory[currRow][i] = rowBuffer[i];
        }
        // regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + rowDelay) +"\nDRAM: Writeback Row: " + to_string(currRow);
        // clock += rowDelay;
    }

    bool sw(int address,int num ){
        if (address%4 != 0 || address<0) {
            cout<< "INVALID Address of memory"<<endl;
            return false;
        }
        
        else if (address >= 4*1024*256){
            cout<<"Memory Overflow!!"<<endl;
        }
        else{
            int colNum = (address%1024)/4;
            
            int rowNum = address/1024;
            updated=  activateRow(rowNum);
            regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
            updates[1] += 1;
            clock += colDelay;
            rowBuffer[colNum] = num;
            return true;            
        }
    }

    bool lw(int address, string reg) {
        if (regs.find(reg) == regs.end()) {
            cout << "INVALID register" << endl;
            return false;
        }
        if (address%4 != 0 || address<0){
            cout << "INVALID address of memory" << endl;
            return false; 
        }
        else if (address >= 4*1024*256){
            cout<<"Memory Overflow!!"<<endl;
        }
        else {
            int colNum = (address%1024)/4;
            
            int rowNum = address/1024;
            updated = activateRow(rowNum);
            regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
            clock += colDelay;
            updates[1] += 1;
            regs[reg] = rowBuffer[colNum];
            return true;            
        }             
    }

    void push(int value){
        S.push(value);
    }

    int pop(){
        try{
            return S.pop();
        } catch (const char* msg){
            cerr<<msg<<endl;
        }
    }\

    void printInstSet(vector<string> v){
        for (auto& it : v) { 
            string str = "";
            if (it != ")" && it != "(" && it != ",") {str = " ";}
            regSteps = regSteps + str +  it; 
        } 
    }

    void printRegSet(int k, vector<string> v){
        string task = v.at(0);
        if (task != "lw" && task != "sw"){
            regSteps = regSteps + "\n" + "\n-----------------------------Line: " +to_string(k) + "----------------------------" +  "\nClock: " + to_string(clock) +"\n  Instruction called: ";

            printInstSet(v);            

            if (task == "add" || task == "sub" || task == "mul" || task == "addi" || task == "slt"){
                regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(regs[v.at(1)]); 
            }

            regSteps = regSteps + "\n";
        }
    }

    void printReg(){
        cout<< regSteps + "\n"<<endl;
    }

    void printClock(){
        cout<< "------------------------------------------Execution complete-------------------------------------\n"<<endl;
        cout<< "Number of clock Cycles: " << clock <<"\n" <<endl;
    }

    void printChangeMem() {
        cout << "Memory Updates" << endl;
        for(int i =0 ; i<1024; i++) {
            for (int j =0; j<256; j++) {
                if (memory[i][j] != 0) {
                    cout<< 4*(j +256*i) << "-" << 4*(j +256*i) +3<< " =" <<memory[i][j] <<endl;
                }
            }
        }
    }

    void printregVal() {
        cout << "\nNon zero register values:"<<endl;

         for (map<string, int>::iterator itr = regs.begin(); itr != regs.end(); ++itr) { 
             if (itr->second != 0){
                cout << itr->first  << " = " << itr->second << " | "; 
             }
        } 
    }

    void printInstCount() {
        cout<< "Number of row buffer updates: " << updates[0]<<endl;
        cout<< "Total number of changes made in row buffer: " << updates[1]<< "\n" << endl;

        printChangeMem();
        cout<< "\nExecution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
        printregVal();
    }

    bool isError(vector<string> v,int line) {

        
        string task = v.at(0);
        if (task == "add" || task == "sub" || task == "mul"  || task =="addi") {
            if (v.size() != 6){
                cout <<"Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (v.at(2) != "," || v.at(4) != ","){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) ==false || isReg(v.at(3)) ==false || (task != "addi" && isReg(v.at(5)) == false)) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (v.at(1) == "$zero"){
                cout << "Syntax error: Cannot change value of zero register at line: "<<line<<endl;
                return false;
            }
            else if (task == "addi"){
                try{
                    stoi(v.at(5));
                }	
                catch(exception &err)
                {
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }

            return true;
        }
        else if (task == "bne" || task == "beq" || task == "slt") {
            if (v.size() != 6 || v.at(2) != "," || v.at(4) != ",") {
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) == false || isReg(v.at(3)) == false) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (task != "slt"){
                try {
                    int temp = stoi(v.at(5));
                    if (isInstPos(temp) == false){
                        cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            else {
                if (isReg(v.at(5)) ==false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                } 
            }
            return true;
        }

        else if (task == "j"){
            if (v.size() != 2){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else{
                try {
                    int temp = stoi(v.at(1));
                    if (isInstPos(temp) == false){
                         cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            return true;
        }

        else if (task == "lw" || task == "sw") {
            if (v.size() <6 || v.size() > 7){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }

            if (task == "lw" && v.at(1) == "$zero") {
                cout << "Syntax error: Cannot load value to zero register at line: "<<line<<endl;
                return false;
            }

            if (v.size() == 6){
                if (v.at(2) != "," || v.at(3) != "(" || v.at(5) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(4)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                return true;
            }

            else{
                if (v.at(2) != "," || v.at(4) != "(" || v.at(6) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(5)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                else{
                    try{
                        stoi(v.at(3));
                    }	
                    catch(exception &err)
                    {
                        cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                        return false;
                    }
                }
                return true;
            }
        }

        cout<<"Instruction not defined at line: " << line << endl;
        return false;
    }


    //exucuting the file 
    bool executeInst(){
        int i=1;
        int n=instructions.size()+1;
        int k=1;
        while(i<n){
            vector<string> v=instructions.at(i);
            auto pointer = lineCount.find(i);
            k=pointer->second;
                string a= v.at(0);
                if (isError(v,k) ==false) {
                    return false;
                }
                if (a=="add"){
                    feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                    clock += 1;
                    instCount[0] += 1;
                    i++;
                }
                else if (a=="sub"){
                    feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                    clock += 1;
                    instCount[2] += 1;
                    i++;
                }
                else if (a=="mul"){
                    feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5))); 
                    clock += 1; 
                    instCount[3] += 1;
                    i++;              
                }
                else if (a=="sw"){
                    clock += 1;
                    regSteps = regSteps + "\n" +  "\n-----------------------------Line: " +to_string(k) + "----------------------------" + "\nClock: " + to_string(clock) +"\n  Instruction called: ";
                    printInstSet(v);
                    regSteps += "\n  DRAM request issued";
                    int addModify;
                    if (v.size()==7){
                        bool done = sw(getRegValue(v.at(5))+stoi(v.at(3)),getRegValue(v.at(1)));
                        if (done == false) {return false;}
                        addModify = getRegValue(v.at(5))+stoi(v.at(3));
                    }
                    else if (v.size()==6){
                        bool done = sw(getRegValue(v.at(4)),getRegValue(v.at(1))); 
                        if (done == false) {return false;}
                        addModify = getRegValue(v.at(4));
                    }
                    instCount[8] += 1;
                    i++ ;      
                    regSteps = regSteps+ "\n  Memory address modified : " + to_string(addModify) + " = " + to_string(regs[v.at(1)]);        
                }
                else if (a=="addi"){
                    feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5))); 
                    clock += 1;
                    instCount[1] += 1;
                    i++;               
                }
                else if (a=="lw"){
                    clock += 1;
                    regSteps = regSteps + "\n"  +"\n-----------------------------Line: " +to_string(k) + "----------------------------"+  "\nClock: " + to_string(clock) + "\n  Instruction called: ";
                    printInstSet(v);
                    regSteps += "\n  DRAM request issued";
                    if (v.size()==7){
                        bool done = lw(getRegValue(v.at(5))+stoi(v.at(3)),v.at(1));
                        if (done == false) {return false;}
                    }
                    else if (v.size()==6){
                        bool done = lw(getRegValue(v.at(4)),v.at(1)); 
                        if (done == false) {return false;}
                    }
                    instCount[7] += 1;
                    i++;
                    regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(regs[v.at(1)]);
                }
                else if (a=="j"){
                    int a=stoi(v.at(1));
                    if (a<1 || a>instructions.size()){
                        cout<<"invalid jump line at line no"<<i<<endl;
                        break;
                    }
                    else {
                        clock += 1;
                        instCount[9] += 1;
                        i=a/4 +1;
                    }
                }
                else if (a=="beq"){
                    if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                        i=stoi(v.at(5))/4+1;
                    }
                    else {
                        i++;
                    }
                    clock += 1; 
                    instCount[4] += 1;             
                }
                else if (a=="bne"){
                    if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                        i=stoi(v.at(5))/4+1;
                    }
                    else {
                        i++;
                    } 
                    clock += 1; 
                    instCount[5] += 1;              
                }
                else if (a=="slt"){
                    if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                        feedReg(v.at(1),1);
                    }
                    else {
                        feedReg(v.at(1),0);
                    }  
                    i++; 
                    clock += 1; 
                    instCount[6] += 1;          
                }
                else {
                    cout<<"Unrecognised operation at line no."<<i<<endl;
                    return false;
                    break;
                } 
                printRegSet(k,v);
        }
        if (updated == false){
            finalWrite();
        }
        return true;

    }
};


class DRAM {
    public:
    bool isOn;
    vector<string> currInst;
    int clock = 0;
    int relClock =0;
    int rowDelay=0;
    int colDelay=0;
    int updates[2] = {0};

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

    void start(vector<string> v){
        currInst = v;
        changeReg  = currInst.at(1);
        if (currInst.size() == 6) {
            addressReg = currInst.at(4);
            address = regs.at(v.at(4));
        }
        else if (currInst.size() == 7) {
            addressReg = currInst.at(5);
            address = regs.at(v.at(5))+stoi(v.at(3));
        }
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        regSteps = "";
        initialClock = clock;

    }

    void check() {
        if (relClock == rowDelay){
            if (rowNum != currRow){
                for (int i=0; i <256; i++){
                    if (isFirst == false){
                        memory[currRow][i] = rowBuffer[i];
                    }
                    rowBuffer[i] = memory[rowNum][i];
                }
                if (isFirst == false) {
                    regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+rowDelay) +"\n  DRAM: Writeback Row: " + to_string(currRow);
                    initialClock += rowDelay;
                    updated = true;

                }
                else {
                    updated = false;
                }
                isFirst = false;
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+rowDelay) +"\n  DRAM: Activate Row: " + to_string(rowNum);
                initialClock += rowDelay;
                updates[0] += 1;
                currRow = rowNum;
            }
            else {
                updated = false;
            }
        }
        else if (relClock == rowDelay +colDelay) {
            if (currInst.at(0) == "lw") {
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock +colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                regs[changeReg] = rowBuffer[colNum];
                regSteps = regSteps+ "\n  Register modified : " + changeReg + " = " + to_string(regs[changeReg]);
                updates[1] += 1;
            }
            else if (currInst.at(0) == "sw"){
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                rowBuffer[colNum] = regs[changeReg];
                regSteps = regSteps+ "\n  Memory address modified : " + to_string(address) + " = " + to_string(regs[changeReg]);
                updates[1] += 1;
            }
            isOn = false;
        }
    }

    void finalWrite() {
        for (int i=0;i<256;i++) {
            memory[currRow][i] = rowBuffer[i];
        }
        // regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + rowDelay) +"\nDRAM: Writeback Row: " + to_string(currRow);
        // clock += rowDelay;
    }
};






class MIPS2 {
    private:
    unordered_map<int, vector<string>> instructions;
    unordered_map<int,int> lineCount;
    string regSteps = "";

    DRAM ram;  
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0}; 
    
    Stack S;
    public:
    int getRegValue(string s){
        return ram.regs.at(s);
    }
    void feedReg(string s,int value){
        ram.regs.at(s)=value;
    }

    bool isReg(string s){
        if (ram.regs.find(s) == ram.regs.end()) {
            return false;
        }
        return true;
    }

    // ------------instruction handling--------------------------

    vector<string> getInst(int line){
        
        unordered_map<int, vector<string>>:: iterator it = instructions.find(line);
        return (it->second);
    }

    void setInst(int line, vector<string> input){
        instructions[line] = input;
    }

    void setInstLine(int addressInst, int line) {
        lineCount[addressInst] = line;
    }

    bool isInstPos(int line){
        if (line%4 != 0) {
            return false;
        }
        if (line/4+1 <= 0 || line/4+1 > instructions.size()){
            return false;
        }
        return true;
    }

    void setDelay(int rowd, int cold){
        ram.rowDelay = rowd;
        ram.colDelay = cold;
    }


    void push(int value){
        S.push(value);
    }

    int pop(){
        try{
            return S.pop();
        } catch (const char* msg){
            cerr<<msg<<endl;
        }
    }

    void printInstSet(vector<string> v){
        for (auto& it : v) { 
            string str = "";
            if (it != ")" && it != "(" && it != ",") {str = " ";}
            regSteps = regSteps + str +  it; 
        } 
    }

    void printRegSet(int k, vector<string> v, string ramRegSteps,bool c){
        string task = v.at(0);
        if (task != "lw" && task != "sw"){
            regSteps = regSteps + "\n-----------------------------Line: " +to_string(k) + "----------------------------";
            if (ramRegSteps != ""){
                regSteps += ramRegSteps + "\n";   
            }
            regSteps+= "\nClock: " + to_string(ram.clock) +"\n  Instruction called: ";


            printInstSet(v);            

            if (task == "add" || task == "sub" || task == "mul" || task == "addi" || task == "slt"){
                regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(ram.regs[v.at(1)]); 
            }

            if (c==false) {
                regSteps += "\n  **DRAM Execution going on**";
            }

        }
        regSteps = regSteps + "\n";
    }

    void printReg(){
        cout<< regSteps + "\n"<<endl;
    }

    void printClock(){
        cout<< "------------------------------------------Execution complete-------------------------------------\n"<<endl;
        cout<< "Number of clock Cycles: " << ram.clock <<"\n" <<endl;
    }

    void printChangeMem() {
        cout << "Memory Updates" << endl;
        for(int i =0 ; i<1024; i++) {
            for (int j =0; j<256; j++) {
                if (ram.memory[i][j] != 0) {
                    cout<< 4*(j +256*i) << "-" << 4*(j +256*i) +3<< " =" <<ram.memory[i][j] <<endl;
                }
            }
        }
    }
    void printregVal() {
        cout << "\nNon zero register values:"<<endl;

         for (map<string, int>::iterator itr = ram.regs.begin(); itr != ram.regs.end(); ++itr) { 
             if (itr->second != 0){
                cout << itr->first  << " = " << itr->second << " | "; 
             }
        } 
    }

    void printInstCount() {
        cout<< "Number of row buffer updates: " << ram.updates[0]<<endl;
        cout<< "Total number of changes made in row buffer: " << ram.updates[1]<< "\n" << endl;

        printChangeMem();
        cout<< "\nExecution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
        printregVal();
    }

    bool isError(vector<string> v,int line) {

        
        string task = v.at(0);
        if (task == "add" || task == "sub" || task == "mul"  || task =="addi") {
            if (v.size() != 6){
                cout <<"Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (v.at(2) != "," || v.at(4) != ","){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) ==false || isReg(v.at(3)) ==false || (task != "addi" && isReg(v.at(5)) == false)) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (v.at(1) == "$zero"){
                cout << "Syntax error: Cannot change value of zero register at line: "<<line<<endl;
                return false;
            }
            else if (task == "addi"){
                try{
                    stoi(v.at(5));
                }	
                catch(exception &err)
                {
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }

            return true;
        }
        else if (task == "bne" || task == "beq" || task == "slt") {
            if (v.size() != 6 || v.at(2) != "," || v.at(4) != ",") {
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) == false || isReg(v.at(3)) == false) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (task != "slt"){
                try {
                    int temp = stoi(v.at(5));
                    if (isInstPos(temp) == false){
                        cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            else {
                if (isReg(v.at(5)) ==false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                } 
            }
            return true;
        }

        else if (task == "j"){
            if (v.size() != 2){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else{
                try {
                    int temp = stoi(v.at(1));
                    if (isInstPos(temp) == false){
                         cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            return true;
        }

        else if (task == "lw" || task == "sw") {
            if (v.size() <6 || v.size() > 7){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }

            if (task == "lw" && v.at(1) == "$zero") {
                cout << "Syntax error: Cannot load value to zero register at line: "<<line<<endl;
                return false;
            }

            if (v.size() == 6){
                if (v.at(2) != "," || v.at(3) != "(" || v.at(5) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(4)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                return true;
            }

            else{
                if (v.at(2) != "," || v.at(4) != "(" || v.at(6) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(5)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                else{
                    try{
                        stoi(v.at(3));
                    }	
                    catch(exception &err)
                    {
                        cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                        return false;
                    }
                }
                return true;
            }
        }

        cout<<"Instruction not defined at line: " << line << endl;
        return false;
    }


    bool ramCheck(vector<string> v){
        if (ram.isOn){
            string task = v.at(0);
            if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt"){
                if (ram.addressReg == v.at(1) || ram.changeReg == v.at(1) || (ram.currInst.at(0) == "lw" && (ram.changeReg == v.at(3) || ram.changeReg == v.at(5)))){
                    if (ram.relClock < ram.rowDelay){
                        ram.clock = ram.rowDelay + ram.initialClock ; 
                        ram.relClock = ram.rowDelay;
                        ram.check();
                    }
                    ram.clock = ram.colDelay + ram.initialClock; 
                    ram.relClock = ram.colDelay + ram.rowDelay;
                    ram.check();
                    return true;
                }
                else {
                    ram.relClock+=1;
                    ram.check();
                    return false;
                }
            }

            else if (task == "bne" || task == "beq") {
                if (ram.currInst.at(0) == "lw" && (ram.changeReg == v.at(1) || ram.changeReg == v.at(3))){
                    if (ram.relClock < ram.rowDelay){
                        ram.clock = ram.rowDelay + ram.initialClock ; 
                        ram.relClock = ram.rowDelay;
                        ram.check();
                    }
                    ram.clock = ram.colDelay + ram.initialClock; 
                    ram.relClock = ram.colDelay + ram.rowDelay;
                    ram.check();
                    return true;
                }
                else {
                    ram.relClock+=1;
                    ram.check();
                    return false;
                }
            }

        }
        return true;
    }


    void doit(){
        if (ram.isOn){
            if (ram.relClock < ram.rowDelay){
                ram.clock = ram.rowDelay + ram.initialClock ; 
                ram.relClock = ram.rowDelay;
                ram.check();
            }
            ram.clock = ram.colDelay + ram.initialClock; 
            ram.relClock = ram.colDelay + ram.rowDelay;
            ram.check();   
        }
    }

    //exucuting the file 
    bool executeInst(){
        int i=1;
        int n=instructions.size()+1;
        int k=1;
        bool c = true;
        while(i<n){
            vector<string> v=instructions.at(i);
            auto pointer = lineCount.find(i);
            k=pointer->second;
                string a= v.at(0);
                if (isError(v,k) ==false) {
                    return false;
                }
                if (a=="add"){
                    c=ramCheck(v);
                    ram.clock+=1;
                    feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                    instCount[0] += 1;
                    i++;
                }
                else if (a=="sub"){
                    c=ramCheck(v);
                    ram.clock+=1;
                    feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                    instCount[2] += 1;
                    i++;
                }
                else if (a=="mul"){
                    c=ramCheck(v);
                    ram.clock+=1;
                    feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5)));
                    instCount[3] += 1;
                    i++;              
                }
                else if (a=="addi"){
                    c=ramCheck(v);
                    ram.clock+=1;
                    feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5)));
                    instCount[1] += 1;
                    i++;               
                }
                else if (a=="lw" || a=="sw"){
                    doit();
                    ram.clock += 1;
                    regSteps += ram.regSteps + "\n";
                    regSteps = regSteps  +"-----------------------------Line: " +to_string(k) + "----------------------------"+  "\nClock: " + to_string(ram.clock) + "\n  Instruction called: ";
                    printInstSet(v);
                    regSteps += "\n  DRAM request issued";
                    ram.start(v);
                    if (ram.address%4 !=0 || ram.address <0 || ram.address >= 4*1024*256){
                        cout << "INVALID address of memory at line "<< k<<endl;
                        return false;
                    }
                    i++;
                    if (a == "sw"){
                        instCount[8] += 1;    
                        
                    }
                    else {
                        instCount[7] += 1;
                        
                    }
                }
                else if (a=="j"){
                    int a=stoi(v.at(1));
                    if (a<1 || a>instructions.size()){
                        cout<<"invalid jump line at line no"<<i<<endl;
                        break;
                    }
                    else {
                        ram.clock += 1;
                        ram.relClock += 1;
                        ram.check();
                        instCount[9] += 1;
                        i=a/4 +1;
                    }
                }
                else if (a=="beq"){
                    c=ramCheck(v);
                    if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                        i=stoi(v.at(5))/4+1;
                    }
                    else {
                        i++;
                    }
                    ram.clock += 1; 
                    instCount[4] += 1;             
                }
                else if (a=="bne"){
                    c=ramCheck(v);
                    if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                        i=stoi(v.at(5))/4+1;
                    }
                    else {
                        i++;
                    } 
                    ram.clock += 1; 
                    instCount[5] += 1;              
                }
                else if (a=="slt"){
                    c=ramCheck(v);
                    if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                        feedReg(v.at(1),1);
                    }
                    else {
                        feedReg(v.at(1),0);
                    }  
                    i++; 
                    ram.clock += 1; 

                    instCount[6] += 1;          
                }
                else {
                    cout<<"Unrecognised operation at line no."<<i<<endl;
                    return false;
                    break;
                } 
                printRegSet(k,v,ram.regSteps,c);
                ram.regSteps = "";
        }

        if (ram.isOn == true){
            doit();
            regSteps += ram.regSteps + "\n";
        }
        if (ram.updated == false) {
            ram.finalWrite();
        }
        return true;

    }
};


vector<string> lineToken(string line) {
    vector<string> v;
    int n=line.length();
    int i=0;
    string str="";
    while (i<n){
        char c=line[i];
        while (i<n && (c==' '|| c=='\t')){
            i++;
            if (str!=""){
                v.push_back(str);
                str="";
            }
            c=line[i];
        }
        if (i >= n){break;}
        if (c=='#'){
            if (str!=""){
                v.push_back(str);
            }
            break;
        }
        if (c==',' || c=='(' || c==')'){
            if (str!=""){
                v.push_back(str);
                str="";
            }
            string sc(1,c);
            v.push_back(sc);
        }
        else {
            string s(1,c);
            str=str+s; 
        }
        i++;      
    }
    if (str!=""){
        v.push_back(str);
        str="";
    }
    return v;
}


int main(int argc, char** argv) {

    if (argc == 1) {
        cout << "Enter the file name\n" << endl;
        return 0;
    }

    string infile = argv[1];
    string line;
    ifstream file(infile);

    if (argc < 4){
        cout << "Enter delays \n" << endl;
        return 0;
    }
    int rowdel =  stoi(argv[2]);
    int coldel = stoi(argv[3]);

    if (rowdel < 0 || coldel < 0){
        cout << "Delays can not be negative \n" << endl;
        return 0;
    }

    if (argc < 5) {
        cout << "Enter mode (""a"" for part a and ""b"" for part b) \n" << endl;
        return 0;
    }

    string mode = argv[4];
    if (mode != "a" && mode != "b") {
        cout << "Enter valid mode (""a"" for part a and ""b"" for part b) \n" << endl;
        return 0;
    }



    if (mode == "a") {
        MIPS program;
        program.setDelay(rowdel,coldel);


        int addressVal = 0;
        int lineVal = 0;
        while (getline(file,line)){
            vector<string> currLine = lineToken(line);
            if (currLine.size() ==0 || currLine.at(0) == "main:" || currLine.at(0) == "exit:" || int(currLine.at(0).at(0)) == 0){
                    lineVal += 1;
                }
            else {
                addressVal+=1;
                lineVal += 1;
                program.setInst(addressVal,currLine);
                program.setInstLine(addressVal,lineVal);
            }
        }

        bool isDone = program.executeInst();

        if (isDone == false) {
            return -1;
        }
        else {
            program.printReg();
            program.printClock();
            program.printInstCount();
            return 0;
        }
    }

    else if (mode == "b"){
        MIPS2 program;
        program.setDelay(rowdel,coldel);


        int addressVal = 0;
        int lineVal = 0;
        while (getline(file,line)){
            vector<string> currLine = lineToken(line);
            if (currLine.size() ==0 || currLine.at(0) == "main:" || currLine.at(0) == "exit:" || int(currLine.at(0).at(0)) == 0){
                    lineVal += 1;
                }
            else {
                addressVal+=1;
                lineVal += 1;
                program.setInst(addressVal,currLine);
                program.setInstLine(addressVal,lineVal);
            }
        }

        bool isDone = program.executeInst();

        if (isDone == false) {
            return -1;
        }
        else {
            program.printReg();
            program.printClock();
            program.printInstCount();
            return 0;
        }
    }

}