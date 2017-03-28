% 工作区清理
clear all; %#ok<CLSCR>
close all;
clc;

imu = imread('IMG_UP_0.bmp');
figure,imshow(imu);
imub = imcrop;
imur = imcrop;
imug = imcrop;

aub=mean(mean(imub));
aur=mean(mean(imur));
aug=mean(mean(imug));

imd = imread('IMG_DOWN_0.bmp');
figure,imshow(imd);
imdb = imcrop;
imdr = imcrop;
imdg = imcrop;

adb=mean(mean(imdb));
adr=mean(mean(imdr));
adg=mean(mean(imdg));