#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Database.h"
#include <ctime>
#include <map>
#include <algorithm>

using namespace std;

/******************************************************************************
* Function:         void select
*                   implement the SELECT
* Where:
*                   vector<string> query - TODO
* Return:           void
* Error:            
*****************************************************************************/
void Database::select(vector<string> query){

  Table t; //tabella temporanea su cui operare
  Table result = new Table(); //tabella finale da stampare
  vector<bool> where_lines; //righe della tabella dove ha effetto la where
  map<string, vector<string>> query_map = mapQuery(query);
  int n_tables = query_map.at("FROM").size();
  vector<string>::iterator i;
  vector<int> fields_indexes;

  //checking if all the tables are in the database
  for (i = query_map.at("FROM").begin(); i != query_map.at("FROM").end(); ++i) {
    if (this->table_exists(*i) == false){
      cout << "One of the selected tables does not exists.";
      return; //or throw exception
    }
  }

  // controlla la necessità di concat di tabelle e salva in una variabile locale Table t la tabella su cui operare
  if (n_tables > 1) {
    Table t; //TODO: creare metodo per concatenazione tabelle (necessaria implementazione gestione chiavi esterne)
  } else {
    Table t = this->getTable(query_map.at("FROM").at(0));
  }

  //check WHERE conditions
  try {
    vector<string> where_condition = query_map.at("WHERE");
    where_lines = t.records_where(query);
  }catch(const std::out_of_range& oor) { //niente where nella query
    // TODO: assicurarsi che in questo caso la funzione ritorni un vettore tutto true
  }

  //aggiungo gli indici dei fields richiesti dalla select
  for (i = query_map.at("SELECT").begin(); i != query_map.at("SELECT").end(); ++i)
  {
    fields_indexes.push_back(t.get_field_index(*i));
  }

  //per ogni riga che rispetta le condizioni della where stampo il record corrispondente al field
  // TODO: maybe call a function here like printTable
  for (size_t row = 0; row < where_lines.size(); row++)
  {

    if( (where_lines.at(row)) == true){
        //TODO: stampa dei valori a video
        for (int j = 0; j < fields_indexes.size(); ++j) {
            //next variable is the index of the field
            int col = fields_indexes.at(j);
            //next variable contains the record to be printed
            string record = t.records.at(row).at(col);
            //TODO
        }
    }
  }
  

}


/******************************************************************************
* Function:         map<string, vector<string>> mapQuery
*                   maps a query from a string (TODO or a fstream)
* Where:
*                   vector<string> query - TODO
* Return:           map
* Error:            
*****************************************************************************/
map<string, vector<string>> mapQuery(vector<string> query){
  //TODO
  map<string, vector<string>> ret;
  vector<string> select;
  vector<string> from;

  //lettura file
  map.at("SELECT") = select;

  map["SELECT"] = select;
  map["FROM"] = from;
  return ret;
  
  //leggi il file
}