# Secmyvehic
A ESP32 cam based project to track  vehicles when they are being driven by anoynmous driver 

## Introduction 
This repository uses two microcontrollers 1. Esp32cam and 2. Rpi pico , Esp32 cam serves the purpose for detection and Rpi pico is used as Gps tracker .
With the codes given in this repo onw must be able to use it properly 

Here is a sample video present which shows the detection demo 



https://github.com/TechShark20/Secmyvehic/assets/122713809/d8fe17f3-07ee-4fec-998f-39aeaa538f09


## Actual prototype 

![Actual prototype](https://github.com/TechShark20/Secmyvehic/assets/122713809/f3bfe646-99d7-42eb-987c-db8374cf9f67)

<br>  An additional battery has attached for giving addtional power to gsm module  </br>

you may refer [this](https://github.com/ssDhp/bt-Pico) for the connection's of rpi pico 

## About 

This project is created for the small sized vehicles , which are old and does not support many secuity function other than key,
with our analysis done through serial monitor of arduino  we found that this model is 80.4% accurate over real time data 
  [visit here ](https://github.com/TechShark20/Secmyvehic/blob/main/Recotestesp32/processingenroll.ipynb)

