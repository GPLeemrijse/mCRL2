/*
   SVC -- the SVC (Systems Validation Centre) file format library

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   $Id$ */

#include <time.h>
#include <string>
#include "mcrl2/atermpp/detail/utility.h"
#include <svc/svcerrno.h>
#include <svc/svc.h>

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char* _strdup(const char* s);
#endif

using namespace atermpp;

extern int svcErrno;
int SVCerrno;


/* SVCnewLabel assigns a new index to a label
   of a transition, given as an aterm. The last
   variable indicates whether the addition is
   actually a new one (1=new). SVCnewLabel returns 0 if an
   error occurred, for instance because there
   is no room left to store the new term */

SVClabelIndex SVCnewLabel(SVCfile* file, aterm term, SVCbool* _new)
{
  SVClabelIndex index;


  if (HTmember(&file->file.labelTable, term, &index))
  {
    *_new=SVCfalse;
    return index;
  }
  else
  {
    *_new=SVCtrue;
    if (file->file.fileMode==SVCwrite)
    {
      file->header.numLabels++;
    }
    return HTinsert(&file->file.labelTable, term, NULL);
  }

}

/* SVCaterm2Label gives the label index belonging
   to an aterm. If no such label exists, the
   value -1 is returned */

SVClabelIndex SVCaterm2Label(SVCfile* file, aterm term)
{
  SVClabelIndex index;


  if (HTmember(&file->file.labelTable, term, &index))
  {
    return index;
  }
  else
  {
    return -1L;
  }

}

/* SVClabel2ATerm provides the aterm that belongs
   to a label index. In case of an error NULL is
   returned */

aterm SVClabel2ATerm(SVCfile* file, SVClabelIndex index)
{

  return HTgetTerm(&file->file.labelTable, index);

}


SVCstateIndex SVCnewState(SVCfile* file, aterm term, SVCbool* _new)
{
  SVCstateIndex index;


  if (HTmember(&file->file.stateTable, term, &index))
  {
    *_new=SVCfalse;
  }
  else
  {
    *_new=SVCtrue;
    if (file->file.fileMode==SVCwrite)
    {
      file->header.numStates++;
    }
    index=HTinsert(&file->file.stateTable, term, NULL);
  }

  return index;

}


SVCstateIndex SVCaterm2State(SVCfile* file, aterm term)
{
  SVCstateIndex index;


  if (HTmember(&file->file.stateTable, term, &index))
  {
    return index;
  }
  else
  {
    return -1L;
  }

}

aterm SVCstate2ATerm(SVCfile* file, SVCstateIndex index)
{

  return HTgetTerm(&file->file.stateTable, index);

}


SVCparameterIndex SVCnewParameter(SVCfile* file, aterm term, SVCbool* _new)
{
  SVCparameterIndex index;


  if (HTmember(&file->file.parameterTable, term, &index))
  {
    *_new=SVCfalse;
    return index;
  }
  else
  {
    *_new=SVCtrue;
    if (file->file.fileMode==SVCwrite)
    {
      file->header.numParameters++;
    }
    return HTinsert(&file->file.parameterTable, term,NULL);
  }

}



SVCparameterIndex SVCaterm2Parameter(SVCfile* file, aterm term)
{
  SVCparameterIndex index;


  if (HTmember(&file->file.parameterTable, term, &index))
  {
    return index;
  }
  else
  {
    return -1L;
  }

}



aterm SVCparameter2ATerm(SVCfile* file, SVCparameterIndex index)
{

  return HTgetTerm(&file->file.parameterTable, index);
}


int SVCputTransition(SVCfile* file,
                     SVCstateIndex fromStateIndex,
                     SVClabelIndex labelIndex,
                     SVCstateIndex toStateIndex,
                     SVCparameterIndex paramIndex)
{
  struct ltsTransition transition;
  aterm fromStateTerm,
        toStateTerm,
        labelTerm,
        paramTerm;
  int   ret;

  fromStateTerm=HTgetTerm(&file->file.stateTable, fromStateIndex);
  toStateTerm=HTgetTerm(&file->file.stateTable, toStateIndex);
  labelTerm=HTgetTerm(&file->file.labelTable, labelIndex);
  paramTerm=HTgetTerm(&file->file.parameterTable, paramIndex);

  if (detail::address(fromStateTerm) != NULL && detail::address(toStateTerm)!=NULL &&
      detail::address(labelTerm)!= NULL     && detail::address(paramTerm)   != NULL)
  {
    transition.fromState=fromStateTerm;
    transition.toState=toStateTerm;
    transition.label=labelTerm;
    transition.parameters=paramTerm;

    ret=svcWriteTransition(&file->file, &transition);
    file->header.numTransitions++;

    return ret;

  }
  else
  {

    return 0;

  }
}


int SVCgetNextTransition(SVCfile* file,
                         SVCstateIndex* fromStateIndex, SVClabelIndex* labelIndex,
                         SVCstateIndex* toStateIndex, SVCparameterIndex* paramIndex)
{
  struct ltsTransition transition;
  if (file->transitionNumber>=file->header.numTransitions)
  {
    return 0;
  }

  if (svcReadNextTransition(&file->file, &transition)==0)
  {
    file->transitionNumber++;
    HTmember(&file->file.stateTable,transition.fromState,fromStateIndex);
    HTmember(&file->file.stateTable,transition.toState,toStateIndex);
    HTmember(&file->file.parameterTable,transition.parameters,paramIndex);
    HTmember(&file->file.labelTable,transition.label,labelIndex);
    return 1;
  }
  else
  {
    return 0;
  }
}


int SVCopen(SVCfile* file, char* filename, SVCfileMode mode, SVCbool* indexed)
{
  char* version;
  char buffy[256];
  time_t now;
  SVCbool _new;

  file->transitionNumber = 0L;
  switch (mode)
  {
    case SVCwrite:

      if (svcInit(&file->file, filename, SVCwrite, indexed)>=0)
      {

        (void) time(&now);
        strftime(buffy,256,"%x", localtime(&now));

        file->header.comments=_strdup("");
        file->header.type=_strdup("generic");
        file->header.version=_strdup("0.0");
        file->header.filename=_strdup(filename);
        file->header.date=_strdup(buffy);
        file->header.creator=_strdup("svclib");
        file->header.initialState=_strdup("0");
        file->header.numStates=0L;
        file->header.numTransitions=0L;
        file->header.numLabels=0L;
        file->header.numParameters=0L;
        file->file.indexFlag=*indexed;
        file->file.formatVersion=_strdup(SVC_VERSION);
        return 0;

      }
      else
      {
        SVCerrno=svcErrno;
        return -1;
      }

    case SVCread:

      if (svcInit(&file->file, filename, SVCread, indexed)>=0)
      {
        if (svcReadVersion(&file->file, &version)==0 &&
            svcReadHeader(&file->file, &file->header)==0)
        {
          file->file.formatVersion=version;
          if (SVCnewState(file, read_term_from_string(file->header.initialState), &_new)<0)
          {
            SVCerrno=ENEWSTATE;
            return -1;
          }

          return 0;
        }
        else
        {
          SVCerrno=svcErrno;
          return -1;
        }
      }
      else
      {
        SVCerrno=svcErrno;
        return -1;
      }

    default:
      SVCerrno=EFILEMODE;
      return -1;
  }

}



int SVCclose(SVCfile* file)
{


  if (file->file.fileMode==SVCwrite)
  {

    /* Write file header and trailer */

    svcWriteHeader(&file->file,&file->header);
    svcWriteVersion(&file->file);
    svcWriteTrailer(&file->file);
  }

  /* Clean up memory */

  svcFree(&file->file);

  free(file->header.comments);
  free(file->header.type);
  free(file->header.version);
  free(file->header.filename);
  free(file->header.date);
  free(file->header.creator);
  free(file->header.initialState);

  /* Close file */

  return 0;
}

char* SVCgetFormatVersion(SVCfile* file)
{

  return file->file.formatVersion;

}

SVCbool SVCgetIndexFlag(SVCfile* file)
{

  return file->file.indexFlag;

}


SVCstateIndex SVCgetInitialState(SVCfile* file)
{

  return SVCaterm2State(file, read_term_from_string(file->header.initialState));
}

int SVCsetInitialState(SVCfile* file, SVCstateIndex index)
{
  char str[16];

  free(file->header.initialState);
  if (file->file.indexFlag)
  {
    sprintf(str,"%ld",index);
    file->header.initialState=_strdup(str);
  }
  else
  {
    file->header.initialState=_strdup(to_string(SVCstate2ATerm(file,index)).c_str());
  }
  return 0;
}

/* Functions to put and get header information */
char* SVCgetComments(SVCfile* file)
{

  return file->header.comments;

}
int   SVCsetComments(SVCfile* file, char* comments)
{

  free(file->header.comments);
  file->header.comments=_strdup(comments);
  return 0;
}
char* SVCgetType(SVCfile* file)
{

  return file->header.type;

}
int   SVCsetType(SVCfile* file, char* type)
{

  free(file->header.type);
  file->header.type=_strdup(type);
  return 0;
}
char* SVCgetVersion(SVCfile* file)
{

  return file->header.version;

}
int   SVCsetVersion(SVCfile* file, char* version)
{

  free(file->header.version);
  file->header.version=_strdup(version);
  return 0;

}
char* SVCgetCreator(SVCfile* file)
{

  return file->header.creator;

}
int   SVCsetCreator(SVCfile* file, char* creator)
{

  free(file->header.creator);
  file->header.creator=_strdup(creator);
  return 0;
}
char* SVCgetDate(SVCfile* file)
{

  return file->header.date;

}
char* SVCgetFilename(SVCfile* file)
{

  return file->header.filename;
  return 0;

}
SVCint SVCnumStates(SVCfile* file)
{

  return file->header.numStates;

}
SVCint SVCnumLabels(SVCfile* file)
{

  return file->header.numLabels;
}
SVCint SVCnumParameters(SVCfile* file)
{

  return file->header.numParameters;

}
SVCint SVCnumTransitions(SVCfile* file)
{

  return file->header.numTransitions;
}


std::string SVCerror(int errnum)
{

/* switch (errnum)
  {
    default: */
      return svcError(errnum);
/*  } */
}