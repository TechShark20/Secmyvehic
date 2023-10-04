

// This code is devoloped for calculating time responses with face detection and changing  min_face parameter 
// only for testng purposes

#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "fd_forward.h"
//***********************************Threesold *********************************
#define Threesold         60    // change for threesold here 60 to 90
//****************************************DOne*************************
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

const int Lednorm=14;
static mtmn_config_t mtmn_config = {0};


void setup() {

Serial.begin(115200);
pinMode(Lednorm ,OUTPUT);
digitalWrite(Lednorm,LOW);

 
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
    mtmn_config.type = FAST;
    mtmn_config.min_face = Threesold;
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

    sensor_t * s = esp_camera_sensor_get();
        s->set_framesize(s, FRAMESIZE_QVGA);

 Serial.println("Camera Started");
}

void loop() {

 
  
  bool detected =false;
  
  
while(!detected){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  
  fb = esp_camera_fb_get();
  if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        }
        long int t1 = millis();
     dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
     if(!image_matrix) {Serial.println("image matrix allocation failed"); return ;} 
     if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)){
                        Serial.println("fmt2rgb888 failed");
                        res = ESP_FAIL;
                    }

   
    box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
       if (net_boxes){long int t2 = millis();
       Serial.println(t2-t1);
       Serial.print(",");
       
            digitalWrite(Lednorm,HIGH);
    delay(500);
    digitalWrite(Lednorm,LOW);
     detected =true;

           
       }
       dl_matrix3du_free(image_matrix);
     
    esp_camera_fb_return(fb);
      
       }
   
  
    
  
  // put your main code here, to run repeatedly:

}
