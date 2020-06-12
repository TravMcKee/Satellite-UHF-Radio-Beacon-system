clear; clc;

for num = 0:49

file = strcat('NewFile',num2str(num),'.csv')
data_file = csvread(strcat('NewFile',num2str(num),'.csv'),3);

save(strcat('data',num2str(num),'.mat'),'data_file')

end