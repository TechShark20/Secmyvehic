
//This code is devoloped for establishing connections with raspberry pi pico under the face recognition instances the following code has certain change 
//the code is using espress if deeplearning libraries ->"fd_forward.h","fr_forward.h", "fr_flash.h"
// devoloped on arduino express board manager 1.0.4 #####
// the entire model is mtnn trained 
//....code ia in progress 



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

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7


static mtmn_config_t mtmn_config = {0};
static int8_t detection_enabled = 0;
static int8_t recognition_enabled = 0;
static face_id_list Faceid={0};
static int8_t enrollperson=0;// 1. true :: for enrollment 0. false :: then automaticlally will go to recognise
 
#define NO_FACES 7
#define NO_enrollment 5
const int PushButton =12;
const int Lednorm=2;
const int LedFace=14;
//const int Trigger =12;



    
  
    static int run_facerecognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes,face_id_list &id_list);


    void setup()
    {
 Serial.begin(115200);// Serial helps us to read the prgress otherwise what is  happening we are unable to see
 pinMode(Lednorm, OUTPUT);
 pinMode(LedFace,OUTPUT);
 pinMode(PushButton, INPUT);
 //pinMode(Trigger,OUTPUT);
 int enrollstatus=0;
 
  
 
 
 
 digitalWrite(Lednorm,HIGH);// normal mode i.e. recognition
 digitalWrite(LedFace,LOW);//
// digitalWrite(Trigger,LOW);//
 
 
// camera configuration NOTE:: IT is based on ESP 32 cam only  while using other model carefully try it
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  //......... model configuration.............................................................
    mtmn_config.type = NORMAL;
    mtmn_config.min_face = 50;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.5;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.5;
    mtmn_config.r_threshold.nms = 0.5;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.5;
    mtmn_config.o_threshold.nms = 0.5;
    mtmn_config.o_threshold.candidate_number = 1;
     
  //...........................................................................................   
     // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
    
  }
  
   
   
   
  delay(2000);// 2 sec delay for detecting wether the person has to be enrolled or not 
  enrollstatus =digitalRead(PushButton);
  if(enrollstatus ==HIGH) {enrollperson=1;Serial.printf("enrollment mode"); digitalWrite(Lednorm,LOW);}
   else {enrollperson=0; Serial.printf("recognition");}
  
  Serial.println("face id intialised ::");
  face_id_init(&Faceid,NO_FACES , NO_enrollment);
  
 int y=read_face_id_from_flash(&Faceid);
 if(y==-2)
 {
   Serial.printf("No  prior enrollments than initilaising ");
  }
 Serial.printf(" the y for new id is %d ",y);
      Serial.println("Camera Started");
       sensor_t * s = esp_camera_sensor_get();
        s->set_framesize(s, FRAMESIZE_QVGA);
      
    }

 
  
  
  

void loop()
{

  if(enrollperson==0) digitalWrite(Lednorm,HIGH);
  dl_matrix3d_t *face_id;                       // Face ID pointer
face_id_node *face_recognized;   


  
  int64_t fr_start = esp_timer_get_time();
  bool detected =false;
  camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    while(!detected){
       camera_fb_t * fb = NULL;
      
    fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        }
        //Serial.printf("camera captured image ");
   dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
   if(!image_matrix) {Serial.println("image matrix allocation failed"); return ;} 
    if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)){
                        Serial.println("fmt2rgb888 failed");
                        res = ESP_FAIL;
                    }

  box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
       if (net_boxes){

        
        detected = true;
        Serial.println("face boxes allocated");
      int  perface_id = run_facerecognition(image_matrix, net_boxes,Faceid);
          if(perface_id==-1) {Serial.printf("Unauthorised Access ");
          //digitalWrite(Trigger,HIGH);
          
                   digitalWrite(Lednorm,HIGH);
                   for(int i =0;i<4;i++)
                   {
                    digitalWrite(Lednorm,HIGH);
                    digitalWrite(LedFace,HIGH);
                    delay(100);
                    digitalWrite(Lednorm,LOW);
                    digitalWrite(LedFace,LOW);
                    delay(100);
                    
                    
                    }
                   
          
          }
          else  if (perface_id>=0&&enrollperson==0){Serial.printf("Authorised  Access ");esp_deep_sleep_start();}
           }
          dl_matrix3du_free(image_matrix);
          delay(2000);
           esp_camera_fb_return(fb);
          // Serial.println("detction round x::");
    }
        }

        
        
         

         

    
static int run_facerecognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes,face_id_list& id_list){
    dl_matrix3du_t *aligned_face = NULL;
    int matched_id = 0;

    aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
    if(!aligned_face){
        Serial.println("Could not allocate face recognition buffer");
        return matched_id;
    }
    if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK){
        if (enrollperson == 1){
          digitalWrite(LedFace,HIGH);
            int x= enroll_face_id_to_flash( &id_list, aligned_face);
                Serial.printf("left sample ::%d\n",x);

            if(x == (NO_enrollment - 1)){
                Serial.printf("Enrolling Face ID: %d\n", id_list.tail);
            
            }
            Serial.printf("Enrolling Face ID: %d sample %d\n", id_list.tail, ENROLL_CONFIRM_TIMES - x);
            
               
                     if (x == 0){
                enrollperson = 0;
                Serial.printf("Enrolled Face ID: %d\n", id_list);
                   int x= enroll_face_id_to_flash( &id_list, aligned_face);
                   Serial.printf("left sample ::%d\n",x);
                   
            }
        } else {
            matched_id = recognize_face(&id_list, aligned_face);
            if (matched_id >= 0) {
                Serial.printf("Match Face ID: %u\n", matched_id);
               
            } else {
                Serial.println("No Match Found");
                
                matched_id = -1;
            }
        }
        delay(500);
        digitalWrite(LedFace,LOW);
    } else {
        Serial.println("Face Not Aligned");
        
    }

    dl_matrix3du_free(aligned_face);
    return matched_id;
}
