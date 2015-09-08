// Switches class
class Switches {
  public:
    Switches();

    //byte positions to ignore (default: none)
    unsigned int* ignore_list;
    int ignore_list_len;

    bool slow_mode;                //slow mode (default: off)
    bool fast_mode;                //fast mode (default: off)
    bool brute_mode;               //brute mode (default: off)
    bool pdf_bmp_mode;             //wrap BMP header around PDF images
                                   //  (default: off);
    bool debug_mode;               //debug mode (default: off)

    unsigned int min_ident_size;   //minimal identical bytes (default: 4)

    //(p)recompression types to use (default: all)
    bool use_pdf;
    bool use_zip;
    bool use_gzip;
    bool use_png;
    bool use_gif;
    bool use_jpg;

    bool level_switch;            //level switch used? (default: no)
    bool use_comp_level[9];       //compression levels to use (default: all)
    bool use_mem_level[9];        //memory levels to use (default: all)
};

//Switches constructor
Switches::Switches() {
  ignore_list = NULL;
  ignore_list_len = 0;
  slow_mode = false;
  fast_mode = false;
  brute_mode = false;
  pdf_bmp_mode = false;
  debug_mode = false;
  min_ident_size = 64;
  use_pdf = true;
  use_zip = true;
  use_gzip = true;
  use_png = true;
  use_gif = true;
  use_jpg = true;
  level_switch = false;
  for (int i = 0; i < 9; i++) {
    use_comp_level[i] = true;
    use_mem_level[i] = true;
  }
}
