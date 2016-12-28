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
[px,py] = ginput(2);
close(figure(1));

angle = atand((py(2)-py(1))/(px(2)-px(1)));
if angle<0
    fprintf('Angle is left %f', abs(angle));
else
    fprintf('Angle is right %f', angle);
end