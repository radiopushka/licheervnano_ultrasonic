#ifndef GPIO_LIB

//enable the gpio pin via the kernel number
 int enable_gpio(int gpio_number);

 //set the gpio value via the kernel number
 int set_pin_value(int gpio_number,int value);

 //set gpio pin direction
 int set_pin_direction(int gpio_number,char* direction);

 //get gpio read value (0,1), returns -1 upon error
 int get_pin_value(int gpio_number);



#endif // !GPIO_LIB
