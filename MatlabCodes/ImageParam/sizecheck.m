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
imcroped = imcrop;
close(figure(1));

[m,n] = size(imcroped);
height = zeros(1,n);
for i=1:n
    findUpperEdge = 0;
    edge_up = 0;
    edge_down = 0;
    for j = 2:m
        if imcroped(j,i)-imcroped(j-1,i) > 20
            edge_up = j;
            findUpperEdge =1;
            break;
        end
    end
    if 1 == findUpperEdge
        for k = m-1:-1:1
            if imcroped(k,i)-imcroped(k+1,i) > 20
                edge_down = k;
                break;
            end
        end
    end
    if edge_up~=0 && edge_down~=0
        height(1,i) = edge_down - edge_up;
    end
end

height_average = mean(height);
figure;plot(height);axis([1 n 100 500]);title(height_average);