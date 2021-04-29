#include <iostream>
#include <fstream>
#include <vector>
#include "interpreter2.0.cpp"
using namespace std;

class MIPS2 : public MIPS {
    public:

    void printRegSet2(int k, vector<string> v, string ramRegSteps,string p){
        string task = v.at(0);
        regSteps = regSteps + "\n-----------------------------Line: " +to_string(k) + "----------------------------";


        if (ramRegSteps != ""){
            regSteps += ramRegSteps + "\n";   
        }

        regSteps+= "\nClock: " + to_string(ram.clock) +"\n  Instruction called: ";
        printInstSet(v);   

        if (task == "add" || task == "sub" || task == "mul" || task == "addi" || task == "slt"){
            regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(ram.regs[v.at(1)]); 
        }
        else {
            regSteps += "\n" + p;
        }

        p = "";
        if (ram.isOn) {
            string a;
            for (auto& it : ram.currInst) { 
                a = a + " " +  it; 
            }
            p += "\n";
            p += "\n  [DRAM Execution going on: " + a+  ']';
            p += "\n  [Row active: " + to_string(ram.rowNum) + "]";
            string temp = "";
            for (auto &it : ram.rowSort) {
                temp += to_string(it.first) + ":" + to_string(it.second.size()) + " ";
            }
            p = p + "\n  [Instructions in queue: " + to_string(ram.queue.size()) + " (" + temp + ")]";
            if (ram.queue.size() != 0) {
                p += "\n  [Queue: \n";
                p += ram.printQ() + "  ]";
            }
        }      

        regSteps = regSteps + p + "\n";
    }

    bool executeInst2(int mainInst){

        int i= mainInst;
        int n=instructions.size()+1;
        int k=1;
        string p;
        while(i<n){
            vector<string> v=instructions.at(i);
            auto pointer = lineCount.find(i);
            k=pointer->second;
            p = "";
            // cout << ram.printQ() << endl;
            string a= v.at(0);
            if (isError(v,k) ==false) {
                return false;
            }
            if (a=="add"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                instCount[0] += 1;
                i++;
            }
            else if (a=="sub"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                instCount[2] += 1;
                i++;
            }
            else if (a=="mul"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5)));
                instCount[3] += 1;
                i++;              
            }
            else if (a=="addi"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5)));
                instCount[1] += 1;
                i++;               
            }
            else if (a=="lw" || a=="sw"){

                // -----------------------------------ass4-addition------------------------------------------------
                int add;
                if (v.size() == 6) {
                    add = ram.regs.at(v.at(4));
                }
                else if (v.size() == 7) {
                    add = ram.regs.at(v.at(5))+stoi(v.at(3));
                }

                if (add%4 !=0 || add<0 || add >= 4*1024*256) {
                    cout << "INVALID address of memory at line "<< k<<endl;
                    return false;
                }
                ram.doLS(v);
                p = "\n  DRAM request issued";
                i++;
                if (a == "sw"){
                    instCount[8] += 1;    
                    
                }
                else {
                    instCount[7] += 1;
                    
                }
            }
            else if (a=="j"){

                //  ---------------------------------ass4-addition-------------------------------------------

                int a = labelLine[v.at(1)];
                ram.clock += 1;
                ram.relClock += 1;
                ram.check();
                instCount[9] += 1;
                i=a;
                p = "   Jumping to line: " + to_string(lineCount[i]);


            }
            else if (a=="beq"){

                ram.doLSnot(v);
                // ---------------------------------ass4-addition----------------------------------------------------

                if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                    i=labelLine[v.at(5)];
                    p = "   Jumping to line: " + to_string(lineCount[i]);
                }

                else {
                    p = "   No jumping";
                    i++;
                }
                instCount[4] += 1;
            }
            else if (a=="bne"){

                ram.doLSnot(v);
                // ---------------------------------ass4-addition----------------------------------------------------

                if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                    i=labelLine[v.at(5)];
                    p = "   Jumping to line: " + to_string(lineCount[i]);
                }

                else {
                    p = "   No jumping";
                    i++;
                } 
                instCount[5] += 1;              
            }
            else if (a=="slt"){

                ram.doLSnot(v);
                if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                    feedReg(v.at(1),1);
                }
                else {
                    feedReg(v.at(1),0);
                }  
                i++; 

                instCount[6] += 1;          
            }
            else {
                cout<<"Unrecognised operation at line no."<<i<<endl;
                return false;
                break;
            } 
            
            printRegSet2(k,v,ram.regSteps,p);
            ram.regSteps = "";
        }

        if (ram.isOn == true){
            ram.doAll();
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
        if (isspace(line[i])){
            if (str != "") {
                v.push_back(str);
                str = "";
            }
        }
        else if (line[i] == ',' || line[i] == ':' || line[i] == '(' || line[i] == ')') {
            if (str != "") {
                v.push_back(str);
                str = "";
            }
            v.push_back(string (1, line[i]));
        }
        else if (line[i] == '#') {
            if (str != "") {
                v.push_back(str);
                str = "";
            }
            break;
        }
        else {
            str += line[i];
        }
        i += 1;
    }
    if (str != "") {
        v.push_back(str);
    }
    return v;
}


int main(int argc, char const *argv[])
{

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

    MIPS2 program;
    program.setDelay(rowdel,coldel);


    int addressVal = 0;
    int lineVal = 0;
    
    bool isMain = false;
    bool isError = false;
    int mainInst = 1;

    while (getline(file,line)){
        vector<string> currLine = lineToken(line);
        if (currLine.size() ==0 || int(currLine.at(0).at(0)) == 0){
                lineVal += 1;
        }
        else if (currLine.size() > 1 && currLine.at(1) == ":") {
            lineVal += 1;
            if (currLine.size() != 2){
                isError = true;
                break;
            }
            if (currLine.at(0) == "main"){
                isMain = true;
                mainInst = addressVal + 1;
            }
            program.setLabel(currLine.at(0),addressVal+1);
        }
        else {
            addressVal+=1;
            lineVal += 1;
            program.setInst(addressVal,currLine);
            program.setInstLine(addressVal,lineVal);
        }
    }

    if (isError) {
        cout<<"Invalid label at line: " << lineVal << endl;
        return -1;
    }

    if (isMain == false){
        cout<< "Missing Main Label" << endl;
        return -1;
    }

    
    bool isDone;
    if (argc == 4 || stoi(argv[4]) == 0) {

        isDone = program.executeInst2(mainInst);
    }
    else {
        cout << "\n\nExecuting without reorder" << endl;
        isDone = program.executeInst(mainInst);
    }

    if (isDone == false) {
        return -1;
    }

    program.printReg();
    program.printClock();
    program.printInstCount();

    if (argc > 4) {
        string filename;

        if (stoi(argv[4]) == 0) {
            filename = "new.txt";
        }
        else {
            filename = "old.txt";
        }
        ofstream myfile; 

        myfile.open(filename);
        myfile<<"Clock "<<program.getClock()<<endl;
        myfile<<program.getOut()<<endl;
    }
    return 0;
}
