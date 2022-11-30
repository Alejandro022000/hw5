#include "b_tree.h"
#include "ArgumentManager.h"


using namespace std;

int main(int argc, char *argv[]) {
    ArgumentManager am(argc, argv);
    ofstream output(am.get("outputput"));
    ifstream commands(am.get("command"));
    ifstream in(am.get("input"));
    /*
    ifstream in("input1.txt");
    ifstream commands("command1.txt");
    ofstream output("output1.txt");
    */
    int degree,num;
    vector<int> list_of_levels,keys;
    string command;
    B_tree my_tree(degree);

    while (in.peek() != EOF) {        
        in >> num;        
        keys.push_back(num);        
    }
    while (commands.peek() != EOF) {
        getline(commands, command);

        if (command.find("Degree"))
            degree = stoi(command.substr(command.find("=") + 1));

        else if (command.find("Level "))
            list_of_levels.push_back(stol(command.substr(command.find(" ") + 1)));
    }
    for (int i: keys){
        my_tree.insert(i);
    }  
    output << "Height=" << my_tree.getHeight() << endl; 
    for (int i: list_of_levels) {
        my_tree.print(i, output);
        output << endl;
    }
    return 0;
}