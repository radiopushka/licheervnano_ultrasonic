#include "SyncedPSKASK_modulationLibrary/modulator.h"
#include "alsa/alsa.h"
#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pwmplus.c"
#include "gpio/gpio.h"
#include "PINS.h"


//Evan Nikitin Wed Oct  2 09:22:05 PM -00 2024

char unlockcode[7];//maximum 6 digits;

 

void lock_d(){
  set_pin_value(LOCKED_PIN,1);
  set_pin_value(UNLOCKED_PIN,0);
}
void unlock_d(){
  //printf("unlocked\n");
  set_pin_value(LOCKED_PIN,0);
  set_pin_value(UNLOCKED_PIN,1);
}



int calculate_message_chrsum(unsigned char* message,int size){
  int i;
  int csum=0;
  for(i=0;i<size;i++){
    csum=csum+message[i];
  }
  return csum;
}

int loop[10];
int checkchar_loop(int input){

}
 //PID gain control
  double gaincont = 1;

void receive_signal(short* frame, int size, int framegain){
    aread(frame,size);
    float error,mval;
    demod_carrier(frame,size);
    de_sample(frame,size,8);
    prepare_array(frame,size,gaincont);
    //printf("\n gain; %g\n",gaincont);
    
    mval=getmaxval(frame,size);
    error=(framegain>>1)-mval;
    //printf("%d\n",mval);

  /*
  //PID controller
    //500 hz bandwidth
    gaincont=0.00059*(error) + 0*(error-previous_error) + 0.00025*(error_over);
    //300 hz band width
    //gaincont=0.001*(error) + 0*(error-previous_error) + 0.001*(error_over);
    */
  //schmidt controller
  //courtesy of Sergey Nikitin
  gaincont=gaincont+sin(error/32768.0)*10;
  //debug
  //printf("gain: %g, value: %d\n",gaincont,mval);
  //bounds
  if(gaincont>20){
    gaincont=20;
  }
  if(gaincont<-20){
    gaincont=-20;
  }



}

  //checksum and data validation
  int bsize=-1;
  int chrsrx=-1;
  int rxcount=0;
void process_message(unsigned char* tbuff,int output){
        int outputcpy=output;
        int position;
        if(output!=-1){
          position=(output>>16);
          output=output&65535;
          if(outputcpy%273==0){
              outputcpy=outputcpy/273;
              if(outputcpy!=0){
                if(outputcpy<=(255*bsize)){
                  chrsrx=outputcpy;
                }
              }
            
          }else if(output%257==0){
            /*}else if(output>256){
              if(output%257!=0){
                int nonp=output/257;
                double decc=output;
                double dval=decc/257.0;
                double middle=nonp+0.5;
                if(dval>middle){
                  output=nonp+1;
                }else{
                  output=nonp;
                }
                


                
    
              }else{
                 output=output/257;
              }*/
            output=output/257;
            if(output!=0){
              if(position-1<=bsize&&position>1){

                if(strlen(tbuff)==bsize){
                    int bef=calculate_message_chrsum(tbuff,bsize);
                    unsigned char prev=tbuff[position-2];
                    tbuff[position-2]=output;
                    int cur=calculate_message_chrsum(tbuff,bsize);
                    if(bef==chrsrx&&cur!=chrsrx)
                      tbuff[position-2]=prev;
                }else{
                  if(rxcount<bsize && tbuff[position-2] ==0 )
                  rxcount++;
                  tbuff[position-2]=output;
                }

                tbuff[bsize]=0;
                int chrsm=calculate_message_chrsum(tbuff,bsize);
                if(strlen(tbuff)==bsize && chrsm==chrsrx){

                 if(strcmp(tbuff,unlockcode)==0){
                      unlock_d();
                  }else{
                      lock_d();
                  }
                 //printf("\n%s\n",tbuff);
                 bzero(tbuff,sizeof(char)*29);
                 bsize=-1;
                 rxcount=0;
                  chrsrx=-1;
                 //msgrx=1;
                }else{
            //\r is backline
                 // printf("received: %d/%d %d vs %d         \r",rxcount,bsize,chrsm,chrsrx);
                 // fflush(stdout);
                }
              }else if(position==1){
                if(output<29)
                bsize=output;
              }
            }
      }
  }
  }

void print_example(char* prgrm){
  printf("%s [card number] [6 digit password]\n",prgrm);
}

void limit_print(char* targ,char* in,int size){
  int i;
  for(i=0;i<size;i++){
    targ[i]=in[i];
  }
  targ[i]=0;
}

int main(int argn, char* argv[]){

  if( enable_gpio(LOCKED_PIN)==-1){
    printf("failed to stat gpio%d\n",LOCKED_PIN);
  }

  if( enable_gpio(UNLOCKED_PIN)==-1){
    printf("failed to stat gpio%d\n",UNLOCKED_PIN);
  }
  set_pin_direction(LOCKED_PIN,"out");
  set_pin_direction(UNLOCKED_PIN,"out");
  lock_d();
  printf("gpio ready\n");
  printf("make sure to run \"amixer -Dhw:0 cset name='ADC Capture Volume' 24\" before hand or else the program will segfault\n");

  if(argn < 2){
    printf("please specify the interface or card number\n");
    print_example(argv[0]);
    return 0;
  }

  if(argn < 3){
    printf("please specify 6 digit password\n");
    print_example(argv[0]);
    return 0;
  }

  limit_print(unlockcode,argv[2],6);
  printf("password: %s\n",unlockcode);

  
  init_modulation_scheme(48000,21,500,6,1);
  create_receiver(48000,24000);

  char tbuff[29];
  bzero(tbuff,sizeof(char)*29);

  int size = calculate_frame_size(3,3);
  printf("initialized\n");
  short frame[size];
  short frame2[size];
  bzero(frame,sizeof(short)*size);
  unsigned int itterator = 0;
  //create_header(frame, &itterator);
  //
  //
  if(setup_alsa(argv[1],NULL,size,48000)<0)
    return 0;

  printf("alsa ready\n");
  printf("\n");


  int msgrx=0;

  int framegain=10000;
  int sqg=framegain/2;


  while(msgrx==0){
    receive_signal(frame,size,framegain);

    while(itterator<=size){
      if(wait_for_sync(frame,&itterator,size,sqg)!=-1){


        if(size-itterator<get_packet_size_buffer()){
        
  
          int freesize=(size-itterator);//size left uncovered
          int treaded_size=itterator;//size covered


          memcpy(frame2,frame+itterator,sizeof(short)*freesize);
          memcpy(frame,frame2,sizeof(short)*freesize);
          receive_signal(frame2,treaded_size,framegain);
          memcpy(frame+freesize,frame2,sizeof(short)*treaded_size);

          //printf("%d\n",size-itterator);
          itterator=0;
        }
        //printf("%d\n",size-itterator);
        //demod2(frame,size,sqg);
        
        //itterator=size;
          int output=(int)demod(frame,&itterator,size,0);

          if(output>=0)
          process_message((unsigned char*)tbuff,output);
          
        }
      }

    
    itterator=0;
  }
    //printf("%g %d %d %d \n",cgain, success,success_past,mval);
  
  free_alsa();
  /*
  //create_packet(frame,69,&itterator);
  itterator = 0;
  printf("packets generated\n");
  reset_scheme();
  prepare_array(frame,size,6);
    
  //while(wait_for_sync(frame,&itterator,size,100)==-1);
  int pcount=1;
  for(i=0;i<1000;i++){
    while(wait_for_sync(frame,&itterator,size,400)==-1);
    int output=(int)demod(frame,&itterator,size,400);
    if(output!=-1){
     printf("%d %d\n",output,pcount);
     pcount++;
    }
  }
  free_mod_mem();
  printf("array spots: %d\n",size);
  dump_to_file(frame,size,10);
*/
  return 0;
  
}
