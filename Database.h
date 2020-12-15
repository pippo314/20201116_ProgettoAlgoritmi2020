#ifndef PROGETTOALGORITMI2020_DATABASE_H
#define PROGETTOALGORITMI2020_DATABASE_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <fstream>
#include <cstring>
#include <utility>

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "Table.h"

using namespace std;

struct T{
    string name; //nome della tabella
    Table table; //tabella contenente i valori
};

class Database {
private:
    vector<T> tables; //mappa di tabelle ordinate per nome
    static map<string,int> getWordsMap();   //dizionario delle parole di comando accettate
public:
    bool table_exists(string name); //controlla che esista la tabella
    vector<string> read_query_from_file(string filename = "db.txt");  //legge il file e riempe un vector con le parole contenute nella query
    bool runQuery(const vector<string>& query); //return true se è andato tutto bene
    void menu();    //stampa il menu che interagisce con l'utente
    static string commands(); //stampa i comandi possibili per l'utente
    void save_db(const string& filename = "db.txt"); //salva il db su file oggetto
    void load_db(string filename = "db.txt"); //legge db da file oggetto
    vector<T>::iterator find_table_by_name(string name); //restituisce la posizione nel vector delle tabelle
    bool drop_table(const string& table_name);  //TODO da finire? Miriana
    bool create_table(vector<string> query);    //crea la tabella in base alle istruzioni della query
    bool insert_values(vector<string> query);   //inserisce i valori in tabella data la query
    bool update(vector<string> query);  //aggiorna i valori in tabella data la query
    string print_DB();
    void select(vector<string> query);
    Table getTable(string name);
};


#endif //PROGETTOALGORITMI2020_DATABASE_H

