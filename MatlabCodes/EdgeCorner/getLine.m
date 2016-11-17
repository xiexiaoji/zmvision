function [ p ] = getLine( edgePoint )
%GETLINE �˴���ʾ�йش˺�����ժҪ
%   �˴���ʾ��ϸ˵��

[p,q] = size(edgePoint);
xe = [];
ye = [];
count = 1;
for i = 1:p
    for j = 1:q
        if 1 == edgePoint(i,j)
            xe(count) = i;
            ye(count) = j;
            count = count + 1;
        end
    end
end

[p] = polyfit(xe,ye,1);

end

