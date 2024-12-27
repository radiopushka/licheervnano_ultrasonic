#include<stdio.h>
#include<string.h>

int enable_gpio(int gpio_number){

    FILE* f=fopen("/sys/class/gpio/export","wb");
    if(!f){
      return -1;
    }
    char pinstr[8];
    memset(pinstr,0,sizeof(char)*8);
    sprintf(pinstr,"%d",gpio_number);
    fwrite(pinstr,sizeof(char),8,f);
    fclose(f);
    return 1;
}

int set_pin_direction(int gpio_number,char* direction){

  char path[100];
  sprintf(path,"/sys/class/gpio/gpio%d/direction",gpio_number);
  FILE* f=fopen(path,"wb");
  if(!f){
    return -1;
  }
  fwrite(direction,sizeof(char),strlen(direction),f);
  fclose(f);
  return 1;

}

int set_pin_value(int gpio_number,int value){
  value=value&1;
  char path[100];
  sprintf(path,"/sys/class/gpio/gpio%d/value",gpio_number);
  FILE* f=fopen(path,"wb");
  if(!f){
    return -1;
  }
  char data[2];
  switch(value){
    case 1:
      data[0]='1';
      break;
    case 0:
      data[0]='0';
      break;
  }
  data[1]=0;
  fwrite(data,sizeof(char),2,f);
  fclose(f);
  return 1;
}

int get_pin_value(int gpio_number){
  char path[100];
  sprintf(path,"/sys/class/gpio/gpio%d/value",gpio_number);
  FILE* f=fopen(path,"rb");
  if(!f){
    return -1;
  }
  char data;
  if(fread(&data,sizeof(char),1,f)!=1){
    fclose(f);
    return -1;
  }
  fclose(f);
  return data;
}

