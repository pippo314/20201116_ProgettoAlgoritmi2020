#ifndef PROGETTOALGORITMI2020_TABLE_H
#define PROGETTOALGORITMI2020_TABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <ctime>
#include <fstream>
#include <cstring>

using namespace std;

struct Field{
    string name;
    string type;
    bool not_null;
    bool auto_increment;
};

class Table {
private:
    string primary_key; // nome della chiave primaria della tabella
    string secondary_key; //nome della chiave secondaria della tabella
    string name; //nome della tabella
public:
    void set_name(string n);
    vector<Field> fields; //vettore dei campi della tabella TODO rendo privato?
    vector<vector<string> > records; //mappa dei valori della tabella
    Table(); //costruttore
    Table(Table *t);
    void add_field(Field f);    //aggiunge un campo alla tabella
    bool add_record(vector<string> fields_name, vector<string> values);  //aggiunge il record alla tabella
    void set_primary_key(string key);   //setta il nome della chiave primaria
    string get_primary_key();   //restituisce il nome della chiave primaria
    void set_secondary_key(string key); //setta il nome della chiave secondaria
    string get_secondary_key(); //rstituisce il nome della chiave secondaria
    vector<Field>::iterator get_field_number(string name);  //restituisce la posizione nel vettore del campo
    vector<bool> records_where(vector<string> query);   //restituisce il vettore delle righe della tabella (true) dove ha effetto la where
    map<string, int> getWhereMap(); //operatori della where
    void set_w(bool *_and, vector<bool> ret, int r);    //serve alla where
    float stof(string basicString); //converte string a float
};



#endif //PROGETTOALGORITMI2020_TABLE_H
