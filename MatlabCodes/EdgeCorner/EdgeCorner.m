% 工作区清理
clear all; %#ok<CLSCR>
close all;
clc;

im_ori = imread('testimg.bmp');
[m,n] = size(im_ori);

thres_edge = 15;
im_edge = zeros(m,n);

% 寻找左右边界
for i = 1:m
    findLeftEdge = 0;
    for j = 18:n-16
        if im_ori(i,j)-im_ori(i,j-1) > thres_edge
            im_edge(i,j) = 1;
            findLeftEdge =1;
            break;
        end
    end
    if 1 == findLeftEdge
        for k = n-17:-1:17
            if im_ori(i,k)-im_ori(i,k+1) > thres_edge
                im_edge(i,k) = 1;
                break;
            end
        end
    end
end

% 寻找上下边界
for i = 17:n-16
    findUpperEdge = 0;
    for j = 2:m
        if im_ori(j,i)-im_ori(j-1,i) > thres_edge
            im_edge(j,i) = 1;
            findUpperEdge =1;
            break;
        end
    end
    if 1 == findUpperEdge
        for k = m-1:-1:1
            if im_ori(k,i)-im_ori(k+1,i) > thres_edge
                im_edge(k,i) = 1;
                break;
            end
        end
    end
end

% 边界直线拟合
edge_upper = zeros(m,n);
edge_down = zeros(m,n);
edge_left = zeros(m,n);
edge_right = zeros(m,n);
edge_upper(1:round(m/3), round(n/3):round(n*2/3)) = im_edge(1:round(m/3), round(n/3):round(n*2/3));
edge_down(round(m*2/3):m, round(n/3):round(n*2/3)) = im_edge(round(m*2/3):m, round(n/3):round(n*2/3));
edge_left(round(m/3):round(m*2/3), 1:round(n/3)) = im_edge(round(m/3):round(m*2/3), 1:round(n/3));
edge_right(round(m/3):round(m*2/3), round(n*2/3):n) = im_edge(round(m/3):round(m*2/3), round(n*2/3):n);

pu = getLine(edge_upper);
pd = getLine(edge_down);
pl = getLine(edge_left);
pr = getLine(edge_right);

% 求四个角点
x1 = (pl(2)-pu(2))/(pu(1)-pl(1));
y1 = pu(1)*x1+pu(2);
x1 = round(x1);
y1 = round(y1);
x2 = (pr(2)-pu(2))/(pu(1)-pr(1));
y2 = pu(1)*x2+pu(2);
x2 = round(x2);
y2 = round(y2);
x3 = (pl(2)-pd(2))/(pd(1)-pl(1));
y3 = pd(1)*x3+pd(2);
x3 = round(x3);
y3 = round(y3);
x4 = (pr(2)-pd(2))/(pd(1)-pr(1));
y4 = pd(1)*x4+pd(2);
x4 = round(x4);
y4 = round(y4);

% 求投射变换矩阵
xc = [x1 x2 x3 x4];
yc = [y1 y2 y3 y4];
xn = [1 1 240 240];
yn = [1 600 1 600];
B=[xn(1) yn(1) xn(2) yn(2) xn(3) yn(3) xn(4) yn(4)]';
A=[xc(1) yc(1) 1 0 0 0 -xn(1)*xc(1) -xn(1)*yc(1);             
   0 0 0 xc(1) yc(1) 1 -yn(1)*xc(1) -yn(1)*yc(1);
   xc(2) yc(2) 1 0 0 0 -xn(2)*xc(2) -xn(2)*yc(2);
   0 0 0 xc(2) yc(2) 1 -yn(2)*xc(2) -yn(2)*yc(2);
   xc(3) yc(3) 1 0 0 0 -xn(3)*xc(3) -xn(3)*yc(3);
   0 0 0 xc(3) yc(3) 1 -yn(3)*xc(3) -yn(3)*yc(3);
   xc(4) yc(4) 1 0 0 0 -xn(4)*xc(4) -xn(4)*yc(4);
   0 0 0 xc(4) yc(4) 1 -yn(4)*xc(4) -yn(4)*yc(4)];

fa=inv(A)*B;        %用四点求得的方程的解，也是全局变换系数
a=fa(1);b=fa(2);c=fa(3);
d=fa(4);e=fa(5);f=fa(6);
g=fa(7);h=fa(8);

rot=[a b c;
     d e f;
     g h 1];        %公式中第一个数是x,Matlab第一个表示y，所以我矩阵1,2行互换了

pix1=rot*[1 1 1]'/(g*1+h*1+1);  %变换后图像左上点
pix2=rot*[1 n 1]'/(g*1+h*n+1);  %变换后图像右上点
pix3=rot*[m 1 1]'/(g*m+h*1+1);  %变换后图像左下点
pix4=rot*[m n 1]'/(g*m+h*n+1);  %变换后图像右下点

height=round(max([pix1(1) pix2(1) pix3(1) pix4(1)])-min([pix1(1) pix2(1) pix3(1) pix4(1)]));     %变换后图像的高度
width=round(max([pix1(2) pix2(2) pix3(2) pix4(2)])-min([pix1(2) pix2(2) pix3(2) pix4(2)]));      %变换后图像的宽度
im_fin=zeros(height,width);

delta_y=round(abs(min([pix1(1) pix2(1) pix3(1) pix4(1)])));            %取得y方向的负轴超出的偏移量
delta_x=round(abs(min([pix1(2) pix2(2) pix3(2) pix4(2)])));            %取得x方向的负轴超出的偏移量
inv_rot=inv(rot);

for i = 1-delta_y:height-delta_y                        %从变换图像中反向寻找原图像的点，以免出现空洞，和旋转放大原理一样
    for j = 1-delta_x:width-delta_x
        pix=inv_rot*[i j 1]';       %求原图像中坐标，因为[YW XW W]=fa*[y x 1],所以这里求的是[YW XW W],W=gy+hx+1;
        pix=inv([g*pix(1)-1 h*pix(1);g*pix(2) h*pix(2)-1])*[-pix(1) -pix(2)]'; %相当于解[pix(1)*(gy+hx+1) pix(2)*(gy+hx+1)]=[y x],这样一个方程，求y和x，最后pix=[y x];
        
        if pix(1)>=0.5 && pix(2)>=0.5 && pix(1)<=m && pix(2)<=n
            im_fin(i+delta_y,j+delta_x)=im_ori(round(pix(1)),round(pix(2)));     %最邻近插值,也可以用双线性或双立方插值
        end  
    end
end
im_fin = uint8(im_fin);
im_fin_cash = im_fin(1+delta_y:240+delta_y,1+delta_x:600+delta_x);

% 输出结果
subplot(2,2,1);imshow(im_ori);
subplot(2,2,2);imshow(im_edge);
hold on;
px = 1:m;
pyu = pu(1)*px + pu(2);
pyd = pd(1)*px + pd(2);
pyl = pl(1)*px + pl(2);
pyr = pr(1)*px + pr(2);
subplot(2,2,2);plot(pyu, px, 'r');
subplot(2,2,2);plot(pyd, px, 'r');
subplot(2,2,2);plot(pyl, px, 'r');
subplot(2,2,2);plot(pyr, px, 'r');
subplot(2,2,2);plot(y1, x1, '*y');
subplot(2,2,2);plot(y2, x2, '*y');
subplot(2,2,2);plot(y3, x3, '*y');
subplot(2,2,2);plot(y4, x4, '*y');
hold off;
subplot(2,2,3);imshow(im_fin);
subplot(2,2,4);imshow(im_fin_cash);