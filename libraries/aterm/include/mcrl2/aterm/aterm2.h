/**
  * aterm2.h: Definition of the level 2 interface
  * of the ATerm library.
  */

#ifndef ATERM2_H
#define ATERM2_H

/**
  * The level 2 interface is a strict superset
  * of the level 1 interface.
  */

#include <unistd.h>
#include <stack>
#include "mcrl2/aterm/aterm1.h"
#include "mcrl2/aterm/afun.h"

/* The largest size_t is used as an indicator that an element does not exist.
   This is used as a replacement of a negative number as an indicator of non
   existence */


namespace aterm
{

class _ATermInt:public _ATerm
{
  public:
    union
    {
      int value;
      MachineWord reserved; /* Only use lower 32 bits as int. The value is used ambiguously
                               as integer and as MachineWord. For all cases using bitwise
                               operations, the MachineWord version must be used,
                               as failing to do so may lead to improper initialisation
                               of the last 32 bits during casting. */
    };
};

static const size_t TERM_SIZE_INT = sizeof(_ATermInt)/sizeof(size_t);

class ATermInt:public ATerm
{
  public:

    ATermInt():ATerm()
    {}

    ATermInt(_ATermInt *t):ATerm(reinterpret_cast<_ATerm*>(t))
    {
    }

    explicit ATermInt(const ATerm &t):ATerm(t) 
    {
    }

    ATermInt &operator=(const ATermInt &t)
    {
      copy_term(t.m_aterm);
      return *this;
    }


    _ATermInt & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_aterm is checked,
      // which is done quite commonly.
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return *reinterpret_cast<_ATermInt*>(m_aterm); 
    }

    _ATermInt *operator ->() const
    {
      assert(m_aterm!=NULL);
      assert(m_aterm->reference_count>0);
      return reinterpret_cast<_ATermInt*>(m_aterm);
    }
};

class _ATermAppl:public _ATerm
{
  public:
    _ATerm      *arg[1000];   /* This value 1000 is completely arbitrary, and should not be used
                                (therefore it is excessive). Using mallocs an array of the
                                appropriate length is declared, where it is possible that
                                the array has size 0, i.e. is absent. If the value is small
                                (it was 1), the clang compiler provides warnings. */
};

class ATermAppl:public ATerm
{
  public:

    ATermAppl():ATerm()
    {}

    ATermAppl (_ATermAppl *t):ATerm(reinterpret_cast<_ATerm*>(t))
    {
    }

    explicit ATermAppl (const ATerm &t):ATerm(t)
    {
    }

    ATermAppl &operator=(const ATermAppl &t)
    {
      copy_term(t.m_aterm);
      return *this;
    }

    _ATermAppl & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_aterm is checked,
      // which is done quite commonly.
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return *reinterpret_cast<_ATermAppl*>(m_aterm); 
    }

    _ATermAppl *operator ->() const
    {
      assert(m_aterm!=NULL);
      assert(m_aterm->reference_count>0);
      return reinterpret_cast<_ATermAppl*>(m_aterm);
    }
};

class _ATermList:public _ATerm
{
  public:
    _ATerm* head;
    _ATermList* tail;
};

static const size_t TERM_SIZE_LIST = sizeof(_ATermList)/sizeof(size_t);


class ATermList:public ATerm
{
  public:

    ATermList ():ATerm()
    {
    }

    ATermList(_ATermList *t):ATerm(reinterpret_cast<_ATerm *>(t))
    {
    }

    explicit ATermList(const ATerm &t):ATerm(t)
    {
    }

    ATermList &operator=(const ATermList &t)
    {
      copy_term(t.m_aterm);
      return *this;
    }

    _ATermList & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_aterm is checked,
      // which is done quite commonly.
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return *reinterpret_cast<_ATermList*>(m_aterm); 
    }

    _ATermList *operator ->() const
    {
      assert(m_aterm!=NULL);
      assert(m_aterm->reference_count>0);
      return reinterpret_cast<_ATermList*>(m_aterm);
    }
};

struct _ATermTable;

typedef _ATermTable *ATermIndexedSet;

typedef _ATermTable *ATermTable;


/* Convenience macro's to circumvent gcc's (correct) warning:
 *   "dereferencing type-punned pointer will break strict-aliasing rules"
 * example usage: ATprotectList(&ATempty);
 */
inline
void ATprotectTerm(const ATerm* p)
{
  ATprotect(p);
}

inline
void ATprotectList(const ATermList* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATprotectAppl(const ATermAppl* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATprotectInt(const ATermInt* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATunprotectTerm(const ATerm* p)
{
  ATunprotect(p);
}

inline
void ATunprotectList(const ATermList* p)
{
  ATunprotect((const ATerm*) p);
}

inline
void ATunprotectAppl(const ATermAppl* p)
{
  ATunprotect((const ATerm*) p);
}

inline
void ATunprotectInt(const ATermInt* p)
{
  ATunprotect((const ATerm*) p);
}

/** The following functions implement the operations of
  * the 'standard' ATerm interface, and should appear
  * in some form in every implementation of the ATerm
  * datatype.
  */

/* The ATermInt type */
ATermInt ATmakeInt(const int value);

inline
int ATgetInt(const ATermInt &t)
{
  return t->value;
}

/* The ATermAppl type */
ATermAppl ATmakeAppl(const AFun &sym, ...);

/* The implementation of the function below can be found in memory.h */
// template <class TERM_ITERATOR>
// ATermAppl ATmakeAppl(const AFun &sym, const TERM_ITERATOR begin, const TERM_ITERATOR end);

ATermAppl ATmakeAppl0(const AFun &sym);
ATermAppl ATmakeAppl1(const AFun &sym, const ATerm &arg0);
ATermAppl ATmakeAppl2(const AFun &sym, const ATerm &arg0, const ATerm &arg1);
ATermAppl ATmakeAppl3(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2);
ATermAppl ATmakeAppl4(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3);
ATermAppl ATmakeAppl5(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg4, const ATerm &arg5);
ATermAppl ATmakeAppl6(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg4, const ATerm &arg5, const ATerm &arg6);


inline
size_t ATgetAFun(const _ATermAppl* appl)
{
  return GET_SYMBOL(appl->header);
}

inline
size_t ATgetAFun(const ATermAppl &appl)
{
  return GET_SYMBOL(appl->header);
}

inline
const ATerm &ATgetArgument(const ATermAppl &appl, const size_t idx)
{
  return (ATerm &)appl->arg[idx];
}

ATermAppl ATsetArgument(const ATermAppl &appl, const ATerm &arg, const size_t n);

/* Portability */
ATermList ATgetArguments(const ATermAppl &appl);
ATermAppl ATmakeApplList(const AFun &sym, const ATermList &args);
ATermAppl ATmakeApplArray(const AFun &sym, const ATerm args[]);

size_t ATgetLength(const ATermList &list);

inline
ATerm &ATgetFirst(const ATermList &l)
{
  return (ATerm &)l->head;
}

inline
const ATermList &ATgetNext(const ATermList &l)
{
  return (ATermList &)l->tail;
}

/* The ATermList type */
extern ATermList ATempty;

inline
bool ATisEmpty(const ATermList &l)
{
  return l == ATempty;

  // return l->head == NULL && l->tail == NULL;
}

ATermList ATgetTail(const ATermList &list, const int &start);
ATermList ATgetSlice(const ATermList &list, const size_t start, const size_t end);
ATermList ATinsert(const ATermList &list, const ATerm &el);
ATermList ATappend(const ATermList &list, const ATerm &el);
ATermList ATconcat(const ATermList &list1, const ATermList &list2);
size_t    ATindexOf(const ATermList &list, const ATerm &el, const int start);
const ATerm& ATelementAt(const ATermList &list, size_t index);
ATermList ATremoveElement(const ATermList &list, const ATerm &el);
ATermList ATremoveElementAt(const ATermList &list, const size_t idx);
ATermList ATreplace(const ATermList &list, const ATerm &el, const size_t idx);
ATermList ATreverse(const ATermList &list);
ATermList ATsort(const ATermList &list, int (*compare)(const ATerm &t1, const ATerm &t2));

/* ATermList ATmakeList0(); */
inline
ATermList ATmakeList0()
{
  return ATempty;
}

ATermList ATmakeList1(const ATerm &el0);

inline
ATermList ATmakeList2(const ATerm &el0, const ATerm &el1)
{
  return ATinsert(ATmakeList1(el1), el0);
}

inline
ATermList ATmakeList3(const ATerm &el0, const ATerm &el1, const ATerm &el2)
{
  return ATinsert(ATmakeList2(el1, el2), el0);
}

inline
ATermList ATmakeList4(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3)
{
  return ATinsert(ATmakeList3(el1, el2, el3), el0);
}

inline
ATermList ATmakeList5(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4)
{
  return ATinsert(ATmakeList4(el1, el2, el3, el4), el0);
}

inline
ATermList ATmakeList6(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4, const ATerm &el5)
{
  return ATinsert(ATmakeList5(el1, el2, el3, el4, el5), el0);
}

ATermTable ATtableCreate(const size_t initial_size, const unsigned int max_load_pct);
void       ATtableDestroy(ATermTable table);
void       ATtableReset(ATermTable table);
void       ATtablePut(ATermTable table, const ATerm &key, const ATerm &value);
ATerm      ATtableGet(ATermTable table, const ATerm &key);
bool     ATtableRemove(ATermTable table, const ATerm &key); /* Returns true if removal was successful. */
ATermList  ATtableKeys(ATermTable table);
ATermList  ATtableValues(ATermTable table);

ATermIndexedSet
ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct);
void       ATindexedSetDestroy(ATermIndexedSet set);
void       ATindexedSetReset(ATermIndexedSet set);
size_t     ATindexedSetPut(ATermIndexedSet set, const ATerm &elem, bool* isnew);
ssize_t    ATindexedSetGetIndex(ATermIndexedSet set, const ATerm &elem); /* A negative value represents non existence. */
bool     ATindexedSetRemove(ATermIndexedSet set, const ATerm &elem);   /* Returns true if removal was successful. */
ATermList  ATindexedSetElements(ATermIndexedSet set);
ATerm      ATindexedSetGetElem(ATermIndexedSet set, size_t index);

// AFun  ATmakeAFun(const char* name, const size_t arity, const bool quoted);

inline
char* ATgetName(const AFun &sym)
{
  return AFun::at_lookup_table[sym.number()]->name;
}

/* inline
size_t ATgetArity(const size_t n)
{
  return GET_LENGTH(AFun::at_lookup_table[n]->header >> 1);
} */

inline
size_t ATgetArity(const AFun &sym)
{
  return sym.arity();
} 


inline
bool ATisQuoted(const AFun &sym)
{
  return sym.is_quoted();
}

void    ATprotectAFun(const AFun &sym);
void    ATunprotectAFun(const AFun &sym);

} // namespace aterm

#endif
