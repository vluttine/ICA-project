#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <itpp/itbase.h>
#include <itpp/signal/fastica.h>
#include <math.h>
#include <sys/time.h>
#include <fileread.h>
#include <vector>
#include <ctime>

using namespace std;
using namespace itpp;

void writeWav(float * buffer, int bufferSize, const char* filename) {
	SF_INFO info;
	info.channels = 1;
	info.samplerate = 8000;
	info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	
	SNDFILE * outfile = sf_open(filename,SFM_WRITE, &info);
	sf_write_float(outfile, &buffer[0], bufferSize);
	sf_write_sync(outfile);
	sf_close(outfile);
}

pair<int,float*> readWav(const char* filename, int i)
	{
	SNDFILE *sf;
    SF_INFO info;
    int num, num_items;
    float *buf;
    int f,sr,c;    
    
    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open(filename,SFM_READ,&info);
    if (sf == NULL)
        {
        cout << "Failed to open the file." << endl;
        exit(-1);
        }
    /* Print some of the info, and figure out how much data to read. */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
	/*cout << "Details of wav #" << i+1 << ":" << endl;
    cout << "Frames = " << f << endl;
    cout << "Sample rate = " << sr << endl;
    cout << "Channels = " << c << endl;*/
    num_items = f*c;
	
    /* Allocate space for the data to be read, then read it. */
    buf = (float *) malloc(num_items*sizeof(float));
    num = sf_read_float(sf,buf,num_items);
    sf_close(sf);
   //cout << "Read " << num << " items\n" << endl;
	
	return make_pair(num,buf);
	}

int main()
    {
	cout << "\n============== Program starts =============="<< endl;
	clock_t start,end;
	time_t result = time(NULL);
	start = clock();
	int i, k;
	int number_of_inputs = 4;
	char buffer[30];
	ofstream myfile;
	sprintf(buffer,"results/fasticatimes_%d.txt",result);
	myfile.open(buffer);
	
	FileReader fr;
	vector <string> inputs = fr.getFileList();
	
	const char* input_filename_array[inputs.size()];
	
	for (k=0;k<inputs.size();k++){
		input_filename_array[k]=inputs[k].c_str();
	}
		
	pair<int,float*> wav[number_of_inputs];
	for(int y = 0;y<inputs.size()/number_of_inputs;y++){
		string s = inputs[y*number_of_inputs];
		string id = s.substr(6,number_of_inputs);
		for (i = 0; i < number_of_inputs; i++){
			wav[i] = readWav(input_filename_array[y*number_of_inputs+i],i);
			}
		int size=wav[0].first;
		float* samples_wav[number_of_inputs];
		for (i = 0; i < number_of_inputs; i++){
			samples_wav[i] = wav[i].second;
			}
		mat input_mat;
		input_mat.set_size(number_of_inputs,size,false);
			
		for (k = 0; k < number_of_inputs; k++){
		 for (i = 0; i < size; i++)
			{
				input_mat.set(k,i,samples_wav[k][i]);
			}
		}
		
		//cout << "FastICA starts..." << endl;
		clock_t tick,tock;
		tick = clock();
		
		
		Fast_ICA fastica(input_mat);
		//fastica.set_nrof_independent_components(input_mat.s());
		//fastica.set_non_linearity(  FICA_NONLIN_TANH );
		fastica.set_fine_tune(false);
		fastica.set_approach( FICA_APPROACH_DEFL );
		fastica.separate();
		mat ICs = fastica.get_independent_components();
		
		tock = clock();
		//cout << "FastICA done in " << (tock-tick)/CLOCKS_PER_SEC << " seconds." << endl;
		//cout << "Writing output signals to wav files..." << endl;
		
		
		myfile << id << ": " << (tock-tick)/CLOCKS_PER_SEC << "\n";
		
		
		float * output_samples[number_of_inputs];
		for (i = 0; i < number_of_inputs; i++) {
			output_samples[i] = (float *) malloc(size*sizeof(float));
		}
		
		for (k = 0; k < number_of_inputs; k++) {
			for (i = 0; i < size; i++){
				output_samples[k][i] = ICs.get(k,i);
			}
		}
		
		
		
		
		for (i = 0; i < number_of_inputs; i++) {
			sprintf(buffer,"output/output_%s_%d.wav",id.c_str(),i+1);
			writeWav(output_samples[i],size,buffer);
			free(output_samples[i]);
		}
			
		//cout << "Writing done." << endl;
		cout << "Current progress: " << (y+1)*number_of_inputs << "/" << inputs.size() << endl;
	}
	end = clock();
	cout << "============== Program has executed! ==============" << endl;
	cout << "Program execution time: " << (end-start)/CLOCKS_PER_SEC << " seconds." << endl;	
	myfile << "Program execution time: " << (end-start)/CLOCKS_PER_SEC << " seconds." << endl;
	myfile.close();
    return 0;
    }