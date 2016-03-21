/* core tpie configurations and initializations */
#include <tpie/tpie.h>
#include <tpie/file_stream.h>
#include <string>
#include <ctime>

#define NUM_SEGMENTS 50000
#define PERMUTE 1

using std::cerr;
using std::endl;

/* a basic struct storing a 3D segment */
struct segment_t {
  double x1, y1, x2, y2;
  float z1, z2;
};

struct segmentstr_t {
  char str1[1000000];
};
struct segmentstr_t make_segment(int x){
  struct segmentstr_t s;
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
  printf("%s", item.str1);
}


/* write n segment items to tpie::file_stream with given
 * output file name fname. Overwrites any previous file */
void writestream(const std::string& fname, size_t n){

  tpie::file_stream<struct segmentstr_t> out;
  out.open(fname, tpie::open::write_only);
  for(size_t i=0; i<n; i++){
    struct segmentstr_t item = make_segment(i);
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
    //print_item(item); printf("\n");
  }
}

/* copy infile name to outfile name. Assumes infile
 * is a valid tpie::file_stream and overwrites outfile
 */
void copystream(const std::string & infile,
    const std::string & outfile, int *arr) {
  tpie::file_stream<struct segmentstr_t> in;
  tpie::file_stream<struct segmentstr_t> out;

  in.open(infile);
  out.open(outfile, tpie::open::write_only);
  for(int i=0; i < NUM_SEGMENTS; ++i) {
  //while (in.can_read()) {
      in.seek(arr[i]);
      segmentstr_t item = in.read();
      out.write(item);
      //cerr<<"Size of buffer used = "<<tpie::get_memory_manager().used()<<endl;
    //cerr<<"Size of buffer available = "<<tpie::get_memory_manager().available()<<endl;
  }
}

void print_array(int *arr)
{
  cerr<<endl;
  for(int i=0;i<NUM_SEGMENTS;i++)
  {
    cerr<<arr[i]<<" ";
  }
}

void get_permuted_array(int *arr)
{
  int count = 2*NUM_SEGMENTS*log2(NUM_SEGMENTS);
  int index1, temp = 0;
  int index2;

  //cerr<<count;

  for(int i=0;i<count;i++)
  {
    index1 = rand() % NUM_SEGMENTS;
    index2 = rand() % NUM_SEGMENTS;

    temp = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = temp;
  }
}


/* core subsystems needed for using file_streams */
#define SUBSYS tpie::MEMORY_MANAGER | tpie::STREAMS

int main(int /*argc*/, char ** /*argv*/) {
  size_t memory_limit_mb = 1000;
  int permute_array[NUM_SEGMENTS];

  /* start up TPIE subsystems, set memory limit */
  tpie::tpie_init(SUBSYS);
  tpie::get_memory_manager().set_limit(memory_limit_mb*1024*1024);

  std::string a = "test.in";
  std::string b = "test.out";
  //cerr << "Writing " << a << endl;
  writestream(a, NUM_SEGMENTS);
  //cerr<<"Size of buffer available = "<<tpie::get_memory_manager().available()<<endl;
  //cerr<<"Size of buffer used = "<<tpie::get_memory_manager().used()<<endl;
  //cerr << "Copying  " << a << " to " << b << endl;
  //
  //

  for(int i=0;i<NUM_SEGMENTS;i++)
  {
    permute_array[i] = i;
  }

  print_array(permute_array);
  #if PERMUTE==1
  get_permuted_array(permute_array);
  #endif
  print_array(permute_array);


  clock_t begin = clock();
  copystream(a, b, permute_array);
  clock_t end = clock();
  //cerr << "Reading  " << b << endl;
  readstream(b);

  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cerr << elapsed_secs << endl;
  /* wrap up */
  tpie::tpie_finish(SUBSYS);
  return EXIT_SUCCESS;
}
