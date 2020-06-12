time_diff_array = zeros(50,1);

for num = 0:49
    load(strcat('data',num2str(num),'.mat'))
    count=1; count1=1;

    for x = 1:length(data_file(:,3))
        if (data_file(x,2) > 1)
            ch1_index(count) = data_file(x,1);
            count=count+1;
        end
        if (data_file(x,3) > 1)
            ch2_index(count1) = data_file(x,1);
            count1=count1+1;
        end

    end

ch1_min = min(ch1_index);
ch2_min = min(ch2_index);
time_diff_array(num+1) = ch1_min - ch2_min;
end