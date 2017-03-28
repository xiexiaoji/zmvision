% 工作区清理
clear all; %#ok<CLSCR>
close all;
clc;

[filename, pathname] = uigetfile('*.bmp', 'Pick an bmp file...');

if isequal(filename,0)
    disp('User selected Cancel')
else
    img = imread(fullfile(pathname, filename));
end  

figure(1);
imshow(img);
imu = imcrop;
ad=mean(mean(imu));

fprintf('Mean gray is %d\n', uint8(ad));