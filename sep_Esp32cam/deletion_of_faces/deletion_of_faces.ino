// a program to reset fully esp32 cam 

#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"
#include "nvs.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "fr_flash.h"//this library is used to save the recognised faces in the flash memory 
#include "esp_partition.h"//partition lbrary using face recognition
static face_id_list Faceid={0};
#define NO_FACES 7
#define  NO_enrollment 5



void setup() {
  Serial.println("face id intialised ::");
  face_id_init(&Faceid,NO_FACES , NO_enrollment);
  
 int y=read_face_id_from_flash(&Faceid);
  Serial.printf(" the y for new id is %d ",y);
      Serial.println("Camera Started");


 int i=y;
 while(i!=-1){
 i=delete_face_id_in_flash(&Faceid);
 Serial.printf("DEleting faces ");
 Serial.printf(" %d",i);
 } 
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
