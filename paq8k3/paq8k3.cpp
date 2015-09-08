/*
paq8k3 file compressor/archiver.
Copyright (C) 2009 Bill Pettis

Copyright (C) 2009 Matt Mahoney, Serge Osnach, Alexander Ratushnyak,
Przemyslaw Skibinski, Matthew Fite, wowtiger, Andrew Paterson
*/

#define PROGNAME "paq8k3" // Change when breaking compatibility.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#define NDEBUG // remove for debugging (turns on Array bound checks)
#include <assert.h>

#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif

#ifndef DEFAULT_OPTION
#define DEFAULT_OPTION 5
#endif

// 8, 16, 32 bit unsigned types (adjust as appropriate)
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

// min, max functions
#ifndef WINDOWS
inline int min(int a, int b) {return a<b?a:b;}
inline int max(int a, int b) {return a<b?b:a;}
#endif

// Error handler: print message if any, and exit
void quit(const char* message=0){
  throw message;
}

// strings are equal ignoring case?
int equals(const char* a, const char* b){
  assert(a && b);
  while (*a && *b){
    int c1=*a;
    if (c1>='A'&&c1<='Z') c1+='a'-'A';
    int c2=*b;
    if (c2>='A'&&c2<='Z') c2+='a'-'A';
    if (c1!=c2) return 0;
    ++a;
    ++b;
  }
  return *a==*b;
}

// Console color control
#ifdef WINDOWS
HANDLE d;
WORD wOldColorAttrs;
#endif

////////////////////////// Program Checker ///////////////////////////

// Track time and memory used
class ProgramChecker{
  int memused; // bytes allocated by Array<T> now
  int maxmem; // most bytes allocated ever
  clock_t start_time; // in ticks
public:
  void alloc(int n){ // report memory allocated, may be negative
    memused+=n;
    if (memused>maxmem) maxmem=memused;
  }
  ProgramChecker(): memused(0), maxmem(0) {
    start_time=clock();
    assert(sizeof(U8)==1);
    assert(sizeof(U16)==2);
    assert(sizeof(U32)==4);
    assert(sizeof(short)==2);
    assert(sizeof(int)==4);
  }
  void print() const{ // print time and memory used
    printf("Time %1.2f sec, used %d MB of memory\n",
      double(clock()-start_time)/CLOCKS_PER_SEC, maxmem>>20);
  }
}programChecker;

/////////////////////////////// Array ////////////////////////////////

// Array<T, ALIGN> a(n); creates n elements of T initialized to 0 bits.
// Constructors for T are not called.
// Indexing is bounds checked if assertions are on.
// a.size() returns n.
// a.resize(n) changes size to n, padding with 0 bits or truncating.
// a.push_back(x) appends x and increases size by 1, reserving up to size*2.
// a.pop_back() decreases size by 1, does not free memory.
// Copy and assignment are not supported.
// Memory is aligned on a ALIGN byte boundary (power of 2), default is none.

template <class T, int ALIGN=0> class Array{
private:
  int n; // user size
  int reserved; // actual size
  char *ptr; // allocated memory, zeroed
  T* data; // start of n elements of aligned data
  void create(int i); // create with size i
public:
  explicit Array(int i=0) {create(i);}
  ~Array();
  T& operator[](int i){
#ifndef NDEBUG
    if (i<0 || i>=n) fprintf(stderr, "%d out of bounds %d\n", i, n), quit();
#endif
    return data[i];
  }
  const T& operator[](int i) const{
#ifndef NDEBUG
    if (i<0 || i>=n) fprintf(stderr, "%d out of bounds %d\n", i, n), quit();
#endif
    return data[i];
  }
  int size() const {return n;}
  void resize(int i); // change size to i
  void pop_back() {if (n>0) --n;} // decrement size
  void push_back(const T& x); // increment size, append x
private:
  Array(const Array&); // no copy or assignment
  Array& operator=(const Array&);
};

template<class T, int ALIGN> void Array<T, ALIGN>::resize(int i){
  if (i<=reserved){
    n=i;
    return;
  }
  char *saveptr=ptr;
  T *savedata=data;
  int saven=n;
  create(i);
  if (saveptr){
    if (savedata){
      memcpy(data, savedata, sizeof(T)*min(i, saven));
      programChecker.alloc(-ALIGN-n*sizeof(T));
    }
    free(saveptr);
  }
}

template<class T, int ALIGN> void Array<T, ALIGN>::create(int i){
  n=reserved=i;
  if (i<=0){
    data=0;
    ptr=0;
    return;
  }
  const int sz=ALIGN+n*sizeof(T);
  programChecker.alloc(sz);
  ptr = (char*)calloc(sz, 1);
  if (!ptr) quit("Out of memory");
  data = (ALIGN ? (T*)(ptr+ALIGN-(((long)ptr)&(ALIGN-1))) : (T*)ptr);
  assert((char*)data>=ptr && (char*)data<=ptr+ALIGN);
}

template<class T, int ALIGN> Array<T, ALIGN>::~Array(){
  programChecker.alloc(-ALIGN-n*sizeof(T));
  free(ptr);
}

template<class T, int ALIGN> void Array<T, ALIGN>::push_back(const T& x){
  if (n==reserved){
    int saven=n;
    resize(max(1, n*2));
    n=saven;
  }
  data[n++]=x;
}

/////////////////////////// String /////////////////////////////

// A tiny subset of std::string
// size() includes NUL terminator.

class String: public Array<char> {
public:
  const char* c_str() const {return &(*this)[0];}
  void operator=(const char* s){
    resize(strlen(s)+1);
    strcpy(&(*this)[0], s);
  }
  void operator+=(const char* s){
    assert(s);
    pop_back();
    while (*s) push_back(*s++);
    push_back(0);
  }
  String(const char* s=""): Array<char>(1){
    (*this)+=s;
  }
};


//////////////////////////// rnd ///////////////////////////////

// 32-bit pseudo random number generator
class Random{
  Array<U32> table;
  int i;
public:
  Random(): table(64) {
    table[0]=123456789;
    table[1]=987654321;
    for(int j=0; j<62; j++) table[j+2]=table[j+1]*11+table[j]*23/16;
    i=0;
  }
  U32 operator()(){
    return ++i, table[i&63]=table[i-24&63]^table[i-55&63];
  }
}rnd;

/////////////////////// Global context /////////////////////////
// Global context set by Predictor and available to all models.

int level=DEFAULT_OPTION; // Compression level 0 to 9
#define MEM (0x10000<<level)
int y=0; // Last bit, 0 or 1, set by encoder
int c0=1; // Last 0-7 bits of the partial byte with a leading 1 bit (1-255)
U32 c4=0; // Last 4 whole bytes, packed. Last byte is bits 0-7.
int bpos=0; // Bits in c0 (0 to 7)
int pos; // Number of input bytes in buf (not wrapped)
U32 lastnull; // Distance to last null byte
bool uiq; // For UIQ benchmark, disreguard prior to lastnull
U32 prF; // Last 8 Final predictions scaled to [0,14)

////////////////////////////// Buf /////////////////////////////

// Buf(n) buf; creates an array of n bytes (must be a power of 2).
// buf[i] returns a reference to the i'th byte with wrap (no out of bounds).
// buf(i) returns i'th byte back from pos (i > 0)
// buf.size() returns n.

class Buf{
  Array<U8> b;
public:
  Buf(int i=0): b(i) {}
  void setsize(int i){
    if (!i) return;
    assert(i>0 && (i&(i-1))==0);
    b.resize(i);
  }
  U8& operator[](int i){
    return b[i&b.size()-1];
  }
  int operator()(int i, int part=0) const{
    assert(i>0);
    assert(part>=0 && part<5);
    switch(part){
case 0: if (i<lastnull || !uiq) return b[pos-i&b.size()-1];
        else return 0;
case 1: return b[pos-i&b.size()-1] >> 2;
case 2: return ((b[pos-i&b.size()-1]&224)>>3)|b[pos-i&b.size()-1]&3;
case 3: return b[pos-i&b.size()-1]&31;
case 4: return b[pos-i&b.size()-1];
    }
  }
  int size() const{
    return b.size();
  }
}buf;


///////////////////////////// ilog //////////////////////////////

// ilog(x) = round(log2(x) * 16), 0 <= x < 64K
class Ilog{
  Array<U8> t;
public:
  int operator()(U16 x) const {return t[x];}
  Ilog();
}ilog;

// Compute lookup table by numerical integration of 1/x
Ilog::Ilog(): t(65536) {
  U32 x=14155776;
  for (int i=2; i<65536; ++i){
    x+=774541002/(i*2-1); // numerator is 2^29/ln 2
    t[i]=x>>24;
  }
}

// llog(x) accepts 32 bits
inline int llog(U32 x){
  if (x>=0x1000000)
    return 256+ilog(x>>16);
  else if (x>=0x10000)
    return 128+ilog(x>>8);
  else
    return ilog(x);
}

////////////////////////// randomArray //////////////////////////

// randomArray(x) = a consistant random value

class RandomArray{
  Array<U32> t;
public:
  int operator()(U16 x) const {return t[x];}
  RandomArray();
}randomArray;

RandomArray::RandomArray(): t(99) {
  for (int i=0; i<99; ++i)
    t[i]=rnd();
}

///////////////////////// state table ////////////////////////

// State table:
// nex(state, 0) = next state if bit y is 0, 0 <= state < 256
// nex(state, 1) = next state if bit y is 1
// nex(state, 2) = number of zeros in bit history represented by state
// nex(state, 3) = number of ones represented
//
// States represent a bit history within some context.
// State 0 is the starting state (no bits seen).
// States 1-30 represent all possible sequences of 1-4 bits.
// States 31-252 represent a pair of counts, (n0,n1), the number
// of 0 and 1 bits respectively. If n0+n1 < 16 then there are
// two states for each pair, depending on if a 0 or 1 was the last
// bit seen.
// If n0 and n1 are too large, then there is no state to represent this
// pair, so another state with about the same ratio of n0/n1 is substituted.
// Also, when a bit is observed and the count of the opposite bit is large,
// then part of this count is discarded to favor newer data over old.

class StateTable{
  Array<U8> ns; // state*4 -> next state if 0, if 1, n0, n1
  enum {B=5, N=64}; // sizes of b, t
  static const int b[B]; // x -> max y, y -> max x
  static U8 t[N][N][2]; // x,y -> state number, number of states
  int num_states(int x, int y); // compute t[x][y][1]
  void discount(int& x); // set new value of x after 1 or y after 0
  void next_state(int& x, int& y, int b); // new (x,y) after bit b
public:
  int operator()(int state, int sel) {return ns[state*4+sel];}
  StateTable();
}nex;

const int StateTable::b[B]={42,41,13,6,5}; // x -> max y, y -> max x
U8 StateTable::t[N][N][2];

int StateTable::num_states(int x, int y){
  if (x<y) return num_states(y, x);
  if (x<0 || y<0 || x>=N || y>=N || y>=B || x>=b[y]) return 0;

  // States 0-30 are a history of the last 0-4 bits
  if (x+y<=4){ // x+y choose x = (x+y)!/x!y!
    int r=1;
    for (int i=x+1; i<=x+y; ++i) r*=i;
    for (int i=2; i<=y; ++i) r/=i;
    return r;
  }

  // States 31-255 represent a 0,1 count and possibly the last bit
  // if the state is reachable by either a 0 or 1.
  else
    return 1+(y>0 && x+y<16);
}

// New value of count x if the opposite bit is observed
void StateTable::discount(int& x){
  if (x>2) x=ilog(x)/6-1;
}

// compute next x,y (0 to N) given input b (0 or 1)
void StateTable::next_state(int& x, int& y, int b){
  if (x<y)
    next_state(y, x, 1-b);
  else{
    if (b){
      ++y;
      discount(x);
    }
    else{
      ++x;
      discount(y);
    }
    while (!t[x][y][1]){
      if (y<2) --x;
      else{
        x=(x*(y-1)+(y/2))/y;
        --y;
      }
    }
  }
}

// Initialize next state table ns[state*4] -> next if 0, next if 1, x, y
StateTable::StateTable(): ns(1024) {

  // Assign states
  int state=0;
  for (int i=0; i<256; ++i){
    for (int y=0; y<=i; ++y){
      int x=i-y;
      int n=num_states(x, y);
      if (n){
        t[x][y][0]=state;
        t[x][y][1]=n;
        state+=n;
      }
    }
  }

  // Print/generate next state table
  state=0;
  for (int i=0; i<N; ++i){
    for (int y=0; y<=i; ++y){
      int x=i-y;
      for (int k=0; k<t[x][y][1]; ++k){
        int x0=x, y0=y, x1=x, y1=y; // next x,y for input 0,1
        int ns0=0, ns1=0;
        if (state<15){
          ++x0;
          ++y1;
          ns0=t[x0][y0][0]+state-t[x][y][0];
          ns1=t[x1][y1][0]+state-t[x][y][0];
          if (x>0) ns1+=t[x-1][y+1][1];
          ns[state*4]=ns0;
          ns[state*4+1]=ns1;
          ns[state*4+2]=x;
          ns[state*4+3]=y;
        }
        else if (t[x][y][1]){
          next_state(x0, y0, 0);
          next_state(x1, y1, 1);
          ns[state*4]=ns0=t[x0][y0][0];
          ns[state*4+1]=ns1=t[x1][y1][0]+(t[x1][y1][1]>1);
          ns[state*4+2]=x;
          ns[state*4+3]=y;
        }
        // uncomment to print table above
        // printf("{%3d,%3d,%2d,%2d},", ns[state*4], ns[state*4+1],
        // ns[state*4+2], ns[state*4+3]);
        // if (state%4==3) printf(" // %d-%d\n ", state-3, state);
        assert(state>=0 && state<256);
        assert(t[x][y][1]>0);
        assert(t[x][y][0]<=state);
        assert(t[x][y][0]+t[x][y][1]>state);
        assert(t[x][y][1]<=6);
        assert(t[x0][y0][1]>0);
        assert(t[x1][y1][1]>0);
        assert(ns0-t[x0][y0][0]<t[x0][y0][1]);
        assert(ns0-t[x0][y0][0]>=0);
        assert(ns1-t[x1][y1][0]<t[x1][y1][1]);
        assert(ns1-t[x1][y1][0]>=0);
        ++state;
      }
    }
  }
  // printf("%d states\n", state); exit(0); // uncomment to print table above
}

///////////////////////////// Squash //////////////////////////////

// return p = 1/(1 + exp(-d)), d scaled by 8 bits, p scaled by 12 bits
int squash(int d){
  static const int t[33]={
    1,2,3,6,10,16,27,45,73,120,194,310,488,747,1101,
    1546,2047,2549,2994,3348,3607,3785,3901,3975,4022,
    4050,4068,4079,4085,4089,4092,4093,4094};
    if (d>2047) return 4095;
    if (d<-2047) return 0;
    int w=d&127;
    d=(d>>7)+16;
    return (t[d]*(128-w)+t[(d+1)]*w+64) >> 7;
}

//////////////////////////// Stretch ///////////////////////////////

// Inverse of squash. d = ln(p/(1-p)), d scaled by 8 bits, p by 12 bits.
// d has range -2047 to 2047 representing -8 to 8. p has range 0 to 4095.

class Stretch{
  Array<short> t;
public:
  Stretch();
  int operator()(int p) const{
    assert(p>=0 && p<4096);
    return t[p];
  }
}stretch;

Stretch::Stretch(): t(4096) {
  int pi=0;
  for (int x=-2047; x<=2047; ++x){ // invert squash()
    int i=squash(x);
    for (int j=pi; j<=i; ++j)
      t[j]=x;
    pi=i+1;
  }
  t[4095]=2047;
}




//////////////////////////// Mixer /////////////////////////////

// Mixer m(N, M, S=1, w=0) combines models using M neural networks with
// N inputs each, of which up to S may be selected. If S > 1 then
// the outputs of these neural networks are combined using another
// neural network (with parameters S, 1, 1). If S = 1 then the
// output is direct. The weights are initially w (+-32K).
// It is used as follows:
// m.update() trains the network where the expected output is the
// last bit (in the global variable y).
// m.add(stretch(p)) inputs prediction from one of N models. The
// prediction should be positive to predict a 1 bit, negative for 0,
// nominally +-256 to +-2K. The maximum allowed value is +-32K but
// using such large values may cause overflow if N is large.
// m.set(cxt, range) selects cxt as one of 'range' neural networks to
// use. 0 <= cxt < range. Should be called up to S times such
// that the total of the ranges is <= M.
// m.p() returns the output prediction that the next bit is 1 as a
// 12 bit number (0 to 4095).

// dot_product returns dot product t*w of n elements. n is rounded
// up to a multiple of 8. Result is scaled down by 8 bits.
#ifdef NOASM// no assembly language
int dot_product(short *t, short *w, int n){
  int sum=0;
  n=(n+7)&-8;
  for (int i=0; i<n; i+=2)
    sum+=(t[i]*w[i]+t[i+1]*w[i+1]) >> 8;
  return sum;
}
#else// The NASM version uses MMX and is about 8 times faster.
extern "C" int dot_product(short *t, short *w, int n); // in NASM
#endif

// Train neural network weights w[n] given inputs t[n] and err.
// w[i] += t[i]*err, i=0..n-1. t, w, err are signed 16 bits (+- 32K).
// err is scaled 16 bits (representing +- 1/2). w[i] is clamped to +- 32K
// and rounded. n is rounded up to a multiple of 8.
#ifdef NOASM
void train(short *t, short *w, int n, int err){
  n=(n+7)&-8;
  for (int i=0; i<n; ++i){
    int wt=w[i]+((t[i]*err*2>>16)+1>>1);
    if (wt<-32768) wt=-32768;
    if (wt>32767) wt=32767;
    w[i]=wt;
  }
}
#else
extern "C" void train(short *t, short *w, int n, int err); // in NASM
#endif

//////////////////////////// APM //////////////////////////////

// APM maps a probability and a context into a new probability
// that bit y will next be 1. After each guess it updates
// its state to improve future guesses. Methods:
//
// APM a(N) creates with N contexts, uses 66*N bytes memory.
// a.p(pr, cx, rate=7) returned adjusted probability in context cx
// (0 to N-1). rate determines the learning rate (smaller = faster,
// default 7). Probabilities are scaled 12 bits (0-4095).

class APM{
  int index; // last p, context
  const int N; // number of contexts
  Array<U16> t; // [N][33]: p, context -> p
public:
  APM(int n);
  int p(int pr=2048, int cxt=0, int rate=7){
    assert(pr>=0 && pr<4096 && cxt>=0 && cxt<N && rate>0 && rate<32);
    pr=stretch(pr);
    int g=(y<<16)+(y<<rate)-y-y;
    t[index] += g-t[index] >> rate;
    t[index+1] += g-t[index+1] >> rate;
    const int w=pr&127; // interpolation weight (33 points)
    index=(pr+2048>>7)+cxt*33;
    return t[index]*(128-w)+t[index+1]*w >> 11;
  }
};

// maps p, cxt -> p initially
APM::APM(int n=121): index(0), N(n), t(n*33) {
  for (int i=0; i<N; ++i)
    for (int j=0; j<33; ++j)
      t[i*33+j] = i==0 ? squash((j-16)*128)*16 : t[j];
}

class TriAPM{
  APM a1, a2, a3, a4;
public:
  TriAPM();
  int p(int pr, int cxt){
    return a1.p(pr, c0+256*buf(cxt,1), 7)*(6-3*uiq)+
      a2.p(pr, c0+256*buf(cxt,2), 7)*(5-3*uiq)+
      a3.p(pr, c0+256*buf(cxt,3), 7)*(5-3*uiq)+
      a4.p(pr, c0+256*min(lastnull,31),7)*uiq*9+8>>4;
  }
};

TriAPM::TriAPM(): a1(16384), a2(8192), a3(8192), a4(8192){
}

class Mixer{
  const int N, M, S; // max inputs, max contexts, max context sets
  Array<short, 16> tx; // N inputs from add()
  Array<short, 16> wx; // N*M weights
  Array<int> cxt; // S contexts
  int ncxt; // number of contexts (0 to S)
  int base; // offset of next context
  int nx; // Number of inputs in tx, 0 to N
  Array<int> pr; // last result (scaled 12 bits)
  Mixer* mp; // points to a Mixer to combine results
  Array<U32> pr0;
  APM* a;
  APM* b;
  APM* c;
  APM* d;
  TriAPM e1, e2, e3, e4, e5, e6;
public:
  Mixer(int n, int m, int s=1, int w=0);

  // Adjust weights to minimize coding cost of last prediction
  void update(){
    for (int i=0; i<ncxt; ++i){
      int err=((y<<12)-pr[i])*7;
      assert(err>=-32768 && err<32768);
      train(&tx[0], &wx[cxt[i]*N], nx, err);
    }
    nx=base=ncxt=0;
  }

  // Input x (call up to N times)
  void add(int x){
    assert(nx<N);
    tx[nx++]=x;
  }

  // Set a context (call S times, sum of ranges <= M)
  void set(int cx, int range){
    assert(range>=0);
    assert(ncxt<S);
    assert(cx>=0);
    assert(base+cx<M);
    cxt[ncxt++]=base+cx;
    base+=range;
  }

  // predict next bit
  int p(){
    static const int weight[16]={25,25,25,25,37,0,0,0,0,17,17,17,17,17,17,17};
    while (nx&7) tx[nx++]=0; // pad
    if (mp){ // combine outputs
      mp->update();
      int priAnchor=128;
      for (int i=0; i<ncxt; ++i){
        pr[i]=squash(dot_product(&tx[0], &wx[cxt[i]*N], nx)>>5);
        priAnchor+=pr[i]*weight[i];
      }
      priAnchor>>=8;
      for (int i=0; i<ncxt; ++i){
        pr[i]=pr[i]*9+priAnchor*7+8>>4; // Weighted contexts
        for (int j=2;j>=0;j--)
          pr[i]=a[(j)*S+i].p(pr[i],(pr0[i]&15)*11+((pr0[i]>>(1+(j*2)<<2))&15),8)*8
          +b[(j)*S+i].p(pr[i],(prF &15)*11+((prF>>(1+(j*2)<<2))&15),8)*9
          +c[(j)*S+i].p(pr[i],(pr0[i]&15)*11+((pr0[i]>>((j*2)<<2))&15),8)*8
          +d[(j)*S+i].p(pr[i],(prF&15)*11+((prF>>((j*2)<<2))&15),8)*9
          +pr[i]*94+64>>7;
        mp->add(stretch(pr[i]));
        pr0[i]=pr0[i]<<4|((pr[i]*45055)>>24); // Save old contexts
      }
      mp->set(0, 1);
      int mpp=mp->p();
      mpp=e1.p(e2.p(mpp,1),2)*7+e3.p(e4.p(mpp,2),1)*5+e5.p(e6.p(mpp,3),1)*4+8>>4;
      prF=prF<<4|((mpp*45055)>>24); // Save final context
      return mpp;
    }
    else{ // S=1 context
      return pr[0]=squash(dot_product(&tx[0], &wx[0], nx)>>8);
    }
  }
  ~Mixer();
};

Mixer::~Mixer(){
  delete mp;
}

Mixer::Mixer(int n, int m, int s, int w):
N((n+7)&-8), M(m), S(s), tx(N), wx(N*M), cxt(S), ncxt(0),
base(0), nx(0), pr(S), mp(0), pr0(S),
e1(), e2(), e3(), e4(), e5(), e6() {
  assert(n>0 && N>0 && (N&7)==0 && M>0);
  for (int i=0; i<S; ++i)
    pr[i]=2048;
  for (int i=0; i<N*M; ++i)
    wx[i]=w;
  if (S>1) mp=new Mixer(S, 1, 1, 0x7fff);
  a=new APM[S*3];
  b=new APM[S*3];
  c=new APM[S*3];
  d=new APM[S*3];
}

//////////////////////////// StateMap //////////////////////////

// A StateMap maps a nonstationary counter state to a probability.
// After each mapping, the mapping is adjusted to improve future
// predictions. Methods:
//
// sm.p(cx) converts state cx (0-255) to a probability (0-4095).

// Counter state -> probability * 256
class StateMap{
protected:
  int cxt; // context
  Array<U16> t; // 256 states -> probability * 64K
public:
  StateMap();
  int p(int cx){
    assert(cx>=0 && cx<t.size());
    t[cxt]+=(y<<16)-t[cxt]+128 >> 8;
    return t[cxt=cx] >> 4;
  }
};

StateMap::StateMap(): cxt(0), t(256) {
  for (int i=0; i<256; ++i){
    int n0=nex(i,2);
    int n1=nex(i,3);
    if (n0==0) n1*=64;
    if (n1==0) n0*=64;
    t[i] = 65536*(n1+1)/(n0+n1+2);
  }
}

//////////////////////////// hash //////////////////////////////

// Hash 2-5 ints.
inline U32 hash(U32 a, U32 b, U32 c=0xffffffff, U32 d=0xffffffff,
                U32 e=0xffffffff){
                  U32 h=a*200002979u+b*30005491u+c*50004239u+d*70004807u+e*110002499u;
                  return h^h>>9^a>>2^b>>3^c>>4^d>>5^e>>6;
}

///////////////////////////// BH ////////////////////////////////

// A BH maps a 32 bit hash to an array of B bytes (checksum and B-2 values)
//
// BH bh(N); creates N element table with B bytes each.
// N must be a power of 2. The first byte of each element is
// reserved for a checksum to detect collisions. The remaining
// B-1 bytes are values, prioritized by the first value. This
// byte is 0 to mark an unused element.
//
// bh[i] returns a pointer to the i'th element, such that
// bh[i][0] is a checksum of i, bh[i][1] is the priority, and
// bh[i][2..B-1] are other values (0-255).
// The low lg(n) bits as an index into the table.
// If a collision is detected, up to M nearby locations in the same
// cache line are tested and the first matching checksum or
// empty element is returned.
// If no match or empty element is found, then the lowest priority
// element is replaced.

// 2 byte checksum with LRU replacement (except last 2 by priority)
template <int B> class BH{
  enum {M=8}; // search limit
  Array<U8, 64> t; // elements
  U32 n; // size-1
public:
  BH(int i): t(i*B), n(i-1) {
    assert(B>=2 && i>0 && (i&(i-1))==0); // size a power of 2?
  }
  U8* operator[](U32 i);
};

template <int B>
inline U8* BH<B>::operator[](U32 i){
  int chk=(i>>16^i)&0xffff;
  i=i*M&n;
  U8 *p;
  U16 *cp;
  int j;
  for (j=0; j<M; ++j){
    p=&t[(i+j)*B];
    cp=(U16*)p;
    if (p[2]==0) *cp=chk;
    if (*cp==chk) break; // found
  }
  if (j==0) return p+1; // front
  static U8 tmp[B]; // element to move to front
  if (j==M){
    --j;
    memset(tmp, 0, B);
    *(U16*)tmp=chk;
    if (M>2 && t[(i+j)*B+2]>t[(i+j-1)*B+2]) --j;
  }
  else memcpy(tmp, cp, B);
  memmove(&t[(i+1)*B], &t[i*B], j*B);
  memcpy(&t[i*B], tmp, B);
  return &t[i*B+1];
}

/////////////////////////// ContextMap /////////////////////////
//
// A ContextMap maps contexts to a bit histories and makes predictions
// to a Mixer. Methods common to all classes:
//
// ContextMap cm(M, C); creates using about M bytes of memory (a power
// of 2) for C contexts.
// cm.set(cx); sets the next context to cx, called up to C times
// cx is an arbitrary 32 bit value that identifies the context.
// It should be called before predicting the first bit of each byte.
// cm.mix(m) updates Mixer m with the next prediction. Returns 1
// if context cx is found, else 0. Then it extends all the contexts with
// global bit y. It should be called for every bit:
//
// if (bpos==0)
// for (int i=0; i<C; ++i) cm.set(cxt[i]);
// cm.mix(m);

// Predict to mixer m from bit history state s, using sm to map s to
// a probability.
inline int mix2(Mixer& m, int s, StateMap& sm){
  int p1=sm.p(s);
  int n0=nex(s,2);
  int n1=nex(s,3);
  int st=stretch(p1)>>2;
  m.add(st);
  p1>>=4;
  int p0=255-p1;
  // m.add(p1-p0);
  m.add(st*(!n0-!n1));
  if(level>=7) m.add((((p1&-!n0)-(p0&-!n1))*7)>>3);
  if(level>=7) m.add((((p1&-!n1)-(p0&-!n0))*7)>>3);
  return s>0;
}

// Context map for large contexts. Most modeling uses this type of context
// map. It includes a built in RunContextMap to predict the last byte seen
// in the same context, and also bit-level contexts that map to a bit
// history state.
//
// Bit histories are stored in a hash table. The table is organized into
// 64-byte buckets alinged on cache page boundaries. Each bucket contains
// a hash chain of 7 elements, plus a 2 element queue (packed into 1 byte)
// of the last 2 elements accessed for LRU replacement. Each element has
// a 2 byte checksum for detecting collisions, and an array of 7 bit history
// states indexed by the last 0 to 2 bits of context. The buckets are indexed
// by a context ending after 0, 2, or 5 bits of the current byte. Thus, each
// byte modeled results in 3 main memory accesses per context, with all other
// accesses to cache.
//
// On bits 0, 2 and 5, the context is updated and a new bucket is selected.
// The most recently accessed element is tried first, by comparing the
// 16 bit checksum, then the 7 elements are searched linearly. If no match
// is found, then the element with the lowest priority among the 5 elements
// not in the LRU queue is replaced. After a replacement, the queue is
// emptied (so that consecutive misses favor a LFU replacement policy).
// In all cases, the found/replaced element is put in the front of the queue.
//
// The priority is the state number of the first element (the one with 0
// additional bits of context). The states are sorted by increasing n0+n1
// (number of bits seen), implementing a LFU replacement policy.
//
// When the context ends on a byte boundary (bit 0), only 3 of the 7 bit
// history states are used. The remaining 4 bytes implement a run model
// as follows: <count:7,d:1> <b1> <b2> <b3> where <b1> is the last byte
// seen, possibly repeated, and <b2> and <b3> are the two bytes seen
// before the first <b1>. <count:7,d:1> is a 7 bit count and a 1 bit
// flag. If d=0 then <count> = 1..127 is the number of repeats of <b1>
// and no other bytes have been seen, and <b2><b3> are not used.
// If <d> = 1 then the history is <b3>, <b2>, and <count> - 2 repeats
// of <b1>. In this case, <b3> is valid only if <count> >= 3 and
// <b2> is valid only if <count> >= 2.
//
// As an optimization, the last two hash elements of each byte (representing
// contexts with 2-7 bits) are not updated until a context is seen for
// a second time. This is indicated by <count,d> = <1,0>. After update,
// <count,d> is updated to <2,0> or <2,1>.

class ContextMap{
  const int C; // max number of contexts
  class E{ // hash element, 64 bytes
    U16 chk[7]; // byte context checksums
    U8 last; // last 2 accesses (0-6) in low, high nibble
  public:
    U8 bh[7][7];// byte context, 3-bit context -> bit history state
    // bh[][0] = 1st bit, bh[][1,2] = 2nd bit, bh[][3..6] = 3rd bit
    // bh[][0] is also a replacement priority, 0 = empty
    U8* get(U16 chk);// Find element (0-6) matching checksum.
    // If not found, insert or replace lowest priority (not last).
  };
  Array<E, 64> t;// bit histories for bits 0-1, 2-4, 5-7
  // For 0-1, also contains a run count in bh[][4] and value in bh[][5]
  // and pending update count in bh[7]
  Array<U8*> cp; // C pointers to current bit history
  Array<U8*> cp0; // First element of 7 element array containing cp[i]
  Array<U32> cxt; // C whole byte contexts (hashes)
  Array<U8*> runp; // C [0..3] = count, value, unused, unused
  StateMap *sm; // C maps of state -> p
  int cn; // Next context to set by set()
  void update(U32 cx, int c); // train model that context cx predicts c
  int mix1(Mixer& m, int cc, int bp, int c1, int y1);
  // mix() with global context passed as arguments to improve speed.
public:
  ContextMap(int m, int c=1); // m = memory in bytes, a power of 2, C = c
  ~ContextMap();
  void set(U32 cx); // set next whole byte context
  int mix(Mixer& m) {return mix1(m, c0, bpos, buf(1), y);}
};

// Find or create hash element matching checksum ch
inline U8* ContextMap::E::get(U16 ch){
  if (chk[last&15]==ch) return &bh[last&15][0];
  int b=0xffff, bi=0;
  for (int i=0; i<7; ++i){
    if (chk[i]==ch) return last=last<<4|i, &bh[i][0];
    int pri=bh[i][0];
    if ((last&15)!=i && last>>4!=i && pri<b) b=pri, bi=i;
  }
  return last=0xf0|bi, chk[bi]=ch, (U8*)memset(&bh[bi][0], 0, 7);
}

// Construct using m bytes of memory for c contexts
ContextMap::ContextMap(int m, int c): C(c), t(m>>6), cp(c), cp0(c),
cxt(c), runp(c), cn(0) {
  assert(m>=64 && (m&m-1)==0); // power of 2?
  assert(sizeof(E)==64);
  sm=new StateMap[C];
  for (int i=0; i<C; ++i){
    cp0[i]=cp[i]=&t[0].bh[0][0];
    runp[i]=cp[i]+3;
  }
}

ContextMap::~ContextMap(){
  delete[] sm;
}

// Set the i'th context to cx
inline void ContextMap::set(U32 cx){
  int i=cn++;
  assert(i>=0 && i<C);
  cx=cx*987654323+i; // permute (don't hash) cx to spread the distribution
  cx=cx<<16|cx>>16;
  cxt[i]=cx*123456791+i;
}

// Update the model with bit y1, and predict next bit to mixer m.
// Context: cc=c0, bp=bpos, c1=buf(1), y1=y.
int ContextMap::mix1(Mixer& m, int cc, int bp, int c1, int y1){

  // Update model with y
  int result=0;
  for (int i=0; i<cn; ++i){
    if (cp[i]){
      assert(cp[i]>=&t[0].bh[0][0] && cp[i]<=&t[t.size()-1].bh[6][6]);
      assert((long(cp[i])&63)>=15);
      int ns=nex(*cp[i], y1);
      if (ns>=204 && rnd() << (452-ns>>3)) ns-=4; // probabilistic increment
      *cp[i]=ns;
    }

    // Update context pointers
    if (bpos>1 && runp[i][0]==0)
      cp[i]=0;
    else if (bpos==1||bpos==3||bpos==6)
      cp[i]=cp0[i]+1+(cc&1);
    else if (bpos==4||bpos==7)
      cp[i]=cp0[i]+3+(cc&3);
    else{
      cp0[i]=cp[i]=t[cxt[i]+cc&t.size()-1].get(cxt[i]>>16);

      // Update pending bit histories for bits 2-7
      if (bpos==0){
        if (cp0[i][3]==2){
          const int c=cp0[i][4]+256;
          U8 *p=t[cxt[i]+(c>>6)&t.size()-1].get(cxt[i]>>16);
          p[0]=1+((c>>5)&1);
          p[1+((c>>5)&1)]=1+((c>>4)&1);
          p[3+((c>>4)&3)]=1+((c>>3)&1);
          p=t[cxt[i]+(c>>3)&t.size()-1].get(cxt[i]>>16);
          p[0]=1+((c>>2)&1);
          p[1+((c>>2)&1)]=1+((c>>1)&1);
          p[3+((c>>1)&3)]=1+(c&1);
          cp0[i][6]=0;
        }
        // Update run count of previous context
        if (runp[i][0]==0) // new context
          runp[i][0]=2, runp[i][1]=c1;
        else if (runp[i][1]!=c1) // different byte in context
          runp[i][0]=1, runp[i][1]=c1;
        else if (runp[i][0]<4) // same byte in context
          runp[i][0]+=2;
        runp[i]=cp0[i]+3;
      }
    }

    // predict from last byte in context
    int rc=runp[i][0]; // count*2, +1 if 2 different bytes seen
    if (runp[i][1]+256>>8-bp==cc){
      int b=(runp[i][1]>>7-bp&1)*2-1; // predicted bit + for 1, - for 0
      int c=ilog(rc+1)<<2+(~rc&1);
      m.add(b*c);
    }
    else
      m.add(0);

    // predict from bit context
    result+=mix2(m, cp[i] ? *cp[i] : 0, sm[i]);
  }
  if (bp==7) cn=0;
  return result;
}

//////////////////////////// Models //////////////////////////////

// All of the models below take a Mixer as a parameter and write
// predictions to it.

//////////////////////////// matchModel ///////////////////////////

// matchModel() finds the longest matching context and returns its length

int matchModel(Mixer& m){
  const int MAXLEN=65534; // longest allowed match + 1
  static Array<int> t(MEM); // hash table of pointers to contexts
  static int h=0; // hash of last 7 bytes
  static int ptr=0; // points to next byte of match if any
  static int len=0; // length of match, or 0 if no match
  static int result=0;

  if (!bpos){
    h=h*997*8+buf(1)+1&t.size()-1; // update context hash
    if (len) ++len, ++ptr;
    else{ // find match
      ptr=t[h];
      if (ptr && pos-ptr<buf.size())
        while (buf(len+1)==buf[ptr-len-1] && len<MAXLEN) ++len;
    }
    t[h]=pos; // update hash table
    result=len;
  }

  // predict
  if (len>MAXLEN) len=MAXLEN;
  int sgn;
  if (len && buf(1)==buf[ptr-1] && c0==buf[ptr]+256>>8-bpos){
    if (buf[ptr]>>7-bpos&1) sgn=1;
    else sgn=-1;
  }
  else sgn=len=0;
  m.add(sgn*4*ilog(len));
  m.add(sgn*64*min(len, 32));
  return result;
}



//////////////////////////// indirectModel /////////////////////

// The context is a byte string history that occurs within a
// 1 or 2 byte context.

int indirect2Model(Mixer& m){
  static ContextMap cm(MEM*2, 40);
  int p=pos&7, buf1=buf(1);
  if (!bpos){
    cm.set(c4&0x00ff00ff);
    cm.set(c4&0xff0000ff);
    cm.set(c4&0x00ffff00);
    cm.set(c4&0xff00ff00);
    cm.set(buf(1)|buf(5)<<8);
    cm.set(buf(1)|buf(6)<<8);
    cm.set(buf(3)|buf(6)<<8);
    cm.set(buf(4)|buf(8)<<8);
    for (int i=8,c1=buf(8),c2=buf(9);i>0;c2=c1,c1=buf(--i)){
      int t=c2<<8|c1;
      cm.set(t&0xf0f0);
      cm.set(t&0x0f0f);
      cm.set(t&0x0ff0);
      cm.set(t&0xf00f);
    }
  }
  return cm.mix(m)>>2;
}


void randomModel(Mixer& m){
  static ContextMap cm(MEM*16, 468);
  static U32 t1[8][256];
  static U16 t2[8][0x10000];

  if (!bpos){
    for(int i=0;i<8;i++){
      U32 d=buf(i+2)<<8|buf(1), c=d&255;
      U32& r1=t1[i][d>>8];
      r1=r1<<8|c;
      U16& r2=t2[i][buf(i+3)<<8|buf(i+2)];
      r2=r2<<8|c;
      U32 t=c|t1[i][c]<<8;
      cm.set(hash(i+400,t&0xffff));
      cm.set(hash(i+400,t&0xffffff));
      cm.set(hash(i+400,t));
      cm.set(hash(i+400,t&0xff00));
      t=d|t2[i][d]<<16;
      cm.set(hash(i+400,t&0xffffff));
      cm.set(hash(i+400,t));
    }
    for(int i=0;i<35;i++){
      int k=randomArray(i);
      int j=k&c4;
      cm.set(hash(i,j&0x0000ffff));
      cm.set(hash(i,j&0x00ffff00));
      cm.set(hash(i,j&0xff00ff00));
      cm.set(hash(i,j&0x00ff00ff));
      cm.set(hash(i,j&0xff0000ff));

      cm.set(hash(i,k&(buf(1)|buf(5)<<8)));
      cm.set(hash(i,k&(buf(1)|buf(6)<<8)));
      cm.set(hash(i,k&(buf(3)|buf(6)<<8)));
      cm.set(hash(i,k&(buf(4)|buf(8)<<8)));

      cm.set(hash(i,k&(buf(3)|buf(4)<<8)));
      cm.set(hash(i,k&(buf(1)|buf(3)<<8|buf(5)<<16)));
      cm.set(hash(i,k&(buf(2)|buf(4)<<8|buf(6)<<16)));
    }
  }
  cm.mix(m);
}

///////////////////////////// chartModel ////////////////////////////

void chartModel(Mixer& m){
  static U32 chart[20];
  static U8 indirect[1088];
  static U8 indirect2[256];
  static U8 indirect3[256*256];
  static ContextMap cm(MEM*16,64),cn(MEM,23);
  if (!bpos){
    const
      int w = c4&65535,
      w0 = c4&16777215,
      w1 = c4&255,
      w2 = c4<<8&65280,
      w3 = c4<<8&16711680,
      w4 = c4<<8&4278190080,
      a[3]={c4>>5&460551,
      c4>>2&460551,
      c4&197379},
      b[3]={c4>>23&448|
      c4>>18&56|
      c4>>13&7,
      c4>>20&448|
      c4>>15&56|
      c4>>10&7,
      c4>>18&48|
      c4>>12&12|
      c4>>8&3},
      d[3]={c4>>15&448|
      c4>>10&56|
      c4>>5&7,
      c4>>12&448|
      c4>>7&56|
      c4>>2&7,
      c4>>10&48|
      c4>>4&12|
      c4&3},
      c[3]={c4&255,
      c4>>8&255,
      c4>>16&255};



    for (
      int i=0,
      j=0,
      f=b[0],
      e=a[0];

    i<3;
    j=++i<<9,
      f=j|b[i],
      e=a[i]
    ){
      indirect[f]=w1; // <--Update indirect
      const int g =indirect[j|d[i]];
      chart[i<<3|e>>16&255]=w0; // <--Fix chart
      chart[i<<3|e>>8&255]=w<<8; // <--Update chart
      cn.set(e); // <--Model previous/current/next slot
      cn.set(g); // <--Guesses next "c4&0xFF"
      cn.set(w2|g); // <--Guesses next "c4&0xFFFF"
      cn.set(w3|g); // <--Guesses next "c4&0xFF00FF"
      cn.set(w4|g); // <--Guesses next "c4&0xFF0000FF"
      cm.set(c[i]); // <--Models buf(1,2,3)
    }

    indirect2[buf(2)]=buf(1);
    int g=indirect2[buf(1)];
    cn.set(g); // <--Guesses next "c4&0xFF"
    cn.set(w2|g); // <--Guesses next "c4&0xFFFF"
    cn.set(w3|g); // <--Guesses next "c4&0xFF00FF"
    cn.set(w4|g); // <--Guesses next "c4&0xFF0000FF"

    indirect3[buf(3)<<8|buf(2)]=buf(1);
    g=indirect3[buf(2)<<8|buf(1)];
    cn.set(g); // <--Guesses next "c4&0xFF"
    cn.set(w2|g); // <--Guesses next "c4&0xFFFF"
    cn.set(w3|g); // <--Guesses next "c4&0xFF00FF"
    cn.set(w4|g); // <--Guesses next "c4&0xFF0000FF"


    for (
      int
      i=0,
      s=0,
      e=a[0],
      k=chart[0];

    i<20;
    s=++i>>3,
      e=a[s],
      k=chart[i]
    ){ // k e
      cm.set(k<<s); // 111 000
      cm.set(hash(e,k,s)); // 111 111
      cm.set((hash(e&255,k>>16)^(k&255))<<s); // 101 001
    }
    cm.set(0);
  }
  cn.mix(m);
  cm.mix(m);
}

//////////////////////////// contextModel //////////////////////

typedef enum {DEFAULT, JPEG, EXE, TEXT}Filetype;

// This combines all the context models with a Mixer.

int contextModel2(){
  static ContextMap cm(MEM*4, 7*3);
  static Mixer m(3221, 1252+121*7+32, 16, 128);
  static U32 cxt[3][16]; // order 0-11 contexts
  static Filetype filetype=DEFAULT;
  static int size=0; // bytes remaining in block

  // Parse filetype and size
  if (bpos==0){
    --size;
    if (size==-1) filetype=(Filetype)buf(1);
    if (size==-5){
      size=buf(4)<<24|buf(3)<<16|buf(2)<<8|buf(1);
      if (filetype==EXE) size+=8;
    }
  }

  m.update();
  m.add(256);

  matchModel(m);

  // Normal model
  if (bpos==0){
    for (int i=15; i>0; --i){ // update order 0-11 context hashes
      if (lastnull || !uiq)
        for(int j=0;j<3;j++)cxt[j][i]=cxt[j][i-1]*65+buf(1,j+1)+1;
      else for(int j=0;j<3;j++)cxt[j][i]=0;
    }
    for (int i=0; i<7; ++i){
      for(int j=0;j<3;j++)cm.set(cxt[j][i]);
    }
  }
  int order=cm.mix(m)*85>>8;

  chartModel(m);
  int ind = indirect2Model(m);
  if(level>=7) randomModel(m);

  U32 c1=buf(1,4), c2=buf(2,4), c3=buf(3,4), c;

  m.set(c0, 256);
  m.set(c1, 256);
  m.set(c2, 256);
  m.set(c3, 256);
  m.set(min(lastnull,31),32);
  m.set(ind, 11);
  m.set(order+8*(c4>>5&7)+64*(c1==c2), 128);
  m.set(0,1);
  m.set(ind<<3|order,88);

  for(int j=6;j>=0;j--)m.set((prF&15)*11+((prF>>(1+j<<2))&15),121);
  int pr=m.p();
  return pr;
}


//////////////////////////// Predictor /////////////////////////

// A Predictor estimates the probability that the next bit of
// uncompressed data is 1. Methods:
// p() returns P(1) as a 12 bit number (0-4095).
// update(y) trains the predictor with the actual bit (0 or 1).

class Predictor{
  int pr; // next prediction
public:
  Predictor();
  int p() const {assert(pr>=0 && pr<4096); return pr;}
  void update();
};

Predictor::Predictor(): pr(2048) {}

void Predictor::update(){
  // Update global context: pos, bpos, c0, c4, buf
  c0+=c0+y;
  if (c0>=256){
    buf[pos++]=c0;
    if (uiq && c0==256){
      c4=0;
      lastnull=0;
    }
    else{
      c4=(c4<<8)+c0-256;
      lastnull++;
    };
    c0=1;
  }
  bpos=(bpos+1)&7;
  pr=contextModel2();
}

//////////////////////////// Encoder ////////////////////////////

// An Encoder does arithmetic encoding. Methods:
// Encoder(COMPRESS, f) creates encoder for compression to archive f, which
// must be open past any header for writing in binary mode.
// Encoder(DECOMPRESS, f) creates encoder for decompression from archive f,
// which must be open past any header for reading in binary mode.
// code(i) in COMPRESS mode compresses bit i (0 or 1) to file f.
// code() in DECOMPRESS mode returns the next decompressed bit from file f.
// Global y is set to the last bit coded or decoded by code().
// compress(c) in COMPRESS mode compresses one byte.
// decompress() in DECOMPRESS mode decompresses and returns one byte.
// flush() should be called exactly once after compression is done and
// before closing f. It does nothing in DECOMPRESS mode.
// size() returns current length of archive
// setFile(f) sets alternate source to FILE* f for decompress() in COMPRESS
// mode (for testing transforms).
// If level (global) is 0, then data is stored without arithmetic coding.

typedef enum {COMPRESS, DECOMPRESS} Mode;
class Encoder{
private:
  Predictor predictor;
  const Mode mode; // Compress or decompress?
  FILE* archive; // Compressed data file
  U32 x1, x2; // Range, initially [0, 1), scaled by 2^32
  U32 x; // Decompress mode: last 4 input bytes of archive
  FILE *alt; // decompress() source in COMPRESS mode

  // Compress bit y or return decompressed bit
  int code(int i=0){
    int p=predictor.p();
    assert(p>=0 && p<4096);
    p+=p<2048;
    U32 xmid=x1 + (x2-x1>>12)*p + ((x2-x1&0xfff)*p>>12);
    assert(xmid>=x1 && xmid<x2);
    if (mode==DECOMPRESS) y=x<=xmid; else y=i;
    y ? (x2=xmid) : (x1=xmid+1);
    predictor.update();
    while (((x1^x2)&0xff000000)==0){ // pass equal leading bytes of range
      if (mode==COMPRESS) putc(x2>>24, archive);
      x1<<=8;
      x2=(x2<<8)+255;
      if (mode==DECOMPRESS) x=(x<<8)+(getc(archive)&255); // EOF is OK
    }
    return y;
  }

public:
  Encoder(Mode m, FILE* f);
  Mode getMode() const {return mode;}
  long size() const {return ftell(archive);} // length of archive so far
  void flush(); // call this when compression is finished
  void setFile(FILE* f){alt=f;}

  // Compress one byte
  void compress(int c){
    assert(mode==COMPRESS);
    if (level==0)
      putc(c, archive);
    else
      for (int i=7; i>=0; --i)
        code((c>>i)&1);
  }

  // Decompress and return one byte
  int decompress(){
    if (mode==COMPRESS){
      assert(alt);
      return getc(alt);
    }
    else if (level==0)
      return getc(archive);
    else{
      int c=0;
      for (int i=0; i<8; ++i)
        c+=c+code();
      return c;
    }
  }
};

Encoder::Encoder(Mode m, FILE* f):
mode(m), archive(f), x1(0), x2(0xffffffff), x(0), alt(0) {
  if (level>0 && mode==DECOMPRESS){ // x = first 4 bytes of archive
    for (int i=0; i<4; ++i)
      x=(x<<8)+(getc(archive)&255);
  }
}

void Encoder::flush(){
  if (mode==COMPRESS && level>0)
    putc(x1>>24, archive); // Flush first unequal byte of range
}

/////////////////////////// Filters /////////////////////////////////
//
// Before compression, data is encoded in blocks with the following format:
//
// <type> <size> <encoded-data>
//
// Type is 1 byte (type Filetype): DEFAULT=0, JPEG, EXE
// Size is 4 bytes in big-endian format.
// Encoded-data decodes to <size> bytes. The encoded size might be
// different. Encoded data is designed to be more compressible.
//
// void encode(FILE* in, FILE* out, int n);
//
// Reads n bytes of in (open in "rb" mode) and encodes one or
// more blocks to temporary file out (open in "wb+" mode).
// The file pointer of in is advanced n bytes. The file pointer of
// out is positioned after the last byte written.
//
// en.setFile(FILE* out);
// int decode(Encoder& en);
//
// Decodes and returns one byte. Input is from en.decompress(), which
// reads from out if in COMPRESS mode. During compression, n calls
// to decode() must exactly match n bytes of in, or else it is compressed
// as type 0 without encoding.
//
// Filetype detect(FILE* in, int n, Filetype type);
//
// Reads n bytes of in, and detects when the type changes to
// something else. If it does, then the file pointer is repositioned
// to the start of the change and the new type is returned. If the type
// does not change, then it repositions the file pointer n bytes ahead
// and returns the old type.
//
// For each type X there are the following 2 functions:
//
// void encode_X(FILE* in, FILE* out, int n, ...);
//
// encodes n bytes from in to out.
//
// int decode_X(Encoder& en);
//
// decodes one byte from en and returns it. decode() and decode_X()
// maintain state information using static variables.

// Detect EXE or JPEG data
Filetype detect(FILE* in, int n, Filetype type){
  U32 buf1=0, buf0=0; // last 8 bytes

  for (int i=0; i<n; ++i){
    int c=getc(in);
    if (c==EOF) return (Filetype)(-1);
    buf1=buf1<<8|buf0>>24;
    buf0=buf0<<8|c;
  }
  return type;
}

// Default encoding as self
void encode_default(FILE* in, FILE* out, int len){
  while (len--) putc(getc(in), out);
}

int decode_default(Encoder& en){
  return en.decompress();
}

// Split n bytes into blocks by type. For each block, output
// <type> <size> and call encode_X to convert to type X.
void encode(FILE* in, FILE* out, int n){
  Filetype type=DEFAULT;
  long begin=ftell(in);
  while (n>0){
    Filetype nextType=detect(in, n, type);
    long end=ftell(in);
    fseek(in, begin, SEEK_SET);
    int len=int(end-begin);
    if (len>0){
      fprintf(out, "%c%c%c%c%c", type, len>>24, len>>16, len>>8, len);
      switch(type){
case JPEG: encode_default(in, out, len); break;
case EXE: encode_default(in, out, len); break;
default: encode_default(in, out, len); break;
      }
    }
    n-=len;
    type=nextType;
    begin=end;
  }
}

// Decode <type> <len> <data>...
int decode(Encoder& en){
  static Filetype type=DEFAULT;
  static int len=0;
  while (len==0){
    type=(Filetype)en.decompress();
    len=en.decompress()<<24;
    len|=en.decompress()<<16;
    len|=en.decompress()<<8;
    len|=en.decompress();
    if (len<0) len=1;
  }
  --len;
  switch (type){
case JPEG: return decode_default(en);
case EXE: return decode_default(en);
default: return decode_default(en);
  }
}

//////////////////// Compress, Decompress ////////////////////////////

// Print progress: n is the number of bytes compressed or decompressed
void printStatus(int n){
#ifdef WINDOWS
  SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
  if (n>0 && !(n&0x07ff))
    printf("%12d\b\b\b\b\b\b\b\b\b\b\b\b", n), fflush(stdout);
#ifdef WINDOWS
  SetConsoleTextAttribute(d,wOldColorAttrs);
#endif

}

// Compress a file
void compress(const char* filename, long filesize, Encoder& en){
  assert(en.getMode()==COMPRESS);
  assert(filename && filename[0]);
  FILE *f=fopen(filename, "rb");
  if (!f) perror(filename), quit();
  long start=en.size();
  printf("%s %ld -> ", filename, filesize);

  // Transform and test in blocks
  const int BLOCK=536870912; // Uses 512MB blocks
  for (int i=0; filesize>0; i+=BLOCK){
    int size=BLOCK;
    if (size>filesize) size=filesize;
    long savepos=ftell(f);
    // Test transform
   // fseek(f, savepos, SEEK_SET);
    long j;
    int c1=0, c2=0;
 //   for (j=0; j<size; ++j)
  //    if ((c1=decode(en))!=(c2=getc(f))) break;

    // Test fails, compress without transform
    if (1){
  //    printf("Transform fails at %ld, input=%d decoded=%d, skipping...\n", i+j, c2, c1);
      en.compress(0);
      en.compress(size>>24);
      en.compress(size>>16);
      en.compress(size>>8);
      en.compress(size);
      fseek(f, savepos, SEEK_SET);
      for (int j=0; j<size; ++j){
        printStatus(i+j);
        en.compress(getc(f));
      }
    }
    filesize-=size;
  }
  if (f) fclose(f);
#ifdef WINDOWS
  SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
  printf("%-12ld\n", en.size()-start);
#ifdef WINDOWS
  SetConsoleTextAttribute(d,wOldColorAttrs);
#endif
}

// Try to make a directory, return true if successful
bool makedir(const char* dir){
#ifdef WINDOWS
  return CreateDirectory(dir, 0)==TRUE;
#else
#ifdef UNIX
  return mkdir(dir, 0777)==0;
#else
  return false;
#endif
#endif
}

// Decompress a file
void decompress(const char* filename, long filesize, Encoder& en){
  assert(en.getMode()==DECOMPRESS);
  assert(filename && filename[0]);

  // Test if output file exists. If so, then compare.
  FILE* f=fopen(filename, "rb");
  if (f){
    printf("Comparing %s %ld -> ", filename, filesize);
    bool found=false; // mismatch?
    for (int i=0; i<filesize; ++i){
      printStatus(i);
      int c1=found?EOF:getc(f);
      int c2=decode(en);
      if (c1!=c2 && !found){
        printf("differ at %d: file=%d archive=%d\n", i, c1, c2);
        found=true;
      }
    }
    if (!found && getc(f)!=EOF){
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
      printf("file is longer\n");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif
    }
    else if (!found){
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
      printf("identical   \n");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif

    }
    fclose(f);
  }

  // Create file
  else{
    f=fopen(filename, "wb");
    if (!f){ // Try creating directories in path and try again
      String path(filename);
      for (int i=0; path[i]; ++i){
        if (path[i]=='/' || path[i]=='\\'){
          char savechar=path[i];
          path[i]=0;
          if (makedir(path.c_str()))
            printf("Created directory %s\n", path.c_str());
          path[i]=savechar;
        }
      }
      f=fopen(filename, "wb");
    }

    // Decompress
    if (f){
      printf("Extracting %s %ld -> ", filename, filesize);
      for (int i=0; i<filesize; ++i){
        printStatus(i);
        putc(decode(en), f);
      }
      fclose(f);
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
      printf("done \n");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif
    }

    // Can't create, discard data
    else{
      perror(filename);
      printf("Skipping %s %ld -> ", filename, filesize);
      for (int i=0; i<filesize; ++i){
        printStatus(i);
        decode(en);
      }
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
      printf("not extracted\n");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif
    }
  }
}

//////////////////////////// User Interface ////////////////////////////

// Read one line, return NULL at EOF or ^Z. f may be opened ascii or binary.
// Trailing \r\n is dropped. Line length is unlimited.

const char* getline(FILE *f=stdin){
  static String s;
  int len=0, c;
  while ((c=getc(f))!=EOF && c!=26 && c!='\n'){
    if (len>=s.size()) s.resize(len*2+1);
    if (c!='\r') s[len++]=c;
  }
  if (len>=s.size()) s.resize(len+1);
  s[len]=0;
  if (c==EOF || c==26)
    return 0;
  else
    return s.c_str();
}

// int expand(String& archive, String& s, const char* fname, int base) {
// Given file name fname, print its length and base name (beginning
// at fname+base) to archive in format "%ld\t%s\r\n" and append the
// full name (including path) to String s in format "%s\n". If fname
// is a directory then substitute all of its regular files and recursively
// expand any subdirectories. Base initially points to the first
// character after the last / in fname, but in subdirectories includes
// the path from the topmost directory. Return the number of files
// whose names are appended to s and archive.

// Same as expand() except fname is an ordinary file
int putsize(String& archive, String& s, const char* fname, int base){
  int result=0;
  FILE *f=fopen(fname, "rb");
  if (f){
    fseek(f, 0, SEEK_END);
    long len=ftell(f);
    if (len>=0){
      static char blk[24];
      sprintf(blk, "%ld\t", len);
      archive+=blk;
      archive+=(fname+base);
      archive+="\r\n";
      s+=fname;
      s+="\n";
      ++result;
    }
    fclose(f);
  }
  return result;
}

#ifdef WINDOWS

int expand(String& archive, String& s, const char* fname, int base){
  int result=0;
  DWORD attr=GetFileAttributes(fname);
  if ((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY)){
    WIN32_FIND_DATA ffd;
    String fdir(fname);
    fdir+="/*";
    HANDLE h=FindFirstFile(fdir.c_str(), &ffd);
    while (h!=INVALID_HANDLE_VALUE){
      if (!equals(ffd.cFileName, ".") && !equals(ffd.cFileName, "..")){
        String d(fname);
        d+="/";
        d+=ffd.cFileName;
        result+=expand(archive, s, d.c_str(), base);
      }
      if (FindNextFile(h, &ffd)!=TRUE) break;
    }
    FindClose(h);
  }
  else // ordinary file
    result=putsize(archive, s, fname, base);
  return result;
}

#else
#ifdef UNIX

int expand(String& archive, String& s, const char* fname, int base){
  int result=0;
  struct stat sb;
  if (stat(fname, &sb)<0) return 0;

  // If a regular file and readable, get file size
  if (sb.st_mode & S_IFREG && sb.st_mode & 0400)
    result+=putsize(archive, s, fname, base);

  // If a directory with read and execute permission, traverse it
  else if (sb.st_mode & S_IFDIR && sb.st_mode & 0400 && sb.st_mode & 0100){
    DIR *dirp=opendir(fname);
    if (!dirp){
      perror("opendir");
      return result;
    }
    dirent *dp;
    while(errno=0, (dp=readdir(dirp))!=0){
      if (!equals(dp->d_name, ".") && !equals(dp->d_name, "..")){
        String d(fname);
        d+="/";
        d+=dp->d_name;
        result+=expand(archive, s, d.c_str(), base);
      }
    }
    if (errno) perror("readdir");
    closedir(dirp);
  }
  else printf("%s is not a readable file or directory\n", fname);
  return result;
}

#else// Not WINDOWS or UNIX, ignore directories

int expand(String& archive, String& s, const char* fname, int base){
  return putsize(archive, s, fname, base);
}

#endif
#endif


// To compress to file1.paq8k3: paq8k3 [-n] file1 [file2...]
// To decompress: paq8k3 file1.paq8k3 [output_dir]
int main(int argc, char** argv){
#ifdef WINDOWS// Get console color settings to restore on exit.
  d = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbInfo;
  GetConsoleScreenBufferInfo(d,&csbInfo);
  wOldColorAttrs=csbInfo.wAttributes;
#endif

  bool pause=argc<=2; // Pause when done?
  try{

    // Get option
    bool doExtract=false; // -d option
    if (argc>1 && argv[1][0]=='-' && argv[1][1]){
      if (argv[1][1]>='0' && argv[1][1]<='9'){
        level=argv[1][1]-'0';
        if (argv[1][2]){
          if (argv[1][2]=='u') uiq=true;
          else
            quit("Valid options are -0 through -9 or -d\n");
        }
      }
      else if (argv[1][1]=='d')
        doExtract=true;
      else
        quit("Valid options are -0 through -9 or -d\n");
      --argc;
      ++argv;
      pause=false;
    }

    // Print help message
    if (argc<2){
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_INTENSITY);
#endif
      printf(PROGNAME " archiver (C) 2009 Bill Pettis /");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif    
      printf(" paq8f archiver (C) 2006 Matt Mahoney.\n"
      "Free under GPL,");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_INTENSITY);
#endif
      printf(" http://www.gnu.org/licenses/gpl.txt\n"
      "To compress to file1.paq8k3:");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif    
      printf(" paq8k3 [-n] file1 [file2...]\n"
      "To decompress:");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,FOREGROUND_INTENSITY);
#endif
      printf(" paq8k3 file1.paq8k3 [output_dir]\n");
#ifdef WINDOWS
      SetConsoleTextAttribute(d,wOldColorAttrs);
#endif
      quit();
    }

    FILE* archive=0; // compressed file
    int files=0; // number of files to compress/decompress
    Array<char*> fname(1); // file names (resized to files)
    Array<long> fsize(1); // file lengths (resized to files)

    // Compress or decompress? Get archive name
    Mode mode=COMPRESS;
    String archiveName(argv[1]);
    {
      const int prognamesize=strlen(PROGNAME);
      const int arg1size=strlen(argv[1]);
      if (arg1size>prognamesize+1 && argv[1][arg1size-prognamesize-1]=='.'
        && equals(PROGNAME, argv[1]+arg1size-prognamesize)){
          mode=DECOMPRESS;
      }
      else if (doExtract)
        mode=DECOMPRESS;
      else{
        archiveName+=".";
        archiveName+=PROGNAME;
      }
    }

    // Compress: write archive header, get file names and sizes
    String filenames;
    if (mode==COMPRESS){

      // Expand filenames to read later. Write their base names and sizes
      // to archive.
      String header_string;
      for (int i=1; i<argc; ++i){
        String name(argv[i]);
        int len=name.size()-1;
        for (int j=0; j<=len; ++j) // change \ to /
          if (name[j]=='\\') name[j]='/';
        while (len>0 && name[len-1]=='/') // remove trailing /
          name[--len]=0;
        int base=len-1;
        while (base>=0 && name[base]!='/') --base; // find last /
        ++base;
        if (base==0 && len>=2 && name[1]==':') base=2; // chop "C:"
        int expanded=expand(header_string, filenames, name.c_str(), base);
        if (!expanded && (i>1||argc==2))
          printf("%s: not found, skipping...\n", name.c_str());
        files+=expanded;
      }

      // If archive doesn't exist and there is at least one file to compress
      // then create the archive header.
      if (files<1) quit("Nothing to compress\n");
      // archive=fopen(archiveName.c_str(), "rb");
      // if (archive)
      // printf("%s already exists\n", archiveName.c_str()), quit();
      archive=fopen(archiveName.c_str(), "wb+");
      if (!archive) perror(archiveName.c_str()), quit();
      fprintf(archive, PROGNAME " -%d%c\r\n%s\x1A",
        level, uiq?'u':' ', header_string.c_str());
      printf("Creating archive %s with %d file(s)...\n",
        archiveName.c_str(), files);

      // Fill fname[files], fsize[files] with input filenames and sizes
      fname.resize(files);
      fsize.resize(files);
      char *p=&filenames[0];
      rewind(archive);
      getline(archive);
      for (int i=0; i<files; ++i){
        const char *num=getline(archive);
        assert(num);
        fsize[i]=atol(num);
        assert(fsize[i]>=0);
        fname[i]=p;
        while (*p!='\n') ++p;
        assert(p-filenames.c_str()<filenames.size());
        *p++=0;
      }
      fseek(archive, 0, SEEK_END);
    }

    // Decompress: open archive for reading and store file names and sizes
    if (mode==DECOMPRESS){
      archive=fopen(archiveName.c_str(), "rb+");
      if (!archive) perror(archiveName.c_str()), quit();

      // Check for proper format and get option
      const char* header=getline(archive);
      if (strncmp(header, PROGNAME " -", strlen(PROGNAME)+2))
        printf("%s: not a %s file\n", archiveName.c_str(), PROGNAME), quit();
      level=header[strlen(PROGNAME)+2]-'0';
      if(header[strlen(PROGNAME)+3]=='u') uiq=true;
      if (level<0||level>9) level=DEFAULT_OPTION;

      // Fill fname[files], fsize[files] with output file names and sizes
      while (getline(archive)) ++files; // count files
      printf("Extracting %d file(s) from %s -%d\n", files,
        archiveName.c_str(), level);
      long header_size=ftell(archive);
      filenames.resize(header_size+4); // copy of header
      rewind(archive);
      fread(&filenames[0], 1, header_size, archive);
      fname.resize(files);
      fsize.resize(files);
      char* p=&filenames[0];
      while (*p && *p!='\r') ++p; // skip first line
      ++p;
      for (int i=0; i<files; ++i){
        fsize[i]=atol(p+1);
        while (*p && *p!='\t') ++p;
        fname[i]=p+1;
        while (*p && *p!='\r') ++p;
        if (!*p) printf("%s: header corrupted at %d\n", archiveName.c_str(),
          p-&filenames[0]), quit();
        assert(p-&filenames[0]<header_size);
        *p++=0;
      }
    }

    // Set globals according to option
    assert(level>=0 && level<=9);
    if(level<9) buf.setsize(16777216); // 16 MB
    else buf.setsize(268435456); // 256 MB

    // Compress or decompress files
    assert(fname.size()==files);
    assert(fsize.size()==files);
    long total_size=0; // sum of file sizes
    for (int i=0; i<files; ++i) total_size+=fsize[i];
    Encoder en(mode, archive);
    if (mode==COMPRESS){
      for (int i=0; i<files; ++i)
        compress(fname[i], fsize[i], en);
      en.flush();
      printf("%ld -> %ld\n", total_size, en.size());
    }

    // Decompress files to dir2: paq8k3 -d dir1/archive.paq8k3 dir2
    // If there is no dir2, then extract to dir1
    // If there is no dir1, then extract to .
    else{
      assert(argc>=2);
      String dir(argc>2?argv[2]:argv[1]);
      if (argc==2){ // chop "/archive.paq8k3"
        int i;
        for (i=dir.size()-2; i>=0; --i){
          if (dir[i]=='/' || dir[i]=='\\'){
            dir[i]=0;
            break;
          }
          if (i==1 && dir[i]==':'){ // leave "C:"
            dir[i+1]=0;
            break;
          }
        }
        if (i==-1) dir="."; // "/" not found
      }
      dir=dir.c_str();
      if (dir[0] && (dir.size()!=3 || dir[1]!=':')) dir+="/";
      for (int i=0; i<files; ++i){
        String out(dir.c_str());
        out+=fname[i];
        decompress(out.c_str(), fsize[i], en);
      }
    }
    fclose(archive);
    programChecker.print();
  }
  catch(const char* s){
    if (s) printf("%s\n", s);
  }
  if (pause){
    printf("\nClose this window or press ENTER to continue...\n");
    getchar();
  }
  return 0;
}


