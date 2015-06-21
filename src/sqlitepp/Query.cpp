/*
 **        Query.cpp
 **
 **        Published / author: 2005-08-12 / grymse@alhem.net
 **/

/*
Copyright (C) 2001-2006  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sqlite3.h>
#include <iostream>

#include "Database.h"
#include "Query.h"
#include "boost/lexical_cast.hpp"

#ifdef SQLITEW_NAMESPACE
namespace SQLITEW_NAMESPACE {
#endif


Query::Query(Database& dbin) 
: m_db(dbin)
,odb(dbin.grabdb())
,res(NULL)
,row(false)
,cache_rc(0)
,cache_rc_valid(false)
,m_row_count(0)
{
}


Query::Query(Database& dbin,const std::string& sql) 
: m_db(dbin)
,odb(dbin.grabdb())
,res(NULL)
,row(false)
,cache_rc(0)
,cache_rc_valid(false)
,m_row_count(0)
{
        execute(sql);
}

Query::~Query(void)
{
        if(res)
        {
                GetDatabase().error(*this, "sqlite3_finalize in destructor");
                sqlite3_finalize(res);
        }
        if (odb)
        {
                m_db.freedb(odb);
        }
}


Database& Query::GetDatabase() const
{
        return m_db;
}


/*
The sqlite3_finalize() routine deallocates a prepared SQL statement. 
All prepared statements must be finalized before the database can be closed.
*/
bool Query::execute(const std::string& sql)
  {
    // query, no result
    m_last_query = sql;
    if(odb && res)
      { GetDatabase().error(*this, "execute: query busy"); }
    if(odb && !res)
      {
        const char *s = NULL;
        int rc = sqlite3_prepare_v2(odb->db, sql.c_str(), sql.size(), &res, &s);
        if(rc != SQLITE_OK)
          {
            GetDatabase().error(*this, "execute: prepare query failed");
            return false;
          }
        if(!res)
          {
            GetDatabase().error(*this, "execute: query failed");
            return false;
          }
        rc = sqlite3_step(res); // execute
        sqlite3_finalize(res); // deallocate statement
        res = NULL;
        switch (rc)
          {
          case SQLITE_BUSY:
            GetDatabase().error(*this, "execute: database busy");
            return false;
          case SQLITE_DONE:
          case SQLITE_ROW:
            return true;
          case SQLITE_ERROR:
            GetDatabase().error(*this, sqlite3_errmsg(odb->db));
           return false;
          case SQLITE_MISUSE:
            GetDatabase().error(*this, "execute: database misuse");
            return false;
          }
        GetDatabase().error(*this, "execute: unknown result code");
      }
    return false;
  }

//! @brief Inserta un blob en la posición indicada por el primer
//! signo de interrogación que aparece en la sentencia SQL
//! 
bool Query::insert_blob1(const std::string &sql,const void *blobData,const size_t &numBytes)
  {
//     if(numBytes > SQLITE_MAX_LENGTH)
//       {
//         GetDatabase().error(*this, "insert_blob1: - data too big to send to SQLite databse, enlarge BLOBS!");
//         return  false;
//       }
    m_last_query = sql;
    if(odb && res)
      { GetDatabase().error(*this, "insert_blob1: query busy"); }
    if(odb && !res)
      {
        const char  **pzTail= NULL;
        int rc= sqlite3_prepare_v2(odb->db, sql.c_str(), sql.size()+1, &res, pzTail);
        if(rc != SQLITE_OK)
          {
            GetDatabase().error(*this, "insert_blob1: prepare query failed");
            return false;
          }
        if(!res)
          {
            GetDatabase().error(*this, "insert_blob1: query failed");
            return false;
          }
        else
          {
            // For Blob column bind 1
            sqlite3_bind_blob(res, 1, blobData, numBytes, SQLITE_TRANSIENT);
            rc= sqlite3_step(res); // execute
            sqlite3_finalize(res); // deallocate statement
            //sqlite3_exec(odb->db, "COMMIT", NULL, NULL, NULL);
            res = NULL;
            switch (rc)
              {
              case SQLITE_BUSY:
                GetDatabase().error(*this, "insert_blob1: database busy sql: "+ sql);
                return false;
              case SQLITE_DONE:
              case SQLITE_ROW:
                return true;
              case SQLITE_ERROR:
                GetDatabase().error(*this, sqlite3_errmsg(odb->db));
                return false;
              case SQLITE_MISUSE:
                GetDatabase().error(*this, "insert_blob1: database misuse sql: "+ sql);
                return false;
              case SQLITE_CONSTRAINT:
                GetDatabase().error(*this, "insert_blob1: constraint not satisfied sql: "+ sql);
                return false;
              }
            GetDatabase().error(*this, "insert_blob1: unknown result code sql: "+ sql);
          }
      }
    return false;
  }

// methods using db specific api calls

sqlite3_stmt *Query::get_result(const std::string& sql)
{        // query, result
        m_last_query = sql;
        if (odb && res)
        {
                GetDatabase().error(*this, "get_result: query busy");
        }
        if (odb && !res)
        {
                const char *s = NULL;
                int rc = sqlite3_prepare_v2(odb->db, sql.c_str(), sql.size(), &res, &s);
                if (rc != SQLITE_OK)
                {
                        GetDatabase().error(*this, "get_result: prepare query failed");
                        return NULL;
                }
                if (!res)
                {
                        GetDatabase().error(*this, "get_result: query failed");
                        return NULL;
                }
                // get column names from result
                {
                        int i = 0;
                        do
                        {
                                const char *p = sqlite3_column_name(res, i);
                                if (!p)
                                        break;
                                m_nmap[p] = ++i;
                        } while (true);
                }
                cache_rc = sqlite3_step(res);
                cache_rc_valid = true;
                m_row_count = (cache_rc == SQLITE_ROW) ? 1 : 0;
        }
        return res;
}


void Query::free_result()
  {
        if (odb && res)
        {
                sqlite3_finalize(res);
                res = NULL;
                row = false;
                cache_rc_valid = false;
        }
        // clear column names
        while (m_nmap.size())
        {
                std::map<std::string,int>::iterator it = m_nmap.begin();
                m_nmap.erase(it);
        }
}


bool Query::fetch_row(void)
  {
    rowcount = 0;
    row = false;
    if(odb && res)
      {
        int rc = cache_rc_valid ? cache_rc : sqlite3_step(res); // execute
        cache_rc_valid = false;
        switch (rc)
          {
          case SQLITE_BUSY:
            GetDatabase().error(*this, "execute: database busy");
            return false;
                case SQLITE_DONE:
                        return false;
                case SQLITE_ROW:
                        row = true;
                        return true;
                case SQLITE_ERROR:
                        GetDatabase().error(*this, sqlite3_errmsg(odb->db));
                        return false;
                case SQLITE_MISUSE:
                        GetDatabase().error(*this, "execute: database misuse");
                        return false;
                }
                GetDatabase().error(*this, "execute: unknown result code");
        }
        return false;
}


sqlite_int64 Query::insert_id()
{
        if (odb)
        {
                return sqlite3_last_insert_rowid(odb->db);
        }
        else
        {
                return 0;
        }
}


//! @brief Returns 0 if there are no rows to fetch.
long Query::num_rows(void) const
  { return odb && res ? m_row_count : 0; }

//! @brief Devuelve el número de columnas de la consulta.
size_t Query::num_cols(void) const
  { return m_nmap.size(); }

//! @brief Devuelve una cadena de caracteres con los nombres de los campos.
std::string Query::field_names(void) const
  {
    std::string retval= "";
    if(m_nmap.empty()) return retval;
    std::map<std::string,int>::const_iterator i=m_nmap.begin();
    retval+= (*i).first;i++;
    for(;i!=m_nmap.end();i++)
      retval+= ',' + (*i).first;
    return retval;
  }

bool Query::is_null(int x)
  {
        if (odb && res && row)
        {
                if (sqlite3_column_type(res, x) == SQLITE_NULL)
                        return true;
        }
        return false; // ...
  }


const char *Query::getstr(const std::string& x)
  {
    int index = field_index(x);
    if(index >= 0)
      return getstr(index);
    else
      {
        error(":getstr; column name lookup failure: '" + x + "' field_index devuelve "+boost::lexical_cast<std::string>(index));
        return "";
      }
  }

const char *Query::getstr(int x)
  {
    if(odb && res && row && x < sqlite3_column_count(res) )
      {
        const unsigned char *tmp = sqlite3_column_text(res, x);
        return tmp ? (const char *)tmp : "";
      }
    else
      return "";
  }




const char *Query::getstr(void)
  { return getstr(rowcount++); }

const void *Query::getblob(const std::string& x)
  {
    const int index = field_index(x);
    if(index >= 0)
      return getblob(index);
    else
      {
        error(":getblob; column name lookup failure: '" + x + "' field_index devuelve "+boost::lexical_cast<std::string>(index));
        return NULL;
      }
  }

const void *Query::getblob(int x)
  {
    if(odb && res && row && x < sqlite3_column_count(res) )
      return sqlite3_column_blob(res, x);
    else
      return NULL;
  }

const void *Query::getblob(void)
  { return getblob(rowcount++); }

double Query::getnum(const std::string& x)
  {
    int index = field_index(x);
    if(index >= 0)
      return getnum(index);
    else
      {
        error("Query::getnum; column name lookup failure: " + x);
        return 0;
      }
  }

double Query::getnum(int x)
  {
    if(odb && res && row)
      {
        return sqlite3_column_double(res, x);
      }
    return 0;
  }


long Query::getval(const std::string& x)
  {
    int index = field_index(x);
    if(index >= 0)
      return getval(index);
    else
      {
        error("Query::getval; column name lookup failure: " + x);
        return 0;
      }
  }


long Query::getval(int x)
  {
    if(odb && res && row)
      {        return sqlite3_column_int(res, x); }
    else
      return 0;
  }


double Query::getnum(void)
  { return getnum(rowcount++); }


long Query::getval(void)
  { return getval(rowcount++); }


unsigned long Query::getuval(const std::string& x)
  {
    int index = field_index(x);
    if(index >= 0)
      return getuval(index);
    else
      {
        error("Query::getuval; column name lookup failure: " + x);
        return 0;
      }
  }


unsigned long Query::getuval(int x)
  {
     unsigned long l = 0;
     if(odb && res && row)
       { l = sqlite3_column_int(res, x); }
     return l;
  }


unsigned long Query::getuval(void)
  { return getuval(rowcount++); }


int64_t Query::getbigint(const std::string& x)
  {
    int index= field_index(x);
    if(index >= 0)
      return getbigint(index);
    else
      {
        error("Query::getbigint; Column name lookup failure: " + x);
        return 0;
      }
  }


int64_t Query::getbigint(int x)
  {
    if(odb && res && row)
      {        return sqlite3_column_int64(res, x); }
    return 0;
  }


int64_t Query::getbigint()
  { return getbigint(rowcount++); }


uint64_t Query::getubigint(const std::string& x)
  {
    int index = field_index(x);
    if(index >= 0)
      return getubigint(index);
    error("Query::getubigint; column name lookup failure: " + x);
    return 0;
  }


uint64_t Query::getubigint(int x)
  {
    uint64_t l = 0;
    if(odb && res && row)
      {        l = sqlite3_column_int64(res, x); }
    return l;
  }


uint64_t Query::getubigint()
  { return getubigint(rowcount++); }


double Query::get_num(const std::string& sql)
  {
    double l = 0;
    if(get_result(sql))
      {
                if (fetch_row())
                {
                        l = getnum();
                }
                free_result();
        }
        return l;
}


long Query::get_count(const std::string& sql)
{
        long l = 0;
        if (get_result(sql))
        {
                if (fetch_row())
                        l = getval();
                free_result();
        }
        return l;
}


const char *Query::get_string(const std::string &sql)
  {
    bool found= false;
    m_tmpstr= "";
    if(get_result(sql))
      {
        if(fetch_row())
          {
            m_tmpstr= getstr();
            found= true;
          }
        free_result();
      }
    return m_tmpstr.c_str(); // %! changed from 1.0 which didn't return NULL on failed query
  }


const std::string &Query::GetLastQuery(void) const
  { return m_last_query; }


std::string Query::GetError(void) const
  {
    if(odb)
      return sqlite3_errmsg(odb->db);
    return "";
  }


int Query::GetErrno(void)
  {
    if(odb)
      return sqlite3_errcode(odb->db);
    return 0;
  }


bool Query::Connected()
  { return odb ? true : false; }

void Query::ViewRes()
  {
    if(!res)
      {
        printf("no result stored\n");
        return;
      }
    printf("result column count = %d\n", sqlite3_column_count(res));
    for(int i = 0; i < sqlite3_column_count(res); i++)
      {
        printf(" %2d   type %d   name '%s'", i, sqlite3_column_type(res, i), sqlite3_column_name(res, i));
        printf("  / '%s'", (char *)sqlite3_column_text(res, i));
        printf("  / %d", sqlite3_column_int(res, i));
        printf("  / %f", sqlite3_column_double(res, i));
        printf("\n");
      }
  }


void Query::error(const std::string& msg)
  { GetDatabase().error(*this, msg); }


#ifdef SQLITEW_NAMESPACE
} // namespace SQLITEW_NAMESPACE {
#endif

