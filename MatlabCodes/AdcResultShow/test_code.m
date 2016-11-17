% 工作区清理
clear all; %#ok<CLSCR>
close all;
clc;

% ADC双路采样显示
adctest01 = importdata('E:\tftp\adctest_01.txt');
adctest02 = importdata('E:\tftp\adctest_02.txt');
[m,n] = size(adctest01);

adcm1 = adctest01(1, 1:2:n-1);
adcm2 = adctest01(1, 2:2:n);

capCount = 1;
lastValue = adcm1(1,1);
result1 = [];
resultNum1 = 0;
for i = 2:n/2
    if abs(adcm1(1,i) - lastValue) > 200  
        if resultNum1 > 0
            result1(resultNum1) = capCount;
        end
        resultNum1 = resultNum1 + 1;
        capCount = 0;
    end
    lastValue = adcm1(1,i);
    capCount = capCount + 1;
end

capCount = 1;
lastValue = adcm2(1,1);
result2 = [];
resultNum2 = 0;
for i = 2:n/2
    if abs(adcm2(1,i) - lastValue) > 200  
        if resultNum2 > 0
            result2(resultNum2) = capCount;
        end
        resultNum2 = resultNum2 + 1;
        capCount = 0;
    end
    lastValue = adcm2(1,i);
    capCount = capCount + 1;
end

figure;
subplot(5,1,1);plot(adcm1, 'r');
subplot(5,4,5:7);plot(result1, 'r');
subplot(5,4,8);histogram(result1, 'FaceColor', 'r');
subplot(5,1,3);plot(adcm2, 'b');
subplot(5,4,13:15);plot(result2, 'b');
subplot(5,4,16);histogram(result2, 'FaceColor', 'b');
subplot(5,1,5);plot(adctest02, 'm');

%ADC单路采样显示
% adctest01 = importdata('E:\tftp\adctest_01.txt');
% adctest02 = importdata('E:\tftp\adctest_02.txt');
% [m,n] = size(adctest01);
% 
% capCount = 1;
% lastValue = adctest01(1,1);
% result = [];
% resultNum = 0;
% for i = 2:n
%     if abs(adctest01(1,i) - lastValue) > 200
%         fprintf('%d ', capCount);        
%         if resultNum > 0
%             result(resultNum) = capCount;
%         end
%         resultNum = resultNum + 1;
%         capCount = 0;
%     end
%     lastValue = adctest01(1,i);
%     capCount = capCount + 1;
% end
% 
% result = result(find(result > 50));
% 
% fprintf('\n%d\n ', round(mean(result)));
% figure;
% subplot(3,1,1);plot(adctest01, 'r');
% subplot(3,1,2);plot(adctest02, 'r');
% subplot(3,1,3);plot(result, 'b');