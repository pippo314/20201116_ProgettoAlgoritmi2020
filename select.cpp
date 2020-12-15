#include "Database.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
map<string, vector<string>> mapQuery(vector<string> query, map<string, string> alias_map);


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
  vector<int> order_fields; // index of rows sorted as the order by requests
  map<string, string> alias_map; // tabella corispondenze per alias
  map<string, vector<string>> query_map = mapQuery(query, alias_map);
  int n_tables = query_map.at("FROM").size();
  vector<string>::iterator i;
  vector<int> fields_indexes;
  vector<vector<string>> print_me;

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
  if(query_map.at("WHERE").at(0) != "NONE") {
          where_lines = t.records_where(query);
  } else {
          //niente where nella query
  }
  if(query_map.at("ORDER BY").at(0) != "NONE") {
          order_fields = t.get_order_by_indexes(query_map.at("ORDER BY").at(0), *(query_map.at("ORDER BY").end()));
  } else {
          //niente order by nella query
  }
          


/*  try {
    vector<string> where_condition = query_map.at("WHERE");
    where_lines = t.records_where(query);
  } catch (const std::out_of_range& oor) { //niente where nella query
    // TODO: assicurarsi che in questo caso la funzione ritorni un vettore tutto true
  }

  try {
    order_fields = t.get_order_by_indexes(query_map.at("ORDER BY").at(0), *(query_map.at("ORDER BY").end()));
  } catch (const std::out_of_range& oor) { // no order by nella query
    // TODO: 
  }
*/


  //aggiungo gli indici dei fields richiesti dalla select
  for (i = query_map.at("SELECT").begin(); i != query_map.at("SELECT").end(); ++i)
  {
    fields_indexes.push_back(t.get_field_index(*i));
  }
  //TODO: print Fields first
  for (int j = 0; j < fields_indexes.size(); ++j) {
      int field_index = fields_indexes.at(j);
      string field_name = t.fields.at(field_index).name;
      if (alias_map.count(field_name) > 0) {
          string tmp = alias_map.at(field_name); //variable to be printed
          cout << setw(15) << tmp;
      } else {
          cout << setw(15) << field_name;
      }
  }

  cout << endl;

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
            cout << setw(15) << record;
        }
    }
    cout << endl;
  }
  

}


/******************************************************************************
* Function:         map<string, vector<string>> mapQuery
*                   maps a query from a string 
* Where:
*                   vector<string> query - TODO
* Return:           map
* Error:            
*****************************************************************************/
map<string, vector<string>> mapQuery(vector<string> query, map<string, string> alias_map){
  map<string, vector<string>> ret;
  vector<string> init = {"NONE"};
  vector<string> select;
  vector<string> from;
  vector<string> where;
  vector<string> order_by;
  vector<string>::iterator from_it = find(query.begin(), query.end(), "FROM");
  vector<string>::iterator where_it = find(query.begin(), query.end(), "WHERE");
  vector<string>::iterator order_it = find(query.begin(), query.end(), "ORDER BY");
  vector<string>::iterator it = query.begin() + 1; // esclude "SELECT"

  // ret vector init
  ret["SELECT"] = init;
  ret["FROM"] =  init;
  ret["WHERE"] = init;
  ret["ORDER BY"] = init;

  while(it != from_it){
          if(*it == "AS"){
                  alias_map[*(it - 1)] = *(it + 1);
                  it+=2;
          } else {
                  select.push_back(*it);
                  it++;
          }
  }
  while(it != where_it) {
          from.push_back(*it);
          it++;
  }
  while(it != order_it) {
          where.push_back(*it);
          it++;
  }
  while(it != query.end()){
          order_by.push_back(*it);
          it++;
  }

  ret["SELECT"] = select;
  ret["FROM"] = from;
  ret["WHERE"] = where;
  ret["ORDER BY"] = order_by;

  return ret;
}
