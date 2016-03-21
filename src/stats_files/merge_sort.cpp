/* core tpie configurations and initializations */
#include <tpie/tpie.h>
#include <tpie/file_stream.h>
#include <string>
#include <ctime>

#define NUM_STREAMS 100

using std::cerr;
using std::endl;

/* a basic struct storing a 3D segment */
struct segment_t {
  double x1, y1, x2, y2;
  float z1, z2;
};

struct segmentstr_t {
  int num;
};
struct segmentstr_t make_segment(int x){
  struct segmentstr_t s;
  s.num = x;
  return s;
}

/* make a dummy segment with given x1,y1, but 
 * setting other coordinates to 0 */
struct segment_t make_segment(double x1, double y1){
  struct segment_t s;
  s.x1=x1; s.y1=y1;
  s.x2=s.y2=s.z1=s.z2=0.f;
  return s;
}

/* print x1, y1 coords of segment */
void print_item(const struct segment_t& item){
  printf("(%6.2f %6.2f)", item.x1, item.y1);
}
void print_item(const struct segmentstr_t& item){
  printf("%d", item.num);
}


/* write n segment items to tpie::file_stream with given
 * output file name fname. Overwrites any previous file */
void writestream(const std::string& fname, size_t n){
  int rand_num = 0;

  tpie::file_stream<struct segmentstr_t> out;

  out.open(fname, tpie::open::write_only);
  tpie::file_stream<struct segmentstr_t> out1;
  for(size_t i=0; i<n; i++){
    rand_num = rand() % n;
    struct segmentstr_t item = make_segment(rand_num);
    //print_item(item); printf("\n");
    out.write(item);
  }
}

/* Read all segments from a tpie::file_stream with
 * file name given by fname and prints results */
void readstream(const std::string& fname){

  tpie::file_stream<struct segmentstr_t> in;
  in.open(fname, tpie::open::read_only);
  printf("size of stream: %6ld\n\n", in.size());
  while (in.can_read()) {
    segmentstr_t item = in.read();
    print_item(item); 
    printf("\n");
  }
}

/* copy infile name to outfile name. Assumes infile
 * is a valid tpie::file_stream and overwrites outfile
 */
void copystream(const std::string & infile,
    const std::string & outfile) {
  tpie::file_stream<struct segmentstr_t> in[NUM_STREAMS];
  tpie::file_stream<struct segmentstr_t> out;
  
  out.open(outfile, tpie::open::write_only);
  while (in[0].can_read()) {
    for(int i=0;i<NUM_STREAMS;i++)
    {
      if(in[i].can_read())
      {
        segmentstr_t item = in[i].read();
        out.write(item);
      }
    }
  }
}

void sortstream(const std::string& fname){

  tpie::file_stream<struct segmentstr_t> in,out,temp;
  in.open(fname, tpie::open::read_only);
  out.open("out", tpie::open::write_only);
  int k=1,n = 8,m=1;
  int flag = 0;

  for(int i=(n/2);i>=1;i=i/2)
  {
    if(flag==0)
    {
      in.open(fname, tpie::open::read_only);
      out.open("out", tpie::open::write_only);
    }
    else
    {
      out.open(fname, tpie::open::write_only);
      in.open("out", tpie::open::read_only);
    }
    for(int j=1;j<=i;j++)
    {
      k=1;
      temp.open("temp", tpie::open::write_only);
      while(k<=m)
      {
        if(in.can_read()) 
        {
          segmentstr_t item = in.read();
          print_item(item); 
          temp.write(item);  
        }
        k++;
      }
      temp.open("temp", tpie::open::read_only);
      k=1;
      while(k<=m)
      {
        if(in.can_read() && temp.can_read()) 
        {
          segmentstr_t item = in.read();
          print_item(item); 
          segmentstr_t item1 = temp.read();
          print_item(item1); 
          // Extend the logic for more than 2 elements here
          if(item.num>item1.num)
          {
            out.write(item1);
            out.write(item);
          }
          else
          {
            out.write(item);
            out.write(item1);
          }
        }
        k++;
      }
    }
    m*=2;
    flag = flag - 1;
    break;

  }
}

    





/* core subsystems needed for using file_streams */
#define SUBSYS tpie::MEMORY_MANAGER | tpie::STREAMS

int main(int /*argc*/, char ** /*argv*/) {
  size_t memory_limit_mb = 1000;

  /* start up TPIE subsystems, set memory limit */
  tpie::tpie_init(SUBSYS);
  tpie::get_memory_manager().set_limit(memory_limit_mb*1024*1024);

  std::string a = "test.in";
  std::string b = "test.out";
  //cerr << "Writing " << a << endl;
  writestream(a, 8);
  //cerr<<"Size of buffer available = "<<tpie::get_memory_manager().available()<<endl;
  //cerr<<"Size of buffer used = "<<tpie::get_memory_manager().used()<<endl;
  //cerr << "Copying  " << a << " to " << b << endl;
  clock_t begin = clock();
  //copystream(a, b);
  sortstream(a);
  clock_t end = clock();
  //cerr << "Reading  " << b << endl;
  readstream(a);
  readstream("out");

  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  //cerr << elapsed_secs << endl;
  /* wrap up */
  tpie::tpie_finish(SUBSYS);
  return EXIT_SUCCESS;
}
