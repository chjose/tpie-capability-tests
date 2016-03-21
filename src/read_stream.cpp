#include <tpie/file_stream.h>
#include <string>
using namespace tpie;
using namespace std;
struct segment_t {
    double x1, y1, x2, y2;
    float z1, z2;
};

//const tpie::size_type blocksize=12;
//const tpie::size_type first_element_offset=1024;
void copystream(const string & infile, const string & outfile) {
    cerr << "0";
    file_stream<segment_t> in;
    file_stream<segment_t> out;
    cerr << "1";

    //stream::header h = {stream::header::MAGIC_NUMBER,stream::header::VERSION_NUMBER,blocksize,0,first_element_offset};
    //native_stream<int> the_stream(filename, h, tpie::file::READ, tpie::file::FORWARD);
    in.open(infile);
    cerr << "1";
    out.open(outfile, tpie::access_write);
    cerr <<"2";
    while (in.can_read()) {
        segment_t item = in.read();
        out.write(item);
    }
}

int main(int argc, char ** argv) {
    string a = "test.in";
    string b = "2.txt";

    copystream(a, b);

    return EXIT_SUCCESS;
}
