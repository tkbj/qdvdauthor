/***************************************************************************
    utils.h
                             -------------------
    Utils - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <QString>

class QImage;

class Utils
{
public:
   Utils ( );
  ~Utils ( );


  void frameImage    ( QImage &, int, int, QImage *, unsigned int * );
  void prescaleImage ( QImage &, int );
  void writeZoneCode ( unsigned char, QString );
  bool recMkdir      ( const    QString & );
  unsigned long long SDBMHash ( QString & );
};

#endif // UTILS_H
