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

    int degree;
    vector<int> list_of_levels,keys;

    while (in.peek() != EOF) {
        int i;
        in >> i;        
        keys.push_back(i);        
    }
        
    string s = "";
    while (commands.peek() != EOF) {
        getline(commands, s);
        s.erase(remove(s.begin(), s.end(), '\n'), s.end());
        s.erase(remove(s.begin(), s.end(), '\r'), s.end());

        if (s.size() != 0){
            if (s.find("Degree=") != string::npos)
                degree = stoi(s.substr(s.find("=") + 1));

            else if (s.find("Level ") != string::npos)
                list_of_levels.push_back(stoi(s.substr(s.find(" ") + 1)));
        }
    }


    B_tree my_tree(degree);

    for (int i=0; i< keys.size();i++){
        my_tree.insert(i);
    }
    output << "Height=" << my_tree.getHeight() << endl;

    for (int i=0; i<list_of_levels.size();i++) {
        my_tree.print(i, output);
        output << endl;
    }

    return 0;
}